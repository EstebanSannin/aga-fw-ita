/* -------------------------------------------------------------------------- */
/*
   (c) 2004-2005 U.S. Robotics Corporation.
*/
/* -------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------

	Define the information for a single menu item (both main bar and sub-menu).
	Each menu item has an associated filename, name, and description.
	The name and description are translated, but not the filename.

	The menu bar object contains an array of dropdown menus.
	The dropdown menu object contains an array of menu commands.

	The calling page should call the appropriate members to construct a menubar.
	Example:
		var bar = new MenuBar(7);

		var dropdown = bar.addDropdown("Menu One", 20);
		dropdown.addItem("Command One",		"This is the first item",	"#");
		dropdown.addItem("Command Two",		"This is the second item",	"#");
		dropdown.addItem("Command Three",	"This is the third item",	"#");

		dropdown = bar.addDropdown("Percoidei", 10);
		dropdown.addItem("Remoras",		"",	"#");
		dropdown.addItem("Tigerfishes",	"",	"#");
*/

function MenuBar(iWidth)
{
	// no sense passing this in because it's hardcoded in the CSS
	this._strID = "idMenu";		// id of the entire menubar

	this._iWidth = iWidth;		// width of each menu item on the bar
	this._menus = new Array();	// array of dropdown menus


	this.addDropdown = function(strName, iWidth)
	{
		var dropdown = new MenuDropdown(strName, iWidth);
		this._menus.push(dropdown);
		return dropdown;
	};

	/*
		This writes out the menu bar and its dropdown menus.
	*/
	this.write = function()
	{
		document.write("<ul id=\"" + this._strID + "\">\n");
		for (var i = 0; i < this._menus.length; ++i)
			this._menus[i].write(this._iWidth);
		document.write("<\/ul>\n");
	};
}


/*
	This represents a dropdown menu.
*/
function MenuDropdown(strName, iWidth)
{
	this._strName = strName;
	this._iWidth = iWidth;
	this._menuItems = new Array();


	this.addItem = function(strName, strDescription, strFilename)
	{
		this._menuItems.push(new MenuCommand(strName, strDescription, strFilename));
	};


	/*
		Writes the HTML for a dropdown menu.

		<li width=X><label width=X>Menu One</label>
			<ul width=Y>
				.
				.
			</ul>
		</li>
	*/
	this.write = function(iWidth)
	{
		document.write("<li style=\"width: " + iWidth + "em;\"><label style=\"width: " + iWidth + "em;\">" + this._strName + "<\/label>");
		document.write("	<ul style=\"width: " + this._iWidth + "em;\">\n");
		for (var i = 0; i < this._menuItems.length; ++i)
			this._menuItems[i].write(this._iWidth);
		document.write("	<\/ul>\n");
		document.write("<\/li>\n");
	};
}


/*
	This represents a menu command.
*/
function MenuCommand(strName, strDescription, strFilename)
{
	this._strName = strName;
	this._strDescription = strDescription;

	this._strFilename = strFilename;


	/*
		Writes the HTML for a single menu command.

		<li width=Y><a width=Y href="#" title="...">Command One</a></li>
	*/
	this.write = function(iWidth)
	{
		document.write("<li style=\"width: " + iWidth + "em;\"><a style=\"width: " + iWidth + "em;\" href=\"" + this._strFilename + "\" title=\"" + this._strDescription + "\">" + this._strName + "<\/a><\/li>\n");
	};
}



