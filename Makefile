all:	compiler_test

compiler_test:
	cc -o compilertest compiler.c symboltable.c linkedlist.c tokenize.c find_symbols.c

clean:
	rm compilertest
