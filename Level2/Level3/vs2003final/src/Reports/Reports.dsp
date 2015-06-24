# Microsoft Developer Studio Project File - Name="reports" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=reports - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Reports.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Reports.mak" CFG="reports - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "reports - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "reports - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ARCTERM/Reports", UJAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "reports - Win32 Release"

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
# ADD LIB32 /nologo /out:"..\..\obj\Reports.lib"

!ELSEIF  "$(CFG)" == "reports - Win32 Debug"

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
# ADD LIB32 /nologo /out:"..\..\objd\Reports.lib"

!ENDIF 

# Begin Target

# Name "reports - Win32 Release"
# Name "reports - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AcOperationsReport.cpp
# End Source File
# Begin Source File

SOURCE=.\ActivityBreakdownReport.cpp
# End Source File
# Begin Source File

SOURCE=.\ActivityElements.cpp
# End Source File
# Begin Source File

SOURCE=.\AverageQueueLengthReport.cpp
# End Source File
# Begin Source File

SOURCE=.\BagCountReport.cpp
# End Source File
# Begin Source File

SOURCE=.\BagDeliveryTimeReport.cpp
# End Source File
# Begin Source File

SOURCE=.\BagDistReport.cpp
# End Source File
# Begin Source File

SOURCE=.\BagWaitElement.cpp
# End Source File
# Begin Source File

SOURCE=.\BagWaitReport.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseReport.cpp
# End Source File
# Begin Source File

SOURCE=.\CollisionPaxIdx.cpp
# End Source File
# Begin Source File

SOURCE=.\CollisionReport.cpp
# End Source File
# Begin Source File

SOURCE=.\CollisionUitl.cpp
# End Source File
# Begin Source File

SOURCE=.\CollisonGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\CollisonGridMap.cpp
# End Source File
# Begin Source File

SOURCE=.\CollisonTimeSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\ConveyorWaitLenghtReport.cpp
# End Source File
# Begin Source File

SOURCE=.\ConveyorWaitTimeReport.cpp
# End Source File
# Begin Source File

SOURCE=.\DistanceReport.cpp
# End Source File
# Begin Source File

SOURCE=.\DISTELEM.CPP
# End Source File
# Begin Source File

SOURCE=.\DurationReport.cpp
# End Source File
# Begin Source File

SOURCE=.\FireEvacuationReportElement.cpp
# End Source File
# Begin Source File

SOURCE=.\FireEvacutionReport.cpp
# End Source File
# Begin Source File

SOURCE=.\FromToProcCaculation.cpp
# End Source File
# Begin Source File

SOURCE=.\IntervalStat.cpp
# End Source File
# Begin Source File

SOURCE=.\MissFlightReport.cpp
# End Source File
# Begin Source File

SOURCE=.\MutiRunReportAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\PassengerDensityReport.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxCountReport.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintLog.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\QueueElement.cpp
# End Source File
# Begin Source File

SOURCE=.\QueueLengthReport.cpp
# End Source File
# Begin Source File

SOURCE=.\QueueReport.cpp
# End Source File
# Begin Source File

SOURCE=.\REP_PAX.CPP
# End Source File
# Begin Source File

SOURCE=.\ReportManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportParaDB.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportParameter.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportParaOfPaxType.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportParaOfProcs.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportParaOfReportType.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportParaOfThreshold.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportParaOfTime.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportParaOfTwoGroupProcs.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportParaWithArea.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportParaWithPortal.cpp
# End Source File
# Begin Source File

SOURCE=.\Reports.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportSpecs.cpp
# End Source File
# Begin Source File

SOURCE=.\SERVELEM.CPP
# End Source File
# Begin Source File

SOURCE=.\ServiceReport.cpp
# End Source File
# Begin Source File

SOURCE=.\SimAutoReportPara.cpp
# End Source File
# Begin Source File

SOURCE=.\SimGeneralPara.cpp
# End Source File
# Begin Source File

SOURCE=.\SimLevelOfService.cpp
# End Source File
# Begin Source File

SOURCE=.\SpaceThroughputReport.cpp
# End Source File
# Begin Source File

SOURCE=.\SpaceThroughtoutResult.cpp
# End Source File
# Begin Source File

SOURCE=.\StatisticalTools.cpp
# End Source File
# Begin Source File

SOURCE=.\ThroughputReport.cpp
# End Source File
# Begin Source File

SOURCE=.\UtilizationReport.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AcOperationsReport.h
# End Source File
# Begin Source File

SOURCE=.\ActivityBreakdownReport.h
# End Source File
# Begin Source File

SOURCE=.\ActivityElements.h
# End Source File
# Begin Source File

