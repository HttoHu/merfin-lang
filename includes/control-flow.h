#pragma once
#include "ast.h"
namespace Parser
{
    using std::unique_ptr;
    using std::vector;
    class IfElseAst : public AstNode
    {
    public:
        IfElseAst(unique_ptr<AstNode> c, vector<unique_ptr<AstNode>> &&t, vector<unique_ptr<AstNode>> &&e) : AstNode(IF), condition(std::move(c)),
                                                                                                             then_ast(std::move(t)), else_ast(std::move(e)) {}

        Value *codegen() const override;

    private:
        unique_ptr<AstNode> condition;
        vector<unique_ptr<AstNode>> then_ast, else_ast;
    };
    class ForAst : public AstNode
    {
    public:
        ForAst(unique_ptr<AstNode> _var_decl, unique_ptr<AstNode> _con,
               unique_ptr<AstNode> _step,
               vector<unique_ptr<AstNode>> &&_body) : AstNode(FOR),
                                                      var_decl(std::move(_var_decl)),
                                                      condition(std::move(_con)), step(std::move(_step)), body(std::move(_body))
        {
        }
        Value* codegen()const override;
    private:
        unique_ptr<AstNode> var_decl, condition, step;
        vector<unique_ptr<AstNode>> body;
    };
    class WhileAst : public AstNode
    {
    public:
        WhileAst(unique_ptr<AstNode> _condition, vector<unique_ptr<AstNode>> &&vec) : AstNode(WHILE), condition(std::move(_condition)), body(std::move(vec)) {}
        Value *codegen() const override;

    private:
        unique_ptr<AstNode> condition;
        vector<unique_ptr<AstNode>> body;
    };

    class Jmp :public AstNode{
    public:
        Jmp(NodeType nt):AstNode(nt){}
        Value* codegen()const override;
    private:
    };
    std::unique_ptr<AstNode> parse_if_ast();
    std::unique_ptr<AstNode> parse_while_ast();
    std::unique_ptr<AstNode> parse_for_ast();
    std::vector<unique_ptr<AstNode>> parse_block();
    std::unique_ptr<AstNode> pase_stmts();
    std::unique_ptr<AstNode> parse_break_continue();
}
