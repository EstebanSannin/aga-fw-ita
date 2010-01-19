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

#include <psixml.h>
#include "ftpd.h"
#include "../../shared/rsi_api.h"
#include "../../shared/killprocess.h"

/*
<ftpd>
<user1 state="enable" name="support" home="/" write="yes" />
<user2 state="enable" name="anonymous" home="/var/mounts/sda1/temp" write="no" />
<user3 state="enable" name="roleo" password="password" home="/var/mounts/sda1/roleo" write="yes" />
</ftpd>
*/

int GetFtpd()
{
    int iRet=0;
    char strName[NAME_LENGTH];
    char strPassword[PWD_LENGTH];
    char strState[8];
    char strHome[DIR_LENGTH];
    char strWrite[8];
    char strTmp[256];
    int PasswdLength = 0;
    
    int boo;
    for (boo = 1; boo > 0; boo++) {

	strName[0] = '\0';
	strPassword[0] = '\0';
	strState[0] = '\0';
	strHome[0] = '\0';
	strWrite[0] = '\0';

	// Reading ftpd entry
	strcpy(str_tag1_name, "ftpd");
	sprintf(str_tag2_name, "user%d", boo);
        strcpy(str_attribute_name, "state");
        read_attribute();
        if (find_status.value_found == 1) {
	    sprintf(strTmp, "%s", str_attribute_value);
	    if (strcasecmp("enable", strTmp) == 0) {
	        sprintf(strState, "yes");
	    } else {
	        sprintf(strState, "no");
	    }

            strcpy(str_attribute_name, "name");
            read_attribute();
            if (find_status.value_found == 1) {
        	sprintf(strName, "%s", str_attribute_value);
	    }

            strcpy(str_attribute_name, "password");
            read_attribute();
            if (find_status.value_found == 1) {
		base64Decode(strPassword, str_attribute_value, &PasswdLength);
		strPassword[PasswdLength] = '\0';
//        	sprintf(strPassword, "%s", str_attribute_value);
	    }

            strcpy(str_attribute_name, "home");
            read_attribute();
            if (find_status.value_found == 1) {
		sprintf(strHome, "%s", str_attribute_value);
	    }

            strcpy(str_attribute_name, "write");
            read_attribute();
            if (find_status.value_found == 1) {
		sprintf(strWrite, "%s", str_attribute_value);
	    }

	    if ((strcmp("support", strName) == 0) || (strcmp("anonymous", strName) == 0)) {
		// Support or anonymous user
		printf("<tr>\n");
		printf("<td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", strName);
		printf("<td>%s</td>\n", strState);
		printf("<td>%s</td>\n", strName);
		printf("<td>N.A.</td>\n");
		printf("<td>%s</td>\n", strHome);
		printf("<td>%s</td>\n", strWrite);
		printf("</tr>\n");
	    } else {
		// Other user
		printf("<tr>\n");
		printf("<td align='center'><input type='checkbox' name='rml' value='%s'></td>\n", strName);
		printf("<td>%s</td>\n", strState);
		printf("<td>%s</td>\n", strName);
		printf("<td>********</td>\n");
		printf("<td>%s</td>\n", strHome);
		printf("<td>%s</td>\n", strWrite);
		printf("</tr>\n");
	    }
	    
	} else {
	    boo=-1;
	}
    }
    
    return iRet;    
}

