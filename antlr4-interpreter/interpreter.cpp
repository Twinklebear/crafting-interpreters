#include "interpreter.h"
#include <iostream>
#include "lox_callable.h"
#include "lox_class.h"
#include "util.h"

InterpreterError::InterpreterError(const antlr4::Token *t, const std::string &msg)
    : token(t), message(msg)
{
}

ReturnControlFlow::ReturnControlFlow(const antlrcpp::Any &value) : value(value) {}

Interpreter::Interpreter()
    : float_id(std::type_index(typeid(float))),
      string_id(std::type_index(typeid(StringPtr))),
      bool_id(std::type_index(typeid(bool))),
      nil_id(std::type_index(typeid(void))),
      callable_id(std::type_index(typeid(std::shared_ptr<LoxCallable>)))
{
    type_names[float_id] = pretty_type_name(typeid(float));
    type_names[string_id] = pretty_type_name(typeid(StringPtr));
    type_names[bool_id] = pretty_type_name(typeid(bool));
    type_names[nil_id] = pretty_type_name(typeid(void));

    // Populate the global environment with native functions
    globals->define("clock", std::shared_ptr<LoxCallable>(std::make_shared<Clock>()));
    globals->define("_ci_test_add",
                    std::shared_ptr<LoxCallable>(std::make_shared<CITestAdd>()));
}

void Interpreter::resolve(const antlr4::ParserRuleContext *node, size_t depth)
{
    locals[node] = depth;
}

antlrcpp::Any Interpreter::visitPrimary(LoxParser::PrimaryContext *ctx)
{
    // If we have an identifier it's a variable primary expression, if not
    // it's a literal value
    if (ctx->IDENTIFIER()) {
        try {
            return lookup_variable(ctx->IDENTIFIER()->getSymbol(), ctx);
        } catch (const std::runtime_error &) {
            throw InterpreterError(ctx->IDENTIFIER()->getSymbol(), "Undefined variable");
        }
    } else if (ctx->NUMBER()) {
        return std::stof(ctx->NUMBER()->getText());
    } else if (ctx->STRING()) {
        // Remove the opening and closing quotes from the string
        auto txt = ctx->STRING()->getText();
        txt = txt.substr(1, txt.length() - 2);
        return std::make_shared<std::string>(txt);
    } else if (ctx->getText() == "true") {
        return true;
    } else if (ctx->getText() == "false") {
        return false;
    }
    return antlrcpp::Any();
}

antlrcpp::Any Interpreter::visitUnary(LoxParser::UnaryContext *ctx)
{
    auto rhs = visit(ctx->expr());
    if (ctx->MINUS()) {
        check_type(rhs, {float_id}, ctx->MINUS()->getSymbol());
        return -rhs.as<float>();
    }
    return !is_true(rhs);
}

antlrcpp::Any Interpreter::visitDiv(LoxParser::DivContext *ctx)
{
    auto lhs = visit(ctx->expr(0));
    auto rhs = visit(ctx->expr(1));

    check_same_type(lhs, rhs, ctx->getStart());
    check_type(lhs, {float_id}, ctx->getStart());
    if (rhs.as<float>() == 0.f) {
        throw InterpreterError(ctx->getStart(), "Division by 0");
    }
    return lhs.as<float>() / rhs.as<float>();
}

antlrcpp::Any Interpreter::visitMult(LoxParser::MultContext *ctx)
{
    auto lhs = visit(ctx->expr(0));
    auto rhs = visit(ctx->expr(1));
    check_same_type(lhs, rhs, ctx->getStart());
    check_type(lhs, {float_id}, ctx->getStart());
    return lhs.as<float>() * rhs.as<float>();
}

antlrcpp::Any Interpreter::visitAddSub(LoxParser::AddSubContext *ctx)
{
    auto lhs = visit(ctx->expr(0));
    auto rhs = visit(ctx->expr(1));

    // Addition
    if (ctx->PLUS()) {
        check_type(lhs, {float_id, string_id}, ctx->PLUS()->getSymbol());
        check_type(rhs, {float_id, string_id}, ctx->PLUS()->getSymbol());

        if (lhs.is<float>() && rhs.is<float>()) {
            return lhs.as<float>() + rhs.as<float>();
        } else if (lhs.is<StringPtr>() && rhs.is<StringPtr>()) {
            auto l = lhs.as<StringPtr>();
            auto r = rhs.as<StringPtr>();
            return std::make_shared<std::string>(*l + *r);
        } else {
            // We know one is a string and one is a float
            if (lhs.is<float>()) {
                return std::make_shared<std::string>(std::to_string(lhs.as<float>()) +
                                                     *rhs.as<StringPtr>());
            } else {
                return std::make_shared<std::string>(*lhs.as<StringPtr>() +
                                                     std::to_string(rhs.as<float>()));
            }
        }
    }

    // Subtraction
    check_same_type(lhs, rhs, ctx->MINUS()->getSymbol());
    check_type(rhs, {float_id}, ctx->PLUS()->getSymbol());
    return lhs.as<float>() - rhs.as<float>();
}

