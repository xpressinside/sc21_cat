CC=gcc
CFLAGS=-Wall -Wextra -Werror
STD=-std=c11

s21_grep: main.c
	$(CC) $(CFLAGS) $(STD) main.c -o ../grep/s21_grep

clean:
	rm -rf s21_grep

rebuild: clean s21_grep
