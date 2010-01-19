/* -------------------------------------------------------------------------- */
/*
	(c) 2005 U.S. Robotics Corporation.
*/
/* -------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------

	Return appropriate product name, based on wireless feature.

*/
function getProductName(strWireless)
{
	if (strWireless == "1")
		return "U.S. Robotics Wireless MAXg ADSL Gateway";
	else
		return "U.S. Robotics ADSL 4-Port Router";
}

// version for HTML
function getProductNameHTML(strWireless)
{
	if (strWireless == "1")
		return "U.S.&nbsp;Robotics Wireless <i>MAX</i>g ADSL Gateway";
	else
		return "U.S.&nbsp;Robotics ADSL 4-Port Router";
}


/* --------------------------------------------------------------------------

	Return <title> string for the page.

*/
function setPageTitle(strWireless)
{
	if (document.title == "")
		document.title = getProductName(strWireless);
	else
		document.title = getProductName(strWireless) + " » " + document.title;
}


/* --------------------------------------------------------------------------

	Page header and footer functions

*/

/*
	This function returns the filename of the current page.
	It looks for both slash and backward slash.
*/
function getPageFilename()
{
	var strFilename = document.URL;
	var ixSeparator = strFilename.lastIndexOf("/");
	var ixSeparator2 = strFilename.lastIndexOf("\\");
	if (ixSeparator2 > ixSeparator)
		ixSeparator = ixSeparator2;
	if (ixSeparator != -1)
		strFilename = strFilename.substr(ixSeparator + 1);
	return strFilename;
}

/*
	This writes out a page's header, including the optional menu bar.
	The title parameter is optional. If passed, it's written out
	as the header text.

	<ul id="idMenu">
		<li><label>Sunfishes</label>
			<ul>
				<li><a href="#">Blackbanded sunfish</a></li>
				<li><a href="#">Shadow bass</a></li>
				<li><a href="#">Ozark bass</a></li>
				<li><a href="#">White crappie</a></li>
			</ul>
		</li>

		<li><label>Grunts</label>
			<ul>
				<li><a href="#">Smallmouth grunt</a></li>
				<li><a href="#">Burrito</a></li>
				<li><a href="#">Pigfish</a></li>
			</ul>
		</li>

		<li><label>Remoras</label>
			<ul>
				<li><a href="#">Whalesucker</a></li>
				<li><a href="#">Marlinsucker</a></li>
				<li><a href="#">Ceylonese remora</a></li>
				<li><a href="#">Spearfish remora</a></li>
				<li><a href="#">Slender suckerfish</a></li>
			</ul>
		</li>
	</ul>
*/
function writeHeader(strTitle, strUsername, strStd, strProtocol, bIPext, bFirewall, bNAT, strWireless, bQoS)
{
//	var strFilename = getPageFilename();		  // get filename of current page

	document.write("<img id=\"idImgHeader\" src=\"usr_banner.gif\">\n");

	document.write("<div id=\"idHeader\">" + strTitle + "<\/div>\n");

	var menuMain = createMenu(strUsername, strStd, strProtocol, bIPext, bFirewall, bNAT, strWireless, bQoS);
	menuMain.write();

	document.write("<div id=\"idContent\">");
}


/*
	This outputs the header for a Setup page.
	The passed number indicates which step this is and how many steps there are.

	<img id="idImgHeader" src="banner.gif">

	<h2 class="clsSteps">Setup
		<span class="clsStepCurrent">1</span>
		<span class="clsStepNotDone">&hellip; 2 &hellip; 3</span>
	</h2>

	<h2 class="clsSteps">Setup
		<span class="clsStepDone">1 &hellip;</span>
		<span class="clsStepCurrent">2</span>
		<span class="clsStepNotDone">&hellip; 3</span>
	</h2>


	<h2 class="clsSteps">Setup
		<span class="clsStepDone">1 &hellip; 2 &hellip;</span>
		<span class="clsStepCurrent">3</span>
	</h2>
*/
function writeHeaderSetup(iStep, iSteps)
{
	document.write("<img id=\"idImgHeader\" src=\"usr_banner.gif\">");

	document.write("<div id=\"idSteps\">Setup ");

	for (var i = 1; i <= iSteps; ++i)
	{
		if (i == iStep)
			document.write("<span class=\"clsStepCurrent\">" + i + "<\/span> ");
		else
			document.write("<span class=\"clsStepNotDone\">" + i + "<\/span> ");

		if (i < iSteps)
			document.write("<span class=\"clsStepNotDone\">&hellip;<\/span> ");
	}
	document.write("<\/div>");

	document.write("<div id=\"idContent\">");
}


