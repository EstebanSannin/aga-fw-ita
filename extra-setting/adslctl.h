#define MAX_STRING_SIZE                 64

#define MAX_ADSLCTL_SECTION_SIZE 1024
#define MAX_ADSLCTL_UPDATE_SIZE 1024

int GetadslctlConf();
int SetadslctlRsiConf(int ArgEnable, int ArgSnr, int ArgTrellis);
