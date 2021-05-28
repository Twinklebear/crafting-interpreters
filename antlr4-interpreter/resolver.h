#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "LoxBaseVisitor.h"
#include "antlr4-common.h"
#include "environment.h"
#include "interpreter.h"

using namespace loxgrammar;

enum class FunctionType { NONE, FUNCTION };

struct VariableStatus {
    bool defined = false;
    bool read = false;
};

struct Resolver : public LoxBaseVisitor {
    // Treated as a stack, but we need to access scopes by index as well
    // when resolving variables
    std::vector<std::unordered_map<std::string, VariableStatus>> scopes;
    FunctionType current_function = FunctionType::NONE;

    Interpreter &interpreter;

    Resolver(Interpreter &interpreter);

    // Visitors for expressions
    antlrcpp::Any visitPrimary(LoxParser::PrimaryContext *ctx) override;
    // void visit(const Literal &l) override;
    // void visit(const Variable &v) override;

    // TODO: Assign can also have a callExpr on the left which it needs to evaluate,
    // e.g., when setting a struct member variable
    antlrcpp::Any visitAssign(LoxParser::AssignContext *ctx) override;
    // void visit(const Assign &a) override;
    // void visit(const Set &s) override;

    antlrcpp::Any visitBlock(LoxParser::BlockContext *ctx) override;
    // void visit(const Block &b) override;

    antlrcpp::Any visitVarDeclStmt(LoxParser::VarDeclStmtContext *ctx) override;
    // void visit(const Var &v) override;

    antlrcpp::Any visitFunction(LoxParser::FunctionContext *ctx) override;
    // void visit(const Function &f) override;

    antlrcpp::Any visitReturnStmt(LoxParser::ReturnStmtContext *ctx) override;
    // void visit(const Return &r) override;

    antlrcpp::Any visitClassDecl(LoxParser::ClassDeclContext *ctx) override;
    // void visit(const Class &c) override;

private:
    void begin_scope();
    void end_scope();

    void declare(const antlr4::Token *name);
    void define(const antlr4::Token *name);

    void resolve_local(LoxParser::ExprContext *expr, const antlr4::Token *name);

    void resolve_function(LoxParser::FunctionContext *f, const FunctionType type);
};
