/* -------------------------------------------------------------------------- */
/*
   (c) 2004-2005 U.S. Robotics Corporation.
*/
/* -------------------------------------------------------------------------- */


/*
	Arguments:
		User name:		"admin" or "power" or "user"
		Standard:		"annex_c" or ... what?
		Protocol:		"Bridge" or ... what?
		IP extension:	0 | 1
		Firewall:		0 | 1
		NAT:				0 | 1
		Wireless:		0 | 1
		QoS:				0 | 1
*/
function createMenu(strUsername, strStd, strProtocol, bIPext, bFirewall, bNAT, strWireless, bQoS)
{
	if (strUsername == "user")
		return createMenuUser(strWireless);
	else
		return createMenuAdmin(strStd, strProtocol, bIPext, bFirewall, bNAT, strWireless, bQoS);
}


/* --------------------------------------------------------------------------

	These represent the various menus. There is a main menu bar and some sub-menus.

	The second and third strings in each item need to be translated.
	The first string is a filename and does not need to be translated.

	This is the only section that contains text to be translated.

	The tag below must not be modified. It is used by
	Translation to extract the text to be translated.
	*** TRANSLATION START -->
*/

// N.B.: All filenames must be in lowercase here.

function createMenuUser(strWireless)
{
	var bar = new MenuBar(7);

	var dropdown = bar.addDropdown("Status", 10);
	dropdown.addItem("General", "", "info.html");
	dropdown.addItem("DSL", "", "wancfg.cmd?action=view");
	dropdown.addItem("System Log", "", "logintro.html");
	dropdown.addItem("LAN Statistics", "", "statsifc.html");
	dropdown.addItem("WAN Statistics", "", "statswan.cmd");
	dropdown.addItem("ATM Statistics", "", "statsatm.cmd");
	dropdown.addItem("ADSL Statistics", "", "statsadsl.html");

	dropdown = bar.addDropdown("LAN", 10);
	dropdown.addItem("Routing - Routing Table", "", "rtroutecfg.cmd?action=view");
	dropdown.addItem("Routing - ARP Table", "", "arpview.cmd");

	dropdown = bar.addDropdown("Device", 10);
	dropdown.addItem("Back up Settings", "", "backupsettings.html");
	dropdown.addItem("Diagnostics", "", "diag.html");
	if	(strWireless == "1")
		dropdown.addItem("SNMP", "", "snmpconfig.html");

	return bar;
}


