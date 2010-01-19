#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MAX_SCRIPT_SIZE			4096
#define MAX_REGBUTTON_SECTION_SIZE	4600
#define MAX_HTML_SIZE			20000
#define MAX_ENTRIES			10

typedef struct {
    char name[128];
    char val[MAX_SCRIPT_SIZE];
} entry;

int main(int argc, char *argv[]);
char *findTag(char *buffer, char *tagName);
int readWholeFile(char *FileName, char *buffer, int bufferSize);
int getFormData();
void printWrongArgument();
void printArgumentList(int iArgNum);
int GetRegButtonScript();
int SetRegButtonScript(char *ArgAction, char *ArgScript);

// util.c
void getword(char *word, char *line, char stop);
char *makeword(char *line, char stop);
char *fmakeword(FILE *f, char stop, int *len);
char x2c(char *what);
void unescape_url(char *url);
void plustospace(char *str);
int fileExists(char *filename);
