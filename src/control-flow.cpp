#include "../includes/control-flow.h"
#include "../includes/env.h"
#include "../includes/tools/guard.h"
#include "../includes/stmts.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include <set>

namespace Parser
{

    unique_ptr<AstNode> parse_var_decl();
    // IfAst
    Value *IfElseAst::codegen() const
    {
        using llvm::BasicBlock;
        auto con_val = condition->codegen();
        // type cast
        con_val = env.ir_builder->CreateIntCast(con_val, llvm::Type::getInt32Ty(*env.the_context), true, "cond");
        if (!con_val)
            throw SyntaxError("condition gen failed!");
        Value *int0 = llvm::ConstantInt::get(*env.the_context, llvm::APInt(32, 0));
        con_val = env.ir_builder->CreateICmpNE(con_val,
                                               int0, "ifcond");
        auto llvm_func = env.ir_builder->GetInsertBlock()->getParent();

        BasicBlock *then_bb = BasicBlock::Create(*env.the_context, "then", llvm_func);
        BasicBlock *else_bb = BasicBlock::Create(*env.the_context, "else", llvm_func);
        BasicBlock *merge_bb = BasicBlock::Create(*env.the_context, "ifcont", llvm_func);

        env.ir_builder->CreateCondBr(con_val, then_bb, else_bb);

        env.ir_builder->SetInsertPoint(then_bb);
        for (auto &item : then_ast)
            item->codegen();
        env.ir_builder->CreateBr(merge_bb);

        // else - basic block
        // else_bb->insertInto(llvm_func);
        env.ir_builder->SetInsertPoint(else_bb);

        for (auto &item : else_ast)
            item->codegen();
        env.ir_builder->CreateBr(merge_bb);

        // merge basic block
        // merge_bb->insertInto(llvm_func);
        env.ir_builder->SetInsertPoint(merge_bb);

        return nullptr;
    }

    // WhileAst
    /*
        while(con)
        {
            exprs;
        }
        =>
        start:
            iffalse jmp end
            br true body, false merge
        body:
            exprs;

            br start
        merge:
    */
    Value *WhileAst::codegen() const
    {
        using llvm::BasicBlock;
        auto llvm_func = env.ir_builder->GetInsertBlock()->getParent();

        BasicBlock *start = BasicBlock::Create(*env.the_context, "start", llvm_func);
        env.ir_builder->CreateBr(start);
        env.ir_builder->SetInsertPoint(start);
        // store current continue point
        Tools::Guarder<void()> g([&]
                                 { env.continue_stac.push_back(start); },
                                 [&]
                                 {
                                     env.continue_stac.pop_back();
                                 });

        auto con_val = condition->codegen();
        con_val = env.ir_builder->CreateIntCast(con_val, llvm::Type::getInt32Ty(*env.the_context), true, "cond");
        con_val = env.ir_builder->CreateICmpNE(con_val, llvm::ConstantInt::get(*env.the_context, llvm::APInt(32, 0)), "ifcond");

        if (!con_val)
            throw SyntaxError("while cond gen failed!");

        BasicBlock *body_block = BasicBlock::Create(*env.the_context, "body", llvm_func);
        BasicBlock *merge_block = BasicBlock::Create(*env.the_context, "merge", llvm_func);
        // to store current break point.
        Tools::Guarder<void()> g2([&]
                                  { env.break_stac.push_back(merge_block); },
                                  [&]
                                  {
                                      env.break_stac.pop_back();
                                  });

        env.ir_builder->CreateCondBr(con_val, body_block, merge_block);
        env.ir_builder->SetInsertPoint(body_block);

        for (auto &stmt : body)
            stmt->codegen();
        env.ir_builder->CreateBr(start);
        env.ir_builder->SetInsertPoint(merge_block);
        return nullptr;
    }

