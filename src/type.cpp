#include "../includes/types.h"
#include "../includes/env.h"
std::map<std::string, BasicType *> BasicType::type_tab = {
    {"int", new BasicType(BasicType::INT)}, {"real", new BasicType(BasicType::REAL)}, {"char", new BasicType(BasicType::CHAR)}};

llvm::Type *BasicType::to_llvm_type()
{
    if (kind == INT)
        return llvm::Type::getInt32Ty(*env.the_context);
    else if (kind == REAL)
        return llvm::Type::getDoubleTy(*env.the_context);
    else if (kind == CHAR)
        return llvm::Type::getInt8Ty(*env.the_context);
    else
        throw SyntaxError("invalid type");
}