/*
	This writes the header for an "information" page.
*/
function writeHeaderInfo(strName)
{
	document.write("<img id=\"idImgHeader\" src=\"usr_banner.gif\">");
	document.write("<div id=\"idHeader\">" + strName + "<\/div>");
	document.write("<div id=\"idContent\">");
}


function writeFooter()
{
	document.write("<\/div>");	// close "idContent"

	document.write("<p class=\"clsCopyright\">&copy; 2004-2006 <a href=\"http://www.usr.com\">U.S. Robotics Corporation<\/a>.<\/p>");
}



/* --------------------------------------------------------------------------

	Wireless info functions

*/

/*
	This is called to set the visibility of fields on
	the Status and Tutorial pages.
*/
// initializeSecurity("<%ejGetWl(wlAuthMode)%>", "<%ejGetWl(wlWep)%>")
function initializeSecurity(strAuthMode, strWEPmode)
{
	var method = getSecurityMethod(strAuthMode, strWEPmode);
	if (method.indexOf("RADIUS") != -1)
	{
		// only show Encryption if also WPA
		if (method.indexOf("WPA") != -1)
			setVisibility("idEncrypt", true);

		setVisibility("idRADIUSkey", true);

		// Tutorial
		setVisibility("idKey8021x1", true);
		setVisibility("idKey8021x2", true);
		setVisibility("idKey8021x3", true);
	}
	else if (method.indexOf("WPA") != -1)
	{
		setVisibility("idEncrypt", true);
		setVisibility("idKeyWPA", true);
	}
	else if (method.indexOf("WEP") != -1)
	{
		setVisibility("idKeyWEP", true);
	}
}

// getSecurityMethod("<%ejGetWl(wlAuthMode)%>", "<%ejGetWl(wlWep)%>")
function getSecurityMethod(strAuthMode, strWEPmode)
{
	switch (strAuthMode)
	{
		case "open":
			if (strWEPmode == "enabled")
				return "WEP open";
			return "None";

		case "shared":			return "WEP shared";

		case "psk":				return "WPA";
		case "psk2":			return "WPA2";
		case "psk2mix":		return "WPA2 and WPA";

		case "wpa":				return "WPA with 802.1x (RADIUS)";
		case "wpa2":			return "WPA2 with 802.1x (RADIUS)";
		case "wpa2mix":		return "WPA2 and WPA with 802.1x (RADIUS)";
		case "radius":			return "802.1x (RADIUS)";

		default:					return "None";
	}

	return "";
}


/*
	Return encryption method used for WPA or WEP
*/
function getEncryption(strEncrypt)
{
	switch (strEncrypt)
	{
		case "aes":				return "AES";
		case "tkip":			return "TKIP";
		case "tkip+aes":		return "AES and TKIP";
		default:					return "";
	}
}


/*
	Return the WEP key specified by the current index.
*/
function getWEPkey(strWEPkeyIx, strKey1, strKey2, strKey3, strKey4)
{
	if (strWEPkeyIx == "")
		return "";

	var ixKey = parseInt(strWEPkeyIx);
	if ((ixKey >= 1) && (ixKey <= 4))
	{
		var keys = new Array(strKey1, strKey2, strKey3, strKey4);
		return keys[ixKey - 1];
	}

	return "";
}

function getWEPkeyType(strKey)
{
	return _getKeySize(strKey) + "-" + (_isKeyASCII(strKey) ? "ASCII" : "hex");
}

/*
	Functions to determine key type from the key length
*/

function _isKeyASCII(strKey)
{
	var len = strKey.length;
	return ((len == 5) || (len == 13));
}
function _isKeyHex(strKey)
{
	var len = strKey.length;
	return ((len == 10) || (len == 26));
}

function _getKeySize(strKey)
{
	var len = strKey.length;
	if ((len == 5) || (len == 10))
		return 64;
	if ((len == 13) || (len == 26))
		return 128;

	return 0;
}



/*
	Only call if WEP.

	This is called when the user changes a sub-type of
	encryption (WEP ASCII 128-bit, WEP hex 64-bit, ...).

	Type values:
		type128ASCII
		type128hex
		type64ASCII
		type64hex
*/
function setMaxLength(bClearValues, strType, eltKeys)
{
	var bHex = ((strType == "type128hex") || (strType == "type64hex"));

	var iBits = 128;
	if ((strType == "type64ASCII") || (strType == "type64hex"))
		iBits = 64;

	var iMaxLength = getKeyLength(iBits, bHex);

	document.getElementById("idKeyNumChars").innerHTML = iMaxLength;

	/*
		some page(s) might have only one key field,
		instead of an array of them, so check for that
	*/
	if (eltKeys.length == null)
	{
		eltKeys.maxLength = iMaxLength;
		if (bClearValues)
			eltKeys.value = "";
	}
	else
	{
		for (var i = 0; i < eltKeys.length; ++i)
		{
			eltKeys[i].maxLength = iMaxLength;
			if (bClearValues)
				eltKeys[i].value = "";
		}
	}
}

