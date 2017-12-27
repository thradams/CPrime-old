; Script generated by the Inno Script Studio Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "CPrime"
#define MyAppVersion "0.5"
#define MyAppPublisher "https://github.com/thradams/CPrime"
#define MyAppURL "https://github.com/thradams/CPrime"
#define MyAppExeName "cprime.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{34B53F98-CE90-4D21-9E05-046B87EFC52C}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
OutputDir=d:\InnoSetup
OutputBaseFilename=setup
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "Release\CPrime.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\thiago\source\repos\CPrime\bin-windows\config.h"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\thiago\source\repos\CPrime\bin-windows\hello.c"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\thiago\source\repos\CPrime\bin-windows\Samples.txt"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
