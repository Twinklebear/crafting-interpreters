#include "ast_builder.h"
#include <memory>

antlrcpp::Any ASTBuilder::visitFile(LoxParser::FileContext *ctx)
{
    for (auto &d : ctx->declaration()) {
        statements.push_back(visit(d).as<std::shared_ptr<Stmt>>());
    }
    return antlrcpp::Any();
}

antlrcpp::Any ASTBuilder::visitFunctionDecl(LoxParser::FunctionDeclContext *ctx)
{
    auto fn = visitFunction(ctx->function()).as<std::shared_ptr<Function>>();
    return std::static_pointer_cast<Stmt>(fn);
}

antlrcpp::Any ASTBuilder::visitFunction(LoxParser::FunctionContext *ctx)
{
    auto *name = ctx->IDENTIFIER()->getSymbol();

    std::vector<antlr4::Token *> params;
    if (ctx->parameters()) {
        auto formal_params = ctx->parameters()->IDENTIFIER();
        for (size_t i = 0; i < formal_params.size(); ++i) {
            params.push_back(formal_params[i]->getSymbol());
        }
    }

    auto block = visitBlock(ctx->block());
    auto block_expr = block.as<std::shared_ptr<Block>>();
    return std::make_shared<Function>(name, params, block_expr);
}

antlrcpp::Any ASTBuilder::visitClassDecl(LoxParser::ClassDeclContext *ctx)
{
    auto *name = ctx->IDENTIFIER()->getSymbol();
    std::vector<std::shared_ptr<Function>> methods;
    for (auto &f : ctx->function()) {
        methods.push_back(visitFunction(f).as<std::shared_ptr<Function>>());
    }
    auto c = std::make_shared<Class>(name, methods);
    return std::static_pointer_cast<Stmt>(c);
}

antlrcpp::Any ASTBuilder::visitVarDecl(LoxParser::VarDeclContext *ctx)
{
    auto *name = ctx->IDENTIFIER()->getSymbol();
    std::shared_ptr<Expr> initializer;
    if (ctx->expr()) {
        initializer = visit(ctx->expr()).as<std::shared_ptr<Expr>>();
    }
    auto vardecl = std::make_shared<Var>(name, initializer);
    return std::static_pointer_cast<Stmt>(vardecl);
}

antlrcpp::Any ASTBuilder::visitIfStmt(LoxParser::IfStmtContext *ctx)
{
    auto condition = visit(ctx->expr()).as<std::shared_ptr<Expr>>();
    auto then_branch = visit(ctx->statement(0)).as<std::shared_ptr<Stmt>>();

    std::shared_ptr<Stmt> else_branch;
    if (ctx->ELSE()) {
        else_branch = visit(ctx->statement(1)).as<std::shared_ptr<Stmt>>();
    }
    auto if_stmt = std::make_shared<If>(condition, then_branch, else_branch);
    return std::static_pointer_cast<Stmt>(if_stmt);
}

antlrcpp::Any ASTBuilder::visitWhileStmt(LoxParser::WhileStmtContext *ctx)
{
    auto condition = visit(ctx->expr()).as<std::shared_ptr<Expr>>();
    auto body = visit(ctx->statement()).as<std::shared_ptr<Stmt>>();
    auto while_stmt = std::make_shared<While>(condition, body);
    return std::static_pointer_cast<Stmt>(while_stmt);
}

antlrcpp::Any ASTBuilder::visitForStmt(LoxParser::ForStmtContext *ctx)
{
    // Convert the for syntax sugar to a while statement in the AST
    std::shared_ptr<Stmt> initializer;
    if (ctx->varDecl()) {
        initializer = visitVarDecl(ctx->varDecl()).as<std::shared_ptr<Stmt>>();
    } else if (ctx->forInit()) {
        auto expr = visit(ctx->forInit()->expr()).as<std::shared_ptr<Expr>>();
        initializer = std::make_shared<Expression>(expr);
    }

    std::shared_ptr<Expr> condition;
    if (ctx->forCond()) {
        condition = visit(ctx->forCond()->expr()).as<std::shared_ptr<Expr>>();
    } else {
        condition = std::make_shared<Literal>(true);
    }

    auto body = visit(ctx->statement()).as<std::shared_ptr<Stmt>>();

    if (ctx->forAdvance()) {
        auto expr = visit(ctx->forInit()->expr()).as<std::shared_ptr<Expr>>();
        auto advance = std::make_shared<Expression>(expr);

        body = std::make_shared<Block>(std::vector<std::shared_ptr<Stmt>>{body, advance});
    }

    body = std::make_shared<While>(condition, body);
    if (initializer) {
        body = std::make_shared<Block>(std::vector<std::shared_ptr<Stmt>>{initializer, body});
    }

    return std::static_pointer_cast<Stmt>(body);
}

