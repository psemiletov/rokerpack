; RokerPack Installer for Windows
; Inno Setup Script

[Setup]
AppId=RokerPack
AppName=RokerPack
AppVersion=1.0.0
AppPublisher=Semiletov
AppPublisherURL=https://github.com/psemiletov/rokerpack
AppSupportURL=https://github.com/psemiletov/rokerpack
AppUpdatesURL=https://github.com/psemiletov/rokerpack
DefaultDirName={commoncf64}\VST3
DefaultGroupName=RokerPack
OutputDir=.
OutputBaseFilename=RokerPack-Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=admin
ArchitecturesInstallIn64BitMode=x64
LicenseFile=LICENSE
; UninstallDisplayIcon удалён, так как у плагинов нет иконки

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"

[CustomMessages]
en.OpenFolder=Open plugins folder
ru.OpenFolder=Открыть папку с плагинами
en.SelectTasks=Select additional tasks:
ru.SelectTasks=Выберите дополнительные задачи:
en.Installing=Installing...
ru.Installing=Установка...
en.Finished=Setup has finished installing RokerPack on your computer.
ru.Finished=Программа установки завершила установку RokerPack на ваш компьютер.
en.LaunchExplorer=Open plugins folder after installation
ru.LaunchExplorer=Открыть папку с плагинами после установки

[Dirs]
Name: "{app}"

[Files]
; Плагины будут добавлены автоматически

[Icons]
Name: "{group}\Uninstall RokerPack"; Filename: "{uninstallexe}"

[Tasks]
Name: "launchExplorer"; Description: "{cm:LaunchExplorer}"; Flags: unchecked

[Run]
Filename: "{app}\"; Description: "{cm:OpenFolder}"; Flags: postinstall shellexec; Tasks: launchExplorer