antlrcpp::Any Interpreter::visitComparison(LoxParser::ComparisonContext *ctx)
{
    auto lhs = visit(ctx->expr(0));
    auto rhs = visit(ctx->expr(1));
    check_same_type(lhs, rhs, ctx->getStart());
    check_type(lhs, {float_id}, ctx->getStart());

    if (ctx->GREATER()) {
        return lhs.as<float>() > rhs.as<float>();
    }
    if (ctx->GREATER_EQUAL()) {
        return lhs.as<float>() >= rhs.as<float>();
    }
    if (ctx->LESS()) {
        return lhs.as<float>() < rhs.as<float>();
    }
    // Must be <=
    return lhs.as<float>() <= rhs.as<float>();
}
antlrcpp::Any Interpreter::visitEquality(LoxParser::EqualityContext *ctx)
{
    auto lhs = visit(ctx->expr(0));
    auto rhs = visit(ctx->expr(1));
    if (ctx->NOT_EQUAL()) {
        return !is_equal(lhs, rhs);
    }
    // Must be equals comparsion
    return is_equal(lhs, rhs);
}

antlrcpp::Any Interpreter::visitCallExpr(LoxParser::CallExprContext *ctx)
{
    // We may have a list of call expressions to evaluate from the left to the right
    // But here we may also have a mix like:
    // thing().hello(a,b).bye()
    // If we just iterate through them in order, we lose the ordering of the . vs ()
    // access
    // This can be resolved with another visitor that traverses them and builds up
    // a list in order for the call to flatten it
    // NOTE: might also need to be careful that we don't reprocess the left most
    // side of the identifier/etc. list since that's the function/object
    // we're initially calling. Or maybe it's fine actually, and just
    // build up a list and we pair things up iterating through it after
    // flattening it down with the visitor.
    // Or maybe it doesn't even need a new visitor and I can just iterate
    // through the ctx->children list and check what I've got.
    // Note: the current interpreter I also don't think supports some kind of
    // chained call operation so it's not a big deal at the moment.

    // Evaluate the sequence of function calls and struct member accesses from left to right in
    // a chain. Subsequent calls are applied to the result of the previous expression.
    antlrcpp::Any result = lookup_variable(ctx->IDENTIFIER(0)->getSymbol(), ctx);
    for (size_t i = 1; i < ctx->children.size(); ++i) {
        std::cout << "Call ctx child[" << i << "] = " << ctx->children[i]->toString() << "\n";
        // Passing arguments to the function call
        if (ctx->children[i]->getText() == "(") {
            std::shared_ptr<LoxCallable> fcn;
            if (result.is<std::shared_ptr<LoxCallable>>()) {
                fcn = result.as<std::shared_ptr<LoxCallable>>();
            } else if (result.is<std::shared_ptr<LoxClass>>()) {
                fcn = std::dynamic_pointer_cast<LoxCallable>(
                    result.as<std::shared_ptr<LoxClass>>());
            } else {
                throw InterpreterError(ctx->getStart(),
                                       "Only functions and classes are callable");
            }

            // Evaluate the set of expression arguments to pass to the function
            auto args_ctx = dynamic_cast<LoxParser::ArgumentsContext *>(ctx->children[++i]);
            std::vector<antlrcpp::Any> args;
            if (args_ctx) {
                for (auto &a : args_ctx->children) {
                    if (a->getText() == ",") {
                        continue;
                    }
                    args.push_back(visit(a));
                }
            }

            if (args.size() != fcn->arity()) {
                throw InterpreterError(args_ctx->getStart(),
                                       "Expected " + std::to_string(fcn->arity()) +
                                           " arguments but got " +
                                           std::to_string(args.size()));
            }
            result = fcn->call(*this, args);

            // Advance to the closing parenthesis
            ++i;
        } else {
            // Accessing a struct member
            auto ident = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children[++i]);
            if (result.is<std::shared_ptr<LoxInstance>>()) {
                result = result.as<std::shared_ptr<LoxInstance>>()->get(ident->getSymbol());
            } else {
                throw InterpreterError(
                    ident->getSymbol(),
                    "Attempt to get struct member on variable which is not a struct instance");
            }
        }
    }
    return result;
}

antlrcpp::Any Interpreter::visitLogicOr(LoxParser::LogicOrContext *ctx)
{
    auto result = visit(ctx->children[0]);
    if (is_true(result)) {
        return true;
    }
    return is_true(visit(ctx->children[1]));
}

