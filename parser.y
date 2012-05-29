%{
  #include "node.hpp"
  #include <cstdio>
  #include <cstdlib>
  extern int yylex();
  void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1);}
  Block *programBlock;
%}

%union {
  int token;
  std::string *string;
  Block *block;
  Stmt *stmt;
  Expr *expr;
  Id *id;
}

%token <string> T_ID T_FLOAT T_INT
%token <token> T_MUL T_DIV T_ASS T_LE T_GE T_DOT T_COMMA
%token <token> T_PLUS T_MINUS
%token <token> T_LPRTS T_RPRTS T_LBRACE T_RBRACE
%token <token> T_EQ T_LE_EQ T_GE_EQ T_NEQ
%token <token> T_VAR T_IF T_THEN T_WHILE T_FOR T_SWITCH T_DO T_RETURN

%type <token>  op
%type <block>  program stmts
%type <stmt> stmt var_def
%type <expr> num expr
%type <id> id


%nonassoc EXPR_ASS
%nonassoc EXPR_OP

%left T_PLUS T_MINUS T_MUL T_DIV
%start program

%%
program: stmts { programBlock = $1; }
       ;

stmts: stmt { $$ = new Block(); $$->stmtsList.push_back($<stmt>1); }
     | stmts stmt { $1->stmtsList.push_back($<stmt>2); }
     ;

stmt: expr { $$ = new ExprStmt(*$1); }
    | var_def
    ;

var_def: T_VAR id  { $$ = new VariableDef(*$2); }
       ;

expr: id T_ASS expr %prec EXPR_ASS { $$ = new Ass(*$<id>1, *$3); }
    | num
    | expr op expr  %prec EXPR_OP { $$ = new Op($2, *$1, *$3); }
    | id { $<id>$ = $1; }
    ;

id: T_ID { $$ = new Id(*$1); delete $1; }
  ;


num: T_INT { $$ = new Int(atol($1->c_str())); delete $1; }
   ;

op: T_PLUS
  | T_MINUS
  | T_DIV
  | T_MUL
  ;

%%

/*
int main (int argc, char** argv) {
    yyparse();
    return 0;
}
*/

void yyerror (char *s) {
    fprintf(stderr, "error: %sn", s);
}
