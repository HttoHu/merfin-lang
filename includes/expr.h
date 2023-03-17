#pragma once
#include "ast.h"
#include "lexer.h"
#include "symbol.h"
#include <memory>
#include <llvm/IR/Constant.h>

namespace Mer{
    class ClassType;
}
namespace Parser
{
    using std::unique_ptr;
    class LiteralVal : public AstNode
    {
    public:
        enum LiteralTag
        {
            INT,
            DOUBLE,
            CHAR
        };

    public:
        LiteralVal(int_type val) : AstNode(LITERAL), data(val), type(INT) {}
        LiteralVal(real_type val) : AstNode(LITERAL), data(val), type(DOUBLE) {}
        LiteralVal(char_type val) : AstNode(LITERAL), data(val), type(CHAR) {}
        Value *codegen() const override;
        void print(int indent) const override
        {
            std::cout << std::string(indent, ' ') << std::get<int_type>(data) << "\n";
        }
        BasicType *get_type() const override
        {
            if (type == INT)
                return BasicType::find_type("int");
            else if (type == CHAR)
                return BasicType::find_type("char");
            else if (type == DOUBLE)
                return BasicType::find_type("real");
            throw SyntaxError("literal const val : unknown type!");
        }

    private:
        LiteralTag type;
        std::variant<int_type, real_type, char_type> data;
    };
    class Variable : public AstNode
    {
    public:
        Variable(const std::string &_name) : var_name(_name), AstNode(NodeType::NAMED_VAL)
        {
            type = Sym::VarWord::get_var_type(Sym::VarWord::find_word(var_name));
        }
        BasicType *get_type() const override;
        Value *codegen() const override;
        Value* get_alloc_ptr()const override;
        bool left_value() const override { return true; }
        std::string var_name;

    private:
        BasicType *type;
    };
    class BinOp : public AstNode
    {
    public:
        BinOp(HLex::Tag _type, unique_ptr<AstNode> &&l, unique_ptr<AstNode> &&r) : AstNode(BINOP), type(_type), left(std::move(l)), right(std::move(r)) {}
        Value *codegen() const override;
        static int get_precedence(HLex::Tag optype)
        {
            static bool init = false;
            static std::map<HLex::Tag, int> precedence = {
                {HLex::ASSIGN, 16},
                {HLex::NE, 10},
                {HLex::EQ, 10},

                {HLex::DOT, 2},
                {HLex::LE, 9},
                {HLex::LT, 9},
                {HLex::GE, 9},
                {HLex::GT, 9},

                {HLex::ADD, 6},
                {HLex::SUB, 6},

                {HLex::MOD, 5},
                {HLex::MUL, 5},
                {HLex::DIV, 5}};
            if (!precedence.count(optype))
                return -1;
            return precedence[optype];
        }
        BasicType *get_type() const override
        {
            auto ltype = left->get_type();
            if (ltype != right->get_type())
                throw SyntaxError("Binary operation type not matched!");
            return ltype;
        }
        void print(int indent) const override
        {
            left->print(indent + 2);
            std::cout << std::string(indent, ' ') << HLex::Lexer::tag_to_str(type) << "\n";
            right->print(indent + 2);
        }

    private:
        HLex::Tag type;
        std::unique_ptr<AstNode> left, right;
    };
    class CallExpr : public AstNode
    {
    public:
        CallExpr(const std::string &_fname, std::vector<unique_ptr<AstNode>> &&_args) : AstNode(CALL_FUNC),
                                                                                        func_name(_fname), args(std::move(_args)) {}
        BasicType *get_type() const override;
        Value *codegen() const override;

    private:
        std::string func_name;
        std::vector<unique_ptr<AstNode>> args;
    };
    class UnaryOp : public AstNode
    {
    public:
        UnaryOp(HLex::Tag _op_type, unique_ptr<AstNode> &&_ast_node) : op_type(_op_type),
                                                                       AstNode(UNARYOP),
                                                                       ast_node(std::move(_ast_node)) {}
        Value *codegen() const override;

        HLex::Tag op_type;

    private:
        unique_ptr<AstNode> ast_node;
    };
    class MemberVisit : public AstNode
    {
    public:
        MemberVisit(Node _var, Mer::ClassType *_type, const std::string &member_name);
        Value *codegen() const override;
        Value* get_alloc_ptr()const override;
        BasicType *get_type() const override
        {
            return type;
        }
        bool left_value() const override
        {
            return true;
        }

    private:
        Node variable;
        Mer::ClassType *type_info;
        BasicType *type;
        int idx;
    };
    unique_ptr<AstNode> parse_expr();
    unique_ptr<AstNode> parse_symbol();
    unique_ptr<AstNode> parse_call();
}