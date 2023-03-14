#include "../includes/expr.h"
#include "../includes/env.h"
#include "../includes/symbol.h"

namespace Parser
{
    using namespace llvm;

    Value *LiteralVal::codegen() const
    {
        if (type == DOUBLE)
            return ConstantFP::get(*env.the_context, APFloat(std::get<real_type>(data)));
        else if (type == INT)
            return ConstantInt::get(*env.the_context, APInt(32, std::get<int_type>(data)));
        else
            return ConstantInt::get(*env.the_context, APInt(8, std::get<char_type>(data)));
    }
    Value *Variable::codegen() const
    {
        auto alloc_inst = env.local_var_tab[var_name];
        if (!alloc_inst)
            return nullptr;
        return env.ir_builder->CreateLoad(alloc_inst->getAllocatedType(), alloc_inst, var_name.c_str());
    }
    BasicType *Variable::get_type() const
    {
        return type;
    }
    Value *BinOp::codegen() const
    {
        Value *L = left->codegen(), *R = right->codegen();
        switch (type)
        {
        case HLex::ADD:
            return env.ir_builder->CreateAdd(L, R, "t");
        case HLex::SUB:
            return env.ir_builder->CreateSub(L, R, "t");
        case HLex::MUL:
            return env.ir_builder->CreateSub(L, R, "t");
        case HLex::EQ:
            return env.ir_builder->CreateICmpEQ(L, R, "t");
        case HLex::NE:
            return env.ir_builder->CreateICmpNE(L, R, "t");
        case HLex::GE:
            return env.ir_builder->CreateICmpSGE(L, R, "t");
        case HLex::GT:
            return env.ir_builder->CreateICmpSGT(L, R, "t");
        case HLex::LT:
            return env.ir_builder->CreateICmpSLT(L, R, "t");
        case HLex::LE:
            return env.ir_builder->CreateICmpSLE(L, R, "t");
        case HLex::ASSIGN:
        {
            if (left->get_kind() != NAMED_VAL)
                throw std::runtime_error("left-value expected a variable!");
            if (!R)
                throw std::runtime_error("invalid right-part!");
            auto var_name = static_cast<Variable *>(left.get())->var_name;
            return env.ir_builder->CreateStore(R, env.local_var_tab[var_name]);
        }
        default:
            throw SyntaxError("unknown bin op!");
        }
    }
    // call expr
    BasicType *CallExpr::get_type() const
    {
        return Sym::FuncWord::get_ret_type(Sym::FuncWord::find_word(func_name));
    }
    Value *CallExpr::codegen() const
    {
        auto func = env.the_module->getFunction(func_name);
        if (!func)
            throw std::runtime_error("undefined function " + func_name);
        std::vector<Value *> args_value;
        for (auto &arg : args)
        {
            args_value.push_back(arg->codegen());
            if (!args_value.back())
                throw std::runtime_error("call function " + func_name + " failed!");
        }
        return env.ir_builder->CreateCall(func, args_value, "call");
    }
    // end call expr
    Value *UnaryOp::codegen() const
    {
        switch (op_type)
        {
        case HLex::RETURN:
            return env.ir_builder->CreateRet(ast_node->codegen());
        default:
            return nullptr;
        }
    }
    // parsing functions.
    const int default_precedence = 3;

    unique_ptr<AstNode> parse_expr_iter(int precedence)
    {
        if (precedence == 1)
        {
            auto token = env.cur_token();
            if (token.tag == HLex::LPAR)
            {
                env.match(HLex::LPAR);
                auto ret = parse_expr();
                env.match(HLex::RPAR);
                return ret;
            }
            else if (token.tag == HLex::SYMBOL)
            {
                return parse_symbol();
            }
            else
            {
                env.match(HLex::INT);
                return std::make_unique<LiteralVal>((int_type)stoi(token.val));
            }
        }
        auto left = parse_expr_iter(precedence - 1);

        while (BinOp::get_precedence(env.cur_tag()) == precedence)
        {
            auto op = env.cur_tag();
            env.next_token();
            auto right = parse_expr_iter(precedence - 1);
            left = std::make_unique<BinOp>(op, std::move(left), std::move(right));
        }
        return left;
    }
    unique_ptr<AstNode> parse_expr()
    {
        return parse_expr_iter(16);
    }
    unique_ptr<AstNode> parse_call()
    {
        std::string func_name = env.cur_token().val;
        auto func = Function::find_function(func_name);
        env.next_token();
        env.match(HLex::LPAR);
        std::vector<unique_ptr<AstNode>> args;
        while (env.cur_tag() != HLex::RPAR)
        {
            auto expr = parse_expr();
            args.push_back(std::move(expr));
            if (env.cur_tag() == HLex::COMMA)
                env.match(HLex::COMMA);
        }
        env.match(HLex::RPAR);
        func->check_argument_type(args);
        return std::make_unique<CallExpr>(func_name, std::move(args));
    }

    unique_ptr<AstNode> parse_symbol()
    {
        auto sym_name = env.cur_token().val;
        auto symbol = Sym::Word::find_word(sym_name);
        if (symbol->get_wordtype() == Sym::VAR)
        {
            env.next_token();
            return std::make_unique<Variable>(sym_name);
        }
        else if (symbol->get_wordtype() == Sym::FUNC)
        {
            return parse_call();
        }
        return nullptr;
    }
}