#define NAME_LENGTH			256
#define PWD_LENGTH			256
#define DIR_LENGTH			256
#define MAX_FTPD_ROW_SIZE		512
#define MAX_FTPD_SECTION_SIZE		2048

int GetFtpd();
int GetFtpdModify(char *UserName);
int AddFtpdEntry(int UserState, char *UserName, char *UserPassword, char *UserHome, int UserWrite);
int ModifyFtpdEntry(char *OldUserName, int UserState, char *UserName, char *UserPassword, char *UserHome, int UserWrite);
int RemoveFtpdEntries(char *Users);
int GetUsersNumber();
int GetFtpdEntry(int UserNumber, int *UserState, char *UserName, char *UserPassword,
    char *UserHome, int *UserWrite);
