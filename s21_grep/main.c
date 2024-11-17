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

    string = xrealloc(string, *size + size_expr + 7); //+2 kruglie skobochki i null termintaor
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
    flags.pattern[0] = '\0'; // |pattern1|pattern2
    flags.pattern[1] = '\0'; 
    size_t pattern_size = 0;
    for (;currentFlag != -1
         ;currentFlag = getopt_long(argc, argv, "e:ivclno", 0, 0)) {
        switch (currentFlag)
        {
            break; case 'e':
                flags.pattern = string_append_expr(flags.pattern, &pattern_size, optarg, strlen(optarg));
                // flags.regex_flag |= REG_EXTENDED;      //логическое или
            break; case 'i':
                flags.regex_flag |= REG_ICASE;      //логическое или
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
    if (count_file == 1) {
        printf("%i\n",count); // nado dobavit uslvoie kogda failov bolshe 1

    }
    else 
        printf("%s:%i\n", filename, count); // nado dobavit uslvoie kogda failov bolshe 1
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
    char **pattern = &argv[1];
    char **end = &argv[argc];
    // int count = 0;
    regex_t preg_storage;
    regex_t *preg = &preg_storage;
    if (flags.size == 0) {
        // for (;pattern[0][0] == '-' && pattern != end;++pattern)
        //     ;
        // if (pattern == end) {
        //     fprintf(stderr, "no pattern\n");
        //     exit(1);
        // }
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

    // for (char **filename = argv + 1;filename != end ; ++filename) {
    //     if (**filename == '-')
    //         continue;   
    //     ++count;
    //     if (count >= 2) {
    //         break;
    //     }
    // }

    if (argc == (flags.size ? 2 : 1)){
        if (flags.count) {
            GrepCount(stdin, "", flags, preg, 1);
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
// znacheniya po umolchaniy dlya table ascii
#if 0
void CatSetTable(const char *table[static 256]) {

    const char *rawTable[] = {

        "\00", "\01", "\02", "\03", "\04", "\05", "\06", "\07", "\b", "\t",
        "\n", "\v", "\014", "\r", "\016", "\017", "\020", "\021", "\022", "\023",
        "\024", "\025", "\026", "\027", "\030", "\031", "\032", "\033", "\034", "\035",
        "\036", "\037", " ", "!", "\"", "#", "$", "%", "&", "\'",
        "(", ")", "*", "+", ",", "-", ".", "/", "0", "1",
        "2", "3", "4", "5", "6", "7", "8", "9", ":", ";",
        "<", "=", ">", "?", "@", "A", "B", "C", "D", "E",
        "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
        "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y",
        "Z", "[", "\\", "]", "^", "_", "`", "a", "b", "c",
        "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
        "n", "o", "p", "q", "r", "s", "t", "u", "v", "w",
        "x", "y", "z", "{", "|", "}", "~", "\x7f", "\x80", "\x81",
        "\x82", "\x83", "\x84", "\x85", "\x86", "\x87", "\x88", "\x89", "\x8a", "\x8b",
        "\x8c", "\x8d", "\x8e", "\x8f", "\x90", "\x91", "\x92", "\x93", "\x94", "\x95",
        "\x96", "\x97", "\x98", "\x99", "\x9a", "\x9b", "\x9c", "\x9d", "\x9e", "\x9f",
        "\xa0", "\xa1", "\xa2", "\xa3", "\xa4", "\xa5", "\xa6", "\xa7", "\xa8", "\xa9",
        "\xaa", "\xab", "\xac", "\xad", "\xae", "\xaf", "\xb0", "\xb1", "\xb2", "\xb3",
        "\xb4", "\xb5", "\xb6", "\xb7", "\xb8", "\xb9", "\xba", "\xbb", "\xbc", "\xbd",
        "\xbe", "\xbf", "\xc0", "\xc1", "\xc2", "\xc3", "\xc4", "\xc5", "\xc6", "\xc7",
        "\xc8", "\xc9", "\xca", "\xcb", "\xcc", "\xcd", "\xce", "\xcf", "\xd0", "\xd1",
        "\xd2", "\xd3", "\xd4", "\xd5", "\xd6", "\xd7", "\xd8", "\xd9", "\xda", "\xdb",
        "\xdc", "\xdd", "\xde", "\xdf", "\xe0", "\xe1", "\xe2", "\xe3", "\xe4", "\xe5",
        "\xe6", "\xe7", "\xe8", "\xe9", "\xea", "\xeb", "\xec", "\xed", "\xee", "\xef",
        "\xf0", "\xf1", "\xf2", "\xf3", "\xf4", "\xf5", "\xf6", "\xf7", "\xf8", "\xf9",
        "\xfa", "\xfb", "\xfc", "\xfd", "\xfe", "\xff"};

        memcpy(table, rawTable, sizeof rawTable);
}


void CatSetEndl(const char *table[static 256]) {
    table['\n'] = "$\n";
}

void CatSetTab(const char *table[static 256]) {
    table['\t'] = "^I";
}


void CatSetNonPrintable(const char *table[static 256]) {
    const char *sample1[] = {"^@", "^A", "^B", "^C", "^D", "^E", "^F", "^G", "^H"};
    const char *sample2[] = {
        "^K", "^L", "^M", "^N", "^O", "^P", "^Q", "^R", "^S",
        "^T", "^U", "^V", "^W", "^X", "^Y", "^Z", "^[", "^\\", "^]",
        "^^", "^_"
    };
    const char *sample3[] = {
        "^?", "M-^@", "M-^A",
        "M-^B", "M-^C", "M-^D", "M-^E", "M-^F", "M-^G", "M-^H", "M-^I", "M-^J", "M-^K",
        "M-^L", "M-^M", "M-^N", "M-^O", "M-^P", "M-^Q", "M-^R", "M-^S", "M-^T", "M-^U",
        "M-^V", "M-^W", "M-^X", "M-^Y", "M-^Z", "M-^[", "M-^\\", "M-^]", "M-^^", "M-^_",
        "M- ", "M-!", "M-\"", "M-#", "M-$", "M-%", "M-&", "M-'", "M-(", "M-)",
        "M-*", "M-+", "M-,", "M--", "M-.", "M-/", "M-0", "M-1", "M-2", "M-3",
        "M-4", "M-5", "M-6", "M-7", "M-8", "M-9", "M-:", "M-;", "M-<", "M-=",
        "M->", "M-?", "M-@", "M-A", "M-B", "M-C", "M-D", "M-E", "M-F", "M-G",
        "M-H", "M-I", "M-J", "M-K", "M-L", "M-M", "M-N", "M-O", "M-P", "M-Q",
        "M-R", "M-S", "M-T", "M-U", "M-V", "M-W", "M-X", "M-Y", "M-Z", "M-[",
        "M-\\", "M-]", "M-^", "M-_", "M-`", "M-a", "M-b", "M-c", "M-d", "M-e",
        "M-f", "M-g", "M-h", "M-i", "M-j", "M-k", "M-l", "M-m", "M-n", "M-o",
        "M-p", "M-q", "M-r", "M-s", "M-t", "M-u", "M-v", "M-w", "M-x", "M-y",
        "M-z", "M-{", "M-|", "M-}", "M-~", "M-^?"
    };  
    
    memcpy(table, sample1, sizeof sample1);
    memcpy(&table[9 + 2], sample2, sizeof sample2);
    memcpy(&table['~' + 1], sample3, sizeof sample3);
}
#endif

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
