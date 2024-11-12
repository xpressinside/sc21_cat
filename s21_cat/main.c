#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>


// int getopt_long(int argcm ,char *const argv[],
//                 const char *optstring,
//                 const struct option *longopts, int *longindex);

typedef struct {
    bool numberNonBlank;
    bool markEndl;
    bool numberAll;
    bool squeeze;
    bool tab;
    bool printNonPrintable;
}Flags;

Flags CatReadFlags(int argc,char *argv[]){
    struct option longOptions[] = {{"number-nonblank", 0, NULL, 'b'},{"number", 0, NULL, 'n'},{"squeeze-blank", 0, NULL, 's'},{NULL, 0, NULL, 0}};

    int currentFlag = getopt_long(argc, argv, "bevEnstT", longOptions, NULL);

    Flags flags = {false, false, false, false, false, false};

    for (;currentFlag != -1; currentFlag = getopt_long(argc, argv, "bevEnstT", longOptions, NULL)) {
        switch (currentFlag)
        {
            break; case 'b':
            flags.numberNonBlank = true;
            break; case 'e':
            flags.markEndl = true;
            case 'v':
            flags.printNonPrintable = true;
            break; case 'E':
            flags.markEndl = true;
            break; case 'n':
            flags.numberAll = true;
            break; case 's':
            flags.squeeze = true;
            break; case 't':
            flags.printNonPrintable = true;
            case 'T':
            flags.tab = true;
        }
    }
    return flags;
}

int main(int argc, char *argv[]) {
    Flags flags = CatReadFlags(argc, argv);
    if (flags.numberNonBlank)
        printf("number non blank \n");
    if (flags.markEndl)
        printf("markEndl \n");
    if (flags.numberAll)
        printf("numberAll \n");
    if (flags.squeeze)
        printf("squeeze \n");
    if (flags.tab)
        printf("tab \n");
    if (flags.printNonPrintable)
        printf("printNonPrintable \n");

}