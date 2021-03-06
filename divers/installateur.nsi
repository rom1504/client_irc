; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "client_irc"
!define PRODUCT_VERSION "0.3"
!define PRODUCT_PUBLISHER "rom1504"
!define PRODUCT_WEB_SITE "http://rom1504.no-ip.info/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\client_irc.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "..\images\icone.ico"
!define MUI_UNICON "..\images\icone.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "licence.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\bin\client_irc.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "client_irc0.3.exe"
InstallDir "$PROGRAMFILES\client_irc"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "SectionPrincipale" SEC01
  SetOverwrite on
  SetOutPath "$INSTDIR\bin"
  !cd ".."
  File "bin\client_irc.exe"
  File "C:\Qt\2009.03\qt\bin\QtCore4.dll"
  File "C:\Qt\2009.03\qt\bin\QtGui4.dll"
  File "C:\Qt\2009.03\qt\bin\QtNetwork4.dll"
  File "C:\Qt\2009.03\qt\bin\mingwm10.dll"
  
  SetOutPath "$INSTDIR\bin\plugin"
  File /r "bin\plugin\*.dll"
  
  SetOutPath "$INSTDIR\divers"
  File /r "divers\*.txt"
  
  CreateDirectory "$SMPROGRAMS\client_irc"
  CreateShortCut "$SMPROGRAMS\client_irc\client_irc.lnk" "$INSTDIR\bin\client_irc.exe"
  CreateShortCut "$DESKTOP\client_irc.lnk" "$INSTDIR\bin\client_irc.exe"
  
  
  SetOutPath "$INSTDIR\conf"
  File /r "conf\*.*"
  
  SetOutPath "$INSTDIR\langue"
  File /r "langue\*.qm"
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  CreateShortCut "$SMPROGRAMS\client_irc\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\bin\client_irc.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\bin\client_irc.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) a �t� d�sinstall� avec succ�s de votre ordinateur."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "�tes-vous certains de vouloir d�sinstaller totalement $(^Name) et tous ses composants ?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\uninst.exe"

  Delete "$DESKTOP\client_irc.lnk"

  RMDir /r "$SMPROGRAMS\client_irc"
  RMDir /r "$INSTDIR\divers"
  RMDir /r "$INSTDIR\conf"
  RMDir /r "$INSTDIR\bin"
  RMDir /r "$INSTDIR\langue"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd