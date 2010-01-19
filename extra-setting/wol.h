#define MAC_LENGTH			18
#define DESC_LENGTH			256
#define MAX_WOL_ROW_SIZE		256
#define PORT 2304

int GetWOLNumber();
int GetWOLList();
int GetWOLEntry(int WOLNumber, char *MacAddress, char *Description);
int AddWOLEntry(char *MacAddress, char *Description);
int RemoveWOLEntry(char *WOLEntries);
int hextobin(unsigned char c);
int SendMagicPacket(char *ifName, char *macAddress);
int SendMagicPackets(char *ifName, char *macAddresses);
