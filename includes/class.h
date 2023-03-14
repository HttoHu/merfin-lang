#pragma once
#include <memory>
#include <vector>
#include <map>
#include "../includes/types.h"
#include "../includes/ast.h"
namespace Parser
{
    class UserType
    {
    public:
        UserType() {}

        bool contains(const std::string &name);
        BasicType *member_type(const std::string &name);
        void add_member(const std::string &mem_name,unique_ptr<AstNode> init_val);

    private:
        std::map<std::string, BasicType *> type_tab;
        std::map<std::string, unique_ptr<AstNode>> init_val_tab;
    };
    std::unique_ptr<UserType> parse_class();

};