antlrcpp::Any Interpreter::visitLogicAnd(LoxParser::LogicAndContext *ctx)
{
    auto result = visit(ctx->children[0]);
    if (!is_true(result)) {
        return false;
    }
    return is_true(visit(ctx->children[1]));
}

antlrcpp::Any Interpreter::visitAssign(LoxParser::AssignContext *ctx)
{
    auto val = visit(ctx->expr());

    // Setting a struct member
    if (ctx->callExpr()) {
        auto call_res = visit(ctx->callExpr());

        if (call_res.is<std::shared_ptr<LoxInstance>>()) {
            auto inst = call_res.as<std::shared_ptr<LoxInstance>>();
            inst->set(ctx->IDENTIFIER()->getSymbol(), val);
        } else {
            throw InterpreterError(
                ctx->IDENTIFIER()->getSymbol(),
                "Attempt to set struct member on variable which is not a struct instance");
        }
    } else {
        // Setting a regular variable
        try {
            auto fnd = locals.find(ctx);
            if (fnd != locals.end()) {
                std::cout << "Assigning local var " << ctx->IDENTIFIER()->getText()
                          << " at depth " << fnd->second << "\n";
                environment->assign_at(fnd->second, ctx->IDENTIFIER()->getText(), val);
            } else {
                std::cout << "Assigning global var\n";
                globals->assign(ctx->IDENTIFIER()->getText(), val);
            }
        } catch (const std::runtime_error &) {
            throw InterpreterError(ctx->IDENTIFIER()->getSymbol(), "Undefined variable");
        }
    }
    return antlrcpp::Any();
}

antlrcpp::Any Interpreter::visitBlock(LoxParser::BlockContext *ctx)
{
    EnvironmentContext env_ctx(&environment, std::make_shared<Environment>(environment));
    for (auto *d : ctx->declaration()) {
        visit(d);
    }
    return antlrcpp::Any();
}

antlrcpp::Any Interpreter::visitExprStmt(LoxParser::ExprStmtContext *ctx)
{
    visit(ctx->expr());
    return antlrcpp::Any();
}

antlrcpp::Any Interpreter::visitIfStmt(LoxParser::IfStmtContext *ctx)
{
    if (is_true(visit(ctx->expr()))) {
        visit(ctx->statement(0));
    } else if (ctx->statement().size() > 1) {
        visit(ctx->statement(1));
    }
    return antlrcpp::Any();
}

antlrcpp::Any Interpreter::visitWhileStmt(LoxParser::WhileStmtContext *ctx)
{
    while (is_true(visit(ctx->expr()))) {
        visit(ctx->statement());
    }
    return antlrcpp::Any();
}

antlrcpp::Any Interpreter::visitForStmt(LoxParser::ForStmtContext *ctx)
{
    EnvironmentContext env_context(&environment, std::make_shared<Environment>(environment));

    antlr4::tree::ParseTree *for_init = nullptr;
    if (ctx->varDecl()) {
        // If we declare a loop variable we need a new environment to hold it
        for_init = ctx->varDecl();
    } else {
        for_init = ctx->forInit();
    }

    // Run the for loop initialization, if there's one
    if (for_init) {
        std::cout << "Running for loop init\n";
        visit(for_init);
    }
    environment->print_vars();

    std::cout << "Running for loop\n";
    // Run the for loop
    while (!ctx->forCond() || is_true(visit(ctx->forCond()))) {
        try {
            visit(ctx->statement());
        } catch (const std::shared_ptr<ReturnControlFlow> &ret) {
            throw ret;
        }

        if (ctx->forAdvance()) {
            std::cout << __PRETTY_FUNCTION__ << "\n";
            visit(ctx->forAdvance());
            std::cout << __PRETTY_FUNCTION__ << "\n";
        }
    }
    return antlrcpp::Any();
}

antlrcpp::Any Interpreter::visitPrintStmt(LoxParser::PrintStmtContext *ctx)
{
    auto val = visit(ctx->expr());
    if (val.isNotNull()) {
        if (val.is<float>()) {
            std::cout << val.as<float>() << "\n";
        } else if (val.is<StringPtr>()) {
            std::cout << *val.as<StringPtr>() << "\n";
        } else if (val.is<bool>()) {
            std::cout << (val.as<bool>() ? "true" : "false") << "\n";
        } else if (val.is<std::shared_ptr<LoxCallable>>()) {
            std::cout << val.as<std::shared_ptr<LoxCallable>>()->to_string() << "\n";
        } else if (val.is<std::shared_ptr<LoxClass>>()) {
            std::cout << val.as<std::shared_ptr<LoxClass>>()->to_string() << "\n";
        } else if (val.is<std::shared_ptr<LoxInstance>>()) {
            std::cout << val.as<std::shared_ptr<LoxInstance>>()->to_string() << "\n";
        } else {
            std::cout << "[error]: Print unsupported value type!?\n";
        }
    } else {
        std::cout << "nil\n";
    }
    return antlrcpp::Any();
}

