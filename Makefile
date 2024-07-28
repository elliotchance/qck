C_FILES = main.c expr.c func.c program.c type.c var.c lexer.c parse.c

qck: $(C_FILES)
	clang $(C_FILES) -o qck

test: qck
	./qck tests/hello_world.qck
	./qck tests/values.qck