int GetFtpdModify(char *UserName)
{
    int iRet=0;
    char strName[NAME_LENGTH];
    char strPassword[PWD_LENGTH];
    char strState[8];
    char strHome[DIR_LENGTH];
    char strWrite[8];
    char strTmp[256];
    int PasswdLength = 0;
    
    int boo;
    for (boo = 1; boo > 0; boo++) {

	strName[0] = '\0';
	strPassword[0] = '\0';
	strState[0] = '\0';
	strHome[0] = '\0';
	strWrite[0] = '\0';

	// Reading ftpd entry
	strcpy(str_tag1_name, "ftpd");
	sprintf(str_tag2_name, "user%d", boo);
        strcpy(str_attribute_name, "state");
        read_attribute();
        if (find_status.value_found == 1) {
	    sprintf(strTmp, "%s", str_attribute_value);
	    if (strcasecmp("enable", strTmp) == 0) {
	        sprintf(strState, "checked");
	    } else {
	        strState[0] = '\0';
	    }

            strcpy(str_attribute_name, "name");
            read_attribute();
            if (find_status.value_found == 1) {
        	sprintf(strName, "%s", str_attribute_value);
	    }

	    if (strcmp(UserName, strName) == 0) {
        	strcpy(str_attribute_name, "password");
        	read_attribute();
        	if (find_status.value_found == 1) {
		    base64Decode(strPassword, str_attribute_value, &PasswdLength);
		    strPassword[PasswdLength] = '\0';
//        	    sprintf(strPassword, "%s", str_attribute_value);
		}
	    
        	strcpy(str_attribute_name, "home");
        	read_attribute();
        	if (find_status.value_found == 1) {
		    sprintf(strHome, "%s", str_attribute_value);
		}

        	strcpy(str_attribute_name, "write");
        	read_attribute();
        	if (find_status.value_found == 1) {
		    sprintf(strTmp, "%s", str_attribute_value);
		    if (strcasecmp("yes", strTmp) == 0) {
	    		sprintf(strWrite, "checked");
		    } else {
	    		strWrite[0] = '\0';
		    }
		}

		if ((strcmp("support", strName) == 0) || (strcmp("anonymous", strName) == 0)) {
		    // Support or anonymous user
		    printf("<tr><td><label>Enable:</label></td>");
		    printf("<td><input type='checkbox' name='userState' %s></td></tr>", strState);
		    printf("<tr><td><label>User:</label></td>");
		    printf("<td><input type='hidden' name='oldUserName' value='%s'>", strName);
		    printf("<input type='text' name='userName' maxlength='16' id='userName' value='%s' disabled></td></tr>", strName);
		    printf("<tr><td><label>Password:</label></td>");
		    printf("<td><input type='text' name='userPassword' maxlength='16' value='N.A.' disabled></td></tr>");
		    printf("<tr><td><label>Home directory:</label></td>");
		    printf("<td><input type='text' name='userHome' maxlength='255' id='userHome' value='%s'></td></tr>", strHome);
		    printf("<tr><td><label>Write:</label></td>");
		    printf("<td><input type='checkbox' name='userWrite' %s></td></tr>", strWrite);
		} else {
		    // Other user
		    printf("<tr><td><label>Enable:</label></td>");
		    printf("<td><input type='checkbox' name='userState' %s></td></tr>", strState);
		    printf("<tr><td><label>User:</label></td>");
		    printf("<td><input type='hidden' name='oldUserName' value='%s'>", strName);
		    printf("<input type='text' name='userName' maxlength='16' id='userName' value='%s'></td></tr>", strName);
		    printf("<tr><td><label>Password:</label></td>");
		    printf("<td><input type='text' name='userPassword' maxlength='16' value='%s'></td></tr>", strPassword);
		    printf("<tr><td><label>Home directory:</label></td>");
		    printf("<td><input type='text' name='userHome' maxlength='255' id='userHome' value='%s'></td></tr>", strHome);
		    printf("<tr><td><label>Write:</label></td>");
		    printf("<td><input type='checkbox' name='userWrite' %s></td></tr>", strWrite);
		}
	    }
	    
	} else {
	    boo=-1;
	}
    }
    
    return iRet;    
}

