#include <iostream>
#include <vector>
#include <llvm/Value.h>
//using namespace llvm;

class CodeGenContext;
class Stmt;
class Expr;
class VariableDef;

typedef std::vector<Stmt*> StmtsList;
typedef std::vector<Expr*> ExprsList;
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

class VariableDef: public Stmt {
  public:
    Id &id;
    Expr *assExpr;
    VariableDef(Id &id)
      :id(id) {
      }

    virtual llvm::Value *codeGen(CodeGenContext &context);
};


