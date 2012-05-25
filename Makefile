all: toy

OBJS = parser.tab.o \
			 lex.o \



cl: clean

clean:
	@$(RM) -rf ${OBJS} lex.cpp lex parser.tab.hpp parser.tab.cpp toy

parser.tab.hpp: parser.tab.cpp

lex.cpp: lex.l parser.tab.hpp
	flex -o $@ $^

parser.tab.cpp: parser.y
	bison -d -o $@ $^

%.o: %.cpp
	g++ -o $@ ${CPPFLAGS} -c $<

toy: $(OBJS)
	g++ -o $@ $(OBJS)