int AddFtpdEntry(int UserState, char *UserName, char *UserPassword, char *UserHome, int UserWrite)
{
    static int xmlCfgLen = 0;
    char *strXmlBuf = NULL;

    char strState[8];
    char strWrite[8];

    int rsiSize = 0;
    char *pc = NULL;
    
    char EncPasswd[256];
    char *ptrEncPasswd;
    
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

    if ((xmlCfgLen + MAX_FTPD_ROW_SIZE) > rsiSize) {
	free(strXmlBuf);
	return -3;
    }

    char *ps, *pe;
    char *newRsi;
    long lTmpSize;

    read_section_bounds(strXmlBuf, "ftpd", &ps, &pe);
    if ((ps == NULL) || (pe == NULL)) {
	free(strXmlBuf);
	return -4;
    }
    while (*pe != '<') pe--;

    newRsi = (char *) malloc(rsiSize);
    memset(newRsi, 0xFF, rsiSize);
    memcpy(newRsi, strXmlBuf, pe - strXmlBuf);
    newRsi[pe - strXmlBuf] = '\0';
    
    if (UserState == 0) {
	sprintf(strState, "disable");
    } else {
	sprintf(strState, "enable");
    }
    if (UserWrite == 0) {
	sprintf(strWrite, "no");
    } else {
	sprintf(strWrite, "yes");
    }
    
    if (UserPassword[0] == '\0') {
	sprintf(newRsi, "%s<user%d state=\"%s\" name=\"%s\" home=\"%s\" write=\"%s\" />\n",
	    newRsi, GetUsersNumber() + 1, strState, UserName, UserHome, strWrite);    
    } else{
	ptrEncPasswd = EncPasswd;
	base64Encode(UserPassword, strlen(UserPassword), &ptrEncPasswd);
	sprintf(newRsi, "%s<user%d state=\"%s\" name=\"%s\" password=\"%s\" home=\"%s\" write=\"%s\" />\n",
	    newRsi, GetUsersNumber() + 1, strState, UserName, ptrEncPasswd, UserHome, strWrite);
    }
    
    lTmpSize = strlen(newRsi);
    memcpy(&newRsi[lTmpSize], pe, strXmlBuf + xmlCfgLen - pe);
    newRsi[lTmpSize + strXmlBuf + xmlCfgLen - pe] = '\0';

//    printf("%s\n", newRsi);

    sysRsiSet(newRsi, rsiSize, 0);
        
    free(newRsi);
    free(strXmlBuf);

    system("/bin/rsiconf ftpd");

    system("killall bftpd");
	
    system("/bin/bftpd");
    
    return 0;
}

int ModifyFtpdEntry(char *OldUserName, int UserState, char *UserName, char *UserPassword, char *UserHome, int UserWrite)
{
    static int xmlCfgLen = 0;
    char *strXmlBuf = NULL;

    int rsiSize = 0;
    char *pc = NULL;

    char EncPasswd[256];
    char *ptrEncPasswd;

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

    read_section_bounds(strXmlBuf, "ftpd", &ps, &pe);
    if ((ps == NULL) || (pe == NULL)) {
	free(strXmlBuf);
	return -4;
    }
    while (*(pe + 1) == '\n') pe++;

    newRsi = (char *) malloc(rsiSize);
    memset(newRsi, 0xFF, rsiSize);
    memcpy(newRsi, strXmlBuf, ps - strXmlBuf);
    newRsi[ps - strXmlBuf] = '\0';

    sprintf(newRsi, "%s<ftpd>\n", newRsi);
    
    char strName[NAME_LENGTH];
    char strPassword[PWD_LENGTH];
    char strState[8];
    char strHome[DIR_LENGTH];
    char strWrite[8];
    int iState = 0;
    int iWrite = 0;
    int UsersCount = 0;
    
    while (UsersCount < GetUsersNumber()) {
	GetFtpdEntry(UsersCount + 1, &iState, strName, strPassword, strHome, &iWrite);
	
	if (strcasecmp(strName, OldUserName) == 0) {
	    if (UserState == 0) {
		sprintf(strState, "disable");
	    } else {
		sprintf(strState, "enable");
	    }
	    if ((strcasecmp("support", OldUserName) == 0) || (strcasecmp("anonymous", OldUserName) == 0)) {
		sprintf(strName, "%s", OldUserName);
		strPassword[0] = '\0';
	    } else {
		sprintf(strName, "%s", UserName);
		sprintf(strPassword, "%s", UserPassword);
	    }
	    sprintf(strHome, "%s", UserHome);
	    if (UserWrite == 0) {
		sprintf(strWrite, "no");
	    } else {
		sprintf(strWrite, "yes");
	    }
	} else {
	    if (iState == 0) {
		sprintf(strState, "disable");
	    } else {
		sprintf(strState, "enable");
	    }
	    if ((strcasecmp("support", strName) == 0) || (strcasecmp("anonymous", strName) == 0)) {
		strPassword[0] = '\0';
	    }
	    if (iWrite == 0) {
		sprintf(strWrite, "no");
	    } else {
		sprintf(strWrite, "yes");
	    }	    
	}
	
	if (strPassword[0] == '\0') {
	    sprintf(newRsi, "%s<user%d state=\"%s\" name=\"%s\" home=\"%s\" write=\"%s\" />\n",
	        newRsi, UsersCount + 1, strState, strName, strHome, strWrite);
	} else {
	    ptrEncPasswd = EncPasswd;
	    base64Encode(strPassword, strlen(strPassword), &ptrEncPasswd);
	    sprintf(newRsi, "%s<user%d state=\"%s\" name=\"%s\" password=\"%s\" home=\"%s\" write=\"%s\" />\n",
	        newRsi, UsersCount + 1, strState, strName, ptrEncPasswd, strHome, strWrite);
	}

	UsersCount++;
    }

    sprintf(newRsi, "%s</ftpd>\n", newRsi);

    lTmpSize = strlen(newRsi);
    memcpy(&newRsi[lTmpSize], pe + 1, strXmlBuf + xmlCfgLen - pe - 1);
    newRsi[lTmpSize + strXmlBuf + xmlCfgLen - pe - 1] = '\0';

//    printf("%s\n", newRsi);

    sysRsiSet(newRsi, rsiSize, 0);
        
    free(newRsi);
    free(strXmlBuf);

    system("/bin/rsiconf ftpd");
    
    system("killall bftpd");
	
    system("/bin/bftpd");

    return 0;
}

