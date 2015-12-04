%{
#include <string>
#include <map>
using namespace std;

extern int yylex();
extern void yyerror(const char *);

#include "StructorBuilder.hh"
#include "Structure.hh"
#include "Field.hh"

extern StructorBuilder *sb;

%}


%union {
   int     int_token;
   string* str_val;
}

%token <int_token> COMMA EQUALS LBRACE RBRACE SEMICOLON TYPEDEF ENUM STRUCT INT STAR
%token <str_val> NAME COMMENT

%start hfile

%%


// TODO For mem mgt, need to manage anything new-ed by the lex file.
// As of this writing, NAME is the only returned value that needs to be managed.

// Three elements are recognized in an "h" file:
// - enumeration declaration
// - structure definition
// - comment

hfile: /* empty */
     | hfile enum_decl
          {sb->onMatch("hfile enum_decl@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");}
     | hfile struct_decl
          {sb->onMatch("hfile struct_decl@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");}
     | hfile COMMENT
          {sb->onMatch("comment\n");}

struct_decl: TYPEDEF STRUCT LBRACE member_list RBRACE NAME SEMICOLON
   {sb->onStruct($6); delete $6;}

member_list: member
               {sb->onMatch("member_list");}
             |
             member_list member
               {sb->onMatch("member_list");}

member: NAME NAME SEMICOLON
           {sb->onField($1,$2,0);}
        |
        NAME STAR NAME SEMICOLON
           {sb->onField($1,$3,1);}

enum_decl: TYPEDEF ENUM NAME LBRACE enum_list RBRACE NAME SEMICOLON
   {sb->onEnum($7);}

enum_list: last_enum_item
   {sb->onMatch("last_enum_item 1");}
           |
           inner_enum_items last_enum_item
   {sb->onMatch("last_enum_item 2");}

inner_enum_items: inner_enum_item
   {sb->onMatch("inner_enum_item 1");}
           |
           inner_enum_items inner_enum_item
   {sb->onMatch("inner_enum_item 2");}

inner_enum_item: NAME EQUALS INT COMMA
   {sb->onEnumValue($1);}

last_enum_item: NAME EQUALS INT
   {sb->onMatch("last_enum_item");}
