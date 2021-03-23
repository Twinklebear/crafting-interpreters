#include "resolver.h"
#include "util.h"

Resolver::Resolver(Interpreter &interpreter) : interpreter(interpreter) {}

void Resolver::visit(const Grouping &g)
{
    resolve(g.expr);
}

void Resolver::visit(const Literal &) {}

void Resolver::visit(const Unary &u)
{
    resolve(u.expr);
}

void Resolver::visit(const Binary &b)
{
    resolve(b.left);
    resolve(b.right);
}

void Resolver::visit(const Call &c)
{
    resolve(c.callee);
    for (const auto &arg : c.args) {
        resolve(arg);
    }
}

void Resolver::visit(const Logical &l)
{
    resolve(l.left);
    resolve(l.right);
}

void Resolver::visit(const Variable &v)
{
    // Check if we're trying to assign the variable to itself on accident
    if (!scopes.empty()) {
        auto &scope = scopes.back();
        auto fnd = scope.find(v.name.lexeme);
        if (fnd != scope.end() && fnd->second == false) {
            error(v.name, "Can't read local variable in its own initializer");
        }
    }
    resolve_local(v, v.name);
}

void Resolver::visit(const Assign &a)
{
    resolve(a.value);
    resolve_local(a, a.name);
}

void Resolver::visit(const Block &b)
{
    begin_scope();
    resolve(b.statements);
    end_scope();
}

void Resolver::visit(const Expression &e)
{
    resolve(e.expr);
}

void Resolver::visit(const If &f)
{
    resolve(f.condition);
    resolve(f.then_branch);
    if (f.else_branch) {
        resolve(f.else_branch);
    }
}

void Resolver::visit(const While &w)
{
    resolve(w.condition);
    resolve(w.body);
}

void Resolver::visit(const Print &p)
{
    resolve(p.expr);
}

void Resolver::visit(const Var &v)
{
    declare(v.token);
    if (v.initializer) {
        resolve(v.initializer);
    }
    define(v.token);
}

void Resolver::visit(const Function &f)
{
    declare(f.name);
    define(f.name);
    resolve_function(f);
}

void Resolver::visit(const Return &r)
{
    resolve(r.value);
}

void Resolver::begin_scope()
{
    scopes.push_back(std::unordered_map<std::string, bool>());
}

void Resolver::end_scope()
{
    scopes.pop_back();
}

void Resolver::resolve(const std::vector<std::shared_ptr<Stmt>> &statements)
{
    for (const auto &s : statements) {
        resolve(s);
    }
}

void Resolver::resolve(const std::shared_ptr<Stmt> &statement)
{
    statement->accept(*this);
}

void Resolver::resolve(const std::shared_ptr<Expr> &expr)
{
    expr->accept(*this);
}

void Resolver::declare(const Token &name)
{
    if (scopes.empty()) {
        return;
    }
    auto &scope = scopes.back();
    scope[name.lexeme] = false;
}

void Resolver::define(const Token &name)
{
    if (scopes.empty()) {
        return;
    }
    auto &scope = scopes.back();
    scope[name.lexeme] = true;
}

void Resolver::resolve_local(const Expr &expr, const Token &name)
{
    // Walk back up the scopes until we find the closest one containing the variable
    for (int i = scopes.size() - 1; i >= 0; --i) {
        auto &scope = scopes[i];
        auto fnd = scope.find(name.lexeme);
        if (fnd != scope.end()) {
            interpreter.resolve(expr, scopes.size() - 1 - i);
            return;
        }
    }
}

void Resolver::resolve_function(const Function &f)
{
    begin_scope();
    for (const auto &param : f.params) {
        declare(param);
        define(param);
    }
    resolve(f.body);
    end_scope();
}
