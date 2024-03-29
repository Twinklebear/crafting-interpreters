#include "interpreter.h"
#include <iostream>
#include "LoxParser.h"
#include "lox_callable.h"
#include "lox_class.h"
#include "util.h"

using namespace loxgrammar;

InterpreterError::InterpreterError(const antlr4::Token *t, const std::string &msg)
    : token(t), message(msg)
{
}

ReturnControlFlow::ReturnControlFlow(const std::any &value) : value(value) {}

void Interpreter::evaluate(const std::vector<std::shared_ptr<Stmt>> &statements)
{
    result = std::any();
    try {
        for (const auto &st : statements) {
            st->accept(*this);
            result = std::any();
        }
    } catch (const InterpreterError &e) {
        error(e.token, e.message);
    }
}

const std::any &Interpreter::evaluate(const Expr &expr)
{
    result = std::any();
    expr.accept(*this);
    return result;
}

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

void Interpreter::visit(const Grouping &g)
{
    result = evaluate(*g.expr);
}

void Interpreter::visit(const Literal &l)
{
    result = l.value;
}

void Interpreter::visit(const Unary &u)
{
    std::any right = evaluate(*u.expr);
    switch (u.op->getType()) {
    case LoxParser::MINUS:
        check_type(right, {float_id}, u.op);
        result = -std::any_cast<float>(right);
        break;
    case LoxParser::BANG:
        result = !is_true(right);
        break;
    default:
        break;
    }
}

void Interpreter::visit(const Binary &b)
{
    std::any left = evaluate(*b.left);
    std::any right = evaluate(*b.right);

    switch (b.op->getType()) {
    case LoxParser::PLUS:
        check_type(right, {float_id, string_id}, b.op);
        check_type(left, {float_id, string_id}, b.op);
        if (left.type() == typeid(float) && right.type() == typeid(float)) {
            result = std::any_cast<float>(left) + std::any_cast<float>(right);
        } else if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
            result = std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
        } else {
            // We know one is a string and one is a float
            if (left.type() == typeid(float)) {
                result = std::to_string(std::any_cast<float>(left)) +
                         std::any_cast<std::string>(right);
            } else {
                result = std::any_cast<std::string>(left) +
                         std::to_string(std::any_cast<float>(right));
            }
        }
        break;
    case LoxParser::MINUS:
        check_same_type(left, right, b.op);
        check_type(left, {float_id, string_id}, b.op);
        result = std::any_cast<float>(left) - std::any_cast<float>(right);
        break;
    case LoxParser::SLASH:
        check_same_type(left, right, b.op);
        check_type(left, {float_id}, b.op);
        if (std::any_cast<float>(right) == 0.f) {
            throw InterpreterError(b.op, "Division by 0");
        }
        result = std::any_cast<float>(left) / std::any_cast<float>(right);
        break;
    case LoxParser::STAR:
        check_same_type(left, right, b.op);
        check_type(left, {float_id}, b.op);
        result = std::any_cast<float>(left) * std::any_cast<float>(right);
        break;
    case LoxParser::NOT_EQUAL:
        result = !is_equal(left, right);
        break;
    case LoxParser::EQUAL_EQUAL:
        result = is_equal(left, right);
        break;
    case LoxParser::GREATER:
        check_same_type(left, right, b.op);
        check_type(left, {float_id}, b.op);
        result = std::any_cast<float>(left) > std::any_cast<float>(right);
        break;
    case LoxParser::GREATER_EQUAL:
        check_same_type(left, right, b.op);
        check_type(left, {float_id}, b.op);
        result = std::any_cast<float>(left) >= std::any_cast<float>(right);
        break;
    case LoxParser::LESS:
        check_same_type(left, right, b.op);
        check_type(left, {float_id}, b.op);
        result = std::any_cast<float>(left) < std::any_cast<float>(right);
        break;
    case LoxParser::LESS_EQUAL:
        check_same_type(left, right, b.op);
        check_type(left, {float_id}, b.op);
        result = std::any_cast<float>(left) <= std::any_cast<float>(right);
        break;
    default:
        break;
    }
}

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

void Interpreter::visit(const Logical &l)
{
    result = evaluate(*l.left);

    if (l.op->getType() == LoxParser::OR) {
        if (is_true(result)) {
            return;
        }
    } else if (!is_true(result)) {
        return;
    }

    result = evaluate(*l.right);
}

