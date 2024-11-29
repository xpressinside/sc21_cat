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

// combo flags, -iv+

typedef struct {
    //char *pattern;                  // -e pattern
    //size_t size;            // -e pattern
    int regExtendedIcase;           //+ -e pattern REGEX
    //int ignoreUpperLowerCase;     //+ -i ignore upper_lower_case in patter and file REGEX
    bool invert;                    //+ -v invert match, output lines without pattern
    bool countMatch;                //+ -c output number matched lines
    bool matchNames;                //+ -l output file name if patter found in file
    bool lineMatch;                 //+ -n print each matched line number
    bool noFileName;                //+  dop zadanie -h output matched line without name of files
    bool supressErrors;             //+  dop zadanie -s no errors just exit
    bool fileRegex;                 //  dop zadanie -f file   regex from file 
    bool outputFullMatch;           //  dop zadanie -o output full matched parts of files 
        
}Flags;

// obrabotka flagov
Flags CatReadFlags(int argc, char *argv[]) {


    Flags flags = {
        0,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false
    };

    for (int currentFlag = getopt_long(argc, argv, "e:ivclnhsof:", 0, 0);
        currentFlag != -1;
        currentFlag = getopt_long(argc, argv, "e:ivclnhsof:", 0, 0)) 
        {
            switch (currentFlag)
            {
                case 'e':
                flags.regExtendedIcase |= REG_EXTENDED;
                break; case 'i':
                flags.regExtendedIcase |= REG_ICASE;
                break; case 'v':
                flags.invert = true;
                break; case 'c':
                flags.countMatch = true;
                break; case 'l':
                flags.matchNames = true;
                break; case 'n':
                flags.lineMatch = true;
                break; case 'h':
                flags.noFileName = true;
                break; case 's':
                flags.supressErrors = true;
                break; case 'o':
                flags.outputFullMatch = true;
                break; case 'f':
                flags.fileRegex = true;
            }
        }
    return flags;

}

void GrepCountReadPrintFile(FILE *file, Flags flags, regex_t *preg, int count_file, const char *filename) {
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
    if (count_file == 2 && !(flags.noFileName)) {
        printf("%s:%i\n", filename, count);
    }
    else {
        printf("%i\n",count); 
    }
    free(line);
}

void GrepReadPrintFileWithPatterns(FILE *file, Flags flags, regex_t *preg, int count_file, char *filename, int totalPatterns) {
    (void)flags;
    char *line = 0;
    size_t length = 0;
      // dlya obrabotki sovpadenii
    bool namePrinted = false;
    int count = 0;
    
    while (getline(&line, &length, file) > 0) {
        ++count;
        bool lineMatch = false;
        
        for (int i = 0; i < totalPatterns; ++i) {
            regmatch_t match;
            if (flags.invert) {
                if (regexec(&preg[i], line, 1, &match, 0)) {
                    lineMatch = true;
                }
            }
            else {
                if (!regexec(&preg[i], line, 1, &match, 0)) {
                    lineMatch = true;
                    break;
                }
            }
        }
        if (lineMatch || flags.invert) {
            if (flags.matchNames && !namePrinted) {
                printf("%s\n", filename);
                namePrinted = true;
            }
            if (flags.lineMatch) {
                if (count_file == 2 && !(flags.noFileName)) {
                    printf("%s:%i:%s", filename, count, line);
                }
                else {
                    printf("%i:%s", count, line);
                }
            }
            else {
                if (count_file == 2 && !(flags.noFileName)) {
                    printf("%s:%s", filename, line);
                }
                else {
                    printf("%s", line);
                }
            }
        }
    }
    
    free(line);
    
}

void GrepReadPrintFile(FILE *file, Flags flags, regex_t *preg, int count_file, char *filename) {
    (void)flags;
    char *line = 0;
    size_t length = 0;
    regmatch_t match;       // dlya obrabotki sovpadenii
    bool namePrinted = false;
    int count = 0;
    
    while (getline(&line, &length, file) > 0) {
        ++count;
        bool lineMatch = false;
        if (flags.invert) {
            if (regexec(preg, line, 1, &match, 0)) {
                if (flags.outputFullMatch) {
                    ;
                }
                else {
                    if (flags.matchNames && !namePrinted) {
                        printf("%s\n", filename);
                        namePrinted = true;
                    } 
                    else if (flags.matchNames && namePrinted){
                        continue;
                    }
                    else {
                        if (flags.lineMatch) {
                            if (count_file == 2 && !(flags.noFileName)) {
                                printf("%s:%i:%s", filename, count, line);

                            }
                            else {
                                printf("%i:%s", count, line);
                            }
                        }
                        else {
                            if (count_file == 2 && !(flags.noFileName)) {
                                printf("%s:%s", filename, line);
                            }
                            else {
                                printf("%s", line);
                            }
                        }
                    }
                }                    
            }
        }
        else {
            if (!regexec(preg, line, 1, &match, 0)) {
                if (flags.outputFullMatch) {
                    //printf("test1\n");
                    //printf("%b",flags.matchNames);
                    if (flags.matchNames && !namePrinted) {
                        printf("%s\n", filename);
                        namePrinted = true;
                    }
                    else if (flags.matchNames && namePrinted){
                        continue;
                    }
                    else {
                        if (flags.lineMatch) {
                            if (count_file == 2 && !(flags.noFileName)) {
                                printf("%s:%i:%.*s\n", filename, count, match.rm_eo - match.rm_so, line + match.rm_so);
                            }
                            else {
                                printf("%i:%.*s\n", count, match.rm_eo - match.rm_so, line + match.rm_so);
                            }
                        }
                        else {
                            if (count_file == 2 && !(flags.noFileName)) {
                                printf("%s:%.*s\n", filename, match.rm_eo - match.rm_so, line + match.rm_so);
                            }
                            else {
                                printf("%.*s\n", match.rm_eo - match.rm_so, line + match.rm_so);
                            }
                        }
                        char *remain = line + match.rm_eo;
                        while (!regexec(preg, remain, 1, &match, 0)) {
                            if (flags.lineMatch) {
                                if (count_file == 2 && !(flags.noFileName)) {
                                    printf("%s:%i:%.*s\n", filename, count, match.rm_eo - match.rm_so, remain + match.rm_so);
                                }
                                else {
                                    printf("%i:%.*s\n", count, match.rm_eo - match.rm_so, remain + match.rm_so);
                                }
                            }
                            else {
                                if (count_file == 2 && !(flags.noFileName)) {
                                    printf("%s:%.*s\n", filename, match.rm_eo - match.rm_so, remain + match.rm_so);
                                }
                                else {
                                    printf("%.*s\n", match.rm_eo - match.rm_so, remain + match.rm_so);
                                }
                            }
                            remain = remain + match.rm_eo;
                            //printf("number 1 \n");
                        }
                    }
                }
                else {
                    if (flags.matchNames && !namePrinted) {
                        printf("%s\n", filename);
                        namePrinted = true;
                    }
                    else if (flags.matchNames && namePrinted){
                        continue;
                    }
                    else {
                        if (flags.lineMatch) {
                            if (count_file == 2 && !(flags.noFileName)) {
                                printf("%s:%i:%s", filename, count, line);
                            }
                            else {
                                printf("%i:%s", count, line);
                            }
                        }
                        else {
                            if (count_file == 2 && !(flags.noFileName)) {
                                printf("%s:%s", filename, line);
                            }
                            else {
                                printf("%s", line);
                            }
                        }
                    }
                }                    
            }
        }
    }
        
        // if (!regexec(preg, line, 1, &match, 0)) {
        //     printf("%s", line);
        // }
    
    free(line);
}

