#!/usr/bin/env python3
import sys
import os
import subprocess
import shutil

def define_ast(header, cpp, base_name, types):
    for expr, args in types.items():
        header.write("struct {};\n".format(expr))

    header.write("struct {} {{\n".format(base_name))

    header.write("struct Visitor {\n")
    for expr, args in types.items():
        header.write("virtual void visit(const {} &) = 0;\n".format(expr))
    header.write("virtual ~Visitor(){}")
    header.write("};\n")

    header.write("virtual void accept(Visitor &v) const = 0;\n")
    header.write("};\n")

    for expr, args in types.items():
        header.write("struct {} : {} {{\n".format(expr, base_name))
        for a in args:
            header.write("{};".format(a))
        header.write("{}({});\n".format(expr, ",".join(args)))
        header.write("void accept(Visitor &v) const override;")
        header.write("virtual ~{}(){{}}".format(expr))
        header.write("};\n")

        cpp.write("{}::{}({}) : ".format(expr, expr, ",".join(args)))
        for a in args:
            name = a.split()[1]
            cpp.write("{}({})".format(name, name))
            if a != args[-1]:
                cpp.write(",")            
        cpp.write("{}\n")

        cpp.write("void {}::accept(Visitor &v) const {{ v.visit(*this); }}".format(expr))

if len(sys.argv) != 2:
    print("Usage: gen_expr.py <output>")
    sys.exit(1)

with open(sys.argv[1] + ".h", "w") as header, open(sys.argv[1] + ".cpp", "w") as cpp:
    header.write("#pragma once\n#include <any>\n#include <vector>\n#include <memory>\n#include \"token.h\"\n")
    cpp.write("#include \"{}.h\"\n".format(sys.argv[1]))

    expressions = {
        "Assign": ["Token name", "std::shared_ptr<Expr> value"],
        "Binary": ["std::shared_ptr<Expr> left", "Token op", "std::shared_ptr<Expr> right"],
        "Call": ["std::shared_ptr<Expr> callee", "Token paren", "std::vector<std::shared_ptr<Expr>> args"],
        "Grouping": ["std::shared_ptr<Expr> expr"],
        "Literal": ["std::any value"],
        "Logical": ["std::shared_ptr<Expr> left", "Token op", "std::shared_ptr<Expr> right"],
        "Unary": ["Token op", "std::shared_ptr<Expr> expr"],
        "Variable": ["Token name"],
        "Get": ["std::shared_ptr<Expr> object", "Token name"],
        "Set": ["std::shared_ptr<Expr> object", "Token name", "std::shared_ptr<Expr> value"]
    }

    statements = {
        "Block": ["std::vector<std::shared_ptr<Stmt>> statements"],
        "Expression": ["std::shared_ptr<Expr> expr"],
        "Class": ["Token name", "std::vector<std::shared_ptr<Function>> methods"],
        "If": ["std::shared_ptr<Expr> condition", "std::shared_ptr<Stmt> then_branch",
            "std::shared_ptr<Stmt> else_branch"],
        "Print": ["std::shared_ptr<Expr> expr"],
        "Var": ["Token token", "std::shared_ptr<Expr> initializer"],
        "While": ["std::shared_ptr<Expr> condition", "std::shared_ptr<Stmt> body"],
        "Function": ["Token name", "std::vector<Token> params", "std::shared_ptr<Stmt> body"],
        "Return": ["Token keyword", "std::shared_ptr<Expr> value"]
    }

    define_ast(header, cpp, "Expr", expressions)
    define_ast(header, cpp, "Stmt", statements)

clang_format = shutil.which("clang-format") or os.getenv("CLANG_FORMAT")
if clang_format:
    subprocess.run([clang_format, "-i", sys.argv[1] + ".h", sys.argv[1] + ".cpp"])

