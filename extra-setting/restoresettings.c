// Copiato da http://www.dbforums.com/showthread.php?t=485359
// Non ci sono info sulla licenza

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#include "restoresettings.h"

int main( int argc, char *argv[], char *envp[])
{
	int rc = 0;
	char *up= UPLOADIR;
	char *psz1;
	char *psz2;
	FILE *out = NULL;
	long i, total, count;
	char *ContentLength; /* Pointer to CONTENT_LENGTH environment variable. */
	long InCount; /* The supposed number of incoming bytes. */
	char *filename;
	char sCommand[64];

	ContentLength = getenv("CONTENT_LENGTH");
	InCount = atol(ContentLength);
	if (InCount > MAXSIZE) {
		rc=7;
		goto Error;
	}

// null out file name buffer
	memset(szFile, 0, sizeof(szFile));

// first line should be MIME boundary, prepend cr/lf
	szBoundary[0] = '\r';
	szBoundary[1] = '\n';
	szBoundary[2] = '\0';
	if (fgets(&szBoundary[2], sizeof(szBoundary)-2, stdin) == NULL) {
		rc = 1;
		goto Error;
	}

//strip terminating CR / LF
	if ((psz1=strchr(&szBoundary[2],'\r')) != NULL) {
		*psz1 = '\0';
	}
	if ((psz1=strchr(&szBoundary[2],'\n')) != NULL) {
		*psz1 = '\0';
	}

// second line should contain "Content-Disposition:
	if (fgets(szBuff, sizeof(szBuff), stdin) == NULL) {
		rc = 2;
		goto Error;
	}

// get filename keyword
	if ((psz1=strstr(szBuff, "filename=")) == NULL) {
		rc = 3;
		goto Error;
	}

// get pointer to actual filename (it's in quotes)
	psz1+=strlen("filename=");

	if ((psz1 = strtok(psz1, " \"")) == NULL) {
		rc = 4;
		goto Error;
	}

// remove leading path for both PC and UNIX systems
	if ((psz2 = strrchr(psz1,'\\')) != NULL) {
		psz1 = psz2+1;
	}
	if ((psz2 = strrchr(psz1,'/')) != NULL) {
		psz1 = psz2+1;
	}

//psz1 now points to a file name, try to create it in our system
	sprintf(szFile, "%s/%s",up,psz1);
//file to actually write
	sprintf(myFile, "%s", psz1);
//basename of file
	filename = myFile;

	if ((out = fopen(szFile, "wb+")) == NULL) {
		rc = 6;
		goto Error;
	}

// throw away until we get a blank line
	while (fgets(szBuff, sizeof(szBuff), stdin) != NULL) {
		if (strlen(szBuff) <= 2) {
			break;
		}
	}

// copy the file
	while ((count=fread(szBuff, 1, sizeof(szBuff), stdin)) != 0) {
		if ((i=fwrite(szBuff, 1, count, out)) != count) {
			rc = 7;
			goto Error;
		} // disk write error
	}

// re read last 128 bytes of file, handling files < 128 bytes
	if ((count = ftell(out)) == -1) {
		rc = 8;
		goto Error;
	}

	if (count > 128) {
		count = 128;
	}

	if (fseek(out, 0-count, SEEK_END) != 0) {
		rc = 9;
		goto Error;
	}

// get the new position
	if ((total = ftell(out)) == -1) {
		rc = 10;
		goto Error;
	}

// and read the data
	count = fread(szBuff, 1, sizeof(szBuff), out);
	szBuff[count] = '\0';

// determine offset of terminating boundary line
	rc = 11;
	for (i=0; i<=(count-(long)strlen(szBoundary)); i++) {
		if ((szBuff[i] == szBoundary[0]) && (strncmp(szBoundary, &szBuff[i], strlen(szBoundary)) == 0)) {
			total += i;
			rc = 0;
			break;
		}
	}

// if rc is still set, we didn't find the terminating boundary line
	if (rc != 0) {
		goto Error;
	}

	if (total == 0) {
		rc = 11;
		goto Error;
	}
	
// truncate the file at the correct length
	fflush(out);
	ftruncate(fileno(out), (off_t) total);
	
Error: if (out != NULL) { fclose(out); }

    printf("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n");
    printf("\n");
    printf("<html>\n");
    printf("<body>\n");
    printf("\t<script>\n");

    switch(rc) {
	case 0:
	    printf("\t\teval('location = \"do.cgi?page=restoresettings&result=success\"')\n");
	    printf("\t</script>\n");
	    printf("</body>\n");
	    printf("</html>\n");
	    
	    sprintf(sCommand, "/bin/rsiutil load %s", szFile);
	    system(sCommand);
	    
	    break;
	default:
	    printf("\t\teval('location = \"do.cgi?page=restoresettings&result=error\"')\n");
	    printf("\t</script>\n");
	    printf("</body>\n");
	    printf("</html>\n");
	    
	    break;
    }
    
    return 0;
}

