CC=gcc
CFLAGS=-Wall -Wextra -Werror
BUILD=../src/cat/
STD=-std=c11

s21grep: $main.c
	$(CC) $(CFLAGS) $(STD) -c $main.c -o ($BUILD)s21_grep

clean:
	rm -rf s21_grep

rebuild: clean s21grep
