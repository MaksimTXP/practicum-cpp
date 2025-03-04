#include "sheet.h"
#include "cell.h"
#include "common.h"
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <queue>

Sheet::~Sheet() = default;

void Sheet::SetCell(Position pos, std::string text) {
    
    ValidatePosition(pos);

    auto cell_it = sheet_table_.find(pos);
    Cell* cell;
    if (cell_it == sheet_table_.end()) {
        auto new_cell = std::make_unique<Cell>(*this);
        cell = new_cell.get();
        sheet_table_[pos] = std::move(new_cell);
    }
    else {
        cell = cell_it->second.get();
    }

    cell->Set(std::move(text));
}

const CellInterface* Sheet::GetCell(Position pos) const {
    
    ValidatePosition(pos);

    auto it = sheet_table_.find(pos);
    if (it != sheet_table_.end()) {
        return it->second.get();
    }
    return nullptr;
}

CellInterface* Sheet::GetCell(Position pos) {
    return const_cast<CellInterface*>(static_cast<const Sheet*>(this)->GetCell(pos));
}

void Sheet::ClearCell(Position pos) {
   
    ValidatePosition(pos);

    auto it = sheet_table_.find(pos);
    if (it != sheet_table_.end()) {
        it->second->Clear();
        if (!it->second->IsDependent()) {
            sheet_table_.erase(it);
        }
    }
}

Size Sheet::GetPrintableSize() const {
    int max_row = 0;
    int max_col = 0;

    for (const auto& [pos, cell] : sheet_table_) {
        max_row = std::max(max_row, pos.row + 1);
        max_col = std::max(max_col, pos.col + 1);
    }

    return { max_row, max_col };
}

void Sheet::PrintValues(std::ostream& output) const {
    auto [rows, cols] = GetPrintableSize();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (j > 0) {
                output << '\t';
            }
            auto cell = GetCell({ i, j });
            if (cell) {
                auto value = cell->GetValue();
                if (std::holds_alternative<double>(value)) {
                    output << std::get<double>(value);
                }
                else if (std::holds_alternative<std::string>(value)) {
                    output << std::get<std::string>(value);
                }
                else if (std::holds_alternative<FormulaError>(value)) {
                    output << std::get<FormulaError>(value).ToString();
                }
            }
        }
        output << '\n';
    }
}

void Sheet::PrintTexts(std::ostream& output) const {
    auto [rows, cols] = GetPrintableSize();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (j > 0) {
                output << '\t';
            }
            auto cell = GetCell({ i, j });
            if (cell) {
                output << cell->GetText();
            }
        }
        output << '\n';
    }

}

void Sheet::ValidatePosition(const Position& pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}
