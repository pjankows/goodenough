exe_name := gdeh

ifdef DEBUG
debug := -g
else
debug :=
endif

all: goodenough.c
	gcc -Wall $(debug) -o $(exe_name) goodenough.c

clean:
	rm -f $(exe_name) goodenough.o

vg: all
	valgrind --leak-check=full ./$(exe_name) 127.0.0.1 9090

err: all
	valgrind --leak-check=full ./$(exe_name) 127.0.0.1 80

