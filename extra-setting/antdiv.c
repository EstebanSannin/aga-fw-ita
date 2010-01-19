#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <psixml.h>
#include "nanoxml.h"
#include "errno.h"

#include "antdiv.h"
#include "../../shared/rsi_api.h"

int GetAntDivConf()
{
    int iRet=0;
    int state = 0;
    char strTmp[256];
    char strCommand[256];
    
    strTmp[0]='\0';
    strCommand[0]='\0';
    
    // Reading wlan parms
    strcpy(str_tag1_name, "wlan");
    sprintf(str_tag2_name, "diversity");
    strcpy(str_attribute_name, "state");
    read_attribute();
    if (find_status.value_found == 1) {
        sprintf(strTmp, "%s", str_attribute_value);
        if (strcasecmp("enable", strTmp) == 0) {
            state = 1;
        } else {
            state = 0;
        }

    } else {
        iRet = -1;
    }

    if (state == 0) {
	printf("var ena = false;\n");
    } else {
	printf("var ena = true;\n");
    }

    return iRet;
}

int SetAntDivRsiConf(int ArgEnable)
{
    static int xmlCfgLen = 0;
    char *strXmlBuf = NULL;

    char strEnable[8];

    int rsiSize = 0;
    char *pc = NULL;
    
    rsiSize = sysGetRsiSize();
    if ((strXmlBuf = malloc(rsiSize + 1)) == NULL)
    {
        printf("Failed to allocate %d bytes of memory.\n", rsiSize);
        return -1;
    }
    memset(strXmlBuf, 0xFF, rsiSize + 1);
    sysRsiGet(strXmlBuf, rsiSize, 0);
    strXmlBuf[rsiSize] = '\0';

    // if <rsitree> cannot be found then return
    if (memcmp(strXmlBuf, XML_RSI_START_TAG, strlen(XML_RSI_START_TAG)) == 0) {
	// determine data length
	pc = strstr(strXmlBuf, XML_RSI_END_TAG);
	if (pc != NULL)
	    xmlCfgLen = pc - strXmlBuf + strlen(XML_RSI_END_TAG) + 1;
        strXmlBuf[xmlCfgLen - 1] = '\0';
    } else  {
	printf("Invalid configuration data.\n");
	free(strXmlBuf);
	return -2;
    }

    if ((xmlCfgLen + MAX_WLAN_SECTION_SIZE) > rsiSize) {
	free(strXmlBuf);
	return -3;
    }

    char *ps, *pe;
    char *newRsi;
    long lTmpSize;

    read_section_bounds(strXmlBuf, "wlan", &ps, &pe);
    if ((ps == NULL) || (pe == NULL)) {
	free(strXmlBuf);
	return -4;
    }
    while (*(pe + 1) == '\n') pe++;

    newRsi = (char *) malloc(rsiSize);
    memset(newRsi, 0xFF, rsiSize);
    memcpy(newRsi, strXmlBuf, ps - strXmlBuf);
    newRsi[ps - strXmlBuf] = '\0';

    if (ArgEnable == 0) {
	sprintf(strEnable, "disable");
    } else {
    	sprintf(strEnable, "enable");
    }

    sprintf(newRsi, "%s<wlan>\n<diversity state=\"%s\" />\n</wlan>\n",
        newRsi, strEnable);
    
    lTmpSize = strlen(newRsi);
    memcpy(&newRsi[lTmpSize], pe + 1, strXmlBuf + xmlCfgLen - pe - 1);
    newRsi[lTmpSize + strXmlBuf + xmlCfgLen - pe - 1] = '\0';
//    printf("%s\n", newRsi);

    sysRsiSet(newRsi, rsiSize, 0);
        
    free(newRsi);
    free(strXmlBuf);

    // If disabled, force normal configuration because rsiconf doesn't call wlctl
    if (ArgEnable == 0) {
	system("/bin/wlctl ucantdiv 0");
	system("/bin/wlctl antdiv 0");
	system("/bin/wlctl txant 0");
    }

    system("/bin/rsiconf antdiv");
    
    return 0;
}

