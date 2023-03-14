#include "../includes/class.h"
#include "../includes/env.h"
#include "../includes/lexer.h"
#include "class.h"

namespace Parser
{
    BasicType *UserType::member_type(const std::string &name)
    {
        auto it = type_tab.find(name);
        if (it == type_tab.end())
            throw SyntaxError("class unknown member " + name);
        return it->second;
    }

    void UserType::add_member(const std::string &mem_name, unique_ptr<AstNode> init_val)
    {
        type_tab.insert({mem_name, init_val->get_type()});
        init_val_tab.insert({mem_name, std::move(init_val)});
    }

    std::unique_ptr<UserType> parse_class()
    {
        std::unique_ptr<UserType> ret = std::make_unique<UserType>();
        env.match(HLex::CLASS);
        auto class_name = env.cur_token().val;
        env.match(HLex::SYMBOL);
        env.match(HLex::BEGIN);

        while (env.cur_tag() != HLex::END)
        {
            env.match(HLex::LET);
            auto name = env.cur_token().val;
            env.match(HLex::SYMBOL);
            env.match(HLex::ASSIGN);
            auto init_val = parse_expr();
            env.match(HLex::SEMI);
            ret->add_member(name, std::move(init_val));
        }
        env.match(HLex::SEMI);
        return ret;
    }
    
}