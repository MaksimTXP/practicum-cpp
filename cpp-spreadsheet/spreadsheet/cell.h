#pragma once

#include "common.h"
#include "formula.h"

#include <unordered_set>
#include <memory>
#include <vector>
#include <string>
#include <optional>

class SheetInterface;

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    bool IsDependent() const;

    void ClearReferenced();
    void ClearCache();
    void SetReferencedAndDependent();

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;

    void CheckCircularDependency(const std::vector<Position>& referenced, const Cell* cell) const;

    std::unique_ptr<Impl> impl_;
    SheetInterface& sheet_;
    std::unordered_set<Cell*> referenced_cells_;
    std::unordered_set<Cell*> dependent_cells_;
};
