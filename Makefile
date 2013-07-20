exe_name := gdeh

ifdef DEBUG
debug := -g
else
debug :=
endif

all: gdeh.c
	gcc -Wall $(debug) -o $(exe_name) gdeh.c

clean:
	rm -f $(exe_name) gdeh.o

vg: all
	valgrind --leak-check=full ./$(exe_name) 127.0.0.1

