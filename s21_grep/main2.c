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
    bool fileRegex;                 //+  dop zadanie -f file   regex from file 
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
    int currentFlag;
    //for (int currentFlag = getopt_long(argc, argv, "e:ivclnhsof:", 0, 0);
    //    currentFlag != -1;
    //    currentFlag = getopt_long(argc, argv, "e:ivclnhsof:", 0, 0)) 
    while ((currentFlag = getopt_long(argc, argv, "e:ivclnhsof:", 0, 0)) != -1)
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
                if (!(flags.noFileName) && count_file == 2) {
                    printf("%s:%i:%s", filename, count, line);
                }
                else {
                    printf("%i:%s", count, line);
                }
            }
            else {
                if (!(flags.noFileName) && count_file == 2) {
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
        else {
            if (!regexec(preg, line, 1, &match, 0)) {
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
        
        // if (!regexec(preg, line, 1, &match, 0)) {
        //     printf("%s", line);
        // }
    
    free(line);
}

void GrepOpenFile(int argc, char *argv[], Flags flags) {
   
    regex_t preg_storage;
    regex_t *preg = &preg_storage;
    //printf("%s\n", *argv);
    char **pattern = argv;
    //printf("%s\n", *pattern);
    char **end = &argv[argc];
    int count = 0;

    //printf("test 1\n");
    for (;pattern != end && pattern[0][0] == '-'; ++pattern)
        ;
    if (pattern == end) {        
        fprintf(stderr, "no pattern\n");
        exit(1);
    }
    int i = 0;
    regex_t regexArray[100];
    //printf("test 2\n");
    if (flags.fileRegex) {
        //printf("test 2.1\n");
        FILE *regFile = fopen(*pattern, "r");
        if (!regFile) {
            perror("ERROR");
            exit(1);
        }
        char *filePatterns = NULL;
        size_t lengthFP = 0;
        //printf("test 2.2\n");
        while (getline(&filePatterns, &lengthFP, regFile) != -1) {
            filePatterns[strcspn(filePatterns,"\n")] = 0;
            if (regcomp(&regexArray[i], filePatterns, 0) != 0) {
                fprintf(stderr,"error compile regex");
            }
            (i)++;
        }
        //printf("test 2.3\n");
        free(filePatterns);
        fclose(regFile);
        //pattern += 1;
        //printf("test 2.4\n");
    }
    
    //printf("%s\n", *pattern);
    //printf("test 3\n");
    //if (regcomp(preg, *pattern, flags.regExtendedIcase)) {
    //    fprintf(stderr, "fail compile regex \n");
    //    exit(1);
    //}
    //printf("test 4\n");
    for (char **filename = pattern + 1;
        filename != end;
        ++filename) {
            //printf("test 4.1\n");
            if (**filename == '-') {
                continue;
            }
            ++count;
            if (count >= 2){
                break;
            }
        }
    
    //printf("test 5\n");
    //printf("%s\n", *pattern);
    for (char **filename = pattern; filename != end; ++filename) {
        if (**filename == '-') {
            continue;
        }
        //printf("%s\n", *filename);
        FILE *file = fopen(*filename, "rb");
        //printf("test 5.1\n");
        if (errno && !(flags.supressErrors)) {
            //fprintf(stderr, " %s", argv[0]);
            perror(*filename);
            continue;
        } 
        else if (errno && flags.supressErrors){
            //printf("test 5.2\n");
            exit(1);
        }
        //printf("%s\n", *filename);
        //printf("test 5.3\n");
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
    if (argc == 1 || (argc == 2 && *argv[1] == '-')) {
        fprintf(stderr,"Usage: s21_grep [OPTION]... PATTERNS [FILE]...\n");
        exit(1);
    }
    else {
        if (argc == 2) {
            char input[256];
            while (fgets(input, sizeof(input), stdin) != NULL) {
                ;
            }
        }
    }
    Flags flags = CatReadFlags(argc, argv);
    argc -= optind;
    argv += optind;
    GrepOpenFile(argc, argv, flags);
}
