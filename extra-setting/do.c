#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "errno.h"

#include "do.h"
#include "dhcpstaticleases.h"
#include "dmesg.h"
#include "partitions.h"
//#include "aptconfig.h"
#include "wlctlscan.h"
#include "wol.h"
#include "adslctl.h"
#include "backupsettings.h"
#include "resetsettings.h"
#include "antdiv.h"
#include "p910nd.h"
#include "ftpd.h"
#include "../../shared/samba.h"

// Variabili contenenti gli argomenti passati in form post o get
entry entries[MAX_ENTRIES];
int numEntries;

int main(int argc, char *argv[])
{    
    char strHtmlBuffer[MAX_HTML_SIZE];
    char strHtmlPrintBuffer[MAX_HTML_SIZE];
    char *pTmp, *pOld;
    char tag[64];
    char strFileToLoad[128];
    char strStdTitle[256];
    char strStdText[1024];
    char strEval[64];
    char strArg[64];
    int mountPresent = 0;
//    int accessGranted = 0;
    int nRet;
    char contentType[64];
        
    strFileToLoad[0] = '\0';
    strStdTitle[0] = '\0';
    strStdText[0] = '\0';
    strEval[0] = '\0';
    strArg[0] = '\0';

    // Imposto il content type di default
    sprintf(contentType, "text/html");

    // Leggo gli argomenti e se ce n'e' almeno uno esco
    numEntries = getFormData();
    if (numEntries < 0) {
	printf("Content-type: %s%c%c", contentType, 10, 10);
	printWrongArgument();
	exit(-1);
    }

//    DEBUG
//    printArgumentList(numEntries);
//    exit(0);

    
    // Il primo argomento e' il file html da caricare
    // Verifico se esiste se non e' aptconfig.xml
    if (strncmp("aptconfig", entries[0].val, 9) == 0) {
	sprintf(strFileToLoad, "%s.xml", entries[0].val);
    } else {
	sprintf(strFileToLoad, "%s.html", entries[0].val);
    }
    if (fileExists(strFileToLoad) != 0) {
        printf("Content-type: %s%c%c", contentType, 10, 10);
        printWrongArgument();
        exit (-2);
    }

    // Se non c'e' una mount /dev/sda1 imposto una variabile
    // Tutti quei servizi che richiedono la mount non potranno funzionare
    // DEBUG da mettere != per debuggare
    //if (verifyMount() != 0) {
    if (verifyMount() == 0) {
	mountPresent = 1;
    } else {
	mountPresent = 0;
    }
    
/*    if (verifyAccessControl() == 1) {
	accessGranted = 1;
    } else {
	accessGranted = 0;
    }*/
    
    // Se il numero di argomenti e' maggiore di 1 devo processare la richiesta
    // altrimenti devo solo visualizzare la pagina html
    if (numEntries > 1) {

	// Elenco fisso di possibili elaborazioni
	// DHCPSTATICLEASES
	if (strncmp("dhcpstaticleases", entries[0].val, 16) == 0) {
	    if (strncmp("add", entries[1].val, 3) == 0) {
		nRet = AddDhcpStaticLeases(entries[2].val, entries[3].val);
		switch (nRet) {
		    case 1:
			sprintf(strFileToLoad, STD_HTML_FILE);
			sprintf(strStdTitle, "DHCP Static Leases");
			sprintf(strStdText, "MAC address already exists");
			break;
		    case 2:
			sprintf(strFileToLoad, STD_HTML_FILE);
			sprintf(strStdTitle, "DHCP Static Leases");
			sprintf(strStdText, "IP address already exists");
			break;
		    case -1:
		    case -2:
			sprintf(strFileToLoad, STD_HTML_FILE);
			sprintf(strStdTitle, "DHCP Static Leases");
			sprintf(strStdText, "Generic error");
			break;
		    case 0:
			sprintf(strFileToLoad, EVAL_HTML_FILE);
			sprintf(strEval, "page=dhcpstaticleases");
		}
	    } else if (strncmp("remove", entries[1].val, 6) == 0) {
		nRet = RemoveDhcpStaticLeases(entries[2].val);
		switch (nRet) {
		    case -1:
			sprintf(strFileToLoad, STD_HTML_FILE);
			sprintf(strStdTitle, "DHCP Static Leases");
			sprintf(strStdText, "Generic error");
			break;
		    case 0:
			sprintf(strFileToLoad, EVAL_HTML_FILE);
			sprintf(strEval, "page=dhcpstaticleases");
		}
	    }
	// SAMBA
	} else if (strncmp("samba", entries[0].val, 5) == 0) {
	    if (numEntries != 7) {
		sprintf(strFileToLoad, STD_HTML_FILE);
		sprintf(strStdTitle, "Samba configuration");
		sprintf(strStdText, "Generic error");    
	    } else if (strncmp("modify", entries[1].val, 6) == 0) {
		if (strcasecmp("false", entries[2].val) == 0) {
		    if (strcasecmp("false", entries[6].val) == 0) {
			SetSambaRsiConf(0,entries[3].val, entries[4].val, entries[5].val, 0);
		    } else {
			SetSambaRsiConf(0,entries[3].val, entries[4].val, entries[5].val, 1);
		    }
		} else {
		    if (strcasecmp("false", entries[6].val) == 0) {
			SetSambaRsiConf(1, entries[3].val, entries[4].val, entries[5].val, 0);
		    } else {
			SetSambaRsiConf(1, entries[3].val, entries[4].val, entries[5].val, 1);
		    }
		}
		sprintf(strFileToLoad, EVAL_HTML_FILE);
		sprintf(strEval, "page=samba");
	    }
	// PARTITIONS
	} else if (strncmp("partitions", entries[0].val, 10) == 0) {
	    if (strncmp("mount", entries[1].val, 5) == 0) {
	        ExecMount(entries[2].val);
	    } else if (strncmp("umount", entries[1].val, 6) == 0) {
	        ExecUmount(entries[2].val, 0);
	    } else if (strncmp("killumount", entries[1].val, 10) == 0) {
	        ExecUmount(entries[2].val, 1);
	    }
	    sprintf(strFileToLoad, EVAL_HTML_FILE);
    	    sprintf(strEval, "page=partitions");
	//PARTITIONCHECK
	} else if (strncmp("partitioncheck", entries[0].val, 14) == 0) {
	    if (strncmp("view", entries[1].val, 4) == 0) {
	        sprintf(strArg, "%s", entries[2].val);
	    } else if (strncmp("start", entries[1].val, 4) == 0) {
	        ExecFsck(entries[2].val);
	        sprintf(strFileToLoad, EVAL_HTML_FILE);
	        sprintf(strEval, "page=partitioncheck&action=view&device=%s", entries[2].val);
	    }
	//WAKE ON LAN
	} else if (strncmp("wol", entries[0].val, 3) == 0) {
	    if (strncmp("add", entries[1].val, 3) == 0) {
		nRet = AddWOLEntry(entries[2].val, entries[3].val);
		switch (nRet) {
		    case 1:
			sprintf(strFileToLoad, STD_HTML_FILE);
			sprintf(strStdTitle, "Wake On Lan");
			sprintf(strStdText, "MAC address already exists");
			break;
		    case -1:
		    case -2:
			sprintf(strFileToLoad, STD_HTML_FILE);
			sprintf(strStdTitle, "Wake On Lan");
			sprintf(strStdText, "Generic error");
			break;
		    case 0:
			sprintf(strFileToLoad, EVAL_HTML_FILE);
			sprintf(strEval, "page=wol");
		}
	    } else if (strncmp("remove", entries[1].val, 6) == 0) {
		nRet = RemoveWOLEntry(entries[2].val);
		switch (nRet) {
		    case -1:
			sprintf(strFileToLoad, STD_HTML_FILE);
			sprintf(strStdTitle, "Wake On Lan");
			sprintf(strStdText, "Generic error");
			break;
		    case 0:
			sprintf(strFileToLoad, EVAL_HTML_FILE);
			sprintf(strEval, "page=wol");
		}
	    } else if (strncmp("send", entries[1].val, 4) == 0) {
		nRet = SendMagicPackets("br0", entries[2].val);
		sprintf(strFileToLoad, EVAL_HTML_FILE);
	        switch (nRet) {
		    case 0:
	    		sprintf(strEval, "page=wol&action=sent&result=success");
			break;
		    default:
	    		sprintf(strEval, "page=wol&action=sent&result=error");
			break;
		}
	    } else if (strncmp("sent", entries[1].val, 4) == 0) {
		if (strncmp("success", entries[2].val, 7) == 0) {
		    sprintf(strFileToLoad, STD_HTML_FILE);
		    sprintf(strStdTitle, "Wake On Lan");
		    sprintf(strStdText, "Magic packets succesfully sent");    
		} else {
		    sprintf(strFileToLoad, STD_HTML_FILE);
		    sprintf(strStdTitle, "Wake On Lan");
		    sprintf(strStdText, "Error: unable to send magic packets");    
		}
	    }
	// ADSLCTL
	} else if (strncmp("adslctl", entries[0].val, 7) == 0) {
	    if (numEntries != 5) {
		sprintf(strFileToLoad, STD_HTML_FILE);
		sprintf(strStdTitle, "adslctl configuration");
		sprintf(strStdText, "Generic error");    
	    } else if (strncmp("modify", entries[1].val, 6) == 0) {
		int ena = 0;
		long int snr = -1;
		int trellis = 0;
		if (strcasecmp("true", entries[2].val) == 0) {
		    ena = 1;
		}
		if (strcasecmp("true", entries[4].val) == 0) {
		    trellis = 1;
		}
        	snr = strtol(entries[3].val, NULL, 10);
	        if ((errno == EINVAL) || (errno == ERANGE)) {
		    sprintf(strFileToLoad, STD_HTML_FILE);
		    sprintf(strStdTitle, "adslctl configuration");
		    sprintf(strStdText, "Generic error");    
		} else {
		    SetadslctlRsiConf(ena, snr, trellis);
		    sprintf(strFileToLoad, EVAL_HTML_FILE);
		    sprintf(strEval, "page=adslctl");
		}
	    }
	// ANTENNA DIVERSITY
	} else if (strncmp("antdiv", entries[0].val, 6) == 0) {
	    if (numEntries != 3) {
		sprintf(strFileToLoad, STD_HTML_FILE);
		sprintf(strStdTitle, "Antenna Diversity configuration");
		sprintf(strStdText, "Generic error");    
	    } else if (strncmp("modify", entries[1].val, 6) == 0) {
		int ena = 0;
		if (strcasecmp("true", entries[2].val) == 0) {
		    ena = 1;
		}
		SetAntDivRsiConf(ena);
		sprintf(strFileToLoad, EVAL_HTML_FILE);
		sprintf(strEval, "page=antdiv");
	    }
	// PRINTER
	} else if (strncmp("p910nd", entries[0].val, 6) == 0) {
	    if (numEntries != 3) {
		sprintf(strFileToLoad, STD_HTML_FILE);
		sprintf(strStdTitle, "Printer configuration");
		sprintf(strStdText, "Generic error");    
	    } else if (strncmp("modify", entries[1].val, 6) == 0) {
		int ena = 0;
		if (strcasecmp("true", entries[2].val) == 0) {
		    ena = 1;
		}
		SetP910ndRsiConf(ena);
		sprintf(strFileToLoad, EVAL_HTML_FILE);
		sprintf(strEval, "page=p910nd");
	    }
	// FTPD
	} else if (strncmp("ftpdmodify", entries[0].val, 10) == 0) {
	    if (strncmp("modify", entries[1].val, 6) == 0) {
		sprintf(strArg, "%s", entries[2].val);
	    }
	} else if (strncmp("ftpd", entries[0].val, 4) == 0) {
	    if (numEntries < 2) {
		sprintf(strFileToLoad, STD_HTML_FILE);
		sprintf(strStdTitle, "FTPD configuration");
		sprintf(strStdText, "Generic error");    
	    } else if (strncmp("add", entries[1].val, 3) == 0) {
		int userState = 0;
		int userWrite = 0;

		if (strcasecmp("true", entries[2].val) == 0)
		    userState = 1;
		if (strcasecmp("true", entries[6].val) == 0)
		    userWrite = 1;

		AddFtpdEntry(userState, entries[3].val, entries[4].val,
		    entries[5].val, userWrite);
		sprintf(strFileToLoad, EVAL_HTML_FILE);
		sprintf(strEval, "page=ftpd");
	    } else if (strncmp("modify", entries[1].val, 6) == 0) {
		int userState = 0;
		int userWrite = 0;

		if (strcasecmp("true", entries[3].val) == 0)
		    userState = 1;
		if (strcasecmp("true", entries[7].val) == 0)
		    userWrite = 1;

		ModifyFtpdEntry(entries[2].val, userState, entries[4].val,
		    entries[5].val, entries[6].val, userWrite);
		sprintf(strFileToLoad, EVAL_HTML_FILE);
		sprintf(strEval, "page=ftpd");
	    } else if (strncmp("remove", entries[1].val, 6) == 0) {
		RemoveFtpdEntries(entries[2].val);
		sprintf(strFileToLoad, EVAL_HTML_FILE);
		sprintf(strEval, "page=ftpd");
	    }
	// BACKUPSETTINGS
	} else if (strncmp("backupsettings", entries[0].val, 14) == 0) {
	    if (numEntries != 2) {
		sprintf(strFileToLoad, STD_HTML_FILE);
		sprintf(strStdTitle, "Backup Extra Settings");
		sprintf(strStdText, "Generic error");    
	    } else if (strncmp("backup", entries[1].val, 6) == 0) {
		DoBackupSettings();
		return 0;
	    }
	// RESTORESETTINGS
	} else if (strncmp("restoresettings", entries[0].val, 15) == 0) {
	    if (numEntries != 2) {
		sprintf(strFileToLoad, STD_HTML_FILE);
		sprintf(strStdTitle, "Restore Extra Settings");
		sprintf(strStdText, "Generic error");
	    } else if (strncmp("success", entries[1].val, 7) == 0) {
		sprintf(strFileToLoad, STD_HTML_FILE);
		sprintf(strStdTitle, "Restore Extra Settings");
		sprintf(strStdText, "Backup Settings restored");    
	    } else if (strncmp("error", entries[1].val, 5) == 0) {
		sprintf(strFileToLoad, STD_HTML_FILE);
		sprintf(strStdTitle, "Restore Extra Settings");
		sprintf(strStdText, "Error: restore failed");    
	    }
	// RESETSETTINGS
	} else if (strncmp("resetsettings", entries[0].val, 13) == 0) {
	    if (numEntries != 2) {
		sprintf(strFileToLoad, STD_HTML_FILE);
		sprintf(strStdTitle, "Reset Extra Settings");
		sprintf(strStdText, "Generic error");
	    } else if (strncmp("reset", entries[1].val, 5) == 0) {
		DoResetSettings();
		sprintf(strFileToLoad, STD_HTML_FILE);
		sprintf(strStdTitle, "Reset Extra Settings");
		sprintf(strStdText, "Reset completed successfully");
	    }
	}
    }
    
    // Leggo in memoria l'intero file html: okkio alla dimensione fissa del buffer
    strHtmlBuffer[0] = '\0';
    readWholeFile(strFileToLoad, strHtmlBuffer, MAX_HTML_SIZE);

    // Stampo il content type
    printf("Content-type: %s%c%c", contentType, 10, 10);

    // Faccio il parsing del file html alla ricerca delle tag <%TAG%> e chiamo
    // le relative funzioni che scrivono in stdout il codice html in
    // sostituzione alle tag
    pTmp = strHtmlBuffer;
    pOld = pTmp;

    while ((pTmp = findTag(pOld, tag)) != NULL) {
	strncpy(strHtmlPrintBuffer, pOld, pTmp - pOld);
	strHtmlPrintBuffer[pTmp - pOld] = '\0';
	printf("%s", strHtmlPrintBuffer);
	if (strcasecmp("GetStatus()", tag) == 0) {
	    if (verifyAccessControl() == 1) {
		printf("<p>Access to Extra Settings web pages from Internet is enabled.<br>");
		printf("To deny remote access to Extra Settings web pages go to <b>Internet</b> -> <b>Access Control - Services</b> menu<br>");
		printf("and deselect the <b>HTTP Internet</b> check box. Restart the system to make changes effective.</p>");
	    } else {
		printf("<p>Access to Extra Settings web pages from Internet is disabled.<br>");
		printf("To allow remote access to Extra Settings web pages go to <b>Internet</b> -> <b>Access Control - Services</b> menu<br>");
		printf("and select the <b>HTTP Internet</b> check box. Restart the system to make changes effective.</p>");
	    }
	    printf("<p>If you want to use all Extra Settings features you need to configure this device as a router (not in bridge mode).</p>");
	} else if (strcasecmp("GetDhcpStaticLeases()", tag) == 0) {
	    GetDhcpStaticLeases();
	} else if (strcasecmp("GetSambaConf()", tag) == 0) {
	    GetSambaConf();
	} else if (strcasecmp("Getdmesg()", tag) == 0) {
	    Getdmesg();
	} else if (strcasecmp("GetPartitionsAndMounts()", tag) == 0) {
	    GetPartitionsAndMounts();
	} else if (strcasecmp("GetFsckVar()", tag) == 0) {
	    GetFsckVar();
	} else if (strcasecmp("GetFsckRefresh()", tag) == 0) {
	    GetFsckRefresh(2);
	} else if (strcasecmp("GetFsckStatus()", tag) == 0) {
	    GetFsckStatus(strArg);
	} else if (strcasecmp("GetFsckOut()", tag) == 0) {
	    GetFsckOut(strArg);
	} else if (strcasecmp("GetwlctlScan()", tag) == 0) {
	    GetwlctlScan();
	} else if (strcasecmp("GetWOLLastResult()", tag) == 0) {
	    printf(strArg);
	} else if (strcasecmp("GetWOLList()", tag) == 0) {
	    GetWOLList();
	} else if (strcasecmp("GetadslctlConf()", tag) == 0) {
	    GetadslctlConf();
	} else if (strcasecmp("GetAntDivConf()", tag) == 0) {
	    GetAntDivConf();
	} else if (strcasecmp("GetP910ndConf()", tag) == 0) {
	    GetP910ndConf();
	} else if (strcasecmp("GetFtpd()", tag) == 0) {
	    GetFtpd();
	} else if (strcasecmp("GetFtpdModify()", tag) == 0) {
	    GetFtpdModify(strArg);
/*	} else if (strcasecmp("ReadXMLFile()", tag) == 0) {
	    ReadXMLFile(strArg); */
	} else if (strcasecmp("GetStdTitle()", tag) == 0) {
	    printf(strStdTitle);
	} else if (strcasecmp("GetStdText()", tag) == 0) {
	    printf(strStdText);
	} else if (strcasecmp("GetEval()", tag) == 0) {
	    printf(strEval);
	}

	pTmp = pTmp + strlen(tag) + 6;
	pOld = pTmp;
    }
    printf("%s", pOld);

    return 0;
}

