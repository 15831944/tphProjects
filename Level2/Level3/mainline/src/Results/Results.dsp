# Microsoft Developer Studio Project File - Name="results" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=results - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Results.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Results.mak" CFG="results - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "results - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "results - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ARCTERM/Results", XKAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "results - Win32 Release"

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
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\obj\Results.lib"

!ELSEIF  "$(CFG)" == "results - Win32 Debug"

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
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\objd\Results.lib"

!ENDIF 

# Begin Target

# Name "results - Win32 Release"
# Name "results - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BAGENTRY.CPP
# End Source File
# Begin Source File

SOURCE=.\BAGLOG.CPP
# End Source File
# Begin Source File

SOURCE=.\ElevatorLog.cpp
# End Source File
# Begin Source File

SOURCE=.\ElevatorLogEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\EventLogBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\EventLogBufManager.cpp
# End Source File
# Begin Source File

SOURCE=.\FLTENTRY.CPP
# End Source File
# Begin Source File

SOURCE=.\FLTLOG.CPP
# End Source File
# Begin Source File

SOURCE=.\OUT_TERM.CPP
# End Source File
# Begin Source File

SOURCE=.\OUTPAX.CPP
# End Source File
# Begin Source File

SOURCE=.\PaxDestDiagnoseInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxDestDiagnoseLog.cpp
# End Source File
# Begin Source File

SOURCE=.\PAXENTRY.CPP
# End Source File
# Begin Source File

SOURCE=.\PaxLiveTrack.cpp
# End Source File
# Begin Source File

SOURCE=.\PAXLOG.CPP
# End Source File
# Begin Source File

SOURCE=.\PROCLOG.CPP
# End Source File
# Begin Source File

SOURCE=.\PROCNTRY.CPP
# End Source File
# Begin Source File

SOURCE=.\ResourceElementLog.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceElementLogEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\Results.cpp
# End Source File
# Begin Source File

SOURCE=.\TrainLog.cpp
# End Source File
# Begin Source File

SOURCE=.\TrainLogEntry.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BAGENTRY.H
# End Source File
# Begin Source File

SOURCE=.\BAGLOG.H
# End Source File
# Begin Source File

SOURCE=.\BASELOG.H
# End Source File
# Begin Source File

SOURCE=.\ElevatorLog.h
# End Source File
# Begin Source File

SOURCE=.\ElevatorLogEntry.h
# End Source File
# Begin Source File

SOURCE=.\EVENTLOG.H
# End Source File
# Begin Source File

SOURCE=.\EventLogBuffer.h
# End Source File
# Begin Source File

SOURCE=.\EventLogBufManager.h
# End Source File
# Begin Source File

SOURCE=.\FLTENTRY.H
# End Source File
# Begin Source File

SOURCE=.\FLTLOG.H
# End Source File
# Begin Source File

SOURCE=.\LOGITEM.H
# End Source File
# Begin Source File

SOURCE=.\OUT_TERM.H
# End Source File
# Begin Source File

SOURCE=.\Outpax.h
# End Source File
# Begin Source File

SOURCE=.\PaxDestDiagnoseInfo.h
# End Source File
# Begin Source File

SOURCE=.\PaxDestDiagnoseLog.h
# End Source File
# Begin Source File

SOURCE=.\PAXENTRY.H
# End Source File
# Begin Source File

SOURCE=.\PaxLiveTrack.h
# End Source File
# Begin Source File

SOURCE=.\PAXLOG.H
# End Source File
# Begin Source File

SOURCE=.\PROCLOG.H
# End Source File
# Begin Source File

SOURCE=.\PROCNTRY.H
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ResourceElementLog.h
# End Source File
# Begin Source File

SOURCE=.\ResourceElementLogEntry.h
# End Source File
# Begin Source File

SOURCE=.\Results.h
# End Source File
# Begin Source File

SOURCE=.\ResultsDll.h
# End Source File
# Begin Source File

SOURCE=.\TrainLog.h
# End Source File
# Begin Source File

SOURCE=.\TrainLogEntry.h
# End Source File
# End Group
# End Target
# End Project
