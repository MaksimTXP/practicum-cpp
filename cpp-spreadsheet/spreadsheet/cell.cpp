#include "cell.h"
#include "sheet.h"
#include <cassert>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using namespace std::literals;

class Cell::Impl {
public:

    virtual ~Impl() = default;

    virtual Value GetValue() const = 0;

    virtual std::string GetText() const = 0;

    virtual std::vector<Position> GetReferencedCells() const {
        return {};
    }
    virtual void ClearCache() {}
};

class Cell::EmptyImpl : public Cell::Impl {
public:
    Value GetValue() const override {
        return "";
    }
    std::string GetText() const override {
        return "";
    }
};

class Cell::TextImpl : public Cell::Impl {
public:

    TextImpl(std::string text) : text_(std::move(text)) {
        if (text_.empty()) {
            throw std::logic_error("Text cannot be empty");
        }
    }

    Value GetValue() const override {
        if (text_[0] == ESCAPE_SIGN) {
            return text_.substr(1);
        }
        return text_;
    }

    std::string GetText() const override {
        return text_;
    }

private:
    std::string text_;
};

class Cell::FormulaImpl : public Cell::Impl {
public:
    FormulaImpl(std::string expression, const SheetInterface& sheet) : sheet_(sheet) {
        if (expression.empty() || expression[0] != FORMULA_SIGN) {
            throw std::logic_error("Invalid formula");
        }
        formula_ = ParseFormula(expression.substr(1));
    }

    Value GetValue() const override {
        if (!cache_) cache_ = formula_->Evaluate(sheet_);

        auto value = formula_->Evaluate(sheet_);
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }

        return std::get<FormulaError>(value);
    }

    std::string GetText() const override {
        return FORMULA_SIGN + formula_->GetExpression();
    }

    std::vector<Position> GetReferencedCells() const override {
        return formula_->GetReferencedCells();
    }

    void ClearCahce() {
        cache_.reset();
    }


private:
    const SheetInterface& sheet_;
    std::unique_ptr<FormulaInterface> formula_;
    mutable std::optional<FormulaInterface::Value> cache_;
};

Cell::Cell(SheetInterface& sheet) : impl_(std::make_unique<EmptyImpl>()), sheet_(sheet) {}

Cell::~Cell() {}

void Cell::Set(std::string text) {

    std::unique_ptr<Impl> new_impl;
    if (text.empty()) {
        new_impl = std::make_unique<EmptyImpl>();
    }
    else if (text.front() == FORMULA_SIGN && text.size() > 1) {
        new_impl = std::make_unique<FormulaImpl>(text, sheet_);
        auto referenced_cells = new_impl->GetReferencedCells();
        CheckCircularDependency(referenced_cells, this);
    }
    else {
        new_impl = std::make_unique<TextImpl>(text);
    }

    impl_ = std::move(new_impl);
    ClearReferenced();
    SetReferencedAndDependent();
    ClearCache();
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
    ClearReferenced();
    ClearCache();
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue();
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
}

bool Cell::IsDependent() const {
    return !dependent_cells_.empty();
}

void Cell::CheckCircularDependency(const std::vector<Position>& referenced, const Cell* cell) const {
    for (const auto& referenced_pos : referenced) {
        auto referenced_cell = dynamic_cast<Cell*>(sheet_.GetCell(referenced_pos));
        if (referenced_cell) {
            if (referenced_cell == cell) {
                throw CircularDependencyException("Cycle detected");
            }            referenced_cell->CheckCircularDependency(referenced_cell->GetReferencedCells(), cell);
        }
    }
}

void Cell::ClearReferenced() {
    for (auto referenced : referenced_cells_) {
        referenced->dependent_cells_.erase(this);
    }
    referenced_cells_.clear();
}

void Cell::ClearCache() {
    impl_->ClearCache();
    for (auto&& dependent : dependent_cells_) {
        dependent->ClearCache();
    }
}

void Cell::SetReferencedAndDependent() {
    for (const auto& referenced_pos : GetReferencedCells()) {
        if (sheet_.GetCell(referenced_pos) == nullptr) {
            sheet_.SetCell(referenced_pos, ""s); 
        }
        auto referenced_cell = static_cast<Cell*>(sheet_.GetCell(referenced_pos));
        referenced_cell->dependent_cells_.insert(this);
        referenced_cells_.insert(referenced_cell);
    }
}

