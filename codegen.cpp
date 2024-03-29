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

  // declare i32 @putchar(i64)

  putchar_func = cast<Function>(module->
      getOrInsertFunction("putchar", IntegerType::getInt64Ty(gC),
                          IntegerType::getInt64Ty(gC), NULL));

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
  cout << "Creating Op " << op <<endl;
  IRBuilder<> *builder = context.currentBuilder();

  Value *tape_1;
  Value *lshValue = lhs.codeGen(context);
  Value *rshValue = rhs.codeGen(context);
  switch(op) {
    case T_PLUS:
      tape_1 = builder->CreateAdd(lshValue,
          rshValue, "addtmp");
      break;
    case T_MINUS:
      tape_1 = builder->CreateSub(lshValue,
          rshValue, "subtmp");
      break;
    case T_DIV:
      tape_1 = builder->CreateUDiv(lshValue,
          rshValue, "divtmp");
      break;
    case T_MUL:
      tape_1 = builder->CreateMul(lshValue,
          rshValue, "multmp");
      break;
    case T_LT:
      tape_1 = builder->CreateICmpSLT(lshValue,
          rshValue, "lttmp");
      break;
    case T_GT:
      tape_1 = builder->CreateICmpSGT(lshValue,
          rshValue, "gttmp");
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
  if ( assExpr != NULL) {
    context.locals()[id.name] = alloc;
    Value *idValue = (*assExpr).codeGen(context);
    return builder->CreateStore(idValue, context.locals()[id.name]);
  }
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

Value* If::codeGen(CodeGenContext &context) {
  Value* conditionValue = condition.codeGen(context);

  IRBuilder<> *builder = context.currentBuilder();
  Function *mainFunction = builder->GetInsertBlock()->getParent();

  BasicBlock *thenBb = BasicBlock::Create(getGlobalContext(), "then",
      mainFunction);
  BasicBlock *mergeBb = BasicBlock::Create(getGlobalContext(), "ifcont");
  BasicBlock *elseBb = BasicBlock::Create(getGlobalContext(), "else");

  if (elseStmt == NULL) {
    std::cout << "Creating if  then "  << endl;
    builder->CreateCondBr(conditionValue, thenBb, mergeBb);

    builder->SetInsertPoint(thenBb);
    Value *thenValue = (*thenStmt).codeGen(context);
    builder->CreateBr(mergeBb);
    thenBb = builder->GetInsertBlock();

    mainFunction->getBasicBlockList().push_back(mergeBb);
    builder->SetInsertPoint(mergeBb);
    return NULL;

  } else {
    cout << "Creating if then else" << endl;
    builder->CreateCondBr(conditionValue, thenBb, elseBb);

    builder->SetInsertPoint(thenBb);
    Value *thenValue = (*thenStmt).codeGen(context);
    builder->CreateBr(mergeBb);
    thenBb = builder->GetInsertBlock();

    mainFunction->getBasicBlockList().push_back(elseBb);
    builder->SetInsertPoint(elseBb);
    Value *elseValue = (*elseStmt).codeGen(context);
    builder->CreateBr(mergeBb);
    elseBb = builder->GetInsertBlock();

    mainFunction->getBasicBlockList().push_back(mergeBb);
    builder->SetInsertPoint(mergeBb);
    return NULL;
   }
 }

Value* While::codeGen(CodeGenContext &context) {

  //Value* conditionValue = condition.codeGen(context);

  IRBuilder<> *builder = context.currentBuilder();
  Function *mainFunction = builder->GetInsertBlock()->getParent();

  BasicBlock *condBb = BasicBlock::Create(getGlobalContext(), "whilecond",
      mainFunction);
  BasicBlock *doBb = BasicBlock::Create(getGlobalContext(), "whiledo");
  BasicBlock *afterWhileBb = BasicBlock::Create(getGlobalContext(), "afterWhile");


  builder->CreateBr(condBb);
  builder->SetInsertPoint(condBb);
  Value* conditionValue = condition.codeGen(context);
  builder->CreateCondBr(conditionValue, doBb, afterWhileBb);
  condBb = builder->GetInsertBlock();

  mainFunction->getBasicBlockList().push_back(doBb);
  builder->SetInsertPoint(doBb);
  Value *doValue = (*doStmt).codeGen(context);
  builder->CreateBr(condBb);
  doBb = builder->GetInsertBlock();

  mainFunction->getBasicBlockList().push_back(afterWhileBb);
  builder->SetInsertPoint(afterWhileBb);
  return NULL;

}

Value* MethodCall::codeGen(CodeGenContext &context) {
  Function *function = context.module->getFunction(methodId.name.c_str());
  assert( function != NULL );
  std::vector<Value*> args;

  IRBuilder<> *builder = context.currentBuilder();

  ArgsList::const_iterator it;

  for (it = argsList.begin(); it != argsList.end(); it++) {
    args.push_back((**it).codeGen(context));
  }

  return builder->CreateCall(function, ArrayRef<Value*>(args) 
      , methodId.name.c_str());
}

Value* Return::codeGen(CodeGenContext &context) {
  IRBuilder<> *builder = context.currentBuilder();
  Value *returnValue = returnExpr.codeGen(context);
  return builder->CreateRet(returnValue);
}

Value* FunctionDef::codeGen(CodeGenContext &context) {
  IRBuilder<> *builder = context.currentBuilder();
  std::vector<Type*> argsType;
  ArgsDefList::const_iterator it;

  for (it = argsDefList.begin(); it != argsDefList.end(); it++) {
    argsType.push_back(builder->getInt64Ty());
  }

  FunctionType *ftype = FunctionType::get(builder->getInt64Ty()
      , ArrayRef<Type*>(argsType), false);

  Function *function = Function::Create(ftype, GlobalValue::InternalLinkage
      , functionName.name.c_str(), context.module);

  BasicBlock *fBBlock = BasicBlock::Create(getGlobalContext()
      , "entry", function, 0);
  context.pushBlock(fBBlock);

  builder = context.currentBuilder();
  Function::arg_iterator args = function->arg_begin();

  for (it = argsDefList.begin(); it != argsDefList.end(); it++) {
    Value* a = args++;
    Id &argId = (**it);
    cout<<"argument name is "<<argId.name.c_str()<<endl;
    a->setName(argId.name.c_str());

    Value *alloc = builder->CreateAlloca(builder->getInt64Ty(), 0 , argId.name.c_str());
    context.locals()[argId.name] = alloc;
    builder->CreateStore(a, context.locals()[argId.name]);
  }

  (*functionBlock).codeGen(context);
  context.popBlock();
  return NULL;
}