int RemoveFtpdEntries(char *Users)
{
    static int xmlCfgLen = 0;
    char *strXmlBuf = NULL;

    int rsiSize = 0;
    char *pc = NULL;

    char EncPasswd[256];
    char *ptrEncPasswd;

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

    read_section_bounds(strXmlBuf, "ftpd", &ps, &pe);
    if ((ps == NULL) || (pe == NULL)) {
	free(strXmlBuf);
	return -4;
    }
    while (*(pe + 1) == '\n') pe++;

    newRsi = (char *) malloc(rsiSize);
    memset(newRsi, 0xFF, rsiSize);
    memcpy(newRsi, strXmlBuf, ps - strXmlBuf);
    newRsi[ps - strXmlBuf] = '\0';

    sprintf(newRsi, "%s<ftpd>\n", newRsi);
    
    char *UsersCopy;
    char *tokMac;
    char strName[NAME_LENGTH];
    char strPassword[PWD_LENGTH];
    char strState[8];
    char strHome[DIR_LENGTH];
    char strWrite[8];
    int iState = 0;
    int iWrite = 0;
    int UsersCountOld = 0;
    int UsersCountNew = 0;
    int iUserFound;
    
    UsersCopy = (char *) malloc((strlen(Users) + 1) * sizeof(char));

    while (UsersCountOld < GetUsersNumber()) {
	iUserFound = 0;
	GetFtpdEntry(UsersCountOld + 1, &iState, strName, strPassword, strHome, &iWrite);
	if (iState == 0) {
	    sprintf(strState, "disable");
	} else {
	    sprintf(strState, "enable");
	}
	if (iWrite == 0) {
	    sprintf(strWrite, "no");
	} else {
	    sprintf(strWrite, "yes");
	}
	if ((strcmp("support", strName) == 0) || (strcmp("anonymous", strName) == 0)) {
	    strPassword[0] = '\0';
	} else {
    	    strcpy(UsersCopy, Users);
	    tokMac = strtok(UsersCopy, ", ");
	    while (tokMac != NULL) {
		if (strcmp(strName, tokMac) == 0) {
	    	    iUserFound = 1;
		}
		tokMac = strtok(NULL, ", ");
	    }
	}
	if (iUserFound == 0) {
	    if (strPassword[0] == '\0') {
		sprintf(newRsi, "%s<user%d state=\"%s\" name=\"%s\" home=\"%s\" write=\"%s\" />\n",
		    newRsi, UsersCountNew + 1, strState, strName, strHome, strWrite);
	    } else {
		ptrEncPasswd = EncPasswd;
		base64Encode(strPassword, strlen(strPassword), &ptrEncPasswd);
		sprintf(newRsi, "%s<user%d state=\"%s\" name=\"%s\" password=\"%s\" home=\"%s\" write=\"%s\" />\n",
		    newRsi, UsersCountNew + 1, strState, strName, ptrEncPasswd, strHome, strWrite);
	    }
	    UsersCountNew++;
	}
	UsersCountOld++;
    }

    sprintf(newRsi, "%s</ftpd>\n", newRsi);

    free(UsersCopy);

    lTmpSize = strlen(newRsi);
    memcpy(&newRsi[lTmpSize], pe + 1, strXmlBuf + xmlCfgLen - pe - 1);
    newRsi[lTmpSize + strXmlBuf + xmlCfgLen - pe - 1] = '\0';

//    printf("%s\n", newRsi);

    sysRsiSet(newRsi, rsiSize, 0);
        
    free(newRsi);
    free(strXmlBuf);

    system("/bin/rsiconf ftpd");
    
    system("killall bftpd");
	
    system("/bin/bftpd");

    return 0;
}

