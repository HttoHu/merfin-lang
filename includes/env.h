#pragma once
#include <memory>
#include <deque>

#include "../includes/lexer.h"
#include "../includes/error.h"
#include "../includes/function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
extern int cur_line;
class Env
{
public:

    // init env
    Env()
    {
        the_context = std::make_unique<llvm::LLVMContext>();
        ir_builder = std::make_unique<llvm::IRBuilder<>>(*the_context);
        the_module = std::make_unique<llvm::Module>("merdog5", *the_context);
    }

    // read a new file and generate tokens
    void load_file(const std::string &content, bool reset_cur_line = false)
    {
        if (reset_cur_line)
            cur_line = 1;
        auto vec = lexer.lex(content);
        tokens.insert(tokens.end(), vec.begin(), vec.end());
    }
    void match(HLex::Tag tag)
    {
        skip_newline();
        if (pos < tokens.size() && tokens[pos].tag == tag)
        {
            pos++;
            return;
        }
        throw SyntaxError(cur_line, "unexpected token " + HLex::Lexer::tag_to_str(tokens[pos].tag) + " expected " + HLex::Lexer::tag_to_str(tag));
    }
    HLex::Token cur_token()
    {
        skip_newline();
        if (pos < tokens.size())
            return tokens[pos];
        throw SyntaxError(cur_line, "unexpected end!");
    }
    HLex::Tag cur_tag()
    {
        return cur_token().tag;
    }

    // return current and move to next
    HLex::Token next_token()
    {
        auto cur = cur_token();
        skip_newline();
        pos++;
        return cur;
    }

public:
    std::unique_ptr<llvm::LLVMContext> the_context;
    std::unique_ptr<llvm::IRBuilder<>> ir_builder;
    std::unique_ptr<llvm::Module> the_module;
    std::map<std::string,llvm::AllocaInst*> local_var_tab;

    // to find the nearst break point 
    std::vector<llvm::BasicBlock*> break_stac;
    // to find the nearst continue point
    std::vector<llvm::BasicBlock*> continue_stac;

    HLex::Lexer lexer;
    int pos = 0;
    std::vector<HLex::Token> tokens;
    std::map<std::string, Parser::Function *>
        func_table;
    llvm::Function * cur_func;
private:
    void skip_newline()
    {
        while (pos < tokens.size() && tokens[pos].tag == HLex::NEWLINE)
            pos++, cur_line++;
    }
};
extern Env env;