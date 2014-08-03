# NSI Script adapted from Paul Colby's Bipolar Project
# at https://github.com/pcolby/bipolar

SetCompressor lzma
!include "MUI2.nsh"

# Installer Attributes: General Attributes.
InstallDir "$PROGRAMFILES\V800 Downloader"
Name "V800 Downloader"
OutFile V800DownloaderInstall.exe
RequestExecutionLevel highest # Required for Windows Vista+
XPStyle on

# Variables.
Var StartMenuFolder

# Modern UI2 Interface Configuration.
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\win.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "${NSISDIR}\Contrib\Graphics\Header\win.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\win.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\win.bmp"
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_ABORTWARNING

# Modern UI2 Install Pages.
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "gpl.txt"
!insertmacro MUI_PAGE_DIRECTORY
#!insertmacro MUI_PAGE_COMPONENTS
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\V800 Downloader"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "startMenuFolder"
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
  
# Modern UI2 Uninstall Pages.
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Modern UI2 Languages.
!insertmacro MUI_LANGUAGE "English"

# Custom install pages.

# Sections to install.

Section "application"
    # Files to install.
    SetOutPath $INSTDIR
    File "V800Downloader.exe"
    File /r "qtlibs\*"
    WriteRegStr HKCU "Software\Software\V800 Downloader" "" $INSTDIR
    WriteUninstaller $INSTDIR\Uninstall.exe
    # The various shortcuts.
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
        CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
        CreateShortCut "$SMPROGRAMS\$StartMenuFolder\V800 Downloader.lnk" "$INSTDIR\V800Downloader.exe"
        CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
        CreateShortCut "$DESKTOP\V800 Downloader.lnk" "$INSTDIR\V800Downloader.exe"
    !insertmacro MUI_STARTMENU_WRITE_END
    # Windows' add/remove programs information.
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\V800 Downloader" "DisplayName" "V800 Downloader"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\V800 Downloader" "DisplayIcon" "$\"$INSTDIR\V800Downloader.exe$\",0"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\V800 Downloader" "DisplayVersion" "Beta2"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\V800 Downloader" "Publisher" "Christian Weber"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\V800 Downloader" "URLInfoAbout" "https://github.com/profanum429/v800_downloader"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\V800 Downloader" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\V800 Downloader" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
    WriteRegDword HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\V800 Downloader" "NoModify" 1
    WriteRegDword HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\V800 Downloader" "NoRepair" 1
SectionEnd

Function .onInstFailed
  Push "$INSTDIR\Install.log"
FunctionEnd

Function .onInstSuccess
  Push "$INSTDIR\Install.log"
FunctionEnd

# Sections to uninstall.

Section "un.application"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\V800 Downloader"
    Delete "$DESKTOP\V800 Downloader.lnk"
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
    Delete "$SMPROGRAMS\$StartMenuFolder\V800 Downloader.lnk"
    RMDir "$SMPROGRAMS\$StartMenuFolder"
    Delete $INSTDIR\V800Downloader.exe
    Delete $INSTDIR\D3Dcompiler_*.dll
    Delete $INSTDIR\icu*.dll
    Delete $INSTDIR\libEGL.dll
    Delete $INSTDIR\libGLESv2.dll
    Delete $INSTDIR\Install.log
    Delete $INSTDIR\msvc*.dll
    Delete $INSTDIR\Qt5*.dll
    Delete $INSTDIR\qt_*.qm
    Delete $INSTDIR\libgcc_s_dw2-1.dll
    Delete $INSTDIR\libstdc++-6.dll
    Delete $INSTDIR\libwinpthread-1.dll
    Delete $INSTDIR\Uninstall.exe
    RMDir /r $INSTDIR\accessible
    RMDir /r $INSTDIR\iconengines
    RMDir /r $INSTDIR\imageformats
    RMDir /r $INSTDIR\platforms
    RMDir $INSTDIR
    DeleteRegKey /ifempty HKCU "Software\V800 Downloader"
SectionEnd

# Installer Attributes: Version Information.
VIProductVersion "0.1.0.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "https://github.com/profanum429/v800_downloader"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Christian Weber"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "V800 Downloader installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "Beta2"
VIAddVersionKey /LANG=${LANG_ENGLISH} "InternalName" "V800 Downloader-Beta2"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "2014 Christian Weber"
#VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" ""
VIAddVersionKey /LANG=${LANG_ENGLISH} "OriginalFilename" "V800 Downloader.exe"
#VIAddVersionKey /LANG=${LANG_ENGLISH} "PrivateBuild" ""
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "V800 Downloader"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "Beta2"
#VIAddVersionKey /LANG=${LANG_ENGLISH} "SpecialBuild" ""
