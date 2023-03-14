#include "../includes/var_decl.h"
#include "../includes/symbol.h"
#include "../includes/env.h"
namespace Parser
{
    Value *VarDecl::codegen() const
    {
        for (auto &[var_name, ast_node] : units)
        {
            auto val = ast_node->codegen();
            auto llvm_alloc = create_var_alloc(env.cur_func, ast_node->get_type()->to_llvm_type(), var_name);
            env.ir_builder->CreateStore(val, llvm_alloc);
            env.local_var_tab[var_name] = llvm_alloc;
        }
        return nullptr;
    }

    llvm::AllocaInst *create_var_alloc(llvm::Function *func, llvm::Type *type, const std::string &var_name)
    {
        using llvm::IRBuilder;

        IRBuilder<> tmp_b(&func->getEntryBlock(), func->getEntryBlock().begin());

        auto alloc_node = tmp_b.CreateAlloca(type, nullptr, var_name);

        return alloc_node;
    }

    unique_ptr<AstNode> parse_var_decl()
    {
        env.match(HLex::LET);
        unique_ptr<VarDecl> ret = std::make_unique<VarDecl>();
        while (true)
        {
            std::string var_name = env.cur_token().val;
            env.match(HLex::SYMBOL);
            env.match(HLex::ASSIGN);
            auto expr = parse_expr();
            Sym::Word::set_symbol(var_name, new Sym::VarWord(expr->get_type()));
            ret->push_var(var_name, std::move(expr));

            if (env.cur_tag() == HLex::COMMA)
                env.match(HLex::COMMA);
            else
                break;
        }
        return ret;
    }
}