// Trova la tag tagName nel buffer
char *findTag(char *buffer, char *tagName)
{
    char *pStart, *pEnd;

    pStart = strstr(buffer, "<%ro");
    if (pStart != NULL) {
	pEnd = strstr(pStart, "%>");
	strncpy(tagName, pStart + 4, pEnd - pStart - 4);
	tagName[pEnd - pStart - 4] = '\0';
    } else {
	tagName[0] = '\0';
    }
    return pStart;
}

// Legge l'intero file e lo mette in buffer
int readWholeFile(char *FileName, char *buffer, int bufferSize)
{
    struct stat s;
    FILE *pFileConf;
    size_t result;
    int iRet;

    if (stat(FileName, &s) == 0) {
	if (bufferSize >= s.st_size) {
    	    if ((pFileConf = fopen(FileName, "rb")) != NULL) {
		result = fread(buffer, 1, s.st_size, pFileConf);
		if (result == s.st_size) {
		    buffer[result] = '\0';
		    iRet = 0;
		} else {
		    iRet = -4;
		}
		fclose (pFileConf);
    	    } else {
        	iRet = -3;
    	    }
	} else {
	    iRet = -2;
	}
    } else {
        iRet = -1;
    }

    return iRet;
}

// Verifica se esiste una mount di /dev/sda1
int verifyMount()
{
    int iRet;
    FILE *fs;
    char sMDev[64], sMPoint[64];
    char s1[64], s2[64], s3[64], s4[64];
    struct stat s;
    mode_t perms = 0777;
    
    iRet = -1;
    if ((fs=fopen("/proc/mounts","r")) != NULL) {
        while (fscanf(fs, "%s%s%s%s%s%s", sMDev, sMPoint, s1, s2, s3, s4) != EOF) {
	    if (strncmp(sMPoint, MOUNT_DIR, 16) == 0) {
		iRet = 0;
	    }
	}
	fclose(fs);
    } else {
	iRet = -2;
    }
    
    if (iRet == 0) {
	if (stat(MOUNT_VAR_DIR, &s) == 0) {
	    if (!S_ISDIR(s.st_mode)) {
		iRet = -3;
	    }
	} else {
	    mkdir(MOUNT_VAR_DIR, perms);
	}
    }
    
    return iRet;
}

