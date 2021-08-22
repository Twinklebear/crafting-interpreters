#pragma once

#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include "LoxParserBaseVisitor.h"
#include "antlr4-common.h"
#include "environment.h"
#include "expr.h"

using namespace loxgrammar;

// Construct the AST by visiting the input parse tree
struct ASTBuilder : public LoxParserBaseVisitor {
    std::vector<std::shared_ptr<Stmt>> statements;

    antlrcpp::Any visitFile(LoxParser::FileContext *ctx) override;

    antlrcpp::Any visitFunctionDecl(LoxParser::FunctionDeclContext *ctx) override;
    // void visit(const Function &f) override;

    antlrcpp::Any visitFunction(LoxParser::FunctionContext *ctx) override;

    antlrcpp::Any visitClassDecl(LoxParser::ClassDeclContext *ctx) override;
    // void visit(const Class &c) override;

    antlrcpp::Any visitVarDecl(LoxParser::VarDeclContext *ctx) override;
    // void visit(const Var &v) override;

    antlrcpp::Any visitIfStmt(LoxParser::IfStmtContext *ctx) override;
    // void visit(const If &f) override;

    antlrcpp::Any visitWhileStmt(LoxParser::WhileStmtContext *ctx) override;
    // void visit(const While &w) override;

    // TODO: Here can convert it to a while in the AST
    antlrcpp::Any visitForStmt(LoxParser::ForStmtContext *ctx) override;

    antlrcpp::Any visitPrintStmt(LoxParser::PrintStmtContext *ctx) override;
    // void visit(const Print &p) override;

    antlrcpp::Any visitReturnStmt(LoxParser::ReturnStmtContext *ctx) override;
    // void visit(const Return &r) override;

    antlrcpp::Any visitBlock(LoxParser::BlockContext *ctx) override;
    // void visit(const Block &b) override;

    antlrcpp::Any visitExprStmt(LoxParser::ExprStmtContext *ctx) override;
    // void visit(const Expression &e) override;

    antlrcpp::Any visitUnary(LoxParser::UnaryContext *ctx) override;
    // void visit(const Unary &u) override;

    antlrcpp::Any visitCallExpr(LoxParser::CallExprContext *ctx) override;
    antlrcpp::Any visitArguments(LoxParser::ArgumentsContext *ctx) override;
    antlrcpp::Any visitMemberIdentifier(LoxParser::MemberIdentifierContext *ctx) override;
    // void visit(const Call &c) override;
    // void visit(const Get &g) override;

    antlrcpp::Any visitMult(LoxParser::MultContext *ctx) override;
    antlrcpp::Any visitDiv(LoxParser::DivContext *ctx) override;
    antlrcpp::Any visitAddSub(LoxParser::AddSubContext *ctx) override;
    antlrcpp::Any visitComparison(LoxParser::ComparisonContext *ctx) override;
    antlrcpp::Any visitEquality(LoxParser::EqualityContext *ctx) override;
    // void visit(const Binary &b) override;

    antlrcpp::Any visitLogicAnd(LoxParser::LogicAndContext *ctx) override;
    antlrcpp::Any visitLogicOr(LoxParser::LogicOrContext *ctx) override;
    // void visit(const Logical &l) override;

    antlrcpp::Any visitAssign(LoxParser::AssignContext *ctx) override;
    // void visit(const Assign &a) override;
    // void visit(const Set &s) override;

    antlrcpp::Any visitParens(LoxParser::ParensContext *ctx) override;
    // void visit(const Grouping &g) override;

    antlrcpp::Any visitPrimary(LoxParser::PrimaryContext *ctx) override;
};
