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

; Отключаем все лишние страницы мастера
DisableDirPage=yes
DisableProgramGroupPage=yes
DisableReadyPage=yes
DisableFinishedPage=no

; Отключаем диалог выбора языка
ShowLanguageDialog=no

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"

[CustomMessages]
en.OpenFolder=Open plugins folder
ru.OpenFolder=Открыть папку с плагинами
en.Installing=Installing RokerPack plugins...
ru.Installing=Установка плагинов RokerPack...
en.Finished=Setup has finished installing RokerPack plugins on your computer.
ru.Finished=Программа установки завершила установку плагинов RokerPack на ваш компьютер.

[Code]
var
  LanguageCode: string;

function GetSystemLanguageCode: string;
var
  Lang: string;
begin
  Lang := ActiveLanguage;
  { Определяем язык системы через GetUserDefaultLangID }
  case GetUserDefaultLangID of
    $0419, $0419: Result := 'ru';  { Русский }
  else
    Result := 'en';
  end;
end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin
  Result := False;
  { Пропускаем страницу выбора языка }
  if PageID = wpSelectLanguage then
    Result := True;
end;

procedure InitializeWizard;
var
  SelectedLanguage: string;
  SysLang: string;
begin
  { Определяем язык системы }
  case GetUserDefaultLangID of
    $0419, $0419: SysLang := 'ru';  { Русский }
  else
    SysLang := 'en';
  end;
  
  { Выбираем язык интерфейса автоматически }
  if SysLang = 'ru' then
    SelectedLanguage := 'Russian'
  else
    SelectedLanguage := 'English';
  
  { Устанавливаем выбранный язык }
  ActiveLanguage := SelectedLanguage;
  
  { Сохраняем код языка для дальнейшего использования }
  if SysLang = 'ru' then
    LanguageCode := 'ru'
  else
    LanguageCode := 'en';
end;

procedure CurPageChanged(CurPageID: Integer);
begin
  { Настройка текста на странице завершения }
  if CurPageID = wpFinished then
  begin
    { Отключаем сообщение о запуске приложения }
    WizardForm.RunList.Visible := False;
    { Отключаем чекбокс "Запустить программу" }
    WizardForm.RunCheckBox.Visible := False;
    WizardForm.RunLabel.Visible := False;
    
    { Настраиваем финальное сообщение }
    if LanguageCode = 'ru' then
      WizardForm.FinishedLabel.Caption := 'Установка плагинов RokerPack завершена.'
    else
      WizardForm.FinishedLabel.Caption := 'Installation of RokerPack plugins is complete.';
  end;
end;

[Dirs]
Name: "{app}"

[Files]
; Плагины будут добавлены автоматически

[UninstallDelete]
; Удаляем только папки плагинов, но не корневую VST3
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