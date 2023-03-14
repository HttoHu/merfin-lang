#pragma once
#include <llvm/IR/Value.h>
#include "types.h"
#include "error.h"
namespace Parser
{
    using llvm::Value;
    using int_type = int32_t;
    using char_type = char;
    using real_type = double;
    enum NodeType
    {
        NO_OP,
        LITERAL,
        BINOP,
        UNARYOP,
        NAMED_VAL,
        CALL_FUNC,
        IF,
        FOR,
        WHILE,
        VAR_DECL,
        BREAK,
        CONTINUE
    };
    class AstNode
    {
    public:
        AstNode(NodeType _type) : type(_type) {}
        // node type
        NodeType get_kind() const { return type; }
        virtual Value *codegen() const
        {
            return nullptr;
        }
        // value type
        virtual BasicType* get_type() const { 
            throw std::runtime_error("call get_type on raw ast node!");    
        }
        virtual void print(int indent) const {}
        virtual ~AstNode() {}

    private:
        NodeType type;
    };
}