#pragma once
#include <memory>
#include <string>
#include <vector>
#include "../includes/types.h"
#include "../includes/ast.h"
#include "../includes/expr.h"

namespace Parser
{
    class FunctionType
    {
    public:
        FunctionType() {}
        FunctionType(const std::string &name, const std::vector<BasicType *> &a, const std::vector<std::string> &b) : func_name(name), param_types(a), param_names(b) {}

        llvm::Function *get_func();

        std::string func_name;
        BasicType *ret_type;
        std::vector<BasicType *> param_types;
        std::vector<std::string> param_names;
    };
    class Function
    {
    public:
        static Function *find_function(const std::string &func_name);
        static void set_function(const std::string &, Function *);

        llvm::Function *gen_func();
        virtual void check_argument_type(const std::vector<unique_ptr<AstNode>> &args)
        {
            if (args.size() != func_type.param_types.size())
            {
                throw SyntaxError("call function but parameters' size don't match arguments!");
            }
            for (int i = 0; i < args.size(); i++)
            {
                if (args[i]->get_type() != func_type.param_types[i])
                    throw SyntaxError("params don't match arguments");
            }
        }

    public:
        FunctionType func_type;
        bool defined = false;
        std::vector<unique_ptr<AstNode>> stmts;
    };
    FunctionType parse_function_type();
    Function *parse_function();
}