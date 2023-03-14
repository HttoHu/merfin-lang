#pragma once
#include <string>
#include <map>
#include <vector>
#include <llvm/IR/IRBuilder.h>
#include "types.h"
#include "error.h"

namespace Sym
{
    enum WordType
    {
        VAR,
        GVAR,
        FUNC,
        CLASS
    };
    class Word
    {
    public:
        Word(WordType _type) : type(_type) {}
        WordType get_wordtype() const { return type; }
        virtual ~Word()
        {
        }
        static std::vector<std::map<std::string, Word *>> word_tab;
        static void set_symbol(const std::string &name, Word *val)
        {
            if (word_tab.back().count(name))
                throw SyntaxError("symbol " + name + " redefined!");
            word_tab.back().insert({name, val});
        }
        static void set_gsymbol(const std::string &name, Word *val)
        {
            if (word_tab.front().count(name))
                throw SyntaxError("symbol " + name + " redefined!");
            word_tab.front().insert({name, val});
        }
        static Word *find_word(const std::string &name)
        {
            for (int i = word_tab.size() - 1; i >= 0; i--)
            {
                auto res = word_tab[i].find(name);
                if (res == word_tab[i].end())
                    continue;
                return res->second;
            }
            throw SyntaxError("Word::find_word unknown symbol " + name);
        }
        static void new_block()
        {
            word_tab.push_back({});
        }
        static void end_block()
        {
            word_tab.pop_back();
        }

    private:
        WordType type;
    };
    // to record variable
    class VarWord : public Word
    {
    public:
        static BasicType *get_var_type(Word *word)
        {
            if (word->get_wordtype() != VAR)
                throw SyntaxError("internal error! expected a var symbol!");
            return static_cast<VarWord *>(word)->var_type;
        }
        VarWord(BasicType *_var_type) : Word(VAR), var_type(_var_type) {}

    private:
        BasicType *var_type;
    };
    // to record function symbol info
    class FuncWord : public Word
    {
    public:
        static BasicType *get_ret_type(Word *word)
        {
            if (word->get_wordtype() != FUNC)
                throw SyntaxError("internal error! expected a function symbol!");
            return static_cast<FuncWord *>(word)->ret_type;
        }

        FuncWord(BasicType *_ret_type) : Word(FUNC), ret_type(_ret_type) {}

    private:
        BasicType *ret_type;
    };

}