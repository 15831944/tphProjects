# Microsoft Developer Studio Project File - Name="common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=common - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak" CFG="common - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "common - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "common - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ARCTERM/Common", DAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "common - Win32 Release"

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
# ADD LIB32 /nologo /out:"..\..\obj\common.lib"

!ELSEIF  "$(CFG)" == "common - Win32 Debug"

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
# ADD LIB32 /nologo /out:"..\..\objd\Common.lib"

!ENDIF 

# Begin Target

# Name "common - Win32 Release"
# Name "common - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ActermExcption.cpp
# End Source File
# Begin Source File

SOURCE=.\ACTypesManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Aircraft.cpp
# End Source File
# Begin Source File

SOURCE=.\Airline.cpp
# End Source File
# Begin Source File

SOURCE=.\AirlineManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Airport.cpp
# End Source File
# Begin Source File

SOURCE=.\AirportDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\AirportDatabaseList.cpp
# End Source File
# Begin Source File

SOURCE=.\AirportsManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ARCColor.cpp
# End Source File
# Begin Source File

SOURCE=.\ARCException.cpp
# End Source File
# Begin Source File

SOURCE=.\ARCLine.cpp
# End Source File
# Begin Source File

SOURCE=.\ARCMathCommon.cpp
# End Source File
# Begin Source File

SOURCE=.\ARCMatrix4x4.cpp
# End Source File
# Begin Source File

SOURCE=.\ARCPath.cpp
# End Source File
# Begin Source File

SOURCE=.\ARCPolygon.cpp
# End Source File
# Begin Source File

SOURCE=.\ARCVector.cpp
# End Source File
# Begin Source File

SOURCE=.\CodeTimeTest.cpp
# End Source File
# Begin Source File

SOURCE=.\Common.cpp
# End Source File
# Begin Source File

SOURCE=.\csv_file.cpp
# End Source File
# Begin Source File

SOURCE=.\DATASET.CPP
# End Source File
# Begin Source File

SOURCE=.\DirectoryUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\DistancePath.cpp
# End Source File
# Begin Source File

SOURCE=.\elaptime.cpp
# End Source File
# Begin Source File

SOURCE=.\ExceptionMsgDB.cpp
# End Source File
# Begin Source File

SOURCE=.\exeption.cpp
# End Source File
# Begin Source File

SOURCE=.\fileman.cpp
# End Source File
# Begin Source File

SOURCE=.\FloatUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\fsstream.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalDB.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalDBList.cpp
# End Source File
# Begin Source File

SOURCE=.\IDGather.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoZip.cpp
# End Source File
# Begin Source File

SOURCE=.\LINE.CPP
# End Source File
# Begin Source File

SOURCE=.\LogDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\MutilReport.cpp
# End Source File
# Begin Source File

SOURCE=.\PATH.CPP
# End Source File
# Begin Source File

SOURCE=.\POINT.CPP
# End Source File
# Begin Source File

SOURCE=.\POLLYGON.CPP
# End Source File
# Begin Source File

SOURCE=.\ProgressBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Sector.cpp
# End Source File
# Begin Source File

SOURCE=.\SimAndReportManager.cpp
# End Source File
# Begin Source File

SOURCE=.\SimResult.cpp
# End Source File
# Begin Source File

SOURCE=.\StartDate.cpp
# End Source File
# Begin Source File

SOURCE=.\STATES.CPP
# End Source File
# Begin Source File

SOURCE=.\StateTimePoint.cpp
# End Source File
# Begin Source File

SOURCE=.\strdict.cpp
# End Source File
# Begin Source File

SOURCE=.\StringConvertToFlight.cpp
# End Source File
# Begin Source File

SOURCE=.\Subairline.cpp
# End Source File
# Begin Source File

SOURCE=.\TEMPLATE.CPP
# End Source File
# Begin Source File

SOURCE=.\TERM_ERR.CPP
# End Source File
# Begin Source File

SOURCE=.\TERMFILE.CPP
# End Source File
# Begin Source File

SOURCE=.\TimePoint.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoManager.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoProject.cpp
# End Source File
# Begin Source File

SOURCE=.\UTIL.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ActermExcption.h
# End Source File
# Begin Source File

SOURCE=.\ACTypesManager.h
# End Source File
# Begin Source File

SOURCE=.\Aircraft.h
# End Source File
# Begin Source File

SOURCE=.\Airline.h
# End Source File
# Begin Source File

SOURCE=.\AirlineManager.h
# End Source File
# Begin Source File

SOURCE=.\Airport.h
# End Source File
# Begin Source File

SOURCE=.\AirportDatabase.h
# End Source File
# Begin Source File