/* --------------------------------------------------------------------------

	This function works-around an IE bug.

	Instead of copying the following code into every menu-using HTML file's head,
	we make the HTML call initMenu() after the page is loaded.

	<script type="text/javascript"><!--//--><![CDATA[//><!--
fctIEfix = function()
				{
					var eltsLI = document.getElementById("idMenu").getElementsByTagName("LI");
					for (var i = 0; i < eltsLI.length; ++i)
					{
						eltsLI[i].onmouseover = function()
														{
															this.className += " clsIEhover";
														}
						eltsLI[i].onmouseout = function()
														{
															this.className = this.className.replace(new RegExp(" clsIEhover\\b"), "");
														}
					}
				}
// Do this for IE only.
if (window.attachEvent)
	window.attachEvent("onload", fctIEfix);

	//--><!]]></script>

*/
function initMenu()
{
	/*
		We only need to do this kludge if it's IE.
		(And if we do it for Firefox, it breaks.)
	*/
	if (window.navigator.userAgent.indexOf("MSIE") <= 0)
		return;


	// no sense passing this in because it's hardcoded in the CSS
	var strMenuID = "idMenu";

	// avoid error if there's no menu
	var eltMenu = document.getElementById(strMenuID);
	if (eltMenu == null)
		return;

	var eltsLI = eltMenu.getElementsByTagName("LI");
	for (var i = 0; i < eltsLI.length; ++i)
	{
		eltsLI[i].onmouseover = function()
										{
											this.className += " clsIEhover";
										}
		eltsLI[i].onmouseout = function()
										{
											this.className = this.className.replace(new RegExp(" clsIEhover\\b"), "");
										}
	}


	/*
		Fix other IE bug(s).
	*/
	fixIEselect(strMenuID);
}

/* --------------------------------------------------------------------------

	BUG: IE always displays SELECT elements on top.
	FIX: Shim an <IFRAME> below the <UL> element.

	We insert an IFRAME as the first child of each submenu's UL
	element and set their z-indexes so that the IFRAME is above
	SELECT elements but below the UL submenu itself.
*/
function fixIEselect(strMenuID)
{
	/*
		We only need to do this kludge if it's IE.
	*/
	if (window.navigator.userAgent.indexOf("MSIE") <= 0)
		return;
	/*
		Only do it if there are <select> elements on the page.
	*/
	if (document.getElementsByTagName("SELECT").length == 0)
		return;


	var ieULs = document.getElementById(strMenuID).getElementsByTagName("UL");
	// IE script to cover <SELECT> elements with <IFRAME>s
	for (var i = 0; i < ieULs.length; i++)
	{
		var eltSubmenu = ieULs[i];

		// The frame should be UNDER the submenu, so we set
		// "opacity=80" to make it visible in case of a problem.
/* We use the DOM instead of manipulating the HTML directly.
		eltSubmenu.innerHTML = "<iframe src=\"about:blank\" scrolling=\"no\" frameborder=\"no\" \
									style=\"position: absolute; left: 0; top: 0; z-index: -1; \
										filter: progid:DXImageTransform.Microsoft.Alpha(style=0,opacity=80);\">\
									</iframe>" + eltSubmenu.innerHTML;

		// get the IFRAME element and size it
		//also works	var eltOverlay = eltSubmenu.childNodes[0];
		var eltOverlay = eltSubmenu.firstChild;
*/
		var eltOverlay = document.createElement("iframe");
		eltOverlay.setAttribute("src", "about:blank");
		eltOverlay.setAttribute("scrolling", "no");
		eltOverlay.setAttribute("frameBorder", "no");
		eltOverlay.style.position = "absolute";
		eltOverlay.style.left = 0;
		eltOverlay.style.top = 0;
		eltOverlay.style.zIndex = -1;
		eltOverlay.style.filter = "progid:DXImageTransform.Microsoft.Alpha(style=0,opacity=80);";
		eltSubmenu.insertBefore(eltOverlay, eltSubmenu.firstChild);

		// the width of the submenu, the <li> elements of the submenu <ul> tag
		eltOverlay.style.width = eltSubmenu.offsetWidth + "px";

//		// Calculate height of "tallest/longest" menu
//		var iNumItems = eltSubmenu.getElementsByTagName("LI").length;
//		eltOverlay.style.height = (iNumItems * eltSubmenu.offsetHeight) + "px";
		eltOverlay.style.height = eltSubmenu.offsetHeight + "px";

		// put the submenu on top
		eltSubmenu.style.zIndex = "99";
	}
}
