#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MOUNT_DIR			"/var/mounts/sda1"
#define MOUNT_VAR_DIR			"/var/mounts/sda1/var"
#define MOUNT_NOT_FOUND_HTML_FILE      	"mountnotfound.html"
#define STD_HTML_FILE		      	"std.html"
#define EVAL_HTML_FILE      		"eval.html"
#define APTCONFIG_XML_FILE      	"aptconfig.xml"
#define MAX_SCRIPT_SIZE			16384
#define MAX_PROFILE_SECTION_SIZE	17000
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
int GetProfileScript();
int SetProfileScript(int ArgEnable, char *ArgScript);

// util.c
void getword(char *word, char *line, char stop);
char *makeword(char *line, char stop);
char *fmakeword(FILE *f, char stop, int *len);
char x2c(char *what);
void unescape_url(char *url);
void plustospace(char *str);
int fileExists(char *filename);