/*
				ASCII		Hex
	64-bits		 5		10
	128-bits		13		26
*/
function getKeyLength(bits, bHex)
{
	var len = 0;

	if (bHex)
	{
		// hex
		switch (bits)
		{
			case 64:		len = 10;	break;
			case 128:
			default:		len = 26;	break;
		}
	}
	else
	{
		// ASCII
		switch (bits)
		{
			case 64:		len = 5;	break;
			case 128:
			default:		len = 13;	break;
		}
	}

	return len;
}



/* --------------------------------------------------------------------------

	General helper functions

*/

/*
	This function tests the passed element's value for being inside
	the passed range--inclusive. If it's not, it displays an error
	and sets focus to the field...and returns false.
*/
function inRange(strName, elt, min, max)
{
	if ((elt.value == "") || (elt.value < min) || (elt.value > max))
	{
		alert("The " + strName + " setting must be between " + min + " and " + max + ", inclusive.");
		elt.focus();
		return false;
	}
	return true;
}

/*
	This function tests the passed element's length for being at least the
	passed number of characters. If it's not, it displays an error and sets
	focus to the field...and returns false.
*/
function minLength(strName, elt, min)
{
	if (elt.value.length < min)
	{
		alert("The " + strName + " field must have at least " + min + " characters.");
		elt.focus();
		return false;
	}

	return true;
}


function toggleVisibility(id)
{
	var elt = document.getElementById(id);
	if ((elt == null) || !elt.style || (elt.style.display == null))
		return;

	if (elt.style.display == "none")
		elt.style.display = "";
	else
		elt.style.display = "none";
}


function setVisibility(id, state)
{
	var elt = document.getElementById(id);
	if ((elt == null) || !elt.style || (elt.style.display == null))
		return;

	if (state)
		elt.style.display = "";
	else
		elt.style.display = "none";
}


function setEnabled(id, state)
{
	var elt = document.getElementById(id);
	if ((elt == null) || !elt.style || (elt.style.display == null))
		return;

	elt.disabled = !state;
}


function validateUsername(elt)
{
	if (elt.value.length == 0)
	{
		alert("Please enter a user name.");
		elt.focus();
		return false;
	}

	return isPrintable("User name", elt);
}

function validatePassword(elt)
{
	if (elt.value.length == 0)
	{
		alert("Please enter a password.");
		elt.focus();
		return false;
	}

	return isPrintable("Password", elt);
}


/*
	This function determines if the passed string contains only printable characters.
	It returns true if the string is okay. It displays an error and returns false
	if the string contains an invalid character.

	95 "printable" characters:
	<space>!"#$%&'()*+,-./:;<=>?@[\]^_`{|}~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz

	We support ALL "printable" characters in the ASCII and Extended ASCII table.
	These are character codes	32-126, 128-254, inclusive (decimal), or
										0x20-0x7E, 0x80-0xFE, inclusive (hex).
	www.lookuptables.com
*/
function isPrintable(strFieldName, elt)
{
	/*
		Note: We have to escape characters both for Javascript and for RegExp.
				Thus, " must be escaped ONLY for Javascript: \"
						\ and [ and ] must be escaped for RegExp and then the
						escape characters have to be escaped for Javascript:
						\\\\ and \\[ and \\]
	*/
	// Kludge to fix Safari regex bug with Unicode escape sequences
	var re = new RegExp("[^\\x20-\\x7E\\x80-\\xFE]", "g");

	var arMatch = elt.value.match(re);
	// Most browsers return null; Safari returns an empty match object
	if ((arMatch == null) || (arMatch.length == 0))
		return true;

	var strIllegalChars = "";
	for (var i = 0; i < arMatch.length; ++i)
		strIllegalChars += arMatch[i];

	// There should be some illegal chars at this point, but just in case...
	if (strIllegalChars == "")
		return true;

	// handle KWM Bug
	var strMsg2 = "\n\nPlease use only a-z, A-Z, 0-9, space, punctuation, or charcters in the extended ASCII set, such as accented characters.";
	if (arMatch.length == 1)
		alert("The " + strFieldName + " entry contains an invalid character: " + strIllegalChars + strMsg2);
	else
		alert("The " + strFieldName + " entry contains invalid characters: " + strIllegalChars + strMsg2);

	elt.focus();
	return false;
}
