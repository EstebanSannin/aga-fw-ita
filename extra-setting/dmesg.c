#include <stdio.h>

#include "dmesg.h"

int Getdmesg()
{
    FILE *fp;
    char line[256];
    int iRet = 0;
    
    fp = popen("dmesg", "r");
    if (fp != NULL) {
	while (fgets(line, sizeof(line), fp)) {
	    printf("%s", line);
	}
	pclose(fp);
	iRet = 0;
    } else {
	iRet = -1;
    }
    return iRet;
}
