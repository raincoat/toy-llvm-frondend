%{
  #include <string>
  #include "parser.tab.hpp"
  #define TOKEN(t) (yylval.token = t)
  #define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
  extern "C" int yywrap() {}
%}

%%
[ \t\n] {}
[a-zA-Z_][a-zA-Z_0-9]* { SAVE_TOKEN; return TOKEN(T_ID); }
[0-9]*\.[0-9]+ { SAVE_TOKEN; return TOKEN(T_FLOAT); }
[0-9]+ { SAVE_TOKEN; return TOKEN(T_INT); }

"+"  { return TOKEN(T_PLUS); }
"-"  { return TOKEN(T_MINUS); }
"*"  { return TOKEN(T_MUL); }
"/"  { return TOKEN(T_DIV); }
"="  { return TOKEN(T_AS); }
"<"  { return TOKEN(T_LE); }
">"  { return TOKEN(T_GE); }
"."  { return TOKEN(T_DOT); }
","  { return TOKEN(T_COMMA);}

"("  { return TOKEN(T_LPRTS);}
")"  { return TOKEN(T_RPRTS);}
"{"  { return TOKEN(T_LBRACE);}
"}"  { return TOKEN(T_RBRACE);}

"!="  { return TOKEN(T_NEQ); }
"=="  { return TOKEN(T_EQ); }
"<="  { return TOKEN(T_LE_EQ); }
">="  { return TOKEN(T_GE_EQ); }

%%

#ifdef LEX
int main (int argc, char** argv) {
  int token;

  while (token = yylex()) {
    printf("%d", token);
  }

  return 0;
}
#endif
