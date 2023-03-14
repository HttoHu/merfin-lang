#pragma once
#include <iostream>
#include <llvm/IR/IRBuilder.h>
#include <vector>
#include <memory>
#include "ast.h"
namespace Parser
{
    using std::pair;
    using std::unique_ptr;
    using std::vector;
    class VarDecl : public AstNode
    {
    public:
        VarDecl() : AstNode(VAR_DECL) {}
        Value *codegen() const override;
        void push_var(const std::string &name, unique_ptr<AstNode> &&ast)
        {
            units.push_back({name, std::move(ast)});
        }

    private:
        vector<pair<std::string, unique_ptr<AstNode>>> units;
    };
    unique_ptr<AstNode> parse_var_decl();
    // to alloc space to local-variables, parameters ... and register the basic information to symbol table
    llvm::AllocaInst *create_var_alloc(llvm::Function *func, llvm::Type *type, const std::string &var_name);
}