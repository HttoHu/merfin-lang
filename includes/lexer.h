#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <fstream>

namespace HLex
{
    using std::map;
    using std::vector;
    using char_type = char;
    enum Tag {BREAK,CLASS,CONTINUE,ELSE,FUNCTION,FOR,IF,LET,RETURN,WHILE,SPACE,ADD,SUB,MUL,DIV,MOD,DOT,STRING,INT,REAL,LPAR,RPAR,BEGIN,END,NEWLINE,EQ,GT,LT,GE,LE,NOT,NE,COMMA,SEMI,ASSIGN,SYMBOL};

    struct Token
    {
        Tag tag;
        std::string val;
    };

    class Lexer
    {
    public:
        static std::string tag_to_str(Tag tag){
            static map<Tag,std::string> mp = {{BREAK,"BREAK"},{CLASS,"CLASS"},{CONTINUE,"CONTINUE"},{ELSE,"ELSE"},{FUNCTION,"FUNCTION"},{FOR,"FOR"},{IF,"IF"},{LET,"LET"},{RETURN,"RETURN"},{WHILE,"WHILE"},{SPACE,"SPACE"},{ADD,"ADD"},{SUB,"SUB"},{MUL,"MUL"},{DIV,"DIV"},{MOD,"MOD"},{DOT,"DOT"},{STRING,"STRING"},{INT,"INT"},{REAL,"REAL"},{LPAR,"LPAR"},{RPAR,"RPAR"},{BEGIN,"BEGIN"},{END,"END"},{NEWLINE,"NEWLINE"},{EQ,"EQ"},{GT,"GT"},{LT,"LT"},{GE,"GE"},{LE,"LE"},{NOT,"NOT"},{NE,"NE"},{COMMA,"COMMA"},{SEMI,"SEMI"},{ASSIGN,"ASSIGN"},{SYMBOL,"SYMBOL"}};
            return mp[tag];
        }
        Lexer()
        {
            entry = 27;
            fin_stat_tab = {{1,ADD},{2,ASSIGN},{3,BEGIN},{4,COMMA},{5,DIV},{6,DOT},{7,END},{8,EQ},{9,GE},{10,GT},{11,INT},{12,LE},{13,LPAR},{14,LT},{15,MOD},{16,MUL},{17,NE},{18,NEWLINE},{19,NOT},{20,REAL},{21,RPAR},{22,SEMI},{23,SPACE},{24,STRING},{25,SUB},{26,SYMBOL}};
            tab = {{{'0',20},{'1',20},{'2',20},{'3',20},{'4',20},{'5',20},{'6',20},{'7',20},{'8',20},{'9',20}},{},{{'=',8}},{},{},{},{},{},{},{},{{'=',9}},{{'.',0},{'0',11},{'1',11},{'2',11},{'3',11},{'4',11},{'5',11},{'6',11},{'7',11},{'8',11},{'9',11}},{},{},{{'=',12}},{},{},{},{},{{'=',17}},{{'0',20},{'1',20},{'2',20},{'3',20},{'4',20},{'5',20},{'6',20},{'7',20},{'8',20},{'9',20}},{},{},{},{},{},{{'0',26},{'1',26},{'2',26},{'3',26},{'4',26},{'5',26},{'6',26},{'7',26},{'8',26},{'9',26},{'A',26},{'B',26},{'C',26},{'D',26},{'E',26},{'F',26},{'G',26},{'H',26},{'I',26},{'J',26},{'K',26},{'L',26},{'M',26},{'N',26},{'O',26},{'P',26},{'Q',26},{'R',26},{'S',26},{'T',26},{'U',26},{'V',26},{'W',26},{'X',26},{'Y',26},{'Z',26},{'_',26},{'a',26},{'b',26},{'c',26},{'d',26},{'e',26},{'f',26},{'g',26},{'h',26},{'i',26},{'j',26},{'k',26},{'l',26},{'m',26},{'n',26},{'o',26},{'p',26},{'q',26},{'r',26},{'s',26},{'t',26},{'u',26},{'v',26},{'w',26},{'x',26},{'y',26},{'z',26}},{{'\t',23},{'\n',18},{'\r',28},{' ',23},{'!',19},{'"',24},{'%',15},{'(',13},{')',21},{'*',16},{'+',1},{',',4},{'-',25},{'.',6},{'/',5},{'0',11},{'1',11},{'2',11},{'3',11},{'4',11},{'5',11},{'6',11},{'7',11},{'8',11},{'9',11},{';',22},{'<',14},{'=',2},{'>',10},{'A',26},{'B',26},{'C',26},{'D',26},{'E',26},{'F',26},{'G',26},{'H',26},{'I',26},{'J',26},{'K',26},{'L',26},{'M',26},{'N',26},{'O',26},{'P',26},{'Q',26},{'R',26},{'S',26},{'T',26},{'U',26},{'V',26},{'W',26},{'X',26},{'Y',26},{'Z',26},{'_',26},{'a',26},{'b',26},{'c',26},{'d',26},{'e',26},{'f',26},{'g',26},{'h',26},{'i',26},{'j',26},{'k',26},{'l',26},{'m',26},{'n',26},{'o',26},{'p',26},{'q',26},{'r',26},{'s',26},{'t',26},{'u',26},{'v',26},{'w',26},{'x',26},{'y',26},{'z',26},{'{',3},{'}',7}},{{'\n',18}}};
            ignore = {SPACE};
            keywords = {{"break",BREAK},{"class",CLASS},{"continue",CONTINUE},{"else",ELSE},{"fn",FUNCTION},{"for",FOR},{"if",IF},{"let",LET},{"return",RETURN},{"while",WHILE}};
            user_defs = {{STRING,[](const std::string &s,int &pos){
    pos++;
    std::map<char, std::string> escape = {
         {'n', "\n"}, {'t', "\t"}, {'r', "\r"}, {'\\', "\\"}, {'\'', "\\'"}, {'\"', "\""}};
    std::string ret = "";
    while (pos < s.size() && s[pos] != '\"')
    {
        if (s[pos] == '\\')
        {
            if (pos == s.size())
                throw std::runtime_error("invalid string");
            pos++;
            if (!escape.count(s[pos]))
                throw std::runtime_error("invalid escape string!");
            ret += escape[s[pos]];
        }
        else
            ret += s[pos];
        pos++;
    }
    pos++;
    return ret;
}}};
        }
        std::vector<Token> lex(const std::string &content)
        {
            std::vector<Token> ret;
            int cur_state = entry;
            // to roll back state tag val
            std::vector<std::pair<int, Token>> pos_stac;
            // str pos
            int pos = 0;
            std::string cur_token;
            Tag cur_tag;

            while (pos < content.size())
            {
                if (tab[cur_state].count(content[pos]))
                {
                    cur_state = tab[cur_state][content[pos]];
                    cur_token += content[pos];

                    if (fin_stat_tab.count(cur_state))
                    {                            
                        cur_tag = fin_stat_tab[cur_state];
                        pos_stac.clear();
                        pos_stac.push_back({pos + 1, {cur_tag, cur_token}});
                    }
                    pos++;
                }
                else
                {
                    if (pos_stac.empty())
                    {
                        print_line(content, pos);
                        throw std::runtime_error(" LexerGenerator::lex: Lexer Error");
                    }
                    auto [p, tok] = pos_stac.back();
                    // if a symbol is a keyword or ignore
                    auto val = tok.val;
                    if (keywords.count(val))
                        tok = Token{keywords[val], val};
                    if (user_defs.count(tok.tag))
                    {
                        p -= tok.val.size();
                        tok.val = user_defs[tok.tag](content, p);
                    }
                    if (!ignore.count(tok.tag))
                        ret.push_back(tok);

                    // roll back
                    pos = p;
                    cur_state = entry;
                    cur_token = "";
                    pos_stac.clear();
                }
            }
            if (pos_stac.size())
                ret.push_back(pos_stac.back().second);
            return ret;
        }
        void print_line(const std::string &s, int &pos)
        {
            int cnt = 100;
            while (pos < s.size() && cnt)
            {
                std::cout << s[pos++];
                cnt--;
            }
            std::cout.flush();
        }

    private:
        int pos = 0;
        std::map<std::string, Tag> keywords;
        std::map<Tag, std::function<std::string(const std::string &, int &)>> user_defs;
        std::set<Tag> ignore;

        int entry;
        map<int, Tag> fin_stat_tab;
        vector<map<char_type, int>> tab;
    };
}