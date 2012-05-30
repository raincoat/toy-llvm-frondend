#include <iostream>
#include <vector>
#include <llvm/Value.h>
//using namespace llvm;

class CodeGenContext;
class Stmt;
class Expr;
class VariableDef;

typedef std::vector<Stmt*> StmtsList;
typedef std::vector<Expr*> ArgsList;
//typedef std::vector<Variable*> VariableList;

class Node {
  public:
    virtual  ~Node() {};
    virtual  llvm::Value* codeGen(CodeGenContext &context) {};
};

class Expr: public Node {
};

class Stmt: public Node {
};

class ExprStmt: public Stmt {
  public:
    Expr &expr;
    ExprStmt(Expr &expr)
      : expr(expr) { }
    virtual llvm::Value* codeGen(CodeGenContext &context);
};

class Block: public Stmt {
  public:
    StmtsList stmtsList;
    Block() {}
    virtual llvm::Value* codeGen(CodeGenContext &context);
};

class Int: public Expr {
  public:
    long long value;
    Int(long long value) : value(value) { }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class Op: public Expr {
  public:
    Expr &lhs;
    Expr &rhs;
    int op;

    Op(int op, Expr &lhs, Expr &rhs)
      : op(op), lhs(lhs), rhs(rhs) {
      }

    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class Id: public Expr {
  public:
    std::string name;
    Id(const std::string &name)
      :name(name) {
      }

    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class Ass: public Expr { //Assignment
  public:
    Id &lhs;
    Expr &rhs;
    Ass(Id &lhs, Expr &rhs)
      :lhs(lhs), rhs(rhs) {
      }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class MethodCall: public Expr {
  public:
    Id &methodId;
    ArgsList &argsList;
    MethodCall(Id &methodId, ArgsList &argsList)
      :methodId(methodId), argsList(argsList) {
      }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class VariableDef: public Stmt {
  public:
    Id &id;
    Expr *assExpr;

    VariableDef(Id &id)
      : id(id) {
      }

    VariableDef(Id &id, Expr *assExpr)
      : id(id), assExpr(assExpr) {
      }

    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class If: public Stmt {
  public:
    Expr &condition;
    Stmt *thenStmt;
    Stmt *elseStmt;

    If(Expr &condition, Stmt *thenStmt)
      : condition(condition), thenStmt(thenStmt) {
      }

    If(Expr &condition, Stmt *thenStmt, Stmt *elseStmt)
      : condition(condition), thenStmt(thenStmt), elseStmt(elseStmt) {
      }

    virtual llvm::Value *codeGen(CodeGenContext &context);
};