    std::unique_ptr<AstNode> parse_if_ast()
    {
        env.match(HLex::IF);
        auto condition = parse_expr();
        vector<unique_ptr<AstNode>> then_block, else_block;
        if (env.cur_tag() == HLex::BEGIN)
            then_block = std::move(parse_block());
        else
            then_block.push_back(parse_stmts());
        if (env.cur_tag() == HLex::ELSE)
        {
            env.match(HLex::ELSE);
            if (env.cur_tag() == HLex::BEGIN)
                else_block = std::move(parse_block());
            else
                else_block.push_back(parse_stmts());
        }
        return std::make_unique<IfElseAst>(std::move(condition), std::move(then_block), std::move(else_block));
    }

    std::unique_ptr<AstNode> parse_while_ast()
    {
        env.match(HLex::WHILE);
        auto condition = parse_expr();
        auto body = parse_block();
        return std::make_unique<WhileAst>(std::move(condition), std::move(body));
    }

    std::unique_ptr<AstNode> parse_for_ast()
    {
        env.match(HLex::FOR);
        env.match(HLex::LPAR);
        unique_ptr<AstNode> var_decl, condition, step;
        var_decl = parse_var_decl();
        env.match(HLex::SEMI);
        condition = parse_expr();
        env.match(HLex::SEMI);
        step = parse_expr();
        env.match(HLex::RPAR);
        auto body = parse_block();

        return std::make_unique<ForAst>(std::move(var_decl), std::move(condition), std::move(step), std::move(body));
    }

    Value *ForAst::codegen() const
    {
        using llvm::BasicBlock;
        auto llvm_func = env.ir_builder->GetInsertBlock()->getParent();

        var_decl->codegen();

        BasicBlock *start = BasicBlock::Create(*env.the_context, "start", llvm_func);
        // store current continue point
        Tools::Guarder<void()> g([&]
                                 { env.continue_stac.push_back(start); },
                                 [&]
                                 {
                                     env.continue_stac.pop_back();
                                 });

        env.ir_builder->CreateBr(start);
        env.ir_builder->SetInsertPoint(start);

        auto con_val = condition->codegen();
        con_val = env.ir_builder->CreateIntCast(con_val, llvm::Type::getInt32Ty(*env.the_context), true, "cond");
        con_val = env.ir_builder->CreateICmpNE(con_val, llvm::ConstantInt::get(*env.the_context, llvm::APInt(32, 0)), "ifcond");

        if (!con_val)
            throw SyntaxError("while cond gen failed!");

        BasicBlock *body_block = BasicBlock::Create(*env.the_context, "body", llvm_func);
        BasicBlock *merge_block = BasicBlock::Create(*env.the_context, "merge", llvm_func);

        env.ir_builder->CreateCondBr(con_val, body_block, merge_block);
        env.ir_builder->SetInsertPoint(body_block);

        // to store current break point.
        Tools::Guarder<void()> g2([&]
                                  { env.break_stac.push_back(merge_block); },
                                  [&]
                                  {
                                      env.break_stac.pop_back();
                                  });
        
        for (auto &stmt : body)
            stmt->codegen();
        step->codegen();

        env.ir_builder->CreateBr(start);
        env.ir_builder->SetInsertPoint(merge_block);
        return nullptr;
    }

    Value *Parser::Jmp::codegen() const
    {
        if (get_kind() == BREAK)
        {
            if(env.break_stac.empty())
                throw SyntaxError("use break in non-loop block!");
            env.ir_builder->CreateBr(env.break_stac.back());
        }
        else {
            if(env.continue_stac.empty())
                throw SyntaxError("use break in non-loop block!");
            env.ir_builder->CreateBr(env.continue_stac.back());
        }
        return nullptr;
    }

    std::unique_ptr<AstNode> parse_break_continue()
    {
        if (env.cur_tag() == HLex::BREAK)
        {
            env.match(HLex::BREAK);
            return std::make_unique<Jmp>(BREAK);
        }
        else
        {
            env.match(HLex::CONTINUE);
            return std::make_unique<Jmp>(CONTINUE);
        }
    }
}
