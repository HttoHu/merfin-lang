#pragma once
#include <memory>
#include <vector>
#include <map>
#include "../includes/types.h"
#include "../includes/ast.h"

namespace Mer
{
    class ClassType : public BasicType
    {
    public:
        ClassType() : BasicType(CLASS) {}
        llvm::Type *to_llvm_type() override;
        void add_member(const std::string &name, Parser::Node node);

        std::pair<int,BasicType*> find_member(const std::string &name);

        llvm::Value* construct();
    private:
        int cnt = 0;
        llvm::Type *llvm_type = nullptr;
        std::vector<std::pair<std::string, Parser::Node>> members;
        std::map<std::string, int> name_tab;
    };
}
namespace Parser
{
    Mer::ClassType *parse_class();

    class ClassConstructor final : public AstNode
    {
    public:
        ClassConstructor(Mer::ClassType *uty) : type(uty), AstNode(CLASS_CONS) {}
        llvm::Value *codegen() const override;
        BasicType *get_type() const override;

    private:
        Mer::ClassType *type;
    };
};
