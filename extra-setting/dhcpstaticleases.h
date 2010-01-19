#define MAC_LENGTH      18
#define IP_LENGTH       16
#define MAX_DHCP_ROW_SIZE	64
#define DHCPD_STATIC_LEASES_FILE        "/var/mounts/sda1/var/udhcpd_static_leases.conf"

int GetDhcpStaticLeasesNumber();
int GetDhcpStaticLeases();
int GetDhcpStaticLease(int LeaseNumber, char *MacAddress, char *IPAddress);
int AddDhcpStaticLeases(char *MacAddress, char *IPAddress);
int RemoveDhcpStaticLeases(char *MacAddress);