int GetUsersNumber()
{
    int iRet=0;
    char strState[8];
    char strName[NAME_LENGTH];

    strState[0]='\0';
    strName[0]='\0';

    int boo;
    for (boo =1; boo > 0; boo++) {
	// Reading ftpd entry
	strcpy(str_tag1_name, "ftpd");
	sprintf(str_tag2_name, "user%d", boo);
        strcpy(str_attribute_name, "state");
        read_attribute();
        if (find_status.value_found == 1) {
            sprintf(strState, "%s", str_attribute_value);

            strcpy(str_attribute_name, "name");
            read_attribute();
            if (find_status.value_found == 1) {
                sprintf(strName, "%s", str_attribute_value);
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

int GetFtpdEntry(int UserNumber, int *UserState, char *UserName, char *UserPassword,
    char *UserHome, int *UserWrite)
{
    int iRet=0;
    char strTmp[256];
    int PasswdLength = 0;

    strTmp[0] = '\0';
    
    // Reading ftpd entry
    strcpy(str_tag1_name, "ftpd");
    sprintf(str_tag2_name, "user%d", UserNumber);
    strcpy(str_attribute_name, "state");
    read_attribute();
    if (find_status.value_found == 1) {
        sprintf(strTmp, "%s", str_attribute_value);
	if (strcasecmp("enable", strTmp) == 0) {
	    *UserState = 1;
	} else {
	    *UserState = 0;
	}
	
        strcpy(str_attribute_name, "name");
        read_attribute();
        if (find_status.value_found == 1) {
    	    sprintf(UserName, "%s", str_attribute_value);
	}

        strcpy(str_attribute_name, "password");
        read_attribute();
        if (find_status.value_found == 1) {
	    base64Decode(UserPassword, str_attribute_value, &PasswdLength);
	    UserPassword[PasswdLength] = '\0';
//    	    sprintf(UserPassword, "%s", str_attribute_value);
	}
	
        strcpy(str_attribute_name, "home");
        read_attribute();
        if (find_status.value_found == 1) {
	    sprintf(UserHome, "%s", str_attribute_value);
	}

        strcpy(str_attribute_name, "write");
        read_attribute();
        if (find_status.value_found == 1) {
	    sprintf(strTmp, "%s", str_attribute_value);
	    if (strcasecmp("yes", strTmp) == 0) {
		*UserWrite = 1;
	    } else {
		*UserWrite = 0;
	    }
	}
    } else {
	iRet = -1;
    }

    return iRet;    
}
