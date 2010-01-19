#define MOUNTDIR	"/var/mounts"
#define MOUNT		"/bin/mount"
#define MOUNTNTFS3G	"/sbin/mount.ntfs-3g"
#define UMOUNT		"/bin/umount"
#define HOTPLUG		"/sbin/hotplug"
#define FSCK		"/sbin/fsck"
#define FSCK_OUT	"/var/fsck.out"
#define FSCK_OUT_END	"---OPERATION-COMPLETED---"

typedef struct {
    char dev[64];
    int mounted;
    char point[64];
    char fstype[16];
} partition;

int GetPartitionsAndMounts();
int ExecMount(char *device);
int ExecUmount(char *device, int kill);
int ExecFsck(char *device);
int readMounts(partition *arrayPartition, int partNum);
int readPartitions(partition *arrayPartition, int partNum);
int appendToTextFile(char *strFilename, char *strPrint);
void GetFsckVar();
void GetFsckRefresh(int secs);
void GetFsckStatus(char *device);
void GetFsckOut(char *device);

// util.c
int fileExists(char *filename);