// Ricava gli argomenti dalle richieste post e get e riempie un array globale
// ricavato dagli esempi www.w3.org/CGI/
int getFormData()
{
    register int x, m = 0;
    char *clg;
    int clp;
    int iRet = 0;
    char *cTmp;

    cTmp = getenv("REQUEST_METHOD");
    
    if ((cTmp != NULL) && (strcmp(cTmp, "GET") == 0)) {
	clg = getenv("QUERY_STRING");
	if(clg == NULL) {
//	    printf("No query information to decode.\n");
	    iRet = -1;
	} else {
	    for(x=0; clg[0] != '\0'; x++) {
		m = x;
		getword(entries[x].val, clg, '&');
		plustospace(entries[x].val);
		unescape_url(entries[x].val);
		getword(entries[x].name, entries[x].val,'=');
	    }
	    iRet = m + 1;
	}
    } else if ((cTmp != NULL) && (strcmp(cTmp, "POST") == 0)) {
	cTmp = getenv("CONTENT_TYPE");
        if ((cTmp == NULL) || (strcmp(cTmp, "application/x-www-form-urlencoded") != 0)) {
//            printf("This script can only be used to decode form results. \n");
    	    iRet = -2;
	} else {
	    clp = atoi(getenv("CONTENT_LENGTH"));
	    
	    char *pVal, *pName;
	    for(x = 0; clp && (!feof(stdin)); x++) {
    		m = x;
//    		entries[x].val = fmakeword(stdin, '&', &clp);
//    		plustospace(entries[x].val);
//    		unescape_url(entries[x].val);
//    		entries[x].name = makeword(entries[x].val, '=');
    		pVal = fmakeword(stdin, '&', &clp);
    		plustospace(pVal);
    		unescape_url(pVal);
    		pName = makeword(pVal, '=');
		strcpy(entries[x].val, pVal);
		strcpy(entries[x].name, pName);
	    }
    	    iRet = m + 1;
	}
    } else {
	iRet = -3;
    }

    return iRet;
}

