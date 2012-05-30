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
  std::vector<Expr*> *exprvec;
}

%token <string> T_ID T_FLOAT T_INT
%token <token> T_MUL T_DIV T_ASS T_LE T_GE T_DOT T_COMMA
%token <token> T_PLUS T_MINUS
%token <token> T_LPRTS T_RPRTS T_LBRACE T_RBRACE
%token <token> T_EQ T_NEQ T_LT T_GT
%token <token> T_VAR T_IF T_THEN T_WHILE T_FOR T_SWITCH T_DO T_RETURN T_ELSE

%type <token>  op
%type <block>  program stmts
%type <stmt> stmt var_def block if_stmt
%type <expr> num expr
%type <id> id
%type <exprvec> call_args


%nonassoc EXPR_IF
%nonassoc EXPR_IFE
%nonassoc EXPR_ASS
%nonassoc EXPR_OP


%left T_PLUS T_MINUS T_MUL T_DIV T_LT T_GT
%start program

%%
program: stmts { programBlock = $1; }
       ;

stmts: stmt { $$ = new Block(); $$->stmtsList.push_back($<stmt>1); }
     | stmts stmt { $1->stmtsList.push_back($<stmt>2); }
     ;

stmt: expr { $$ = new ExprStmt(*$1); }
    | var_def
    | if_stmt
    ;

if_stmt: T_IF expr block  { $$ = new If(*$2, $3); }
       | T_IF expr block T_ELSE block  { $$ = new If(*$2, $3, $5); }
       ;

block: T_LBRACE T_RBRACE { $$ = new Block(); }
     | T_LBRACE stmts T_RBRACE {  $$ = $2;  }
     ;

var_def: T_VAR id  { $$ = new VariableDef(*$2); }
       | T_VAR id T_ASS expr { $$ = new VariableDef(*$2, $4); }
       ;

expr: id T_ASS expr %prec EXPR_ASS { $$ = new Ass(*$<id>1, *$3); }
    | num
    | expr op expr  %prec EXPR_OP { $$ = new Op($2, *$1, *$3); }
    | id { $<id>$ = $1; }
    | id T_LPRTS call_args T_RPRTS { $$ = new MethodCall(*$1, *$3); }
    | T_LPRTS expr T_RPRTS { $$ = $2; }
    ;

id: T_ID { $$ = new Id(*$1); delete $1; }
  ;

call_args:/*blank*/ { $$ = new ArgsList(); }
         | expr { $$ = new ArgsList(); $$->push_back($1); }
         | call_args T_COMMA expr { $1->push_back($3); }
         ;

num: T_INT { $$ = new Int(atol($1->c_str())); delete $1; }
   ;

op: T_PLUS
  | T_MINUS
  | T_DIV
  | T_MUL
  | T_LT
  | T_GT
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
