
#pragma once 
#include "ast.h"
#include <memory>
namespace Parser{
    std::unique_ptr<AstNode> parse_stmts();
}