function createMenuAdmin(strStd, strProtocol, bIPext, bFirewall, bNAT, strWireless, bQoS)
{
	var bar = new MenuBar(7);

	/*
		STATUS
	*/
	var dropdown = bar.addDropdown("Status", 10);
	dropdown.addItem("General", "", "info.html");
	dropdown.addItem("DSL", "", "wancfg.cmd?action=view");
	dropdown.addItem("System Log", "", "logview.cmd");
	dropdown.addItem("LAN Statistics", "", "statsifc.html");
	dropdown.addItem("WAN Statistics", "", "statswan.cmd");
	dropdown.addItem("ATM Statistics", "", "statsatm.cmd");
	dropdown.addItem("ADSL Statistics", "", "statsadsl.html");


	/*
		INTERNET
	*/
	dropdown = bar.addDropdown("Internet", 17);
	dropdown.addItem("WAN Setup", "", "wancfg.cmd");
	if (strStd == "annex_c")
		dropdown.addItem("ADSL Settings", "", "adslcfgc.html");
	else
		dropdown.addItem("ADSL Settings", "", "adslcfg.html");

	if (	((strProtocol == "PPPoE") && !bIPext) ||
			((strProtocol == "PPPoA") && !bIPext) ||
			(strProtocol == "MER") ||
			(strProtocol == "IPoA"))
	{
		dropdown.addItem("Default Gateway", "", "rtdefaultcfg.html");
		dropdown.addItem("DNS Server", "", "dnscfg.html");
	}

	if (strProtocol != "Bridge")
	{
		dropdown.addItem("Access Control - IP Addresses", "", "scacccntr.cmd?action=view");
		dropdown.addItem("Access Control - Services", "", "scsrvcntr.cmd?action=view");
	}
	dropdown.addItem("Dynamic DNS", "", "ddnsmngr.cmd");


	/*
		LAN
	*/
	if (	((strProtocol == "PPPoE") && !bIPext) ||
			((strProtocol == "PPPoA") && !bIPext) ||
			(strProtocol == "MER") ||
			(strProtocol == "IPoA") ||
			(strProtocol == "Bridge"))
	{
		dropdown = bar.addDropdown("LAN", 13);
		if (	((strProtocol == "PPPoE") && !bIPext) ||
				((strProtocol == "PPPoA") && !bIPext) ||
				(strProtocol == "MER") ||
				(strProtocol == "IPoA"))
		{
			dropdown.addItem("DHCP Server", "", "lancfg2.html");
			dropdown.addItem("Mapped Interfaces", "", "portmap.cmd");
			dropdown.addItem("Routing - Routing Table", "", "rtroutecfg.cmd?action=view");
			dropdown.addItem("Routing - Static Routes", "", "rtroutecfg.cmd?action=viewcfg");
			dropdown.addItem("Routing - RIP", "", "ripcfg.cmd?action=view");
			dropdown.addItem("Routing - ARP Table", "", "arpview.cmd");
			dropdown.addItem("DHCP Clients", "", "dhcpinfo.html");
			if (bQoS)
				dropdown.addItem("Quality of Service", "", "qoscls.cmd?action=view");
		}
		else if (strProtocol == "Bridge")
		{
			dropdown.addItem("DHCP Server", "", "lancfg2.html");
			dropdown.addItem("Mapped Interfaces", "", "portmap.cmd");

			if (bQoS)
				dropdown.addItem("Quality of Service", "", "qoscls.cmd?action=view");
		}
	}


	/*
		WIRELESS
	*/
	if (strWireless == "1")
	{
		dropdown = bar.addDropdown("Wireless", 11);
		dropdown.addItem("Setup", "", "wlcfg.html");
		dropdown.addItem("Client List", "", "wlstationlist.cmd?");
		dropdown.addItem("AP Mode", "", "wlwds.cmd?action=view");
		dropdown.addItem("Advanced Settings", "", "wlcfgadv.html");
		dropdown.addItem("Quality of Service", "", "wlqos.cmd");
	}


	/*
		SECURITY
	*/
	dropdown = bar.addDropdown("Security", 17);
	if (strWireless == "1")
	{
		dropdown.addItem("Wireless", "", "wlsecurity.html");
		dropdown.addItem("MAC Filter", "", "wlmacflt.cmd?action=view");
	}
//WHY?	if (strProtocol != "Bridge")
	dropdown.addItem("Device Login", "", "password.html");
	if ((strProtocol != "Not Applicable") && !bIPext)
	{
		if (strProtocol == "Bridge")
		{
			dropdown.addItem("Internet Access Control", "", "todmngr.tod?action=view");
			dropdown.addItem("LAN MAC Filter", "", "scmacflt.cmd?action=view");
		}

		/*
			If it's not a bridge and the firewall is on, display filter commands
		*/
		if ((strProtocol != "Bridge") && bFirewall)
		{
			dropdown.addItem("Internet Access Control", "", "todmngr.tod?action=view");
			dropdown.addItem("IP Filtering - Outbound Filters", "", "scoutflt.cmd?action=view");
			dropdown.addItem("IP Filtering - Inbound Filters", "", "scinflt.cmd?action=view");
		}

		/*
			If NAT is enabled, display virtual server commands
		*/
		if (bNAT)
		{
			dropdown.addItem("Virtual Servers", "", "scvrtsrv.cmd?action=view");
			dropdown.addItem("Port Triggering", "", "scprttrg.cmd?action=view");
			dropdown.addItem("DMZ Host", "", "scdmz.html");
		}
		
		/*
			Display SIP ALG option
		*/
                dropdown.addItem("ALG", "", "algcfg.html");
	}


	/*
		DEVICE
	*/
	dropdown = bar.addDropdown("Device", 10);
	dropdown.addItem("Time", "", "sntpcfg.html");
	dropdown.addItem("Print Server", "", "ippcfg.html");
	dropdown.addItem("Back Up Settings", "", "backupsettings.html");
	dropdown.addItem("Restore Settings", "", "updatesettings.html");
	dropdown.addItem("Restart", "", "resetrouter.html");
	dropdown.addItem("Upgrade", "", "upload.html");
	dropdown.addItem("Restore Defaults", "", "defaultsettings.html");
	dropdown.addItem("Diagnostics", "", "diag.html");
	if	(strWireless == "1")
		dropdown.addItem("SNMP", "", "snmpconfig.html");

	/*
		EXTRA SETTINGS
	*/
	var dropdown = bar.addDropdown("Extra Settings", 15);
	dropdown.addItem("Go to Extra Settings page", "", "http://" + location.hostname + ":81/do.cgi?page=status");

	return bar;
}

/*
	<-- TRANSLATION END ***
	Nothing after this needs to be translated.
	The above tag must not be modified. It is used by
	Translation to extract the text to be translated.
*/