antlrcpp::Any ASTBuilder::visitPrintStmt(LoxParser::PrintStmtContext *ctx)
{
    auto expr = visit(ctx->expr()).as<std::shared_ptr<Expr>>();
    auto print = std::make_shared<Print>(expr);
    return std::static_pointer_cast<Stmt>(print);
}

antlrcpp::Any ASTBuilder::visitReturnStmt(LoxParser::ReturnStmtContext *ctx)
{
    auto expr = visit(ctx->expr()).as<std::shared_ptr<Expr>>();
    auto ret = std::make_shared<Return>(ctx->RETURN()->getSymbol(), expr);
    return std::static_pointer_cast<Stmt>(ret);
}

antlrcpp::Any ASTBuilder::visitBlock(LoxParser::BlockContext *ctx)
{
    std::vector<std::shared_ptr<Stmt>> block_stmts;
    for (auto &d : ctx->declaration()) {
        block_stmts.push_back(visit(d).as<std::shared_ptr<Stmt>>());
    }

    auto block = std::make_shared<Block>(block_stmts);
    return std::static_pointer_cast<Stmt>(block);
}

antlrcpp::Any ASTBuilder::visitExprStmt(LoxParser::ExprStmtContext *ctx)
{
    auto expr = visit(ctx->expr()).as<std::shared_ptr<Expr>>();
    auto expr_stmt = std::make_shared<Expression>(expr);
    return std::static_pointer_cast<Stmt>(expr_stmt);
}

antlrcpp::Any ASTBuilder::visitUnary(LoxParser::UnaryContext *ctx)
{
    auto *op = ctx->MINUS() ? ctx->MINUS()->getSymbol() : ctx->BANG()->getSymbol();
    auto expr = visit(ctx->expr()).as<std::shared_ptr<Expr>>();
    auto unary = std::make_shared<Unary>(op, expr);
    return std::static_pointer_cast<Expr>(unary);
}

antlrcpp::Any ASTBuilder::visitCallExpr(LoxParser::CallExprContext *ctx)
{
    std::shared_ptr<Expr> expr = std::make_shared<Variable>(ctx->IDENTIFIER()->getSymbol());

    for (size_t i = 1; i < ctx->children.size(); ++i) {
        auto child = ctx->children[i];
        auto res = visit(child);
        // The terminal node/tokens will not return a value
        if (res.isNotNull()) {
            if (res.is<std::vector<std::shared_ptr<Expr>>>()) {
                auto paren = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[i - 1]);
                expr = std::make_shared<Call>(
                    expr, paren->getSymbol(), res.as<std::vector<std::shared_ptr<Expr>>>());
            } else if (res.is<antlr4::Token *>()) {
                auto *name = res.as<antlr4::Token *>();
                expr = std::make_shared<Get>(expr, name);
            }
        }
    }
    return std::static_pointer_cast<Expr>(expr);
}

antlrcpp::Any ASTBuilder::visitArguments(LoxParser::ArgumentsContext *ctx)
{
    std::vector<std::shared_ptr<Expr>> args;
    for (auto &e : ctx->expr()) {
        args.push_back(visit(e).as<std::shared_ptr<Expr>>());
    }
    return args;
}

antlrcpp::Any ASTBuilder::visitMemberIdentifier(LoxParser::MemberIdentifierContext *ctx)
{
    return ctx->IDENTIFIER()->getSymbol();
}

// void visit(const Binary &b) override;
antlrcpp::Any ASTBuilder::visitMult(LoxParser::MultContext *ctx)
{
    auto left = visit(ctx->children[0]).as<std::shared_ptr<Expr>>();
    auto right = visit(ctx->children[1]).as<std::shared_ptr<Expr>>();
    auto mult = std::make_shared<Binary>(left, ctx->STAR()->getSymbol(), right);
    return std::static_pointer_cast<Expr>(mult);
}

antlrcpp::Any ASTBuilder::visitDiv(LoxParser::DivContext *ctx)
{
    auto left = visit(ctx->children[0]).as<std::shared_ptr<Expr>>();
    auto right = visit(ctx->children[1]).as<std::shared_ptr<Expr>>();
    auto div = std::make_shared<Binary>(left, ctx->SLASH()->getSymbol(), right);
    return std::static_pointer_cast<Expr>(div);
}

