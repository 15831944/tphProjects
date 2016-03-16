;NSIS Modern User Interface
;Header Bitmap Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "OcrServer installer"
  OutFile "OcrServerInstaller.exe"
  Icon "OcrServer.ico"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\OcrServer"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\OcrServer" "InstallLocation"

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user

;--------------------------------
;Interface Configuration

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "win.bmp" ; optional
  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "License.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "OcrServer" SecOcrServer

  SetOutPath "$INSTDIR"
  File "ocrserver.js"
  File "License.txt"
  File "OcrServer.ico"
  File "node.exe"
  
  SetOutPath "$INSTDIR\lib"
  File "lib\httpserver.js"
  File "lib\httpclient.js"  

  SetOutPath "$INSTDIR\config"
  File "config\config.js"
  
  SetOutPath "$INSTDIR\doc"
  File "doc\request.json"
  File "doc\response.json"

  SetOutPath "$INSTDIR\bin"
  File "bin\SundayOcrWrapper.exe"
  File "bin\SundayCmb.dll"
  File "bin\Sunday.dll"  
  
  SetOutPath "$INSTDIR\bin\index"
  File "bin\index\abc"
  File "bin\index\bcm"
  File "bin\index\boc"
  File "bin\index\ccb"
  File "bin\index\cmb"
  File "bin\index\icbc"
  
  ;Store installation folder
  WriteRegStr HKCU "Software\OcrServer" "Location" "$INSTDIR"
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  ;Create a shortcut
  SetOutPath "$INSTDIR"
  CreateDirectory "$SMPROGRAMS\OcrServer"
  CreateShortCut  "$SMPROGRAMS\OcrServer\OcrServer.lnk" \
				  "$INSTDIR\node.exe" \
				  '"$INSTDIR\ocrserver.js"' \
				  "$INSTDIR\OcrServer.ico" 0 \
				  SW_SHOWNORMAL \
				  CONTROL|SHIFT|O \
				  "Startup OcrServer"
				  
  CreateShortCut  "$SMPROGRAMS\OcrServer\Uninstall.lnk" \
				  "$INSTDIR\Uninstall.exe" \
				  '' \
				  "$INSTDIR\Uninstall.exe" 0 
				  
SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_OcrServer ${LANG_ENGLISH} "main module for OcrServer"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecOcrServer} $(DESC_OcrServer)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
  Delete "$INSTDIR\ocrserver.js"
  Delete "$INSTDIR\License.txt"
  Delete "$INSTDIR\OcrServer.ico"
  Delete "$INSTDIR\node.exe"

  RMDir /r "$INSTDIR\lib"
  RMDir /r "$INSTDIR\config"
  RMDir /r "$INSTDIR\doc"
  RMDir /r "$INSTDIR\bin"
  
  RMDir /r "$SMPROGRAMS\OcrServer"
  
  Delete "$INSTDIR\Uninstall.exe"

  RMDir /r "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\OcrServer"

SectionEnd