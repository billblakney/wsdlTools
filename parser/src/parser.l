%{
#include <string>
#include "StructorBuilder.hh"

using namespace std;

#include "y.tab.h"
void yyerror(char *);
int yyparse(void);

StructorBuilder *sb;

void pLex(const char *aToken)
{
   static int _debug = 0;
   if( _debug )
      printf("-%s-\n",aToken);
}

%}

%x IN_COMMENT


%%

","                     {pLex("comma");return COMMA;}
"="                     {pLex("equals");return EQUALS;}
"{"                     {pLex("lbrace");return LBRACE;}
"}"                     {pLex("rbrace");return RBRACE;}
";"                     {pLex("semicolon");return SEMICOLON;}
"*"                     {pLex("semicolon");return STAR;}
typedef                 {pLex("typedef");return TYPEDEF;}
enum                    {pLex("enum");return ENUM;}
struct                  {pLex("enum");return STRUCT;}
[0-9]+                  {pLex("int");return INT;}
[a-zA-Z0-9_]+           {pLex("name");yylval.str_val = new string(yytext);return NAME;}
\/\*                    {pLex("BEGIN(comment)");BEGIN(IN_COMMENT);}
<IN_COMMENT>\*\/        {pLex("END(comment)");BEGIN(INITIAL);}
<IN_COMMENT>\n          ;
<IN_COMMENT>.           ;
<IN_COMMENT>\/\*.*\*\/  {pLex("comment");return COMMENT;}
size_t.*(.*);           {pLex("size_t fn");}
void.*(.*);             {pLex("void fn");}
#.*\n                   {pLex("directive");}
[ \t\n]+       ; // do nothing

%%

void yyerror(char */*str*/) {printf("ERROR: Could not parse!\n");}
int yywrap(void) {return 0;}

StructorBuilder *lex_main(char *tHeaderFile)
{
   sb = new StructorBuilder();
   FILE* file = fopen(tHeaderFile,"r");
   yyin = file;
   yyparse();
   fclose(file);
   return sb;
}
