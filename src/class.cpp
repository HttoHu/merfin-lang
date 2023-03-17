#include "../includes/class.h"
#include "../includes/env.h"
#include "../includes/lexer.h"
namespace Parser
{
    Mer::ClassType *parse_class()
    {
        Mer::ClassType *ret = new Mer::ClassType();
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
        env.match(HLex::END);
        env.match(HLex::SEMI);
        Sym::Word::set_gsymbol(class_name, new Sym::Word(Sym::CLASS));
        BasicType::set_type(class_name, ret);
        return ret;
    }

    llvm::Value *ClassConstructor::codegen() const
    {
        return type->construct();
    }
    BasicType *ClassConstructor::get_type() const
    {
        return type;
    }
}

void Mer::ClassType::add_member(const std::string &name, Parser::Node node)
{
    members.push_back({name, std::move(node)});
    name_tab.insert({name, cnt++});
}

std::pair<int, BasicType *> Mer::ClassType::find_member(const std::string &name)
{
    auto it = name_tab.find(name);
    if (it == name_tab.end())
        throw SyntaxError("unknown member " + name);
    return {it->second, (members[it->second].second)->get_type()};
}

llvm::Type *Mer::ClassType::to_llvm_type()
{
    // calculated
    if (llvm_type)
        return llvm_type;

    std::vector<llvm::Type *> types;
    for (int i = 0; i < members.size(); i++)
        types.push_back(std::get<1>(members[i])->get_type()->to_llvm_type());
    auto struct_type = llvm::StructType::create(*env.the_context, types, "myclass");
    llvm_type = struct_type;
    return llvm_type;
}

llvm::Value *Mer::ClassType::construct()
{
    llvm::Value *struct_ptr = env.ir_builder->CreateAlloca(llvm_type, nullptr);

    for (int i = 0; i < members.size(); i++)
    {
        llvm::Value *mem_ptr = env.ir_builder->CreateStructGEP(llvm_type, struct_ptr, i);
        // get initial expression.
        auto &expr = members[i].second;
        env.ir_builder->CreateStore(expr->codegen(), mem_ptr);
    }
    return struct_ptr;
}