void GrepOpenFile(int argc, char *argv[], Flags flags) {
    

    regex_t preg_storage;
    regex_t *preg = &preg_storage;
    char **pattern = argv + 1;
    char **end = &argv[argc];
    int count = 0;
    if (argc == 1 || (argc == 2 && *argv[1] == '-')) {
        fprintf(stderr,"Usage: s21_grep [OPTION]... PATTERNS [FILE]...\n");
        exit(1);
    }    

    for (;pattern != end && pattern[0][0] == '-'; ++pattern)
        ;
    if (pattern == end) {        
        fprintf(stderr, "no pattern\n");
        exit(1);
    }
    int i = 0;
    regex_t regexArray[100];
    if (flags.fileRegex) {
        FILE *regFile = fopen(*pattern, "r");
        if (!regFile) {
            perror("ERROR");
            exit(1);
        }
        char *filePatterns = NULL;
        size_t lengthFP = 0;

        while (getline(&filePatterns, &lengthFP, regFile) != -1) {
            filePatterns[strcspn(filePatterns,"\n")] = 0;
            if (regcomp(&regexArray[i], filePatterns, 0) != 0) {
                fprintf(stderr,"error compile regex");
                exit(1);
            }
            (i)++;
        }
        free(filePatterns);
        fclose(regFile);
        pattern += 1;
 
    }
    
    //printf("%s\n", *pattern);
    if (regcomp(preg, *pattern, flags.regExtendedIcase)) {
        fprintf(stderr, "fail compile regex \n");
        exit(1);
    }
    if (!flags.fileRegex) {
        pattern += 1;
    }
    for (char **filename = pattern;
        filename != end;
        ++filename) {
            if (**filename == '-') {
                continue;
            }
            ++count;
            if (count >= 2){
                break;
            }
        }
    
    if (argc == 2 || (argc == 3 && *argv[1] == '-' && argv[3] == NULL)) {
        //char input[256];
        //while (fgets(input, sizeof(input), stdin) != NULL) {
        while (!NULL) {
            if (flags.fileRegex) {
                if (flags.countMatch){
                    GrepCountReadPrintFile(stdin, flags, regexArray, count, NULL);
                }
                else {
                    GrepReadPrintFileWithPatterns(stdin, flags, regexArray, count, NULL, i);
                }            
            } else {
                if (flags.countMatch){
                    GrepCountReadPrintFile(stdin, flags, preg, count, NULL);
                }
                else {
                    GrepReadPrintFile(stdin, flags, preg, count, NULL);
                }
            }
        }
    }
    //printf("%i\n", count);
    for (char **filename = pattern; filename != end; ++filename) {
        if (**filename == '-') {
            continue;
        }
        FILE *file = fopen(*filename, "rb");
        if (errno && !(flags.supressErrors)) {
            fprintf(stderr, "%s", argv[0]);
            perror(*filename);
            continue;
        } 
        else if (errno && flags.supressErrors){
            exit(1);
        }
        //printf("%s\n", *filename);
        if (flags.fileRegex) {
            if (flags.countMatch){
                GrepCountReadPrintFile(file, flags, regexArray, count, *filename);
            }
            else {
                GrepReadPrintFileWithPatterns(file, flags, regexArray, count, *filename, i);
            }            
        } else {
            if (flags.countMatch){
                GrepCountReadPrintFile(file, flags, preg, count, *filename);
            }
            else {
                GrepReadPrintFile(file, flags, preg, count, *filename);
            }
        }

        fclose(file);
    }
    regfree(preg);
}


int main(int argc, char *argv[]) {
    Flags flags = CatReadFlags(argc, argv);
    // Flags flags = {};
    
    GrepOpenFile(argc, argv, flags);
}
