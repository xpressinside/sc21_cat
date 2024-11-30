CC=gcc
CFLAGS=-Wall -Wextra -Werror
BUILD=../src/cat/
STD=-std=c11

s21_cat: $main.c
	$(CC) $(CFLAGS) $(STD) -c $main.c -o ($BUILD)s21_cat

clean:
	rm -rf s21_cat

rebuild: clean s21_cat
