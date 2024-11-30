#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>


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
void CatReadPrintFile(FILE *file, Flags flags, const char *table[static 256], int *linenumber);
void CatOpenFile(int argc, char *argv[], Flags flags, const char *table[static 256]);
void CatSetTable(const char *table[static 256]);
void CatNonPrintable(const char *table[static 256]);

int main(int argc, char *argv[]) {
    if (argc == 1) {
        char input[256];
        while (fgets(input, sizeof(input), stdin) != NULL) {
            printf("%s", input);
        }
    }
    Flags flags = CatReadFlags(argc, argv);
    argc -= optind;
    argv += optind;
    const char *table[256];
    CatSetTable(table);
    if (flags.markEndl) {           //change \n na $\n
        table['\n'] = "$\n";
    }
    if (flags.printNonPrintable) {      // change table ascii to nonprintable ascii
        CatNonPrintable(table);
    }
    if (flags.tab) {                //change \t na "^I"
        table['\t'] = "^I";
    }
    CatOpenFile(argc, argv, flags, table);
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
void CatReadPrintFile(FILE *file, Flags flags, const char *table[static 256], int *linenumber) {
    int c = 0;
    int last;
    bool squeeze = false;
    last = '\n';
    while (fread(&c, 1, 1, file) > 0) {        
        if (last == '\n') {
            if (flags.squeeze && c == '\n') {
                if (squeeze)
                    continue;
                squeeze = true;
            }
            else
                squeeze = false;
            if (flags.numberNonBlank) {
                if (c != '\n')
                    printf("%6i\t", ++*linenumber);
            }
            else if (flags.numberAll) {
                printf("%6i\t", ++*linenumber);
            }
        }
        printf("%s", table[c]);
        last = c;
    }
    if (flags.moreOneFile)
        printf("\n");
}
// otkrivaem fail
void CatOpenFile(int argc, char *argv[], Flags flags, const char *table[static 256]) {
    if (flags.flagsWasRead && argv[0] == NULL) {
        int linenumber = 0;
        CatReadPrintFile(stdin, flags, table, &linenumber);
    }
    int linenumber = 0;
    
    for (char **filename = argv, **end = &argv[argc];
        filename != end;
        ++filename) {
            if (**filename == '-') {
                continue;
            }
            FILE *file = fopen(*filename, "rb");
            if (errno) {
                fprintf(stderr, "%s", argv[0]);
                perror(*filename);
                continue;
            }
            if (argv[1] != NULL) {
                flags.moreOneFile = true;
            }
            CatReadPrintFile(file, flags, table, &linenumber);
            fclose(file);
        }
}

// tablica ascii simvolov \0 eto NULL-terminator ispolzuetsa dlya oboznacheniya otdelnih simvolov
// \x simvoli s 127 po 255
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

void CatNonPrintable(const char *table[static 256]) {
    const char *s1[] = {"^@", "^A", "^B", "^C", "^D", "^E", "^F", "^G", "^H"};
    const char *s2[] = {
        "^K", "^L", "^M", "^N", "^O", "^P", "^Q", "^R", "^S",
        "^T", "^U", "^V", "^W", "^X", "^Y", "^Z", "^[", "^\\", "^]",
        "^^", "^_"
    };
    const char *s3[] = {
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
    
    memcpy(table, s1, sizeof s1);
    memcpy(&table[9 + 2], s2, sizeof s2);
    memcpy(&table['~' + 1], s3, sizeof s3);
}