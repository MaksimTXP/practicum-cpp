#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

FormulaError::FormulaError(Category category)
    : category_(category) {}

FormulaError::Category FormulaError::GetCategory() const {
    return category_;
}

bool FormulaError::operator==(FormulaError rhs) const {
    return category_ == rhs.category_;
}

std::string_view FormulaError::ToString() const {
    switch (category_) {
    case Category::Ref:
        return "#REF!";
    case Category::Value:
        return "#VALUE!";
    case Category::Arithmetic:
        return "#ARITHM!";
    }
    return "";
}

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}

namespace {

    class Formula : public FormulaInterface {
    public:
        explicit Formula(std::string expression)
            try : ast_(ParseFormulaAST(expression)) {}
        catch (const std::exception& e) {
            std::throw_with_nested(FormulaException(e.what()));
        }

        Value Evaluate(const SheetInterface& sheet) const override {
            const std::function<double(Position)> args = [&sheet](const Position p) -> double {

                if (!p.IsValid()) {
                    throw FormulaError(FormulaError::Category::Ref);
                }

                const auto* cell = sheet.GetCell(p);

                if (!cell) {
                    return 0;
                }

                const auto& value = cell->GetValue();

                if (std::holds_alternative<double>(value)) {
                    return std::get<double>(value);
                }

                if (std::holds_alternative<std::string>(value)) {
                    auto str_value = std::get<std::string>(value);
                    double result = 0;
                    if (!str_value.empty()) {
                        std::istringstream in(str_value);
                        if (!(in >> result) || !in.eof()) {
                            throw FormulaError(FormulaError::Category::Value);
                        }
                    }
                    return result;
                }
                throw FormulaError(std::get<FormulaError>(value));
            };

            try {
                return ast_.Execute(args);
            }
            catch (FormulaError& e) {
                return e;
            }
        }

        std::vector<Position> GetReferencedCells() const override {
            std::vector<Position> cells;
            for (auto cell : ast_.GetReferencedCells()) {
                if (cell.IsValid()) {
                    cells.push_back(cell);
                }
            }
            cells.resize(std::unique(cells.begin(), cells.end()) - cells.begin());
            return cells;
        }

        std::string GetExpression() const override {
            std::ostringstream out;
            ast_.PrintFormula(out);
            return out.str();
        }

    private:
        const FormulaAST ast_;
    };

}

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    }
    catch (...) {
        throw FormulaException("");
    }
}