antlrcpp::Any Interpreter::visitVarDecl(LoxParser::VarDeclContext *ctx)
{
    antlrcpp::Any initializer;
    if (ctx->expr()) {
        std::cout << "Eval var initializer for " << ctx->IDENTIFIER()->getText() << "\n";
        initializer = visit(ctx->expr());
    }
    std::cout << "Defining var '" << ctx->IDENTIFIER()->getText() << "'\n";
    environment->define(ctx->IDENTIFIER()->getText(), initializer);
    return antlrcpp::Any();
}

antlrcpp::Any Interpreter::visitFunction(LoxParser::FunctionContext *ctx)
{
    // Now we will create and add a callable to the globals
    auto fn = std::shared_ptr<LoxCallable>(std::make_shared<LoxFunction>(ctx, environment));
    environment->define(ctx->IDENTIFIER()->getText(), fn);
    return antlrcpp::Any();
}

antlrcpp::Any Interpreter::visitReturnStmt(LoxParser::ReturnStmtContext *ctx)
{
    antlrcpp::Any return_result;
    if (ctx->expr()) {
        return_result = visit(ctx->expr());
    }
    throw std::make_shared<ReturnControlFlow>(return_result);
}

antlrcpp::Any Interpreter::visitClassDecl(LoxParser::ClassDeclContext *ctx)
{
    const std::string class_name = ctx->IDENTIFIER()->getText();
    environment->define(class_name, antlrcpp::Any());
    auto lox_class = std::make_shared<LoxClass>(class_name);
    environment->assign(class_name, lox_class);

    return antlrcpp::Any();
}

void Interpreter::check_type(const antlrcpp::Any &val,
                             const std::vector<std::type_index> &valid_types,
                             const antlr4::Token *t)
{
    for (const auto &t : valid_types) {
        if (t == float_id && val.is<float>()) {
            return;
        }
        if (t == string_id && val.is<StringPtr>()) {
            return;
        }
        if (t == bool_id && val.is<bool>()) {
            return;
        }
    }
    std::string error_msg = "Expected one of {";
    for (size_t i = 0; i < valid_types.size(); ++i) {
        error_msg += type_names[valid_types[i]];
        if (i + 1 < valid_types.size()) {
            error_msg += ", ";
        }
    }
    error_msg += "} but got " + pretty_type_name(val);
    throw InterpreterError(t, error_msg);
}

void Interpreter::check_same_type(const antlrcpp::Any &a,
                                  const antlrcpp::Any &b,
                                  const antlr4::Token *t) const
{
    // TODO: will need a compile time type checker anyways, but this is a bit annoying
    // compared to the C++17 std::any::type() method
    // Should be a better path to doing this via a separate type checking pass
    const bool type_match = (a.is<float>() && b.is<float>()) ||
                            (a.is<StringPtr>() && b.is<StringPtr>()) ||
                            (a.is<bool>() && b.is<bool>()) || (a.isNull() && b.isNull());

    if (!type_match) {
        throw InterpreterError(
            t, "Expected " + pretty_type_name(a) + " but got " + pretty_type_name(b));
    }
}

bool Interpreter::is_true(const antlrcpp::Any &x) const
{
    if (x.isNull()) {
        return false;
    }
    if (x.is<bool>()) {
        return x.as<bool>();
    }
    if (x.is<float>()) {
        return x.as<float>() != 0.f;
    }
    // All strings are "true"
    return true;
}

bool Interpreter::is_equal(const antlrcpp::Any &a, const antlrcpp::Any &b) const
{
    // Comparing objects of different types is always false
    const bool type_match = (a.is<float>() && b.is<float>()) ||
                            (a.is<StringPtr>() && b.is<StringPtr>()) ||
                            (a.is<bool>() && b.is<bool>()) || (a.isNull() && b.isNull());
    if (!type_match) {
        return false;
    }

    if (a.isNull()) {
        return true;
    }
    if (a.is<float>()) {
        return a.as<float>() == b.as<float>();
    }
    if (a.is<StringPtr>()) {
        return *a.as<StringPtr>() == *b.as<StringPtr>();
    }
    return a.as<bool>() == b.as<bool>();
}

antlrcpp::Any Interpreter::lookup_variable(const antlr4::Token *token,
                                           const antlr4::ParserRuleContext *node) const
{
    auto fnd = locals.find(node);
    if (fnd != locals.end()) {
        std::cout << "Lookup local var " << token->getText() << " at depth " << fnd->second
                  << "\n";
        return environment->get_at(fnd->second, token->getText());
    } else {
        std::cout << "looking in globals for '" << token->getText() << "'\n";
        return globals->get(token->getText());
    }
}
