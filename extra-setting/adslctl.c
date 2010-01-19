#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <psixml.h>
#include "nanoxml.h"
#include "errno.h"

#include "adslctl.h"
#include "../../shared/rsi_api.h"

int GetadslctlConf()
{
    int iRet=0;
    int state = 0;
    int snr = -1;
    int trellis = 0;
    char strTmp[256];
    char strCommand[256];
    
    strTmp[0]='\0';
    strCommand[0]='\0';
    
    // Reading adslctl parms
    strcpy(str_tag1_name, "adslctl");
    sprintf(str_tag2_name, "global");
    strcpy(str_attribute_name, "state");
    read_attribute();
    if (find_status.value_found == 1) {
        sprintf(strTmp, "%s", str_attribute_value);
        if (strcasecmp("enable", strTmp) == 0) {
            state = 1;
        } else {
            state = 0;
        }

        strcpy(str_attribute_name, "snr");
        read_attribute();
        if (find_status.value_found == 1) {
            sprintf(strTmp, "%s", str_attribute_value);
            snr = strtol(strTmp, NULL, 10);
            if ((errno == EINVAL) || (errno == ERANGE)) {
	        return -1;
            }
        }

        strcpy(str_attribute_name, "trellis");
        read_attribute();
        if (find_status.value_found == 1) {
            sprintf(strTmp, "%s", str_attribute_value);
            if (strcasecmp("on", strTmp) == 0) {
                trellis = 1;
            }
        }
    } else {
        iRet = -2;
    }

    if (state == 0) {
	printf("var ena = false;\n");
    } else {
	printf("var ena = true;\n");
    }
    if (snr == -1) {
	printf("var snr = '';\n");
    } else {
	printf("var snr = %d;\n", snr);
    }
    if (trellis == 0) {
	printf("var trellis = false;\n");
    } else {
	printf("var trellis = true;\n");
    }

    return iRet;
}

int SetadslctlRsiConf(int ArgEnable, int ArgSnr, int ArgTrellis)
{
    static int xmlCfgLen = 0;
    char *strXmlBuf = NULL;

    char strEnable[8];
    char strTrellis[8];

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

    if ((xmlCfgLen + MAX_ADSLCTL_SECTION_SIZE) > rsiSize) {
	free(strXmlBuf);
	return -3;
    }

    char *ps, *pe;
    char *newRsi;
    long lTmpSize;

    read_section_bounds(strXmlBuf, "adslctl", &ps, &pe);
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
    if (ArgTrellis == 0) {
	sprintf(strTrellis, "off");
    } else {
    	sprintf(strTrellis, "on");
    }

    sprintf(newRsi, "%s<adslctl>\n<global state=\"%s\" snr=\"%d\" trellis=\"%s\" />\n</adslctl>\n",
        newRsi, strEnable, ArgSnr, strTrellis);
    
    lTmpSize = strlen(newRsi);
    memcpy(&newRsi[lTmpSize], pe + 1, strXmlBuf + xmlCfgLen - pe - 1);
    newRsi[lTmpSize + strXmlBuf + xmlCfgLen - pe - 1] = '\0';
//    printf("%s\n", newRsi);

    sysRsiSet(newRsi, rsiSize, 0);
        
    free(newRsi);
    free(strXmlBuf);

    // If disabled, force normal configuration because rsiconf doesn't call adslctl
//    if (ArgEnable == 0) {
//	system("/bin/adslctl configure --snr 100");
//    }

    system("/bin/rsiconf adslctl --force");
    
    return 0;
}

