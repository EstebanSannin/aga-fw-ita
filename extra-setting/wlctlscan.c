#include "wlctlscan.h"

int GetwlctlScan()
{
    FILE *fp;
    char line[256];
    int iRet = 0;
    
    system("wlctl scan");
    sleep (1);
    fp = popen("wlctl scanresults", "r");
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
