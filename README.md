# 支持语法
- 函数声明
- 变量声明
- 函数调用
- IF-THEN/ IF-THEN-ELSE
- While 循环
- 加减乘除 大于 小于的 表达式求值
- putchar/getchar 内建函数

# 依赖
- llvm 3.0


# 使用
```bash
  echo "var a = 97;putchar(97);" | ./toy
  cat ./example/while | ./toy
```

# 目录
- ./example 例子
- codegen.cpp emit llvm ir代码
- node.hpp ast节点对象
