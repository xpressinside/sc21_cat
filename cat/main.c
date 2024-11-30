#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>


// dlya numberNonBlank. numberAll, squeeze menyaem sposob vivoda
// dlya markEndl, tab, printNonPrintable menyaem znaki v tablice ascii

typedef struct {
    bool numberNonBlank;            //b  done
    bool markEndl;                  //e + v, E  done 
    bool numberAll;                 //n  done
    bool squeeze;                   //s  ????done?????
    bool tab;                       //t + v, T  done
    bool printNonPrintable;         //v     done
    bool moreOneFile;
    bool flagsWasRead;
}Flags;

Flags CatReadFlags(int argc, char *argv[]);
void CatReadPrintFile(FILE *file, Flags flags, int *linenumber, int *last, bool *firstLine, bool *squeeze);
void CatOpenFile(int argc, char *argv[], Flags flags);

int main(int argc, char *argv[]) {
    if (argc == 1) {
        char input[256];
        while (fgets(input, sizeof(input), stdin) != NULL) {
            printf("%s", input);
        }
    }
    Flags flags = CatReadFlags(argc, argv);      
    CatOpenFile(argc, argv, flags);
    return 0;
}

// obrabotka flagov
Flags CatReadFlags(int argc, char *argv[]) {
    struct option longFlags[] = {
        {"number-nonblank", 0, NULL, 'b'},
        {"number", 0, NULL, 'n'},
        {"squeeze-blank", 0, NULL, 's'},
        {NULL, 0, NULL, 0}
    };

    Flags flags = {
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
    while ((currentFlag = getopt_long(argc, argv, "bevEnstT", longFlags, NULL)) != -1)
        {
            switch (currentFlag)
            {
                case 'b':
                flags.numberNonBlank = true;
                break; case 'e':
                flags.markEndl = true;
                flags.printNonPrintable = true;
                break; case 'v':
                flags.printNonPrintable = true;
                break; case 'E':
                flags.markEndl = true;
                break; case 'n':
                flags.numberAll = true;
                break; case 's':
                flags.squeeze = true;
                break; case 't':
                flags.tab = true;
                flags.printNonPrintable = true;
                break; case 'T':
                flags.tab = true;
                break;default:
                fprintf(stderr, "Try %s [ -b | -e | -E | -n | -s | -t | -T ]\n", argv[0]);
                exit(1);
            }
        }
    flags.flagsWasRead = true;
    return flags;

}

// chitaem otriki fail i vivdoim soderzhimoe soglasno flagam
void CatReadPrintFile(FILE *file, Flags flags, int *linenumber, int *last, bool *firstLine, bool *squeeze) {
    (void)flags;
    int c = 0;
    int locallast = '\n';
    while (fread(&c, 1, 1, file) > 0) {        
        if (locallast == '\n') {
            if (flags.squeeze && c == '\n' && *last == '\n') {
                if (*last != '\n' && *firstLine) {
                    continue;
                }
                if (*squeeze) {
                    continue;
                }
                *squeeze = true;
            } else {
                *squeeze = false;               
            }            
            if (flags.numberNonBlank) {
                if (c != '\n') {
                    printf("%6i\t", ++*linenumber);                
                }
            } else if (flags.numberAll) {
                    if (*firstLine) {
                        *firstLine = false;
                    } else {
                        printf("%6i\t", ++*linenumber);
                    }
                }
        }
        if (flags.markEndl && c == '\n') {
            printf("$");
        }
        if (flags.tab && c == '\t') {
            printf("^I");
        } else {
            if (flags.printNonPrintable) {
                if (isprint(c)) {
                    putchar(c);                
                } else {
                    if (c >= 128) {
                        printf("M-%c", c - 128);
                    } else {
                        switch (c) {
                        case '\n':
                            putchar('\n');
                            break;
                        case '\t':
                            putchar('\t');
                            break;
                        case '\r':
                            printf("^M");
                            break;
                        case 127:
                            printf("^?");
                            break;
                        default:
                            printf("^%c", c + '@');
                            break;
                        }
                    }
                    
                }
            }
            else {
                putchar(c);
            }
        }
        *last = c;
        locallast = c;
    }
    *firstLine = true;
}
// otkrivaem fail
void CatOpenFile(int argc, char *argv[], Flags flags) {
    int linenumber = 0;
    bool fileEnd = false;
    bool squeeze = false;
    int last = '\n';
    for (int i = optind; i < argc; i++) {
            
            char *filename = argv[i];
            FILE *file = fopen(filename, "rb");
            if (errno) {
                perror(filename);
                continue;
            }
            
            CatReadPrintFile(file, flags, &linenumber, &last, &fileEnd, &squeeze);
            
            fclose(file);
        }
}
