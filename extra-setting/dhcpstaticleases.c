#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include "dhcpstaticleases.h"
#include "../../shared/rsi_api.h"
#include "../../shared/killprocess.h"

int GetDhcpStaticLeasesNumber()
{
    int iRet=0;
    char strMac[MAC_LENGTH];
    char strIP[IP_LENGTH];

    strMac[0]='\0';
    strIP[0]='\0';

    int boo;
    for (boo =1; boo > 0; boo++) {
	// Reading udhcpd static leases: mac, ip
	strcpy(str_tag1_name, "udhcpd");
	sprintf(str_tag2_name, "static%d", boo);
        strcpy(str_attribute_name, "mac");
        read_attribute();
        if (find_status.value_found == 1) {
            sprintf(strMac, "%s", str_attribute_value);

            strcpy(str_attribute_name, "ip");
            read_attribute();
            if (find_status.value_found == 1) {
                sprintf(strIP, "%s", str_attribute_value);
		iRet = boo;
	    } else {
	        boo=-1;
	    }
	} else {
	    boo=-1;
	}
    }

    return iRet;
}

int GetDhcpStaticLeases()
{
    int iRet=0;
    char strMac[MAC_LENGTH];
    char strIP[IP_LENGTH];

    strMac[0]='\0';
    strIP[0]='\0';

    int boo;
    for (boo =1; boo > 0; boo++) {
	// Reading udhcpd static leases: mac, ip
	strcpy(str_tag1_name, "udhcpd");
	sprintf(str_tag2_name, "static%d", boo);
        strcpy(str_attribute_name, "mac");
        read_attribute();
        if (find_status.value_found == 1) {
            sprintf(strMac, "%s", str_attribute_value);

            strcpy(str_attribute_name, "ip");
            read_attribute();
            if (find_status.value_found == 1) {
                sprintf(strIP, "%s", str_attribute_value);

		printf("<tr><td>%s</td><td>%s</td>\n", strMac, strIP);
		printf("<td align='center'><input type='checkbox' name='rml' value='%s'></td></tr>\n", strMac);

	    } else {
	        boo=-1;
	    }
	} else {
	    boo=-1;
	}
    }

    return iRet;    
}

int GetDhcpStaticLease(int LeaseNumber, char *MacAddress, char *IPAddress)
{
    int iRet=0;

    // Reading udhcpd static leases: mac, ip
    strcpy(str_tag1_name, "udhcpd");
    sprintf(str_tag2_name, "static%d", LeaseNumber);
    strcpy(str_attribute_name, "mac");
    read_attribute();
    if (find_status.value_found == 1) {
        sprintf(MacAddress, "%s", str_attribute_value);

        strcpy(str_attribute_name, "ip");
        read_attribute();
    	if (find_status.value_found == 1) {
            sprintf(IPAddress, "%s", str_attribute_value);
        } else {
            iRet=-1;
        }
    } else {
        iRet=-2;
    }

//    printf("%d\n%s\n%s\n", LeaseNumber, MacAddress, IPAddress);
    return iRet;    
}

int AddDhcpStaticLeases(char *MacAddress, char *IPAddress)
{
    static int xmlCfgLen = 0;
    char *strXmlBuf = NULL;

    int rsiSize = 0;
    char *pc = NULL;
    char sCommand[64];
    
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

    if ((xmlCfgLen + MAX_DHCP_ROW_SIZE) > rsiSize) {
	free(strXmlBuf);
	return -3;
    }

    char *ps, *pe;
    char *newRsi;
    long lTmpSize;

    read_section_bounds(strXmlBuf, "udhcpd", &ps, &pe);
    if ((ps == NULL) || (pe == NULL)) {
	free(strXmlBuf);
	return -4;
    }
    while (*pe != '<') pe--;

    newRsi = (char *) malloc(rsiSize);
    memset(newRsi, 0xFF, rsiSize);
    memcpy(newRsi, strXmlBuf, pe - strXmlBuf);
    newRsi[pe - strXmlBuf] = '\0';

    sprintf(newRsi, "%s<static%d mac=\"%s\" ip=\"%s\" />\n", newRsi,
	GetDhcpStaticLeasesNumber() + 1, MacAddress, IPAddress);
    
    lTmpSize = strlen(newRsi);
    memcpy(&newRsi[lTmpSize], pe, strXmlBuf + xmlCfgLen - pe);
    newRsi[lTmpSize + strXmlBuf + xmlCfgLen - pe] = '\0';

//    printf("%s\n", newRsi);

    sysRsiSet(newRsi, rsiSize, 0);
        
    free(newRsi);
    free(strXmlBuf);

    system("/bin/rsiconf udhcpd");

    killProcess("dhcpd", "/var/run/udhcpd.pid");
	
    sprintf(sCommand, "/bin/dhcpd");
    system(sCommand);
    
    return 0;
}

int RemoveDhcpStaticLeases(char *MacAddresses)
{
    static int xmlCfgLen = 0;
    char *strXmlBuf = NULL;

    int rsiSize = 0;
    char *pc = NULL;
    char sCommand[64];

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

    char *ps, *pe;
    char *newRsi;
    long lTmpSize;

    read_section_bounds(strXmlBuf, "udhcpd", &ps, &pe);
    if ((ps == NULL) || (pe == NULL)) {
	free(strXmlBuf);
	return -4;
    }
    while (*(pe + 1) == '\n') pe++;

    newRsi = (char *) malloc(rsiSize);
    memset(newRsi, 0xFF, rsiSize);
    memcpy(newRsi, strXmlBuf, ps - strXmlBuf);
    newRsi[ps - strXmlBuf] = '\0';

    sprintf(newRsi, "%s<udhcpd>\n", newRsi);
    
    char *MacAddressesCopy;
    char *tokMac;
    char sMac[MAC_LENGTH];
    char sIP[IP_LENGTH];
    int LeasesCountOld = 0;
    int LeasesCountNew = 0;
    int iMacFound;
    
    MacAddressesCopy = (char *) malloc((strlen(MacAddresses) + 1) * sizeof(char));

    while (LeasesCountOld < GetDhcpStaticLeasesNumber()) {
	iMacFound = 0;
	GetDhcpStaticLease(LeasesCountOld + 1, sMac, sIP);
	strcpy(MacAddressesCopy, MacAddresses);
	tokMac = strtok(MacAddressesCopy, ", ");
	while (tokMac != NULL) {
	    if (strncmp(sMac, tokMac, 17) == 0) {
	        iMacFound = 1;
	    }
	    tokMac = strtok(NULL, ", ");
	}
	if (iMacFound == 0) {
	    sprintf(newRsi, "%s<static%d mac=\"%s\" ip=\"%s\" />\n", newRsi, LeasesCountNew + 1, sMac, sIP);
	    LeasesCountNew++;
	}
	LeasesCountOld++;
    }

    sprintf(newRsi, "%s</udhcpd>\n", newRsi);

    free(MacAddressesCopy);

    lTmpSize = strlen(newRsi);
    memcpy(&newRsi[lTmpSize], pe + 1, strXmlBuf + xmlCfgLen - pe - 1);
    newRsi[lTmpSize + strXmlBuf + xmlCfgLen - pe - 1] = '\0';

//    printf("%s\n", newRsi);

    sysRsiSet(newRsi, rsiSize, 0);
        
    free(newRsi);
    free(strXmlBuf);

    system("/bin/rsiconf udhcpd");
    
    killProcess("dhcpd", "/var/run/udhcpd.pid");
	
    sprintf(sCommand, "/bin/dhcpd");
    system(sCommand);

    return 0;
}

