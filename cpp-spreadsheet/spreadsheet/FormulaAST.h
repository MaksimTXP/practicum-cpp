#pragma once
#include "FormulaLexer.h"

#include "common.h"
#include <forward_list>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

namespace ASTImpl {
    class Expr;
}

class ParsingError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

using Args = std::function<double(Position)>;

class FormulaAST {
public:

    explicit FormulaAST(std::unique_ptr<ASTImpl::Expr> root_expr, std::forward_list<Position> cells);
    FormulaAST(FormulaAST&&) = default; 
    FormulaAST& operator=(FormulaAST&&) = default; 
    ~FormulaAST(); 

    double Execute(const Args& args) const;

    void Print(std::ostream& out) const;

    void PrintFormula(std::ostream& out) const;

    void PrintCells(std::ostream& out) const;


    std::forward_list<Position>& GetReferencedCells();

    const std::forward_list<Position>& GetReferencedCells() const;

private:
    std::unique_ptr<ASTImpl::Expr> root_expr_; 
    std::forward_list<Position> cells_;        
};

FormulaAST ParseFormulaAST(std::istream& in);
FormulaAST ParseFormulaAST(const std::string& in_str);
