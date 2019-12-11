#include "expr.h"
Unary::Unary(Token op, std::shared_ptr<Expr> expr) : op(op), expr(expr) {}
void Unary::accept(Visitor &v) const
{
    v.visit(*this);
}
Literal::Literal(std::any value) : value(value) {}
void Literal::accept(Visitor &v) const
{
    v.visit(*this);
}
Binary::Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
    : left(left), op(op), right(right)
{
}
void Binary::accept(Visitor &v) const
{
    v.visit(*this);
}
Grouping::Grouping(std::shared_ptr<Expr> expr) : expr(expr) {}
void Grouping::accept(Visitor &v) const
{
    v.visit(*this);
}