SOURCE=.\AverageQueueLengthReport.h
# End Source File
# Begin Source File

SOURCE=.\BagCountReport.h
# End Source File
# Begin Source File

SOURCE=.\BagDeliveryTimeReport.h
# End Source File
# Begin Source File

SOURCE=.\BagDistReport.h
# End Source File
# Begin Source File

SOURCE=.\BagWaitElement.h
# End Source File
# Begin Source File

SOURCE=.\BagWaitReport.h
# End Source File
# Begin Source File

SOURCE=.\BaseReport.h
# End Source File
# Begin Source File

SOURCE=.\CollisionPaxIdx.h
# End Source File
# Begin Source File

SOURCE=.\CollisionReport.h
# End Source File
# Begin Source File

SOURCE=.\CollisionUitl.h
# End Source File
# Begin Source File

SOURCE=.\CollisonGrid.h
# End Source File
# Begin Source File

SOURCE=.\CollisonGridMap.h
# End Source File
# Begin Source File

SOURCE=.\CollisonTimeSegment.h
# End Source File
# Begin Source File

SOURCE=.\ConveyorWaitLenghtReport.h
# End Source File
# Begin Source File

SOURCE=.\ConveyorWaitTimeReport.h
# End Source File
# Begin Source File

SOURCE=.\DistanceReport.h
# End Source File
# Begin Source File

SOURCE=.\DISTELEM.H
# End Source File
# Begin Source File

SOURCE=.\DurationReport.h
# End Source File
# Begin Source File

SOURCE=.\FireEvacuationReportElement.h
# End Source File
# Begin Source File

SOURCE=.\FireEvacutionReport.h
# End Source File
# Begin Source File

SOURCE=.\FromToProcCaculation.h
# End Source File
# Begin Source File

SOURCE=.\IntervalStat.h
# End Source File
# Begin Source File

SOURCE=.\MissFlightReport.h
# End Source File
# Begin Source File

SOURCE=.\MutiReportShowData.h
# End Source File
# Begin Source File

SOURCE=.\MutiRunReportAgent.h
# End Source File
# Begin Source File

SOURCE=.\PassengerDensityReport.h
# End Source File
# Begin Source File

SOURCE=.\PaxCountReport.h
# End Source File
# Begin Source File

SOURCE=.\PrintLog.h
# End Source File
# Begin Source File

SOURCE=.\ProgressWnd.h
# End Source File
# Begin Source File

SOURCE=.\QueueElement.h
# End Source File
# Begin Source File

SOURCE=.\QueueLengthReport.h
# End Source File
# Begin Source File

SOURCE=.\QueueReport.h
# End Source File
# Begin Source File

SOURCE=.\REP_PAX.H
# End Source File
# Begin Source File

SOURCE=.\ReportManager.h
# End Source File
# Begin Source File

SOURCE=.\ReportParaDB.h
# End Source File
# Begin Source File

SOURCE=.\ReportParameter.h
# End Source File
# Begin Source File

SOURCE=.\ReportParaOfPaxType.h
# End Source File
# Begin Source File

SOURCE=.\ReportParaOfProcs.h
# End Source File
# Begin Source File

SOURCE=.\ReportParaOfReportType.h
# End Source File
# Begin Source File

SOURCE=.\ReportParaOfThreshold.h
# End Source File
# Begin Source File

SOURCE=.\ReportParaOfTime.h
# End Source File
# Begin Source File

SOURCE=.\ReportParaOfTwoGroupProcs.h
# End Source File
# Begin Source File

SOURCE=.\ReportParaWithArea.h
# End Source File
# Begin Source File

SOURCE=.\ReportParaWithPortal.h
# End Source File
# Begin Source File

SOURCE=.\Reports.h
# End Source File
# Begin Source File

SOURCE=.\ReportsDll.h
# End Source File
# Begin Source File

SOURCE=.\ReportSpecs.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SERVELEM.H
# End Source File
# Begin Source File

SOURCE=.\ServiceReport.h
# End Source File
# Begin Source File

SOURCE=.\SimAutoReportPara.h
# End Source File
# Begin Source File

SOURCE=.\SimGeneralPara.h
# End Source File
# Begin Source File

SOURCE=.\SimLevelOfService.h
# End Source File
# Begin Source File

SOURCE=.\SpaceThroughputReport.h
# End Source File
# Begin Source File

SOURCE=.\SpaceThroughtoutResult.h
# End Source File
# Begin Source File

SOURCE=.\StatisticalTools.h
# End Source File
# Begin Source File

SOURCE=.\ThroughputReport.h
# End Source File
# Begin Source File

SOURCE=.\UtilizationReport.h
# End Source File
# End Group
# End Target
# End Project
