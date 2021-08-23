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

    antlrcpp::Any visitFunction(LoxParser::FunctionContext *ctx) override;

    antlrcpp::Any visitClassDecl(LoxParser::ClassDeclContext *ctx) override;

    antlrcpp::Any visitVarDeclStmt(LoxParser::VarDeclStmtContext *ctx) override;

    antlrcpp::Any visitVarDecl(LoxParser::VarDeclContext *ctx) override;

    antlrcpp::Any visitIfStmt(LoxParser::IfStmtContext *ctx) override;

    antlrcpp::Any visitWhileStmt(LoxParser::WhileStmtContext *ctx) override;

    antlrcpp::Any visitForStmt(LoxParser::ForStmtContext *ctx) override;

    antlrcpp::Any visitPrintStmt(LoxParser::PrintStmtContext *ctx) override;

    antlrcpp::Any visitReturnStmt(LoxParser::ReturnStmtContext *ctx) override;

    antlrcpp::Any visitBlock(LoxParser::BlockContext *ctx) override;

    antlrcpp::Any visitExprStmt(LoxParser::ExprStmtContext *ctx) override;

    antlrcpp::Any visitUnary(LoxParser::UnaryContext *ctx) override;

    antlrcpp::Any visitCallExpr(LoxParser::CallExprContext *ctx) override;
    antlrcpp::Any visitArguments(LoxParser::ArgumentsContext *ctx) override;
    antlrcpp::Any visitMemberIdentifier(LoxParser::MemberIdentifierContext *ctx) override;

    antlrcpp::Any visitMult(LoxParser::MultContext *ctx) override;
    antlrcpp::Any visitDiv(LoxParser::DivContext *ctx) override;
    antlrcpp::Any visitAddSub(LoxParser::AddSubContext *ctx) override;
    antlrcpp::Any visitComparison(LoxParser::ComparisonContext *ctx) override;
    antlrcpp::Any visitEquality(LoxParser::EqualityContext *ctx) override;

    antlrcpp::Any visitLogicAnd(LoxParser::LogicAndContext *ctx) override;
    antlrcpp::Any visitLogicOr(LoxParser::LogicOrContext *ctx) override;

    antlrcpp::Any visitAssign(LoxParser::AssignContext *ctx) override;

    antlrcpp::Any visitParens(LoxParser::ParensContext *ctx) override;

    antlrcpp::Any visitPrimary(LoxParser::PrimaryContext *ctx) override;
};
