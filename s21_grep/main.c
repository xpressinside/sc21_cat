#define _GNU_SOURCE //getline
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include <stdlib.h>
#include <limits.h>

typedef struct {
    char *pattern;
    size_t size;
    int regex_flag; // flag e,i
    bool invert;
    bool count;
    bool filesMatch;
    bool numberLine;
    bool printMatched;

}Flags;
//  -o

void *xmalloc(size_t size) {
    void *temp;
    temp = malloc(size);
    if (!temp)
        exit(errno);
    return temp;
}

void *xrealloc(void *block, size_t size) {
    void *temp;
    temp = realloc(block, size);
    if (!temp)
        exit(errno);
    return temp;
}

char *string_append_expr(char *string, size_t *size,const char *expr, size_t size_expr) {

    string = xrealloc(string, *size + size_expr + 7);
    string[*size] = '\\';
    string[*size + 1] = '|';
    string[*size + 2] = '\\';
    string[*size + 3] = '(';
    memcpy(string + *size + 4, expr, size_expr);
    *size += size_expr + 4;
    string[*size] = '\\';
    string[*size + 1] = ')';
    string[*size + 2] = '\0';
    *size += 2;
    return string;
}

Flags GrepReadFlags(int argc,char *argv[]){
    Flags flags = {NULL, 0, 0, false, false, false, false, false};
    int currentFlag = getopt_long(argc, argv, "e:ivclno", 0, 0);
    flags.pattern = xmalloc(2);
    flags.pattern[0] = '\0';
    flags.pattern[1] = '\0'; 
    size_t pattern_size = 0;
    for (;currentFlag != -1
         ;currentFlag = getopt_long(argc, argv, "e:ivclno", 0, 0)) {
        switch (currentFlag)
        {
            break; case 'e':
                flags.pattern = string_append_expr(flags.pattern, &pattern_size, optarg, strlen(optarg));
            break; case 'i':
                flags.regex_flag |= REG_ICASE;   
            break; case 'v':
                flags.invert = true;
            break; case 'c':
                flags.count = true;
            break; case 'l':
                flags.filesMatch = true;
            break; case 'n':
                flags.numberLine = true;           
            break; case 'o':
                flags.printMatched = true;   
        }
    }
    if (pattern_size) {
        flags.size = pattern_size;
    }
    flags.size = pattern_size;
    return flags;
}

void GrepCount(FILE *file, const char *filename, Flags flags, regex_t *preg, int count_file) {
    (void)flags;
    (void)filename;
    char *line = 0;
    size_t length = 0;
    regmatch_t match;
    int count = 0;
    while (getline(&line, &length, file) > 0) {
        if (!regexec(preg, line, 1, &match, 0)){
            ++count;
        }
    }
    if (count_file == 2) {
        printf("%i\n",count);
    }
    else 
        printf("%s:%i\n", filename, count);
    free(line);
}


void GrepFile(FILE *file, Flags flags, regex_t *preg, char *filename) {
    (void)flags;
    char *line = 0;
    size_t length = 0;
    regmatch_t match;
    int count = 0;
    while (getline(&line, &length, file) > 0) {
        ++count;
        if (flags.invert) {                                         // invert
            if (regexec(preg, line, 1, &match, 0)){
               if (flags.printMatched) {
                    ;
                }
                else {
                    if (flags.numberLine) {
                        printf("%s:%i:%s", filename, count, line);

                    }
                    else
                        printf("%s", line);

                }
            }    
        }
        else
        {
            if (!regexec(preg, line, 1, &match, 0)){
                if (flags.printMatched) {
                    if (flags.numberLine) {
                        printf("%s:%i:%.*s", filename, count, match.rm_eo - match.rm_so, line + match.rm_so);
                    }
                    else {
                        printf("%.*s\n", match.rm_eo - match.rm_so, line + match.rm_so);
                    }
                    char *remaining = line + match.rm_eo;
                    while (!regexec(preg, remaining, 1, &match, 0)) {
                        if (flags.numberLine) {
                            printf("%s:%i:%.*s", filename, count
                            , match.rm_eo - match.rm_so, remaining + match.rm_so);
                        }
                        else {
                            printf("%.*s\n", match.rm_eo - match.rm_so, remaining + match.rm_so);
                        }                        
                        remaining = line + match.rm_eo;
                    }
                }
                else {
                    if (flags.numberLine) {
                        printf("%s:%i:%s", filename, count, line);
                    }
                    else
                        printf("%s", line);
                }
            }
        }

    }
    free(line);
}

void Grep(int argc, char *argv[], Flags flags) {

    char **end = &argv[argc];
    regex_t preg_storage;
    regex_t *preg = &preg_storage;
    if (flags.size == 0) {
        if (regcomp(preg, argv[0], flags.regex_flag)) {
            fprintf(stderr, "failed to compile regex\n");
            exit(1);
        }
    }
    else {
        if (regcomp(preg, flags.pattern + 2, flags.regex_flag)) {
            fprintf(stderr, "failed to compile regex\n");
            exit(1);
        }
    }
    free(flags.pattern);
    if (argc == (flags.size ? 2 : 1)){
        if (flags.count) {
            GrepCount(stdin, "", flags, preg, 2);
        }
        else
            GrepFile(stdin, flags, preg, "");
    }

    for (char **filename = argv + (flags.size ? 0 : 1);filename != end ; ++filename) {
        FILE *file = fopen(*filename, "rb");
        if (errno) {
            fprintf(stderr,"%s", argv[0]);
            perror(*filename);
            continue;
        }
        if (flags.count) {

            GrepCount(file, *filename, flags, preg, argc);
        }
        else {
            GrepFile(file, flags, preg, *filename);

        }
        fclose(file);
    }
    regfree(preg);
}

int main(int argc, char *argv[]) {
    Flags flags = GrepReadFlags(argc, argv);
    argv += optind;
    argc -= optind;
    if (argc == 0) {
        fprintf(stderr, "no pattern\n");
        exit(1);
    }
    Grep(argc, argv, flags);
}
