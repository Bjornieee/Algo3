CC = gcc
CFLAGS = -std=c99 -Werror -Wextra -Wall -pedantic
EXEC = exo2_TP1

ifeq ($(DEBUG),yes)
	CFLAGS+= -g
else
	CFLAGS+= -O3 -DNDEBUG
endif
all : $(EXEC)
$(EXEC) : main.o dynamicstack.o
	$(CC) -o $@ $^
%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS)
.PHONY : clean mrproper
clean :
	rm -rf main.o dynamicstack.o exo1_TP1
mrproper : clean
	@rm -rf $(EXEC)
doc : main.c stack.h
	doxygen documentation/TP1
## Construction des dépendances
main.o : stack.h