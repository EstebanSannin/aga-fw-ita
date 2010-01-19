/* -------------------------------------------------------------------------- */
/*
	(c) 2005 U.S. Robotics Corporation.
*/
/* -------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------

	Custom functions for DSL modem.

*/
function buildServiceName(ixProt, vpi, vci)
{
	var strProt = "";
	switch (ixProt)
	{
		case 0:	strProt = "pppoa"; 	break;
		case 1:	strProt = "pppoe"; 	break;
		case 2:	strProt = "mer";   	break;
		case 3:	strProt = "ipoa";	break;
		case 4:	strProt = "br"; 	break;
		default:	break;
	}
        
        if( ixProt <= 1 )
	        return strProt + "_" + vpi + "_" + vci + "_1";
        else
	        return strProt + "_" + vpi + "_" + vci;
}
