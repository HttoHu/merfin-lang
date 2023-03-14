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
    enum Tag {BREAK,CLASS,CONTINUE,ELSE,FUNCTION,FOR,IF,LET,RETURN,WHILE,SPACE,ADD,SUB,MUL,DIV,MOD,STRING,INT,REAL,LPAR,RPAR,BEGIN,END,NEWLINE,EQ,GT,LT,GE,LE,NOT,NE,COMMA,SEMI,ASSIGN,SYMBOL};

    struct Token
    {
        Tag tag;
        std::string val;
    };

    class Lexer
    {
    public:
        static std::string tag_to_str(Tag tag){
            static map<Tag,std::string> mp = {{BREAK,"BREAK"},{CLASS,"CLASS"},{CONTINUE,"CONTINUE"},{ELSE,"ELSE"},{FUNCTION,"FUNCTION"},{FOR,"FOR"},{IF,"IF"},{LET,"LET"},{RETURN,"RETURN"},{WHILE,"WHILE"},{SPACE,"SPACE"},{ADD,"ADD"},{SUB,"SUB"},{MUL,"MUL"},{DIV,"DIV"},{MOD,"MOD"},{STRING,"STRING"},{INT,"INT"},{REAL,"REAL"},{LPAR,"LPAR"},{RPAR,"RPAR"},{BEGIN,"BEGIN"},{END,"END"},{NEWLINE,"NEWLINE"},{EQ,"EQ"},{GT,"GT"},{LT,"LT"},{GE,"GE"},{LE,"LE"},{NOT,"NOT"},{NE,"NE"},{COMMA,"COMMA"},{SEMI,"SEMI"},{ASSIGN,"ASSIGN"},{SYMBOL,"SYMBOL"}};
            return mp[tag];
        }
        Lexer()
        {
            entry = 26;
            fin_stat_tab = {{1,ADD},{2,ASSIGN},{3,BEGIN},{4,COMMA},{5,DIV},{6,END},{7,EQ},{8,GE},{9,GT},{10,INT},{11,LE},{12,LPAR},{13,LT},{14,MOD},{15,MUL},{16,NE},{17,NEWLINE},{18,NOT},{19,REAL},{20,RPAR},{21,SEMI},{22,SPACE},{23,STRING},{24,SUB},{25,SYMBOL}};
            tab = {{{'0',19},{'1',19},{'2',19},{'3',19},{'4',19},{'5',19},{'6',19},{'7',19},{'8',19},{'9',19}},{},{{'=',7}},{},{},{},{},{},{},{{'=',8}},{{'.',0},{'0',10},{'1',10},{'2',10},{'3',10},{'4',10},{'5',10},{'6',10},{'7',10},{'8',10},{'9',10}},{},{},{{'=',11}},{},{},{},{},{{'=',16}},{{'0',19},{'1',19},{'2',19},{'3',19},{'4',19},{'5',19},{'6',19},{'7',19},{'8',19},{'9',19}},{},{},{},{},{},{{'0',25},{'1',25},{'2',25},{'3',25},{'4',25},{'5',25},{'6',25},{'7',25},{'8',25},{'9',25},{'A',25},{'B',25},{'C',25},{'D',25},{'E',25},{'F',25},{'G',25},{'H',25},{'I',25},{'J',25},{'K',25},{'L',25},{'M',25},{'N',25},{'O',25},{'P',25},{'Q',25},{'R',25},{'S',25},{'T',25},{'U',25},{'V',25},{'W',25},{'X',25},{'Y',25},{'Z',25},{'_',25},{'a',25},{'b',25},{'c',25},{'d',25},{'e',25},{'f',25},{'g',25},{'h',25},{'i',25},{'j',25},{'k',25},{'l',25},{'m',25},{'n',25},{'o',25},{'p',25},{'q',25},{'r',25},{'s',25},{'t',25},{'u',25},{'v',25},{'w',25},{'x',25},{'y',25},{'z',25}},{{'\t',22},{'\n',17},{'\r',27},{' ',22},{'!',18},{'"',23},{'%',14},{'(',12},{')',20},{'*',15},{'+',1},{',',4},{'-',24},{'/',5},{'0',10},{'1',10},{'2',10},{'3',10},{'4',10},{'5',10},{'6',10},{'7',10},{'8',10},{'9',10},{';',21},{'<',13},{'=',2},{'>',9},{'A',25},{'B',25},{'C',25},{'D',25},{'E',25},{'F',25},{'G',25},{'H',25},{'I',25},{'J',25},{'K',25},{'L',25},{'M',25},{'N',25},{'O',25},{'P',25},{'Q',25},{'R',25},{'S',25},{'T',25},{'U',25},{'V',25},{'W',25},{'X',25},{'Y',25},{'Z',25},{'_',25},{'a',25},{'b',25},{'c',25},{'d',25},{'e',25},{'f',25},{'g',25},{'h',25},{'i',25},{'j',25},{'k',25},{'l',25},{'m',25},{'n',25},{'o',25},{'p',25},{'q',25},{'r',25},{'s',25},{'t',25},{'u',25},{'v',25},{'w',25},{'x',25},{'y',25},{'z',25},{'{',3},{'}',6}},{{'\n',17}}};
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