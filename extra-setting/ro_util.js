function createroMenuES()
{
	var bar = new MenuBar(7);

	/*
		EXTRA SETTINGS - Status
	*/
	var dropdown = bar.addDropdown("Status", 8);
	dropdown.addItem("Status", "", "do.cgi?page=status");

	/*
		EXTRA SETTINGS - Share
	*/
	var dropdown = bar.addDropdown("Share", 12);
	dropdown.addItem("FTPD Configuration", "", "do.cgi?page=ftpd");
	dropdown.addItem("Samba Configuration", "", "do.cgi?page=samba");
	dropdown.addItem("Partitions and Mounts", "", "do.cgi?page=partitions");
	dropdown.addItem("Printer", "", "do.cgi?page=p910nd");

	/*
		EXTRA SETTINGS - LAN
	*/
	var dropdown = bar.addDropdown("LAN/WLAN", 12);
	dropdown.addItem("DHCP Static Leases", "", "do.cgi?page=dhcpstaticleases");
	dropdown.addItem("Antenna Diversity", "", "do.cgi?page=antdiv");
	dropdown.addItem("Wireless Scanner", "", "do.cgi?page=wlctlscan");
	dropdown.addItem("Wake On Lan", "", "do.cgi?page=wol");

	/*
		EXTRA SETTINGS - WAN
	*/
	var dropdown = bar.addDropdown("WAN", 12);
	dropdown.addItem("adslctl Configuration", "", "do.cgi?page=adslctl");

	/*
		EXTRA SETTINGS - Utility
	*/
	var dropdown = bar.addDropdown("Utility", 13);
	dropdown.addItem("Backup Extra Settings", "", "do.cgi?page=backupsettings");
	dropdown.addItem("Restore Extra Settings", "", "do.cgi?page=restoresettings");
	dropdown.addItem("Reset Extra Settings", "", "do.cgi?page=resetsettings");
	dropdown.addItem("Startup Script", "", "profile.cgi");
	dropdown.addItem("REG Button", "", "regbutton.cgi");
	dropdown.addItem("dmesg", "", "do.cgi?page=dmesg");

	/*
		MAIN MENU
	*/
	var dropdown = bar.addDropdown("Main Menu", 15);
	dropdown.addItem("Go to Main Menu page", "", "http://" + location.hostname);

	return bar;
}

function createroMenu()
{
    var menuMain = createroMenuES();
    menuMain.write();
}
