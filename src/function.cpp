#include "../includes/function.h"
#include "../includes/expr.h"
#include "../includes/env.h"
#include "../includes/symbol.h"
#include "../includes/tools/guard.h"
#include "../includes/var_decl.h"
#include <llvm/IR/Verifier.h>
namespace Parser
{
    // decls;
    std::unique_ptr<AstNode> parse_stmts();
    // end_decls
    Function *Function::find_function(const std::string &func_name)
    {
        if (env.func_table.count(func_name))
            return env.func_table[func_name];
        return nullptr;
    }
    void Function::set_function(const std::string &name, Function *func)
    {
        env.func_table.insert({name, func});
    }
    FunctionType parse_function_type()
    {
        FunctionType ret;
        env.match(HLex::FUNCTION);
        auto ret_ty_name = env.cur_token().val;
        env.next_token();
        auto ret_type = BasicType::find_type(ret_ty_name);

        ret.func_name = env.cur_token().val;
        ret.ret_type = ret_type;

        env.match(HLex::SYMBOL);

        env.match(HLex::LPAR);
        while (env.cur_tag() != HLex::RPAR)
        {
            auto cur_param_type = BasicType::find_type(env.next_token().val);
            auto cur_param_name = env.cur_token().val;
            ret.param_types.push_back(cur_param_type);

            ret.param_names.push_back(cur_param_name);

            Sym::Word::set_symbol(cur_param_name, new Sym::VarWord(cur_param_type));

            env.match(HLex::SYMBOL);
            if (env.cur_tag() == HLex::COMMA)
                env.match(HLex::COMMA);
        }

        Sym::Word::set_gsymbol(ret.func_name, new Sym::FuncWord(ret.ret_type));
        env.match(HLex::RPAR);
        return ret;
    }
    Function *parse_function()
    {
        Tools::Guarder<void()> guard([]()
                                     { Sym::Word::new_block(); },
                                     []()
                                     { Sym::Word::end_block(); });
        auto func_type = parse_function_type();
        auto func_name = func_type.func_name;

        Function *func = Function::find_function(func_name);
        if (func)
        {
            if (env.cur_tag() == HLex::SEMI)
            {
                env.match(HLex::SEMI);
                return func;
            }
            throw SyntaxError("funciton " + func_name + " redefined!");
        }
        func = new Function();
        Function::set_function(func_type.func_name, func);

        func->func_type = func_type;
        func_type.get_func();

        if (env.cur_tag() == HLex::SEMI)
        {
            env.match(HLex::SEMI);
            return func;
        }
        env.match(HLex::BEGIN);
        while (env.cur_tag() != HLex::END)
        {
            func->stmts.push_back(parse_stmts());
        }
        env.match(HLex::END);
        func->defined = true;
        return func;
    }

    llvm::Function *FunctionType::get_func()
    {
        std::vector<llvm::Type *> types;
        for (int i = 0; i < param_types.size(); i++)
            types.push_back(param_types[i]->to_llvm_type());
        llvm::FunctionType *ft = llvm::FunctionType::get(ret_type->to_llvm_type(), types, false);
        llvm::Function *func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, func_name, env.the_module.get());

        int idx = 0;
        for (auto &arg : func->args())
            arg.setName(param_names[idx++]);
        return func;
    }

    llvm::Function *Function::gen_func()
    {
        llvm::Function *func = env.the_module->getFunction(func_type.func_name);
        if (!func)
            func = func_type.get_func();
        if (!func)
            return nullptr;

        env.local_var_tab.clear();
        env.cur_func = func;
        llvm::BasicBlock *bb = llvm::BasicBlock::Create(*env.the_context, "entry", func);
        env.ir_builder->SetInsertPoint(bb);
        for (auto &arg : func->args())
        {
            // create var alloc
            auto llvm_alloc = create_var_alloc(func, arg.getType(), std::string(arg.getName()));
            // store the init value
            env.ir_builder->CreateStore(&arg, llvm_alloc);

            env.local_var_tab[std::string(arg.getName())] = llvm_alloc;
        }

        // generate blocks
        for (auto &stmt : stmts)
            stmt->codegen();
        llvm::verifyFunction(*func);
        env.cur_func = nullptr;
        return func;
    }
}