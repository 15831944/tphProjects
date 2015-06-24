# Microsoft Developer Studio Project File - Name="compare" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=compare - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "compare.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "compare.mak" CFG="compare - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "compare - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "compare - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "compare - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "..\..\obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /I "..\main" /I "..\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "compare - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "..\..\objd"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /I "..\main" /I "..\\" /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\objd\compare.lib"

!ENDIF 

# Begin Target

# Name "compare - Win32 Release"
# Name "compare - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ComparativeProject.cpp
# End Source File
# Begin Source File

SOURCE=.\ComparativeQLengthReport.cpp
# End Source File
# Begin Source File

SOURCE=.\ComparativeQTimeReport.cpp
# End Source File
# Begin Source File

SOURCE=.\ComparativeReportManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ComparativeReportResult.cpp
# End Source File
# Begin Source File

SOURCE=.\ComparativeReportResultList.cpp
# End Source File
# Begin Source File

SOURCE=.\ComparativeSpaceDensityReport.cpp
# End Source File
# Begin Source File

SOURCE=.\ComparativeThroughputReport.cpp
# End Source File
# Begin Source File

SOURCE=.\FileOperations.cpp
# End Source File
# Begin Source File

SOURCE=.\InputParameter.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelsManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelToCompare.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportParamToCompare.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportsManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportToCompare.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ComparativeProject.h
# End Source File
# Begin Source File

SOURCE=.\ComparativeQLengthReport.h
# End Source File
# Begin Source File

SOURCE=.\ComparativeQTimeReport.h
# End Source File
# Begin Source File

SOURCE=.\ComparativeReportManager.h
# End Source File
# Begin Source File

SOURCE=.\ComparativeReportResult.h
# End Source File
# Begin Source File

SOURCE=.\ComparativeReportResultList.h
# End Source File
# Begin Source File

SOURCE=.\ComparativeSpaceDensityReport.h
# End Source File
# Begin Source File

SOURCE=.\ComparativeThroughputReport.h
# End Source File
# Begin Source File

SOURCE=.\FileOperations.h
# End Source File
# Begin Source File

SOURCE=.\InputParameter.h
# End Source File
# Begin Source File

SOURCE=.\ModelsManager.h
# End Source File
# Begin Source File

SOURCE=.\ModelToCompare.h
# End Source File
# Begin Source File

SOURCE=.\ReportDef.h
# End Source File
# Begin Source File

SOURCE=.\ReportParamToCompare.h
# End Source File
# Begin Source File

SOURCE=.\ReportsManager.h
# End Source File
# Begin Source File

SOURCE=.\ReportToCompare.h
# End Source File
# End Group
# End Target
# End Project
