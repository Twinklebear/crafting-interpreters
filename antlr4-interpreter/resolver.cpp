#include "resolver.h"
#include <iostream>
#include "util.h"

Resolver::Resolver(Interpreter &interpreter) : interpreter(interpreter) {}

antlrcpp::Any Resolver::visitPrimary(LoxParser::PrimaryContext *ctx)
{
    if (!ctx->IDENTIFIER()) {
        return antlrcpp::Any();
    }

    auto *var = ctx->IDENTIFIER();
    // Check if we're trying to assign the variable to itself on accident
    if (!scopes.empty()) {
        auto &scope = scopes.back();
        auto fnd = scope.find(var->getText());
        if (fnd != scope.end() && !fnd->second.defined) {
            error(var->getSymbol(), "Can't read local variable in its own initializer");
        }
    }
    resolve_local(ctx, var->getSymbol());
    return antlrcpp::Any();
}

antlrcpp::Any Resolver::visitAssign(LoxParser::AssignContext *ctx)
{
    // visit the right hand side then do the resolve local
    // TODO: How to handle a call/etc. or struct being on the left?
    visit(ctx->expr());
    resolve_local(ctx, ctx->IDENTIFIER()->getSymbol());
    return antlrcpp::Any();
}

antlrcpp::Any Resolver::visitBlock(LoxParser::BlockContext *ctx)
{
    begin_scope();
    visitChildren(ctx);
    end_scope();
    return antlrcpp::Any();
}

antlrcpp::Any Resolver::visitVarDeclStmt(LoxParser::VarDeclStmtContext *ctx)
{
    declare(ctx->IDENTIFIER()->getSymbol());
    std::cout << "vardecl has " << ctx->children.size()
              << " children (i expect it's 1? or 2?)\n";
    visitChildren(ctx);
    define(ctx->IDENTIFIER()->getSymbol());
    return antlrcpp::Any();
}

antlrcpp::Any Resolver::visitFunction(LoxParser::FunctionContext *ctx)
{
    declare(ctx->IDENTIFIER()->getSymbol());
    define(ctx->IDENTIFIER()->getSymbol());
    resolve_function(ctx, FunctionType::FUNCTION);
    return antlrcpp::Any();
}

antlrcpp::Any Resolver::visitReturnStmt(LoxParser::ReturnStmtContext *ctx)
{
    if (current_function == FunctionType::NONE) {
        error(ctx->getStart(), "Can't return in top-level code");
    }
    return visitChildren(ctx);
}

antlrcpp::Any Resolver::visitClassDecl(LoxParser::ClassDeclContext *ctx)
{
    declare(ctx->IDENTIFIER()->getSymbol());
    define(ctx->IDENTIFIER()->getSymbol());
    return visitChildren(ctx);
}

void Resolver::begin_scope()
{
    scopes.push_back(std::unordered_map<std::string, VariableStatus>());
}

void Resolver::end_scope()
{
    // For unused local var: when we pop the scope, check if it was read from
    for (const auto &v : scopes.back()) {
        if (!v.second.read) {
            std::cout << "Warning: local variable " << v.first << " is never read\n";
        }
    }
    scopes.pop_back();
}

void Resolver::declare(const antlr4::Token *name)
{
    if (scopes.empty()) {
        return;
    }
    auto &scope = scopes.back();
    auto fnd = scope.find(name->getText());
    if (fnd != scope.end()) {
        error(name, "A variable with this name already exists in current scope");
    }
    scope[name->getText()] = VariableStatus();
}

void Resolver::define(const antlr4::Token *name)
{
    if (scopes.empty()) {
        return;
    }
    auto &scope = scopes.back();
    scope[name->getText()].defined = true;
}

void Resolver::resolve_local(LoxParser::ExprContext *expr, const antlr4::Token *name)
{
    std::cout << "At: " << name->getLine() << ":" << name->getCharPositionInLine()
              << ": Resolving " << name->getText() << "\n";
    // Walk back up the scopes until we find the closest one containing the variable
    for (int i = scopes.size() - 1; i >= 0; --i) {
        auto &scope = scopes[i];
        auto fnd = scope.find(name->getText());
        if (fnd != scope.end()) {
            fnd->second.read = true;
            interpreter.resolve(expr, scopes.size() - 1 - i);
            return;
        }
    }
}

void Resolver::resolve_function(LoxParser::FunctionContext *f, const FunctionType type)
{
    auto enclosing_function = current_function;
    current_function = type;

    begin_scope();
    if (f->parameters()) {
        auto params = f->parameters()->IDENTIFIER();
        for (const auto &param : params) {
            declare(param->getSymbol());
            define(param->getSymbol());
        }
    }
    visitBlock(f->block());
    end_scope();

    current_function = enclosing_function;
}
