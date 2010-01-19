//define max size of uploads to 10M
#define MAXSIZE 10000000

//define upload directory relative to cgi-bin
#define UPLOADIR "/tmp"
//DEBUG
//#define UPLOADIR "tmp"

// buffer for storing boundary line and file name
#define MAXLINE 512
char szBoundary[MAXLINE];
char szFile[MAXLINE];   //filename to write to
char myFile[MAXLINE];   //basename of uploaded file

// buffer of arbitrary size for reading from STDIN
#define BUFFSIZE 16*1024
char szBuff[BUFFSIZE];

int main( int argc, char *argv[], char *envp[]);
