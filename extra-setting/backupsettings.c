#include <stdio.h>

#include "backupsettings.h"

int DoBackupSettings()
{
    FILE *fp;
    char line[256];
    int iRet = 0;

        
    fp = popen("/bin/rsiutil dump", "r");
    if (fp != NULL) {
	printf("Content-type: config/conf%c%c", 10, 10);
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
