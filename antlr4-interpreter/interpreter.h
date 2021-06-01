#pragma once

#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include "LoxBaseVisitor.h"
#include "antlr4-common.h"
#include "environment.h"

using namespace loxgrammar;

struct InterpreterError {
    const antlr4::Token *token;
    std::string message;

    InterpreterError(const antlr4::Token *t, const std::string &msg);
};

struct ReturnControlFlow {
    antlrcpp::Any value;

    ReturnControlFlow(const antlrcpp::Any &value);

    ReturnControlFlow(const ReturnControlFlow &r) = default;

    ReturnControlFlow() = default;
};

struct Interpreter : public LoxBaseVisitor {
    std::shared_ptr<Environment> globals = std::make_shared<Environment>();
    std::shared_ptr<Environment> environment = globals;
    // Track the depth each variable expresion is resolved to
    // So couldn't this just be "Variable*"?
    // NOTE: The pointers all refer to objects held in std::shared_ptr, though
    // with how the visitor pattern works here the shared ptr is not directly
    // accessible since visit is called by the object itself.
    // Maybe clox introduces a better design here, or just uses raw pointers throughout?
    std::unordered_map<const antlr4::ParserRuleContext *, size_t> locals;
    std::any result;

    Interpreter();

    void resolve(const LoxParser::ExprContext *expr, size_t depth);

    antlrcpp::Any visitPrimary(LoxParser::PrimaryContext *ctx) override;
    // void visit(const Literal &l) override;
    // void visit(const Variable &v) override;

    antlrcpp::Any visitUnary(LoxParser::UnaryContext *ctx) override;
    // void visit(const Unary &u) override;

    antlrcpp::Any visitDiv(LoxParser::DivContext *ctx) override;
    antlrcpp::Any visitMult(LoxParser::MultContext *ctx) override;
    antlrcpp::Any visitAddSub(LoxParser::AddSubContext *ctx) override;
    antlrcpp::Any visitComparison(LoxParser::ComparisonContext *ctx) override;
    antlrcpp::Any visitEquality(LoxParser::EqualityContext *ctx) override;
    // void visit(const Binary &b) override;

    antlrcpp::Any visitCallExpr(LoxParser::CallExprContext *ctx) override;
    // void visit(const Call &c) override;
    // void visit(const Get &g) override;

    antlrcpp::Any visitLogicOr(LoxParser::LogicOrContext *ctx) override;
    antlrcpp::Any visitLogicAnd(LoxParser::LogicAndContext *ctx) override;
    // void visit(const Logical &l) override;

    // TODO: Assign can also have a callExpr on the left which it needs to evaluate,
    // e.g., when setting a struct member variable
    antlrcpp::Any visitAssign(LoxParser::AssignContext *ctx) override;
    // void visit(const Assign &a) override;
    // void visit(const Set &s) override;

    antlrcpp::Any visitBlock(LoxParser::BlockContext *ctx) override;
    // void visit(const Block &b) override;

    antlrcpp::Any visitExprStmt(LoxParser::ExprStmtContext *ctx) override;
    // void visit(const Expression &e) override;

    antlrcpp::Any visitIfStmt(LoxParser::IfStmtContext *ctx) override;
    // void visit(const If &f) override;

    antlrcpp::Any visitWhileStmt(LoxParser::WhileStmtContext *ctx) override;
    antlrcpp::Any visitForStmt(LoxParser::ForStmtContext *ctx) override;
    // void visit(const While &w) override;

    antlrcpp::Any visitPrintStmt(LoxParser::PrintStmtContext *ctx) override;
    // void visit(const Print &p) override;

    antlrcpp::Any visitVarDeclStmt(LoxParser::VarDeclStmtContext *ctx) override;
    // void visit(const Var &v) override;

    antlrcpp::Any visitFunction(LoxParser::FunctionContext *ctx) override;
    // void visit(const Function &f) override;

    antlrcpp::Any visitReturnStmt(LoxParser::ReturnStmtContext *ctx) override;
    // void visit(const Return &r) override;

    antlrcpp::Any visitClassDecl(LoxParser::ClassDeclContext *ctx) override;
    // void visit(const Class &c) override;

    antlrcpp::Any visit_with_environment(antlr4::tree::ParseTree *tree,
                                         std::shared_ptr<Environment> &env);

private:
    std::type_index float_id, string_id, bool_id, nil_id, callable_id;
    std::unordered_map<std::type_index, std::string> type_names;

    // Check if the type is one of the specified valid types, if not throws an
    // InterpreterError
    void check_type(const antlrcpp::Any &val,
                    const std::vector<std::type_index> &valid_types,
                    const antlr4::Token *t);

    // Check if the two anys have the same type, if not throws an InterpreterError
    void check_same_type(const antlrcpp::Any &a,
                         const antlrcpp::Any &b,
                         const antlr4::Token *t) const;

    bool is_true(const antlrcpp::Any &x) const;

    bool is_equal(const antlrcpp::Any &a, const antlrcpp::Any &b) const;

    antlrcpp::Any lookup_variable(const antlr4::Token *token,
                                  const antlr4::ParserRuleContext *node) const;
};
