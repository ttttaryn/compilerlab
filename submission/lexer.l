%option noyywrap

%{
#include <iostream>
#include <vector>
#include "utils.h"
using namespace std;
%}

/* Definitions */
Keyword     "int"|"return"|"main"
Identifier  [A-Za-z_][0-9A-Za-z_]*
Integer     [+\-]?[0-9]+
Operator    "<="|">="|"=="|"!="|[+\-*/%<>&|^=()]
Delimiter   [;{}]

%% 

{Keyword}       {   thisToken.type = "Keyword";
                    thisToken.value = string(yytext);
                    tokens.push_back(thisToken);
                    // cout << "Keyword: " << yytext << endl;
                }

{Identifier}    {   thisToken.type = "Identifier";
                    thisToken.value = string(yytext);
                    tokens.push_back(thisToken);
                    // cout << "Identifier: " << yytext << endl;
                }

{Integer}       {   thisToken.type = "Constant";
                    thisToken.value = string(yytext);
                    tokens.push_back(thisToken);
                    // cout << "Integer: " << yytext << endl;
                }

{Operator}      {   thisToken.type = "Operator";
                    thisToken.value = string(yytext);
                    tokens.push_back(thisToken);
                    // cout << "Operator: " << yytext << endl;
                }

{Delimiter}     {   thisToken.type = "Delimiter";
                    thisToken.value = string(yytext);
                    tokens.push_back(thisToken);
                    // cout << "Delimeter: " << yytext << endl;
                }
%%

void analyze_lexical(const char* filename) 
{
    /* cout << "\nFlex analyze_lexical Start!\n" << endl; */
    yyin = set_input_file(filename);
    yylex();
    fclose(yyin);

    /* cout << "\nFlex analyze_lexical Done!\n" << endl; */
}