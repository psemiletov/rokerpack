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

; Отключаем все лишние страницы мастера
DisableDirPage=yes
DisableProgramGroupPage=yes
DisableReadyPage=yes
DisableFinishedPage=no

; Убираем все диалоги
ShowLanguageDialog=no

[CustomMessages]
OpenFolder=Open plugins folder
Installing=Installing RokerPack plugins...
Finished=Installation of RokerPack plugins is complete.

[Code]
procedure InitializeWizard;
begin
  WizardForm.FinishedLabel.Caption := CustomMessage('Finished');
end;

procedure CurPageChanged(CurPageID: Integer);
begin
  if CurPageID = wpFinished then
  begin
    WizardForm.RunList.Visible := False;
    WizardForm.RunCheckBox.Visible := False;
    WizardForm.RunLabel.Visible := False;
  end;
end;

[Dirs]
Name: "{app}"

[Files]
; Плагины будут добавлены автоматически

[UninstallDelete]
Type: filesandordirs; Name: "{app}\Bronza.vst3"
Type: filesandordirs; Name: "{app}\Grelka.vst3"
Type: filesandordirs; Name: "{app}\Metalluga.vst3"
Type: filesandordirs; Name: "{app}\Charm.vst3"
Type: filesandordirs; Name: "{app}\Tembr.vst3"
Type: filesandordirs; Name: "{app}\Mistral.vst3"
Type: filesandordirs; Name: "{app}\GuitarTuner.vst3"
Type: filesandordirs; Name: "{app}\BassTuner.vst3"

[Icons]
Name: "{group}\Uninstall RokerPack"; Filename: "{uninstallexe}"