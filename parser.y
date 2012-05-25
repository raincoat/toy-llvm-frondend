%{
  #include <cstdio>
  #include <cstdlib>
  #include <iostream>
  extern int yylex();
  void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1);}
%}

%union {
  int token;
  std::string *string;
}

%token <string> T_ID T_FLOAT T_INT
%token <token> T_PLUS T_MINUS T_MUL T_DIV T_AS T_LE T_GE T_DOT T_COMMA
%token <token> T_LPRTS T_RPRTS T_LBRACE T_RBRACE
%token <token> T_EQ T_LE_EQ T_GE_EQ T_NEQ

%type <string> stmts stmt

%start program

%%
program: stmts
       ;

stmts: stmts stmt
     | stmt
     ;

stmt: T_ID  {}
    ;

%%

int main (int argc, char** argv) {
    yyparse();
    return 0;
}

void yyerror (char *s) {
    fprintf(stderr, "error: %sn", s);
}