SOURCE=.\AirportDatabaseList.h
# End Source File
# Begin Source File

SOURCE=.\AirportsManager.h
# End Source File
# Begin Source File

SOURCE=.\ARCColor.h
# End Source File
# Begin Source File

SOURCE=.\ARCException.h
# End Source File
# Begin Source File

SOURCE=.\ARCLine.h
# End Source File
# Begin Source File

SOURCE=.\ARCMathCommon.h
# End Source File
# Begin Source File

SOURCE=.\ARCMatrix4x4.h
# End Source File
# Begin Source File

SOURCE=.\ARCPath.h
# End Source File
# Begin Source File

SOURCE=.\ARCPolygon.h
# End Source File
# Begin Source File

SOURCE=.\ARCVector.h
# End Source File
# Begin Source File

SOURCE=.\arrays.h
# End Source File
# Begin Source File

SOURCE=.\CodeTimeTest.h
# End Source File
# Begin Source File

SOURCE=.\Common.h
# End Source File
# Begin Source File

SOURCE=.\CommonDll.h
# End Source File
# Begin Source File

SOURCE=.\containr.h
# End Source File
# Begin Source File

SOURCE=.\csv_file.h
# End Source File
# Begin Source File

SOURCE=.\DATASET.H
# End Source File
# Begin Source File

SOURCE=.\DirectoryUtil.h
# End Source File
# Begin Source File

SOURCE=.\DistancePath.h
# End Source File
# Begin Source File

SOURCE=.\elaptime.h
# End Source File
# Begin Source File

SOURCE=.\err.h
# End Source File
# Begin Source File

SOURCE=.\ExceptionMsgDB.h
# End Source File
# Begin Source File

SOURCE=.\exeption.h
# End Source File
# Begin Source File

SOURCE=.\fileman.h
# End Source File
# Begin Source File

SOURCE=.\FloatUtils.h
# End Source File
# Begin Source File

SOURCE=.\fsstream.h
# End Source File
# Begin Source File

SOURCE=.\GlobalDB.h
# End Source File
# Begin Source File

SOURCE=.\GlobalDBList.h
# End Source File
# Begin Source File

SOURCE=.\handler.h
# End Source File
# Begin Source File

SOURCE=.\HELPLIST.H
# End Source File
# Begin Source File

SOURCE=.\IDGather.h
# End Source File
# Begin Source File

SOURCE=.\InfoZip.h
# End Source File
# Begin Source File

SOURCE=.\LINE.H
# End Source File
# Begin Source File

SOURCE=.\LogDebug.h
# End Source File
# Begin Source File

SOURCE=.\PATH.H
# End Source File
# Begin Source File

SOURCE=.\POINT.H
# End Source File
# Begin Source File

SOURCE=.\POLLYGON.H
# End Source File
# Begin Source File

SOURCE=.\ProgressBar.h
# End Source File
# Begin Source File

SOURCE=.\ProjectManager.h
# End Source File
# Begin Source File

SOURCE=.\replace.h
# End Source File
# Begin Source File

SOURCE=.\ReportItem.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Sector.h
# End Source File
# Begin Source File

SOURCE=.\SimAndReportManager.h
# End Source File
# Begin Source File

SOURCE=.\SimResult.h
# End Source File
# Begin Source File

SOURCE=.\StartDate.h
# End Source File
# Begin Source File

SOURCE=.\STATES.H
# End Source File
# Begin Source File

SOURCE=.\StateTimePoint.h
# End Source File
# Begin Source File

SOURCE=.\strdict.h
# End Source File
# Begin Source File

SOURCE=.\StringConvertToFlight.h
# End Source File
# Begin Source File

SOURCE=.\Subairline.h
# End Source File
# Begin Source File

SOURCE=.\template.h
# End Source File
# Begin Source File

SOURCE=.\TERM_BIN.H
# End Source File
# Begin Source File

SOURCE=.\TERM_ERR.H
# End Source File
# Begin Source File

SOURCE=.\TERMFILE.H
# End Source File
# Begin Source File

SOURCE=.\TimePoint.h
# End Source File
# Begin Source File

SOURCE=.\UndoManager.h
# End Source File
# Begin Source File

SOURCE=.\UndoProject.h
# End Source File
# Begin Source File

SOURCE=.\UnzipDLL.h
# End Source File
# Begin Source File

SOURCE=.\UTIL.H
# End Source File
# Begin Source File

SOURCE=.\WinMsg.h
# End Source File
# Begin Source File

SOURCE=.\ZCallBck.h
# End Source File
# Begin Source File

SOURCE=.\ZipDLL.h
# End Source File
# End Group
# End Target
# End Project
