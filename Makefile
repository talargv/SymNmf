# Specify the target executable and the source files needed to build it
my_app: symnmf.o symnmf.h
	gcc -o my_app symnmf.o -ansi -Wall -Wextra -Werror -pedantic-errors -lm
    
# Specify the object files that are generated from the corresponding source files
symnmf.o: symnmf.c
	gcc -c symnmf.c -ansi -Wall -Wextra -Werror -pedantic-errors -lm
