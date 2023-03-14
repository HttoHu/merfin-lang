#pragma once
#include <map>
#include <llvm/IR/Type.h>
#include "lexer.h"
#include "error.h"
#include <string>
class BasicType
{
public:
    enum Kind
    {
        INT = 0,
        CHAR = 1,
        REAL = 2,
        ARRAY,
        CLASS
    };
    static std::map<std::string, BasicType *> type_tab;
    static BasicType *find_type(const std::string &name)
    {
        auto it = type_tab.find(name);
        if (it == type_tab.end())
            throw SyntaxError("unknown type " + name);
        return it->second;
    }
    static void set_type(const std::string &name, BasicType *ty)
    {
        type_tab.insert({name, ty});
    }

public:
    BasicType(Kind _kind) : kind(_kind)
    {
    }
    ~BasicType() {}
    virtual llvm::Type *to_llvm_type();
    Kind kind;
};
namespace Type
{
    class ClassTyep : public BasicType
    {
    public:
        
    private:

    };
}