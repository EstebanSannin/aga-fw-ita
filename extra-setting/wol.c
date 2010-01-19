#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "wol.h"
#include "../../shared/rsi_api.h"
#include "../../shared/killprocess.h"

int GetWOLNumber()
{
    int iRet=0;
    char strMac[MAC_LENGTH];
    char strDesc[DESC_LENGTH];
    strMac[0]='\0';
    strDesc[0]='\0';

    int boo;
    for (boo =1; boo > 0; boo++) {
	// Reading wol entry: mac, description
	strcpy(str_tag1_name, "wol");
	sprintf(str_tag2_name, "entry%d", boo);
        strcpy(str_attribute_name, "mac");
        read_attribute();
        if (find_status.value_found == 1) {
            sprintf(strMac, "%s", str_attribute_value);

            strcpy(str_attribute_name, "description");
            read_attribute();
            if (find_status.value_found == 1) {
                sprintf(strDesc, "%s", str_attribute_value);
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

int GetWOLList()
{
    int iRet=0;
    char strMac[MAC_LENGTH];
    char strDesc[DESC_LENGTH];

    strMac[0]='\0';
    strDesc[0]='\0';

    int boo;
    for (boo =1; boo > 0; boo++) {
	// Reading wol entry: mac, description
	strcpy(str_tag1_name, "wol");
	sprintf(str_tag2_name, "entry%d", boo);
        strcpy(str_attribute_name, "mac");
        read_attribute();
        if (find_status.value_found == 1) {
            sprintf(strMac, "%s", str_attribute_value);

            strcpy(str_attribute_name, "description");
            read_attribute();
            if (find_status.value_found == 1) {
                sprintf(strDesc, "%s", str_attribute_value);

		printf("<tr><td>%s</td>\n<td>%s</td>\n", strMac, strDesc);
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

int GetWOLEntry(int WOLNumber, char *MacAddress, char *Description)
{
    int iRet=0;

    // Reading wol entry: mac, description
    strcpy(str_tag1_name, "wol");
    sprintf(str_tag2_name, "entry%d", WOLNumber);
    strcpy(str_attribute_name, "mac");
    read_attribute();
    if (find_status.value_found == 1) {
        sprintf(MacAddress, "%s", str_attribute_value);

        strcpy(str_attribute_name, "description");
        read_attribute();
    	if (find_status.value_found == 1) {
            sprintf(Description, "%s", str_attribute_value);
        } else {
            iRet=-1;
        }
    } else {
        iRet=-2;
    }

//    printf("%d\n%s\n%s\n", LeaseNumber, MacAddress, IPAddress);
    return iRet;    
}

int AddWOLEntry(char *MacAddress, char *Description)
{
    static int xmlCfgLen = 0;
    char *strXmlBuf = NULL;

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

    if ((xmlCfgLen + MAX_WOL_ROW_SIZE) > rsiSize) {
	free(strXmlBuf);
	return -3;
    }

    char *ps, *pe;
    char *newRsi;
    long lTmpSize;

    read_section_bounds(strXmlBuf, "wol", &ps, &pe);
    if ((ps == NULL) || (pe == NULL)) {
	free(strXmlBuf);
	return -4;
    }
    while (*pe != '<') pe--;

    newRsi = (char *) malloc(rsiSize);
    memset(newRsi, 0xFF, rsiSize);
    memcpy(newRsi, strXmlBuf, pe - strXmlBuf);
    newRsi[pe - strXmlBuf] = '\0';

    sprintf(newRsi, "%s<entry%d mac=\"%s\" description=\"%s\" />\n", newRsi,
	GetWOLNumber() + 1, MacAddress, Description);
    
    lTmpSize = strlen(newRsi);
    memcpy(&newRsi[lTmpSize], pe, strXmlBuf + xmlCfgLen - pe);
    newRsi[lTmpSize + strXmlBuf + xmlCfgLen - pe] = '\0';

//    printf("%s\n", newRsi);

    sysRsiSet(newRsi, rsiSize, 0);
        
    free(newRsi);
    free(strXmlBuf);
    
    return 0;
}

int RemoveWOLEntry(char *WOLEntries)
{
    static int xmlCfgLen = 0;
    char *strXmlBuf = NULL;

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

    char *ps, *pe;
    char *newRsi;
    long lTmpSize;

    read_section_bounds(strXmlBuf, "wol", &ps, &pe);
    if ((ps == NULL) || (pe == NULL)) {
	free(strXmlBuf);
	return -4;
    }
    while (*(pe + 1) == '\n') pe++;

    newRsi = (char *) malloc(rsiSize);
    memset(newRsi, 0xFF, rsiSize);
    memcpy(newRsi, strXmlBuf, ps - strXmlBuf);
    newRsi[ps - strXmlBuf] = '\0';

    sprintf(newRsi, "%s<wol>\n", newRsi);
    
    char *WOLEntriesCopy;
    char *tokMac;
    char sMac[MAC_LENGTH];
    char sDescription[DESC_LENGTH];
    int EntriesCountOld = 0;
    int EntriesCountNew = 0;
    int iMacFound;
    
    WOLEntriesCopy = (char *) malloc((strlen(WOLEntries) + 1) * sizeof(char));

    while (EntriesCountOld < GetWOLNumber()) {
	iMacFound = 0;
	GetWOLEntry(EntriesCountOld + 1, sMac, sDescription);
	strcpy(WOLEntriesCopy, WOLEntries);
	tokMac = strtok(WOLEntriesCopy, ", ");
	while (tokMac != NULL) {
	    if (strncmp(sMac, tokMac, 17) == 0) {
	        iMacFound = 1;
	    }
	    tokMac = strtok(NULL, ", ");
	}
	if (iMacFound == 0) {
	    sprintf(newRsi, "%s<entry%d mac=\"%s\" description=\"%s\" />\n", newRsi, EntriesCountNew + 1, sMac, sDescription);
	    EntriesCountNew++;
	}
	EntriesCountOld++;
    }

    sprintf(newRsi, "%s</wol>\n", newRsi);

    free(WOLEntriesCopy);

    lTmpSize = strlen(newRsi);
    memcpy(&newRsi[lTmpSize], pe + 1, strXmlBuf + xmlCfgLen - pe - 1);
    newRsi[lTmpSize + strXmlBuf + xmlCfgLen - pe - 1] = '\0';

//    printf("%s\n", newRsi);

    sysRsiSet(newRsi, rsiSize, 0);
        
    free(newRsi);
    free(strXmlBuf);

    return 0;
}

int hextobin(unsigned char c)
{
/*
    Function to convert hex character to binary.
    Returns value or -1 if there is any error.
*/
    if(isdigit(c))		return c-'0';
    else if(isxdigit(c))	return tolower(c)-'a'+10;
    else			return -1;
}

int SendMagicPacket(char *ifName, char *macAddress)
{
    const int on = 1;
    short int cont;
    int skt_target;
    unsigned char ma[6];
    unsigned char packet[102];

    char buff[1024];
    struct sockaddr_in sindst;
    struct ifreq *ifr = NULL;
    struct ifconf ifc;
    int i, if_found = 0;
    int iRet = 0;
    
    // Make sure argument was supplied
    if ((ifName == NULL) || (ifName == '\0'))
    {
	// Print how-to use the program then exit
	//fprintf(stderr, "Usage: %s <if name> <mac address>\n", argv[0]);
	return(-1);
    }
    
    // Test length of argument
    if (strlen(macAddress) != 17)
    {
    	//fprintf(stderr, "Mac address incorrect\nUse 12:34:56:78:90:ab or any other separator\n");
	return(-2);
    }
	
    // Test and convert hex string to binary
    ma[0]=0;
    ma[1]=0;
    ma[2]=0;
    ma[3]=0;
    ma[4]=0;
    ma[5]=0;

    for (cont=0;cont<18;cont++)
    {
	if (cont%3 != 2)
	{
	    if ((macAddress[cont] < '0' || macAddress[cont] > '9') &&
		(macAddress[cont] < 'A' || macAddress[cont] > 'F') &&
		(macAddress[cont] < 'a' || macAddress[cont] > 'f'))
	    {
    		fprintf(stderr, "Mac address incorrect\nUse 12:34:56:78:90:ab or any other separator\n");
		return(-3);
    	    }
	    else
	    {
		ma[cont/3]=ma[cont/3]*16+hextobin(macAddress[cont]);
	    }
	}
    }    

    // Create broadcast magic packet
    // 6*FF + 16*MAC_ADDRESS
    for(cont=0;cont<=5;cont++)
        packet[cont]='\xFF';

    for(cont=0;cont<=15;cont++)
    {
        packet[6+cont*6] = ma[0];
        packet[7+cont*6] = ma[1];
	packet[8+cont*6] = ma[2];
	packet[9+cont*6] = ma[3];
	packet[10+cont*6] = ma[4];
	packet[11+cont*6] = ma[5];
    }

    // Open Socket
    skt_target = socket( AF_INET, SOCK_DGRAM, 0);
    if (  skt_target < 0 ) {
        fprintf( stderr, "Cannot open SOCKET.\n");
        return(-4);
    }

    // Prepare dest address
    memset (&sindst,0,sizeof (sindst));
    sindst.sin_family = AF_INET;
    sindst.sin_port = htons (PORT);

    // Retrieve interfaces informations
    ifc.ifc_len = sizeof(buff);
    ifc.ifc_buf = buff;

    if (ioctl(skt_target, SIOCGIFCONF, &ifc) < 0) {
	perror("ioctl SIOCGIFCONF");
	return(-5);
    }

    ifr = ifc.ifc_req;
    
    // Retrieve the interface broadcast address
    for (i = ifc.ifc_len / sizeof(struct ifreq); --i >= 0; ifr++) 
    {
	if (strncmp(ifName, ifr->ifr_name, 3) == 0) {
    	    if_found = 1;

	    if (ioctl(skt_target, SIOCGIFBRDADDR, ifr) < 0) {
		perror("ioctl SIOCGIFBRDADDR");
		return(-6);
	    }

//	    printf("  Broadcast: %s\n", inet_ntoa(((struct sockaddr_in *) 
//		&(ifr->ifr_broadaddr))->sin_addr));
	    
	    sindst.sin_addr = ((struct sockaddr_in *) 
		&(ifr->ifr_broadaddr))->sin_addr;
	    break;
	}
    }

    if (if_found == 0) {
	return(-7);
    }
	
    // Set up Broadcast
    iRet = setsockopt (skt_target, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
    if (iRet != 0) {
	return(-8);
    }

    // Send UDP packet
    iRet = (int) sendto( skt_target, packet, 102, 0,
        (struct sockaddr*)&sindst, sizeof(sindst));
    if (iRet != 102) {
	return(-9);
    }

    // Close socket and exit
    close (skt_target);
    return 0;
}

int SendMagicPackets(char *ifName, char *macAddresses)
{    
    char *MacAddressesCopy;
    char *tokMac;
    int iRet = 0;
    int iResult = 0;
    
    MacAddressesCopy = (char *) malloc((strlen(macAddresses) + 1) * sizeof(char));

    strcpy(MacAddressesCopy, macAddresses);
    tokMac = strtok(MacAddressesCopy, ", ");
    while (tokMac != NULL) {
	iResult = SendMagicPacket(ifName, tokMac);
	if (iResult < 0) iRet = -1;

	tokMac = strtok(NULL, ", ");
    }

    free(MacAddressesCopy);

    return iRet;
}
