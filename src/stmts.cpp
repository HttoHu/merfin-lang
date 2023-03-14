#include "../includes/stmts.h"
#include "../includes/env.h"
#include "../includes/symbol.h"

namespace Parser
{
    std::unique_ptr<AstNode> parse_if_ast();
    std::unique_ptr<AstNode> parse_expr();
    unique_ptr<AstNode> parse_var_decl();
    std::unique_ptr<AstNode> parse_while_ast();
    std::unique_ptr<AstNode> parse_for_ast();
    std::unique_ptr<AstNode> parse_break_continue();

    std::unique_ptr<AstNode> parse_stmts()
    {
        std::unique_ptr<AstNode> ret_node;
        switch (env.cur_tag())
        {
        case HLex::IF:
            return parse_if_ast();
        case HLex::LET:
            ret_node = parse_var_decl();
            break;
        case HLex::WHILE:
            return parse_while_ast();
        case HLex::FOR:
            return parse_for_ast();
        case HLex::BREAK:
        case HLex::CONTINUE:
            ret_node = parse_break_continue();
            break;
        case HLex::RETURN:
        {
            env.match(HLex::RETURN);
            ret_node = std::make_unique<UnaryOp>(HLex::RETURN, parse_expr());
            break;
        }
        default:
            ret_node = parse_expr();
        }
        env.match(HLex::SEMI);
        return ret_node;
    }
    std::vector<unique_ptr<AstNode>> parse_block()
    {
        Sym::Word::new_block();

        std::vector<unique_ptr<AstNode>> ret;
        if (env.cur_tag() != HLex::BEGIN)
        {
            ret.push_back(parse_stmts());
            return ret;
        }
        env.match(HLex::BEGIN);
        while (env.cur_tag() != HLex::END)
        {
            ret.push_back(parse_stmts());
        }

        Sym::Word::end_block();
        env.match(HLex::END);
        return ret;
    }
}