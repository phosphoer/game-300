; Filename: install.iss
; Author:  Original script generated by the Inno Setup Script Wizard.
;          Compiled and modfied/rewrote by Jake Leonard
;
; Project: Subduction
; Game Team: Improbability Drive
; Section: GAM350
;
; All content � 2010-2012 DigiPen (USA) Corporation, all rights reserved.

[Setup]
AppId={{A96AC362-0814-4B3F-9487-8FE0316ED15D}
AppName=Subduction
AppVerName=Subduction
AppVersion=1.5
AppPublisher=DigiPen Corporation
AppCopyright=� 2010-2012 DigiPen (USA) Corporation
AppReadmeFile=subduction_readme.txt
AppComments=2D/3D Puzzle Platformer
AppPublisherURL=http://www.digipen.edu/
PrivilegesRequired=admin
VersionInfoCopyright=� 2010-2012 DigiPen (USA) Corporation
VersionInfoCompany=Improbability Drive
VersionInfoDescription=Subduction
VersionInfoVersion=1.5
VersionInfoTextVersion=1.5
VersionInfoProductVersion=1.5

;Installer Locations
DefaultDirName={pf}\DigiPen\Subduction
DefaultGroupName=DigiPen\Subduction

;Installer flags
AllowNoIcons=yes
OutputDir=..\installer\
OutputBaseFilename=subduction_setup
;http://www.jrsoftware.org/ishelp/index.php?topic=setup_compression
Compression=lzma2
SolidCompression=yes
SetupIconFile=icons\Subduction.ico
LicenseFile=text\license.txt
InfoBeforeFile=..\SUBMISSION\subduction_readme.txt
UninstallDisplayIcon={app}\Subduction.ico

; Add the required copyright to the installer
[Messages]
BeveledLabel=� 2010-2012 DigiPen (USA) Corporation

; Things to do after install
[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

; We can't add quick launch icons on Windows versions under NT 6.1
; I don't really know, I didn't write the following line.
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

; Our game files.
[Files]
; Redistributables
Source: .\REDIST\vcredist_2008_x86.exe; DestDir: {tmp}; Flags: ignoreversion
Source: .\REDIST\vcredist_x86.exe; DestDir: {tmp}; Flags: ignoreversion

; ONLINE DIRECTX INSTALLER 
Source: .\REDIST\dxwebsetup.exe; DestDir: {tmp}; Flags: ignoreversion

; OFFLINE DIRECTX INSTALLER
;Source: .\REDIST\directx_Jun2010_redist.exe; DestDir: {tmp}; Flags: ignoreversion

; Exe file, dlls, etc
Source: "..\bin\Release\Subduction.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\*.dll"; DestDir: "{app}"; Excludes: "python32_d.dll, d3d9.dll, fmodex.dll"; Flags: ignoreversion
Source: "..\python32.zip"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\*.pak"; DestDir: "{app}"; Flags: ignoreversion
Source: "text\license.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\SUBMISSION\subduction_readme.txt"; DestDir: "{app}"; Flags: ignoreversion

; Hide our icons.
Source: "icons\Subduction.ico"; DestDir: "{app}"; Flags: ignoreversion; Attribs: hidden
Source: "icons\digipen.ico"; DestDir: "{app}"; Flags: ignoreversion; Attribs: hidden

; Font(s)
Source: "fonts\UniversElse-Light.ttf"; DestDir: "{fonts}"; FontInstall: "Univers Else Light (OpenType)"; Flags: fontisnttruetype onlyifdoesntexist uninsneveruninstall

; Grab the other directories
Source: "..\locales\*"; DestDir: "{app}\locales\"; Excludes: "*.dat"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\audio\*"; DestDir: "{app}\audio\"; Excludes: "*.wav"; Flags: ignoreversion
Source: "..\res\*"; DestDir: "{app}\res\"; Excludes: "*.dat, presentation-space.spc, playable0*.jpg, *.pyc, *.pdn"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\res_ui\*"; DestDir: "{app}\res_ui\"; Flags: ignoreversion recursesubdirs createallsubdirs

; The installer will give the user the choice if they want to add shortcuts. Here are the shortcuts that we could create.
; Do not remove the working dir. Because we don't modify the registry (to provide a look up key),
; there is no way to figure out where the game's resources are (because we use relative paths).

[Icons]
; Start Menu
Name: "{group}\Subduction"; Filename: "{app}\Subduction.exe"; IconFilename: "{app}\Subduction.ico"; Comment: "An Improbability Drive Game"; WorkingDir: "{app}"
Name: "{group}\{cm:ProgramOnTheWeb,Subduction}"; Filename: "http://www.digipen.edu/"; IconFilename: "{app}\digipen.ico"
Name: "{group}\{cm:UninstallProgram,Subduction}"; Filename: "{uninstallexe}"
; Desktop
Name: "{commondesktop}\Subduction"; Filename: "{app}\Subduction.exe"; IconFilename: "{app}\Subduction.ico"; Comment: "An Improbability Drive Game"; WorkingDir: "{app}"; Tasks: desktopicon
; Quick Launch
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\Subduction"; IconFilename: "{app}\Subduction.ico"; Filename: "{app}\Subduction.exe"; Comment: "An Improbability Drive Game"; WorkingDir: "{app}"; Tasks: quicklaunchicon

; Possible actions that can be performed after install.
[Run]
FileName: {tmp}\vcredist_2008_x86.exe; Parameters: /q; StatusMsg: Installing Visual C++ 2008 Redistributable...
Filename: {tmp}\vcredist_x86.exe; Parameters: /q; StatusMsg: Installing Visual C++ 2010 Redistributable...

; ONLINE DIRECTX INSTALLER
Filename: {tmp}\dxwebsetup.exe; Parameters: /q; StatusMsg: Installing DirectX...

; OFFLINE DIRECTX INSTALLER
;Filename: {tmp}\directx_Jun2010_redist.exe; Parameters: /q /T:{tmp}\dirx; StatusMsg: Extracting DirectX...
;Filename: {tmp}\dirx\DXSETUP.exe; Parameters: /silent; StatusMsg: Installing DirectX...

Filename: "{app}\Subduction.exe"; Description: "{cm:LaunchProgram,Subduction}"; Flags: nowait postinstall skipifsilent

; Please don't modify the flags. If you are curious, they HAVE to be the same
; Otherwise, you'll get some pretty funny results.
Filename: "{app}\subduction_readme.txt"; Description: "View the README file"; Flags: postinstall shellexec skipifsilent
Filename: "{app}\license.txt"; Description: "View the License file"; Flags: postinstall shellexec skipifsilent
