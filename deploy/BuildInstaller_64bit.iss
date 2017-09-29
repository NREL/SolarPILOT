; -- 64Bit.iss --
; SolarPILOT
; Compiler for 64-bit architectures

[Setup]
AppId={{67C0F37C-E0F4-4D6A-9A20-5BA8FB27308E}}
AppName=SolarPILOT

; Update these to match the version
AppVersion=2017.9.29
DefaultDirName={pf}\SolarPILOT\2017.9.29
OutputBaseFilename=solarpilot-install-x64
; --------

DefaultGroupName=SolarPILOT
AppPublisher=National Renewable Energy Laboratory
AppPublisherURL=mike.wagner@nrel.gov
UninstallDisplayIcon={app}\x64\SolarPILOT.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.
; 64 bit only
ArchitecturesAllowed=x64 ia64
ArchitecturesInstallIn64BitMode=x64 ia64
ChangesAssociations=yes
UsePreviousAppDir=no

[Registry]
Root: HKCR; Subkey: ".spt"; ValueType: string; ValueName: ""; ValueData: "SolarPILOTFile"; Flags: uninsdeletevalue
; ".myp" is the extension we're associating. "MyProgramFile" is the internal name for the file type as stored in the registry. 
; Make sure you use a unique name for this so you don't inadvertently overwrite another application's registry key.

Root: HKCR; Subkey: "SolarPILOTFile"; ValueType: string; ValueName: ""; ValueData: "SolarPILOT Program File"; Flags: uninsdeletekey
; "My Program File" above is the name for the file type as shown in Explorer.

Root: HKCR; Subkey: "SolarPILOTFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\x64\SOLARPILOT.EXE,0"
; "DefaultIcon" is the registry key that specifies the filename containing the icon to associate with the file type. 
; ",0" tells Explorer to use the first icon from MYPROG.EXE. (",1" would mean the second icon.)

Root: HKCR; Subkey: "SolarPILOTFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\x64\SOLARPILOT.EXE""""%1""" 
;"shell\open\command" is the registry key that specifies the program to execute when a file of the type is double-clicked in Explorer. 
; The surrounding quotes are in the command line so it handles long filenames correctly.

[Files]
Source: "x64/SolarPILOT.exe"; DestDir: "{app}\x64"; DestName: "SolarPILOT.exe";
Source: "x64/msvcp120.dll"; DestDir: "{app}\x64";
Source: "x64/msvcr120.dll"; DestDir: "{app}\x64";
Source: "x64/dbghelp.dll"; DestDir: "{app}\x64";
Source: "x64/libcurl.dll"; DestDir: "{app}\x64";
Source: "x64/libeay32.dll"; DestDir: "{app}\x64";
Source: "x64/libssh2.dll"; DestDir: "{app}\x64";
Source: "x64/ssleay32.dll"; DestDir: "{app}\x64";
Source: "README.txt"; DestDir: "{app}";
Source: "exelib/*"; DestDir: "{app}/exelib"; Excludes: ".svn,\custom"; Flags: recursesubdirs createallsubdirs
Source: "climate_files/*"; DestDir: "{app}/climate_files"; Excludes: ".svn"; Flags: recursesubdirs createallsubdirs


[Icons]
Name: "{group}\SolarPILOT"; Filename: "{app}\x64\SolarPILOT.exe"
Name: "{group}\Uninstall SolarPILOT"; Filename: "{app}\unins000.exe"
Name: "{group}\Release notes"; Filename: "{app}\readme.txt"

[Tasks]
;Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: desktopicon; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:";
;Name: desktopicon\common; Description: "For all users"; GroupDescription: "Additional icons:"; Components: main; Flags: exclusive
;Name: desktopicon\user; Description: "For the current user only"; GroupDescription: "Additional icons:"; Components: main; Flags: exclusive unchecked
Name: quicklaunchicon; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; 
Name: associate; Description: "&Associate files"; GroupDescription: "Other tasks:"; 