// Stampa una riga di html in caso di argomenti errati
void printWrongArgument(int errNumber)
{
    printf("<html><body>\n");
    printf("<h3>Wrong argument</h3>");
    printf("</body></html>\n");
}

// Stampa l'elenco degli argomenti (solo debug)
void printArgumentList(int iArgNum)
{
    int i;

    printf("<html><body>\n");
    printf("<b>Variabili:</b> %d<br>\n", iArgNum);
    printf("<br>\n");
    
    for (i=0; i<iArgNum; i++) {
	printf("<b>Name:</b> %s<br><b>Val:</b> %s<br>\n", entries[i].name, entries[i].val);
	printf("<br>\n");
    }
    printf("</body></html>\n");
}

// return 1 if access is granted, 0 if access is not granted
int verifyAccessControl()
{
    FILE *fp;
    char line[256];
    char *token;
    int iRet = 0;
    int ruleFound = 0;
    
    fp = popen("iptables -L INPUT 2>/dev/null", "r");   
    if (fp != NULL) {
	while (fgets(line, sizeof(line), fp)) {
	    token=strtok(line, " \t");
	    if ((token != NULL) && (strcmp("ACCEPT", token) == 0)) {
		token=strtok(NULL, " \t");
		if ((token != NULL) && (strcmp("tcp", token) == 0)) {
		    token=strtok(NULL, " \t");
		    if ((token != NULL) && (strcmp("--", token) == 0)) {
			token=strtok(NULL, " \t");
			if ((token != NULL) && (strcmp("anywhere", token) == 0)) {
			    token=strtok(NULL, " \t");
			    if ((token != NULL) && (strcmp("anywhere", token) == 0)) {
				token=strtok(NULL, " \t");
				if ((token != NULL) && (strcmp("tcp", token) == 0)) {
				    token=strtok(NULL, " \t");
				    if ((token != NULL) && (strcmp("dpt:81", token) == 0)) {
					ruleFound = 1;
				    }
				}
			    }
			}
		    }
		}
	    }
	}
	pclose(fp);
    } else {
	iRet = -1;
    }
    
    if (ruleFound == 1) {
	iRet = 1;
    }
    
    return iRet;
}
