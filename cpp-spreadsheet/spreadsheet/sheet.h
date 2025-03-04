#pragma once

#include "cell.h"
#include "common.h"
#include <unordered_map>
#include <memory>

class Sheet : public SheetInterface {
public:
    using SheetTable = std::unordered_map<Position, std::unique_ptr<Cell>, PositionHash, PositionEqual>;

    ~Sheet() override;

    void SetCell(Position pos, std::string text) override;
    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;
    void ClearCell(Position pos) override;
    Size GetPrintableSize() const override;
    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:

    SheetTable sheet_table_;

    void ValidatePosition(const Position& pos) const;
};
std::unique_ptr<SheetInterface> CreateSheet();