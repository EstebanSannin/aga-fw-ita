#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "errno.h"

#include "regbutton.h"
#include "../../shared/rsi_api.h"

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
    char contentType[64];
        
    strFileToLoad[0] = '\0';
    strStdTitle[0] = '\0';
    strStdText[0] = '\0';
    strEval[0] = '\0';
    strArg[0] = '\0';

    // Imposto il content type di default
    sprintf(contentType, "text/html");

    // Leggo gli argomenti e se ce n'è almeno uno esco
    numEntries = getFormData();
    if (numEntries < 0) {
	printf("Content-type: %s%c%c", contentType, 10, 10);
	printWrongArgument();
	exit(-1);
    }

//    DEBUG
//    printArgumentList(numEntries);
//    exit(0);
    
    sprintf(strFileToLoad, "regbutton.html");
    
    // Se il numero di argomenti è maggiore di 1 devo processare la richiesta
    // altrimenti devo solo visualizzare la pagina html
    if (numEntries == 3) {
	if (strncmp("save", entries[0].val, 4) == 0) {
            SetRegButtonScript(entries[1].val, entries[2].val);
	    
	    printf("Content-type: %s%c%c", contentType, 10, 10);
	    printf("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n");
	    printf("\n");
	    printf("<html>\n");
	    printf("<body>\n");
	    printf("<script>\n");
	    printf("\teval('location = \"regbutton.cgi\"');\n");
	    printf("</script>\n");
	    printf("</body>\n");
	    printf("</html>\n");
	    return 0;
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
	if (strcasecmp("GetRegButtonScript()", tag) == 0) {
	    GetRegButtonScript();
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
	    iRet = 0;
	} else {
	    for(x=0; clg[0] != '\0'; x++) {
		m = x;
		getword(entries[x].val, clg, '&');
//		plustospace(entries[x].val);
//		unescape_url(entries[x].val);
		getword(entries[x].name, entries[x].val,'=');
	    }
	    iRet = m + 1;
	}
    } else if ((cTmp != NULL) && (strcmp(cTmp, "POST") == 0)) {
	cTmp = getenv("CONTENT_TYPE");
        if ((cTmp == NULL) || (strcmp(cTmp, "application/x-www-form-urlencoded") != 0)) {
//            printf("This script can only be used to decode form results. \n");
    	    iRet = -1;
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
//    		plustospace(pVal);
//    		unescape_url(pVal);
    		pName = makeword(pVal, '=');
		strcpy(entries[x].val, pVal);
		strcpy(entries[x].name, pName);
	    }
    	    iRet = m + 1;
	}
    } else {
	iRet = -2;
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

int GetRegButtonScript()
{
    int iRet=0;
    char strAction[256];
    char strScript[MAX_SCRIPT_SIZE];
    
    // Reading regbutton parms
    strcpy(str_tag1_name, "regbutton");
    sprintf(str_tag2_name, "global");
    strcpy(str_attribute_name, "action");
    read_attribute();
    if (find_status.value_found == 1) {
        sprintf(strAction, "%s", str_attribute_value);

        strcpy(str_attribute_name, "script");
        read_attribute_big();
        if (find_status.value_found == 1) {
            sprintf(strScript, "%s", str_attribute_value_big);
        }

    } else {
        iRet = -1;
    }

    printf("var act = '%s';\n", strAction);
    printf("var sc = '%s';\n", strScript);

    return iRet;
}

int SetRegButtonScript(char *ArgAction, char *ArgScript)
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

    if ((xmlCfgLen + MAX_REGBUTTON_SECTION_SIZE) > rsiSize) {
	free(strXmlBuf);
	return -3;
    }

    char *ps, *pe;
    char *newRsi;
    long lTmpSize;

    read_section_bounds(strXmlBuf, "regbutton", &ps, &pe);
    if ((ps == NULL) || (pe == NULL)) {
	free(strXmlBuf);
	return -4;
    }
    while (*(pe + 1) == '\n') pe++;

    newRsi = (char *) malloc(rsiSize);
    memset(newRsi, 0xFF, rsiSize);
    memcpy(newRsi, strXmlBuf, ps - strXmlBuf);
    newRsi[ps - strXmlBuf] = '\0';

    sprintf(newRsi, "%s<regbutton>\n<global action=\"%s\" script=\"%s\" />\n</regbutton>\n",
        newRsi, ArgAction, ArgScript);
    
    lTmpSize = strlen(newRsi);
    memcpy(&newRsi[lTmpSize], pe + 1, strXmlBuf + xmlCfgLen - pe - 1);
    newRsi[lTmpSize + strXmlBuf + xmlCfgLen - pe - 1] = '\0';
//    printf("%s\n", newRsi);

    sysRsiSet(newRsi, rsiSize, 0);
        
    free(newRsi);
    free(strXmlBuf);
    
    return 0;
}