antlrcpp::Any ASTBuilder::visitAddSub(LoxParser::AddSubContext *ctx)
{
    auto left = visit(ctx->children[0]).as<std::shared_ptr<Expr>>();
    auto right = visit(ctx->children[1]).as<std::shared_ptr<Expr>>();

    std::shared_ptr<Expr> expr;
    if (ctx->PLUS()) {
        expr = std::make_shared<Binary>(left, ctx->PLUS()->getSymbol(), right);

    } else {
        expr = std::make_shared<Binary>(left, ctx->MINUS()->getSymbol(), right);
    }
    return expr;
}

antlrcpp::Any ASTBuilder::visitComparison(LoxParser::ComparisonContext *ctx)
{
    auto left = visit(ctx->children[0]).as<std::shared_ptr<Expr>>();
    auto right = visit(ctx->children[1]).as<std::shared_ptr<Expr>>();

    std::shared_ptr<Expr> expr;
    if (ctx->LESS()) {
        expr = std::make_shared<Binary>(left, ctx->LESS()->getSymbol(), right);
    } else if (ctx->LESS_EQUAL()) {
        expr = std::make_shared<Binary>(left, ctx->LESS_EQUAL()->getSymbol(), right);
    } else if (ctx->GREATER()) {
        expr = std::make_shared<Binary>(left, ctx->GREATER()->getSymbol(), right);
    } else {
        expr = std::make_shared<Binary>(left, ctx->GREATER_EQUAL()->getSymbol(), right);
    }
    return expr;
}

antlrcpp::Any ASTBuilder::visitEquality(LoxParser::EqualityContext *ctx)
{
    auto left = visit(ctx->children[0]).as<std::shared_ptr<Expr>>();
    auto right = visit(ctx->children[1]).as<std::shared_ptr<Expr>>();

    std::shared_ptr<Expr> expr;
    if (ctx->NOT_EQUAL()) {
        expr = std::make_shared<Binary>(left, ctx->NOT_EQUAL()->getSymbol(), right);
    } else {
        expr = std::make_shared<Binary>(left, ctx->EQUAL_EQUAL()->getSymbol(), right);
    }
    return expr;
}

antlrcpp::Any ASTBuilder::visitLogicAnd(LoxParser::LogicAndContext *ctx)
{
    auto left = visit(ctx->children[0]).as<std::shared_ptr<Expr>>();
    auto right = visit(ctx->children[1]).as<std::shared_ptr<Expr>>();

    auto expr = std::make_shared<Binary>(left, ctx->AND()->getSymbol(), right);
    return std::static_pointer_cast<Expr>(expr);
}

antlrcpp::Any ASTBuilder::visitLogicOr(LoxParser::LogicOrContext *ctx)
{
    auto left = visit(ctx->children[0]).as<std::shared_ptr<Expr>>();
    auto right = visit(ctx->children[1]).as<std::shared_ptr<Expr>>();

    auto expr = std::make_shared<Binary>(left, ctx->OR()->getSymbol(), right);
    return std::static_pointer_cast<Expr>(expr);
}

antlrcpp::Any ASTBuilder::visitAssign(LoxParser::AssignContext *ctx)
{
    auto rhs = visit(ctx->expr()).as<std::shared_ptr<Expr>>();
    // If there's a call expr, we're setting a struct member
    std::shared_ptr<Expr> lhs;
    if (ctx->callExpr()) {
        auto obj = visitCallExpr(ctx->callExpr()).as<std::shared_ptr<Expr>>();
        lhs = std::make_shared<Set>(obj, ctx->IDENTIFIER()->getSymbol(), rhs);
    } else {
        lhs = std::make_shared<Variable>(ctx->IDENTIFIER()->getSymbol());
    }
    return lhs;
}

antlrcpp::Any ASTBuilder::visitParens(LoxParser::ParensContext *ctx)
{
    auto expr = visit(ctx->expr()).as<std::shared_ptr<Expr>>();
    auto group = std::make_shared<Grouping>(expr);
    return std::static_pointer_cast<Expr>(group);
}

antlrcpp::Any ASTBuilder::visitPrimary(LoxParser::PrimaryContext *ctx)
{
    std::shared_ptr<Expr> expr;
    if (ctx->IDENTIFIER()) {
        expr = std::make_shared<Variable>(ctx->IDENTIFIER()->getSymbol());
    } else if (ctx->NUMBER()) {
        expr = std::make_shared<Literal>(std::stof(ctx->NUMBER()->getText()));
    } else if (ctx->STRING()) {
        expr = std::make_shared<Literal>(ctx->NUMBER()->getText());
    } else if (ctx->TRUE()) {
        expr = std::make_shared<Literal>(true);
    } else if (ctx->FALSE()) {
        expr = std::make_shared<Literal>(false);
    } else if (ctx->NIL()) {
        expr = std::make_shared<Literal>(std::any());
    }
    return expr;
}
