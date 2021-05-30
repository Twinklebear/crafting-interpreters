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
      string_id(std::type_index(typeid(std::string))),
      bool_id(std::type_index(typeid(bool))),
      nil_id(std::type_index(typeid(void))),
      callable_id(std::type_index(typeid(std::shared_ptr<LoxCallable>)))
{
    type_names[float_id] = pretty_type_name(typeid(float));
    type_names[string_id] = pretty_type_name(typeid(std::string));
    type_names[bool_id] = pretty_type_name(typeid(bool));
    type_names[nil_id] = pretty_type_name(typeid(void));

    // Populate the global environment with native functions
    globals->define("clock", std::shared_ptr<LoxCallable>(std::make_shared<Clock>()));
    globals->define("_ci_test_add",
                    std::shared_ptr<LoxCallable>(std::make_shared<CITestAdd>()));
}

void Interpreter::resolve(const LoxParser::ExprContext *expr, size_t depth)
{
    locals[expr] = depth;
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
        return ctx->STRING()->getText();
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
        } else if (lhs.is<std::string>() && rhs.is<std::string>()) {
            return lhs.as<std::string>() + rhs.as<std::string>();
        } else {
            // We know one is a string and one is a float
            if (lhs.is<float>()) {
                return std::to_string(lhs.as<float>()) + rhs.as<std::string>();
            } else {
                return lhs.as<std::string>() + std::to_string(rhs.as<float>());
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

    
    /*
    void Interpreter::visit(const Call &c)
    {
        auto callee = evaluate(*c.callee);
        std::vector<std::any> args;
        for (const auto &e : c.args) {
            args.push_back(evaluate(*e));
        }

        std::shared_ptr<LoxCallable> fcn;
        if (callee.type() == typeid(std::shared_ptr<LoxCallable>)) {
            fcn = std::any_cast<std::shared_ptr<LoxCallable>>(callee);
        } else if (callee.type() == typeid(std::shared_ptr<LoxClass>)) {
            fcn = std::dynamic_pointer_cast<LoxCallable>(
                std::any_cast<std::shared_ptr<LoxClass>>(callee));
        } else {
            throw InterpreterError(c.paren, "Only functions and classes are callable");
        }

        if (args.size() != fcn->arity()) {
            throw InterpreterError(c.paren,
                                   "Expected " + std::to_string(fcn->arity()) +
                                       " arguments but got " + std::to_string(args.size()));
        }
        result = fcn->call(*this, args);
    }
    */
}

antlrcpp::Any Interpreter::visitLogicOr(LoxParser::LogicOrContext *ctx)
{
    /*
    // This would be visit
    result = evaluate(*l.left);

    if (l.op.type == TokenType::OR) {
        if (is_true(result)) {
            return true;
        }
    }

    // This would be visit
    result = evaluate(*l.right);
    */
}

antlrcpp::Any Interpreter::visitLogicAnd(LoxParser::LogicAndContext *ctx)
{
    /*
    // This would be visit
    result = evaluate(*l.left);

    if (l.op.type == TokenType::OR) {
        if (is_true(result)) {
            return;
        }
    } else if (!is_true(result)) {
        return;
    }

    // This would be visit
    result = evaluate(*l.right);
    return result;
    */
}

antlrcpp::Any Interpreter::visitAssign(LoxParser::AssignContext *ctx)
{
    /*

void Interpreter::visit(const Assign &a)
{
result = evaluate(*a.value);
try {
    auto fnd = locals.find(&a);
    if (fnd != locals.end()) {
        environment->assign_at(fnd->second, a.name.lexeme, result);
    } else {
        globals->assign(a.name.lexeme, result);
    }
} catch (const std::runtime_error &) {
    throw InterpreterError(a.name, "Undefined variable");
}
}

void Interpreter::visit(const Get &g)
{
auto obj = evaluate(*g.object);
if (obj.type() == typeid(std::shared_ptr<LoxInstance>)) {
    auto inst = std::any_cast<std::shared_ptr<LoxInstance>>(obj);
    result = inst->get(g.name);
}
}
*/
    /*
    void Interpreter::visit(const Set &s)
    {
        auto obj = evaluate(*s.object);
        try {
            auto inst = std::any_cast<std::shared_ptr<LoxInstance>>(obj);
            auto value = evaluate(*s.value);
            inst->set(s.name, value);
            result = value;
        } catch (const std::bad_any_cast &e) {
            throw InterpreterError(s.name, "Only instances have fields");
        }
    }
    */
}

antlrcpp::Any visitBlock(LoxParser::BlockContext *ctx)
{
    /*
    void Interpreter::visit(const Block &b)
    {
        auto env = std::make_shared<Environment>(environment);
        execute_block(b.statements, env);

        auto prev = environment;
        environment = env;
        try {
            evaluate(statements);
        } catch (const std::shared_ptr<ReturnControlFlow> &ret) {
            // Need to restore environments as we return out
            // TODO: Same would apply for break
            // TODO: Needs a better way to handle the environments with the call stack
            environment = prev;
            throw ret;
        }
        environment = prev;

        result = std::any();
    }
    */
}

antlrcpp::Any Interpreter::visitExprStmt(LoxParser::ExprStmtContext *ctx)
{
    // we do want to override b/c expressions shouldn't return a value
    /*
void Interpreter::visit(const Expression &e)
{
    evaluate(*e.expr);
    result = std::any();
}
*/
}

antlrcpp::Any Interpreter::visitIfStmt(LoxParser::IfStmtContext *ctx)
{
    /*
void Interpreter::visit(const If &f)
{
    if (is_true(evaluate(*f.condition))) {
        evaluate({f.then_branch});
    } else if (f.else_branch) {
        evaluate({f.else_branch});
    }
    result = std::any();
}
*/
}

antlrcpp::Any Interpreter::visitWhileStmt(LoxParser::WhileStmtContext *ctx)
{
    /*
void Interpreter::visit(const While &w)
{
    while (is_true(evaluate(*w.condition))) {
        evaluate({w.body});
    }
    result = std::any();
}
*/
}

antlrcpp::Any Interpreter::visitForStmt(LoxParser::ForStmtContext *ctx)
{
    // Now we do need to treat for separately, b/c we don't have the parser
    // convert it to a while
}

antlrcpp::Any Interpreter::visitPrintStmt(LoxParser::PrintStmtContext *ctx)
{
    /*
void Interpreter::visit(const Print &p)
{
    std::any val = evaluate(*p.expr);
    if (val.has_value()) {
        if (val.type() == typeid(float)) {
            std::cout << std::any_cast<float>(val) << "\n";
        } else if (val.type() == typeid(std::string)) {
            std::cout << std::any_cast<std::string>(val) << "\n";
        } else if (val.type() == typeid(bool)) {
            std::cout << (std::any_cast<bool>(val) ? "true" : "false") << "\n";
        } else if (val.type() == typeid(std::shared_ptr<LoxCallable>)) {
            std::cout << std::any_cast<std::shared_ptr<LoxCallable>>(val)->to_string() << "\n";
        } else if (val.type() == typeid(std::shared_ptr<LoxClass>)) {
            std::cout << std::any_cast<std::shared_ptr<LoxClass>>(val)->to_string() << "\n";
        } else if (val.type() == typeid(std::shared_ptr<LoxInstance>)) {
            std::cout << std::any_cast<std::shared_ptr<LoxInstance>>(val)->to_string() << "\n";
        } else {
            std::cout << "[error]: Unsupported val type!?\n";
        }
    } else {
        std::cout << "nil";
    }
    result = std::any();
}
*/
}

antlrcpp::Any Interpreter::visitVarDeclStmt(LoxParser::VarDeclStmtContext *ctx)
{
    /*
void Interpreter::visit(const Var &v)
{
    std::any initializer;
    if (v.initializer) {
        initializer = evaluate(*v.initializer);
    }

    environment->define(v.token.lexeme, initializer);

    result = std::any();
}
*/
}

antlrcpp::Any Interpreter::visitFunction(LoxParser::FunctionContext *ctx)
{
    /*
void Interpreter::visit(const Function &f)
{
    // Now we will create and add a callable to the globals
    environment->define(
        f.name.lexeme,
        std::shared_ptr<LoxCallable>(std::make_shared<LoxFunction>(f, environment)));
    result = std::any();
}
*/
}

antlrcpp::Any Interpreter::visitReturnStmt(LoxParser::ReturnStmtContext *ctx)
{
    /*
void Interpreter::visit(const Return &r)
{
    std::any return_result;
    if (r.value) {
        return_result = evaluate(*r.value);
    }
    throw std::make_shared<ReturnControlFlow>(return_result);
}
*/
}

antlrcpp::Any Interpreter::visitClassDecl(LoxParser::ClassDeclContext *ctx)
{
    /*
void Interpreter::visit(const Class &c)
{
    environment->define(c.name.lexeme, std::any());
    auto lox_class = std::make_shared<LoxClass>(c.name.lexeme);
    environment->assign(c.name.lexeme, lox_class);
}
*/
}

void Interpreter::check_type(const antlrcpp::Any &val,
                             const std::vector<std::type_index> &valid_types,
                             const antlr4::Token *t)
{
    for (const auto &t : valid_types) {
        if (t == float_id && val.is<float>()) {
            return;
        }
        if (t == string_id && val.is<std::string>()) {
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
                            (a.is<std::string>() && b.is<std::string>()) ||
                            (a.is<bool>() && b.is<bool>()) || (a.is<void>() && b.is<void>());

    if (!type_match) {
        throw InterpreterError(
            t, "Expected " + pretty_type_name(a) + " but got " + pretty_type_name(b));
    }
}

bool Interpreter::is_true(const antlrcpp::Any &x) const
{
    if (x.is<void>()) {
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
                            (a.is<std::string>() && b.is<std::string>()) ||
                            (a.is<bool>() && b.is<bool>()) || (a.is<void>() && b.is<void>());
    if (!type_match) {
        return false;
    }

    if (a.is<void>()) {
        return true;
    }
    if (a.is<float>()) {
        return a.as<float>() == b.as<float>();
    }
    if (a.is<std::string>()) {
        return a.as<std::string>() == b.as<std::string>();
    }
    return a.as<bool>() == b.as<bool>();
}

antlrcpp::Any Interpreter::lookup_variable(const antlr4::Token *token,
                                           const LoxParser::ExprContext *expr) const
{
    auto fnd = locals.find(expr);
    if (fnd != locals.end()) {
        return environment->get_at(fnd->second, token->getText());
    } else {
        return globals->get(token->getText());
    }
}