void Interpreter::visit(const Variable &v)
{
    try {
        result = lookup_variable(v.name, v);
    } catch (const std::runtime_error &) {
        throw InterpreterError(v.name, "Undefined variable");
    }
}

void Interpreter::visit(const Assign &a)
{
    result = evaluate(*a.value);
    try {
        auto fnd = locals.find(&a);
        if (fnd != locals.end()) {
            environment->assign_at(fnd->second, a.name->getText(), result);
        } else {
            globals->assign(a.name->getText(), result);
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

void Interpreter::visit(const Block &b)
{
    auto env = std::make_shared<Environment>(environment);
    execute_block(b.statements, env);
    result = std::any();
}

void Interpreter::visit(const Expression &e)
{
    evaluate(*e.expr);
    result = std::any();
}

void Interpreter::visit(const If &f)
{
    if (is_true(evaluate(*f.condition))) {
        evaluate({f.then_branch});
    } else if (f.else_branch) {
        evaluate({f.else_branch});
    }
    result = std::any();
}

void Interpreter::visit(const While &w)
{
    while (is_true(evaluate(*w.condition))) {
        evaluate({w.body});
    }
    result = std::any();
}

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
            std::cerr << "[error]: Unsupported val type!?\n";
        }
    } else {
        std::cout << "nil";
    }
    result = std::any();
}

void Interpreter::visit(const Var &v)
{
    std::any initializer;
    if (v.initializer) {
        initializer = evaluate(*v.initializer);
    }

    environment->define(v.token->getText(), initializer);

    result = std::any();
}

void Interpreter::visit(const Function &f)
{
    // Now we will create and add a callable to the globals
    environment->define(
        f.name->getText(),
        std::shared_ptr<LoxCallable>(std::make_shared<LoxFunction>(f, environment)));
    result = std::any();
}

void Interpreter::visit(const Return &r)
{
    std::any return_result;
    if (r.value) {
        return_result = evaluate(*r.value);
    }
    throw std::make_shared<ReturnControlFlow>(return_result);
}

void Interpreter::visit(const Class &c)
{
    environment->define(c.name->getText(), std::any());
    auto lox_class = std::make_shared<LoxClass>(c.name->getText());
    environment->assign(c.name->getText(), lox_class);
}

void Interpreter::execute_block(const std::vector<std::shared_ptr<Stmt>> &statements,
                                std::shared_ptr<Environment> &env)
{
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
}

void Interpreter::resolve(const Expr &expr, size_t depth)
{
    locals[&expr] = depth;
}

void Interpreter::check_type(const std::any &val,
                             const std::vector<std::type_index> &valid_types,
                             const antlr4::Token *token)
{
    for (const auto &t : valid_types) {
        if (std::type_index(val.type()) == t) {
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
    throw InterpreterError(token, error_msg);
}

void Interpreter::check_same_type(const std::any &a,
                                  const std::any &b,
                                  const antlr4::Token *token) const
{
    if (a.type() != b.type()) {
        throw InterpreterError(
            token, "Expected " + pretty_type_name(a) + " but got " + pretty_type_name(b));
    }
}

bool Interpreter::is_true(const std::any &x) const
{
    const auto ty = std::type_index(x.type());
    if (ty == nil_id) {
        return false;
    }
    if (ty == bool_id) {
        return std::any_cast<bool>(x);
    }
    if (ty == float_id) {
        return std::any_cast<float>(x) != 0.f;
    }
    // All strings are "true"
    return true;
}

bool Interpreter::is_equal(const std::any &a, const std::any &b) const
{
    // Comparing objects of different types is always false
    if (a.type() != b.type()) {
        return false;
    }

    const auto a_ty = std::type_index(a.type());
    if (a_ty == nil_id) {
        return true;
    }
    if (a_ty == float_id) {
        return std::any_cast<float>(a) == std::any_cast<float>(b);
    }
    if (a_ty == string_id) {
        return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
    }
    return std::any_cast<bool>(a) == std::any_cast<bool>(b);
}

std::any Interpreter::lookup_variable(const antlr4::Token *token, const Expr &expr) const
{
    auto fnd = locals.find(&expr);
    if (fnd != locals.end()) {
        return environment->get_at(fnd->second, token->getText());
    } else {
        return globals->get(token->getText());
    }
}
