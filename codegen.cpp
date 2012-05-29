#include "node.hpp"
#include "codegen.hpp"
#include "parser.tab.hpp"

using namespace std;

void CodeGenContext::generateCode(Block &root)
{

  LLVMContext &gC = getGlobalContext(); // globalContext
  ArrayRef<Type*> argTypes;
  FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()),
                                          argTypes, false);

  mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage,
                                  "main", module);

  BasicBlock *bblock = BasicBlock::Create(getGlobalContext(),
                                          "entry", mainFunction, 0);

  /* header */
  // declare void @memset
  Type *memset_params_type[] = {
    Type::getInt8PtrTy(gC),
    Type::getInt32Ty(gC)
  };
  Function *memset_func = Intrinsic::getDeclaration(module, Intrinsic::memset,
                                                    memset_params_type);
  // declare i32 @getchar(i32)

  getchar_func = cast<Function>(module->
      getOrInsertFunction("getchar", IntegerType::getInt32Ty(gC), NULL));

  // declare i32 @putchar(i32)

  putchar_func = cast<Function>(module->
      getOrInsertFunction("putchar", IntegerType::getInt32Ty(gC),
                          IntegerType::getInt32Ty(gC), NULL));

  /* Push a new variable/block context */
  pushBlock(bblock);
  root.codeGen(*this); /* emit bytecode for the toplevel block */
  BasicBlock *endblock = BasicBlock::Create(getGlobalContext(), "end", mainFunction, 0);
  IRBuilder<> *builder;
  builder = (*this).currentBuilder();
  builder->CreateBr(endblock);
  builder->SetInsertPoint(endblock);
  builder->CreateRetVoid();
  popBlock();

  std::cout << "Code is generated.\n";
  PassManager pm;
  pm.add(createPrintModulePass(&outs()));
  pm.run(*module);

}

/* Executes the AST by running the main function */
GenericValue CodeGenContext::runCode()
{
  std::cout << "Running code...\n"<<std::endl;
  ExecutionEngine *ee = EngineBuilder(module).create();
  vector<GenericValue> noargs;
  GenericValue v = ee->runFunction(mainFunction, noargs);
  std::cout <<std::endl<< "Code was run.\n";
  return v;
}

/* Returns an LLVM type based on the identifier */

Value* Block::codeGen(CodeGenContext &context) {
  StmtsList::const_iterator it;
  Value *last = NULL;
  long long value = 20;
  for (it = stmtsList.begin(); it != stmtsList.end(); it++) {
    std::cout << "Generating code for stmt" << endl;
    last = (**it).codeGen(context);
  }
  std::cout << "Creating block" << endl;
  return last;
}

Value* Int::codeGen(CodeGenContext &context) {
  std::cout << "Creating Int " << value << endl;
  return ConstantInt::get(getGlobalContext(), APInt(64, value));
}

Value* Op::codeGen(CodeGenContext &context) {
  cout << "Creating Op " << endl;
  IRBuilder<> *builder = context.currentBuilder();

  Value *tape_1;
  Value *lshValue = lhs.codeGen(context);
  Value *rshValue = rhs.codeGen(context);
  switch(op) {
    case T_PLUS:
      cout << "Creating add" << endl;
      tape_1 = builder->CreateAdd(lshValue,
          rshValue, "addtmp");
      break;
    case T_MINUS:
      tape_1 = builder->CreateSub(lshValue,
          rshValue, "subtmp");
      break;
    case T_DIV:
      tape_1 = builder->CreateFDiv(lshValue,
          rshValue, "divtmp");
      break;
    case T_MUL:
      tape_1 = builder->CreateFMul(lshValue,
          rshValue, "multmp");
      break;
  }
  return tape_1;
}

Value* ExprStmt::codeGen(CodeGenContext &context) {
  std::cout << "Creating ExprStmt" << endl;
  return expr.codeGen(context);
}

Value* VariableDef::codeGen(CodeGenContext &context) {

  IRBuilder<> *builder = context.currentBuilder();
  Value *alloc = builder->CreateAlloca(builder->getInt64Ty(), 0 , id.name.c_str());
  context.locals()[id.name] = alloc;
  return alloc;
}

Value* Id::codeGen(CodeGenContext &context) {
  if (context.locals().find(name) == context.locals().end()) {
    return NULL;
  }

  IRBuilder<> *builder = context.currentBuilder();
  return builder->CreateLoad(context.locals()[name], name);
}

Value* Ass::codeGen(CodeGenContext &context) {
  IRBuilder<> *builder = context.currentBuilder();
  if (context.locals().find(lhs.name) == context.locals().end()) {
    return NULL;
  }
  Value *rhsValue = rhs.codeGen(context);
  return builder->CreateStore(rhsValue, context.locals()[lhs.name]);
}
