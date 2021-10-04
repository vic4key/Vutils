/*
 * for VS Global Settings
 */

var WSH = new ActiveXObject("WScript.Shell");
var XML = new ActiveXObject("MSXML2.DOMDocument");
var FSO = new ActiveXObject("Scripting.FileSystemObject");

// get configurations in the current machine

var TAB = "\t";
var Vutils = "Vutils";
var Vutils_REL_PROPS = "Vutils.user.props";

var MSBuildv40_DIR  = WSH.ExpandEnvironmentStrings("%AppData%\\..\\Local\\Microsoft\\MSBuild\\v4.0\\");
var MSCppUser_PROPS = new Array("Microsoft.Cpp.Win32.user.props", "Microsoft.Cpp.x64.user.props");

// determine that the project is already setup on the machine

var Vutils_DIR = WSH.ExpandEnvironmentStrings("%Vutils%");

if (!FSO.FolderExists(Vutils_DIR))
{
	Vutils_DIR = FSO.GetParentFolderName(WSH.CurrentDirectory);
	if (Vutils_DIR.slice(-1) != "\\")
	{
		Vutils_DIR += "\\";
	}
}

if (!FSO.FolderExists(Vutils_DIR))
{
	WScript.Echo(Vutils + " directory not found");
	WScript.Quit();
}

var Vutils_ABS_PROPS = Vutils_DIR + Vutils_REL_PROPS;

if (!FSO.FileExists(Vutils_ABS_PROPS))
{
	WScript.Echo(Vutils_ABS_PROPS + " file not found");
	WScript.Quit();
}

// iterate Microsoft.Cpp.X.user.props to include the project to the VS global settings

var Updated_Global_Settings = false

for (var i = 0; i < MSCppUser_PROPS.length; i++)
{
	var filePath = MSBuildv40_DIR + MSCppUser_PROPS[i];
	if (FSO.FileExists(filePath))
	{
		Updated_Global_Settings |= AddVutils(filePath);
	}
}

if (!Updated_Global_Settings)
{
	WScript.Echo("NOTE:")
	WScript.Echo("\tUnable to add `Vutils` to VS global settings.")
	WScript.Echo("\tVS global settings no longer used for VS 2017 and later.")
	WScript.Echo("\tLet copy `Directory.Build.props` to your solution directory.")
}

// function add the project to the VS global settings

function AddVutils(filePath)
{
	WScript.Echo("Working on file " + filePath);

	// parse VS global settings

	XML.async = false;
	XML.load(filePath);
	if (XML.parseError.errorCode != 0)
	{
		WScript.Echo(TAB + XML.parseError.reason);
		return false;
	}

	// determime that the project is already existed in the VS global settings or not

	var existed = false;
	var ImportGroup = XML.selectSingleNode("//ImportGroup[@Label='PropertySheets']");
	if (!ImportGroup)
	{
		WScript.Echo(TAB + "Property Sheets not found");
		return false;
	}

	var Import = ImportGroup.getElementsByTagName("Import");
	for (var i = 0; i < Import.length; i++)
	{
		var Project = Import[i].getAttribute("Project").toLowerCase();
		if (Project.indexOf(Vutils.toLowerCase()) != -1)
		{
			existed = true;
			break;
		}
	}

	// ignore if the project is already existed in the VS global settings

	if (existed)
	{
		WScript.Echo(TAB + Vutils + " already existed");
		return true;
	}

	// backup file before updating

	var theDateTime = Math.round(new Date().valueOf() / 1000).toString(16);
	var filePathBackup = filePath + "." + theDateTime + ".bak";
	FSO.CopyFile(filePath, filePathBackup);
	WScript.Echo(TAB + "Backup as " + FSO.GetFileName(filePathBackup));

	// add the project to the VS global settings


	var NODE_ELEMENT  = 1;
	var Vutils_PROPS = "$(Vutils)" + Vutils_REL_PROPS;
	var ns = XML.documentElement.getAttribute("xmlns")
	var element = XML.createNode(NODE_ELEMENT, "Import", ns);
	element.setAttribute("Project", Vutils_PROPS);
	ImportGroup.appendChild(element);
	WScript.Echo(TAB + "Added " + Vutils);

	// save changes

	XML.save(filePath);
	WScript.Echo(TAB + "Saved changes");

	return true;
}

WScript.Quit();