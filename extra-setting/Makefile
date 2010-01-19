# Makefile file for cgi by roleo
#
# License:  GNU General Public License.
#
CFLAGS= -Wall -s -Os -fomit-frame-pointer \
        -I. -I$(INC_BRCMCFM_PATH) \
        -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD) -I$(INC_BRCMDRIVER_PRIV_PATH)/$(BRCM_BOARD) \
        -I$(BROADCOM_CFM_DIR)/inc -I$(BROADCOM_CFM_DIR)/util/psi
        
LDFLAGS=-Wl,-allow-shlib-undefined -L $(INSTALL_DIR)/lib

all: do.cgi restoresettings.cgi profile.cgi

LIBS=-lpsi -lpsixml

HEADS1=do.h dhcpstaticleases.h dmesg.h partitions.h wlctlscan.h wol.h \
adslctl.h backupsettings.h resetsettings.h antdiv.h ftpd.h p910nd.h \
../../shared/killprocess.h ../../shared/samba.h ../../shared/util.h \
../../shared/rsi_api.h
OBJS1=do.o dhcpstaticleases.o dmesg.o partitions.o wlctlscan.o wol.o \
adslctl.o backupsettings.o resetsettings.o antdiv.o ftpd.o p910nd.o \
../../shared/killprocess.o ../../shared/samba.o ../../shared/util.o \
../../shared/rsi_api.o
SRCS1=do.c dhcpstaticleases.c dmesg.c partitions.c wlctlscan.c wol.c \
adslctl.c backupsettings.c resetsettings.c antdiv.c ftpd.c p910nd.c \
../../shared/killprocess.c ../../shared/samba.c ../../shared/util.c \
../../shared/rsi_api.c

HEADS2=restoresettings.h
OBJS2=restoresettings.o
SRCS2=restoresettings.c

HEADS3=profile.h ../../shared/rsi_api.h ../../shared/util.h
OBJS3=profile.o ../../shared/rsi_api.o ../../shared/util.o
SRCS3=profile.c ../../shared/rsi_api.c ../../shared/util.c

HEADS4=regbutton.h ../../shared/rsi_api.h ../../shared/util.h
OBJS4=regbutton.o ../../shared/rsi_api.o ../../shared/util.o
SRCS4=regbutton.c ../../shared/rsi_api.c ../../shared/util.c

install:
	install -m 755 do.cgi $(INSTALL_DIR)/webs2
	install -m 755 restoresettings.cgi $(INSTALL_DIR)/webs2
	install -m 755 profile.cgi $(INSTALL_DIR)/webs2
	install -m 755 regbutton.cgi $(INSTALL_DIR)/webs2
	$(STRIP) $(INSTALL_DIR)/webs2/do.cgi
	$(STRIP) $(INSTALL_DIR)/webs2/restoresettings.cgi
	$(STRIP) $(INSTALL_DIR)/webs2/profile.cgi
	$(STRIP) $(INSTALL_DIR)/webs2/regbutton.cgi
	install -m 644 adslctl.html $(INSTALL_DIR)/webs2
	install -m 644 antdiv.html $(INSTALL_DIR)/webs2
	install -m 644 backupsettings.html $(INSTALL_DIR)/webs2
	install -m 644 dhcpstaticleases.html $(INSTALL_DIR)/webs2
	install -m 644 dhcpstaticleasesadd.html $(INSTALL_DIR)/webs2
	install -m 644 dmesg.html $(INSTALL_DIR)/webs2
	install -m 644 eval.html $(INSTALL_DIR)/webs2
	install -m 644 ftpd.html $(INSTALL_DIR)/webs2
	install -m 644 ftpdadd.html $(INSTALL_DIR)/webs2
	install -m 644 ftpdmodify.html $(INSTALL_DIR)/webs2
	install -m 644 p910nd.html $(INSTALL_DIR)/webs2
	install -m 644 partitioncheck.html $(INSTALL_DIR)/webs2
	install -m 644 partitions.html $(INSTALL_DIR)/webs2
	install -m 644 profile.html $(INSTALL_DIR)/webs2
	install -m 644 regbutton.html $(INSTALL_DIR)/webs2
	install -m 644 resetsettings.html $(INSTALL_DIR)/webs2
	install -m 644 restoresettings.html $(INSTALL_DIR)/webs2
	install -m 644 samba.html $(INSTALL_DIR)/webs2
	install -m 644 status.html $(INSTALL_DIR)/webs2
	install -m 644 std.html $(INSTALL_DIR)/webs2
	install -m 644 wlctlscan.html $(INSTALL_DIR)/webs2
	install -m 644 wol.html $(INSTALL_DIR)/webs2
	install -m 644 woladd.html $(INSTALL_DIR)/webs2
	install -m 644 other/usr_main.css $(INSTALL_DIR)/webs2
	install -m 644 other/usr_menus.css $(INSTALL_DIR)/webs2
	install -m 644 other/usr_menus.js $(INSTALL_DIR)/webs2
	install -m 644 other/util.js $(INSTALL_DIR)/webs2
	install -m 644 other/ro_util.js $(INSTALL_DIR)/webs2
	install -m 644 other/p_red.gif $(INSTALL_DIR)/webs2

dynamic: do.cgi restoresettings.cgi profile.cgi regbutton.cgi install

do.cgi: $(HEADS1) $(OBJS1) $(BOARD_API_OBJ) $(SYSCALL_OBJ)
	$(CC) $(LDFLAGS) $(OBJS1) $(BOARD_API_OBJ) $(SYSCALL_OBJ) $(LIBS) -o do.cgi

restoresettings.cgi: $(HEADS2) $(OBJS2) $(BOARD_API_OBJ) $(SYSCALL_OBJ)
	$(CC) $(LDFLAGS) $(OBJS2) $(BOARD_API_OBJ) $(SYSCALL_OBJ) $(LIBS) -o restoresettings.cgi

profile.cgi: $(HEADS3) $(OBJS3) $(BOARD_API_OBJ) $(SYSCALL_OBJ)
	$(CC) $(LDFLAGS) $(OBJS3) $(BOARD_API_OBJ) $(SYSCALL_OBJ) $(LIBS) -o profile.cgi

regbutton.cgi: $(HEADS4) $(OBJS4) $(BOARD_API_OBJ) $(SYSCALL_OBJ)
	$(CC) $(LDFLAGS) $(OBJS4) $(BOARD_API_OBJ) $(SYSCALL_OBJ) $(LIBS) -o regbutton.cgi

clean:
	-rm -f do.cgi restoresettings.cgi profile.cgi $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(BOARD_API_OBJ) $(SYSCALL_OBJ)
