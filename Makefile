all: clean toy

OBJS = parser.tab.o \
			 codegen.o \
			 main.o \
			 lex.o \

LLVM_MODULES = core jit native

CPPFLAGS = `llvm-config --cppflags $(LLVM_MODULES)`
LDFLAGS = `llvm-config --ldflags $(LLVM_MODULES)`
LIBS = `llvm-config --libs $(LLVM_MODULES)`

cl: clean

m: parser

clean:
	@$(RM) -rf ${OBJS} lex.cpp lex parser.tab.hpp parser.tab.cpp toy parser.output

parser.tab.hpp: parser.tab.cpp

lex.cpp: lex.l parser.tab.hpp
	flex -o $@ $^

parser.tab.cpp: parser.y
	bison -d -o $@ $^ -v

%.o: %.cpp
	g++ -o $@ ${CPPFLAGS} -c $<

toy: $(OBJS)
	g++ -o $@ $(LDFLAGS) $(OBJS) $(LIBS) -ldl -pthread


t.var_decl:
	cat ./example/var_decl | ./toy

t.if:
	cat ./example/if | ./toy

t.if2:
	cat ./example/if2 | ./toy

t.call:
	cat ./example/call | ./toy

t.while:
	cat ./example/while | ./toy

t.func:
	cat ./example/func | ./toy
