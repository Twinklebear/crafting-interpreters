import sys
import os

def define_ast(header, cpp, file_name, base_name, types):
    header.write("#pragma once\n#include <any>\n#include \"token.h\"\n")
    header.write("struct {} {{}};\n".format(base_name))

    cpp.write("#include \"{}.h\"\n".format(file_name))

    for expr, args in types.items():
        header.write("struct {} : public {} {{\n".format(expr, base_name))
        for a in args:
            header.write("{};".format(a))
        header.write("{}({});\n".format(expr, ",".join(args)))
        header.write("};\n")

        cpp.write("{}::{}({}) : ".format(expr, expr, ",".join(args)))
        for a in args:
            name = a.split()[1]
            cpp.write("{}({})".format(name, name))
            if a != args[-1]:
                cpp.write(",")            
        cpp.write("{}\n")

if len(sys.argv) != 2:
    print("Usage: gen_expr.py <output>")
    sys.exit(1)

with open(sys.argv[1] + ".h", "w") as header, open(sys.argv[1] + ".cpp", "w") as cpp:
    define_ast(header, cpp, sys.argv[1], "Expr",
        {
            "Binary": ["Expr left", "Token op", "Expr right"],
            "Grouping": ["Expr expr"],
            "Literal": ["std::any value"],
            "Unary": ["Token op", "Expr expr"]
        })


