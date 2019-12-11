import sys
import os
import subprocess

def define_ast(header, cpp, file_name, base_name, types):
    header.write("#pragma once\n#include <any>\n#include \"token.h\"\n")

    header.write("struct Visitor;\n")

    header.write("struct {} {{\n".format(base_name))
    header.write("virtual void accept(Visitor &v) const = 0;\n")
    header.write("};\n")

    cpp.write("#include \"{}.h\"\n".format(file_name))

    for expr, args in types.items():
        header.write("struct {} : public {} {{\n".format(expr, base_name))
        for a in args:
            header.write("{};".format(a))
        header.write("{}({});\n".format(expr, ",".join(args)))
        header.write("void accept(Visitor &v) const override;")
        header.write("};\n")

        cpp.write("{}::{}({}) : ".format(expr, expr, ",".join(args)))
        for a in args:
            name = a.split()[1]
            cpp.write("{}({})".format(name, name))
            if a != args[-1]:
                cpp.write(",")            
        cpp.write("{}\n")

        cpp.write("void {}::accept(Visitor &v) const {{ v.visit(*this); }}".format(expr))

    header.write("struct Visitor {\n")
    for expr, args in types.items():
        header.write("virtual void visit(const {} &) = 0;\n".format(expr))
    header.write("};\n")

if len(sys.argv) != 2:
    print("Usage: gen_expr.py <output>")
    sys.exit(1)

with open(sys.argv[1] + ".h", "w") as header, open(sys.argv[1] + ".cpp", "w") as cpp:
    define_ast(header, cpp, sys.argv[1], "Expr",
        {
            "Binary": ["std::shared_ptr<Expr> left", "Token op", "std::shared_ptr<Expr> right"],
            "Grouping": ["std::shared_ptr<Expr> expr"],
            "Literal": ["std::any value"],
            "Unary": ["Token op", "std::shared_ptr<Expr> expr"]
        })

subprocess.run(["clang-format.exe", "-i", sys.argv[1] + ".h", sys.argv[1] + ".cpp"])

