#include <iostream>
#include "codegen.hpp"
#include "node.hpp"
using namespace std;

extern int yyparse();
extern Block *programBlock;
int main(int argc, char **argv) {
  yyparse();
  cout<< programBlock << endl;

  InitializeNativeTarget();
  CodeGenContext context;
  context.generateCode(*programBlock);
  context.runCode();

  return 0;
}
