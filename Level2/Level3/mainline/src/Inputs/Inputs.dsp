# Microsoft Developer Studio Project File - Name="inputs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=inputs - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Inputs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Inputs.mak" CFG="inputs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "inputs - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "inputs - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ARCTERM/Inputs", QGAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "inputs - Win32 Release"

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
# ADD LIB32 /nologo /out:"..\..\obj\Inputs.lib"

!ELSEIF  "$(CFG)" == "inputs - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /I "..\main" /I "..\\" /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\objd\Inputs.lib"

!ENDIF 

# Begin Target

# Name "inputs - Win32 Release"
# Name "inputs - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ACDATA.CPP
# End Source File
# Begin Source File

SOURCE=.\AllCarSchedule.cpp
# End Source File
# Begin Source File

SOURCE=.\AllGreetingProcessors.cpp
# End Source File
# Begin Source File

SOURCE=.\AllPaxTypeFlow.cpp
# End Source File
# Begin Source File

SOURCE=.\AllProcessUnitInteralFlow.cpp
# End Source File
# Begin Source File

SOURCE=.\ASSIGN.CPP
# End Source File
# Begin Source File

SOURCE=.\ASSIGNDB.CPP
# End Source File
# Begin Source File

SOURCE=.\BAGDATA.CPP
# End Source File
# Begin Source File

SOURCE=.\BagGateAssign.cpp
# End Source File
# Begin Source File

SOURCE=.\CATEGORY.CPP
# End Source File
# Begin Source File

SOURCE=.\CON_DB.CPP
# End Source File
# Begin Source File

SOURCE=.\CONSTRNT.CPP
# End Source File
# Begin Source File

SOURCE=.\DestProcArcInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Direction.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectionUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\DISTLIST.CPP
# End Source File
# Begin Source File

SOURCE=.\flight.cpp
# End Source File
# Begin Source File

SOURCE=.\FlightConWithProcIDDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\FlightPriorityInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\FlightScheduleCriteriaDB.cpp
# End Source File
# Begin Source File

SOURCE=.\FlowItemEx.cpp
# End Source File
# Begin Source File

SOURCE=.\FlowPair.cpp
# End Source File
# Begin Source File

SOURCE=.\FLT_CNST.CPP
# End Source File
# Begin Source File

SOURCE=.\FLT_DB.CPP
# End Source File
# Begin Source File

SOURCE=.\FLTDATA.CPP
# End Source File
# Begin Source File

SOURCE=.\FLTDIST.CPP
# End Source File
# Begin Source File

SOURCE=.\GageLagTimeData.cpp
# End Source File
# Begin Source File

SOURCE=.\GateAssignmentMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\GateLagTimeDB.cpp
# End Source File
# Begin Source File

SOURCE=.\GateMappingDB.cpp
# End Source File
# Begin Source File

SOURCE=.\GatePriorityInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\GreetingKey.cpp
# End Source File
# Begin Source File

SOURCE=.\GreetingProcessors.cpp
# End Source File
# Begin Source File

SOURCE=.\HistogramManager.cpp
# End Source File
# Begin Source File

SOURCE=.\HUBBING.CPP
# End Source File
# Begin Source File

SOURCE=.\HubbingData.cpp
# End Source File
# Begin Source File

SOURCE=.\HubbingDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\IN_TERM.CPP
# End Source File
# Begin Source File

SOURCE=.\Inputs.cpp
# End Source File
# Begin Source File

SOURCE=.\MISCPROC.CPP
# End Source File
# Begin Source File

SOURCE=.\MobileElemConstraint.cpp
# End Source File
# Begin Source File

SOURCE=.\MobileElemConstraintDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\MobileElemTypeStrDB.cpp
# End Source File
# Begin Source File

SOURCE=.\MOVRDATA.CPP
# End Source File
# Begin Source File

SOURCE=.\MultiMobConstraint.cpp
# End Source File
# Begin Source File

SOURCE=.\PassengerFlowByPaxDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\PAX_CNST.CPP
# End Source File
# Begin Source File

SOURCE=.\PAX_DB.CPP
# End Source File
# Begin Source File

SOURCE=.\PaxBulkInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\PAXDATA.CPP
# End Source File
# Begin Source File

SOURCE=.\PaxDirecton.cpp
# End Source File
# Begin Source File

SOURCE=.\PAXDIST.CPP
# End Source File
# Begin Source File

SOURCE=.\PAXFLOW.CPP
# End Source File
# Begin Source File

SOURCE=.\PaxFlowConvertor.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxFlowTree.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxFlowTreeNode.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxToResourcePoolMap.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxVisitorGreetingPlace.cpp
# End Source File
# Begin Source File

SOURCE=.\Pipe.cpp
# End Source File
# Begin Source File

SOURCE=.\PipeDataSet.cpp
# End Source File
# Begin Source File

SOURCE=.\PipeGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\PipeInterSectionPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\PROBAB.CPP
# End Source File
# Begin Source File

SOURCE=.\ProbDistEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\ProbDistHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\ProbDistManager.cpp
# End Source File
# Begin Source File

SOURCE=.\PROCDATA.CPP
# End Source File
# Begin Source File

SOURCE=.\PROCDB.CPP
# End Source File
# Begin Source File

SOURCE=.\PROCDIST.CPP
# End Source File
# Begin Source File

SOURCE=.\ProcessorDistributionWithPipe.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessorSwitchTimeDB.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessUnitInternalFlow.cpp
# End Source File
# Begin Source File

SOURCE=.\PROCHRCH.CPP
# End Source File
# Begin Source File

SOURCE=.\PROCID.CPP
# End Source File
# Begin Source File

SOURCE=.\ProcToResource.cpp
# End Source File
# Begin Source File

SOURCE=.\RailWayData.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourcePool.cpp
# End Source File
# Begin Source File

SOURCE=.\Schedule.cpp
# End Source File
# Begin Source File

SOURCE=.\SECTLIST.CPP
# End Source File
# Begin Source File

SOURCE=.\SideMoveWalkDataSet.cpp
# End Source File
# Begin Source File

SOURCE=.\SideMoveWalkPaxDist.cpp
# End Source File
# Begin Source File

SOURCE=.\SideMoveWalkProcData.cpp
# End Source File
# Begin Source File

SOURCE=.\SimParameter.cpp
# End Source File
# Begin Source File

SOURCE=.\SingleCarSchedule.cpp
# End Source File
# Begin Source File

SOURCE=.\SinglePaxTypeFlow.cpp
# End Source File
# Begin Source File

SOURCE=.\SRVCTIME.CPP
# End Source File
# Begin Source File

SOURCE=.\StationLayout.cpp
# End Source File
# Begin Source File

SOURCE=.\TimePointOnSideWalk.cpp
# End Source File
# Begin Source File

SOURCE=.\TYPELIST.CPP
# End Source File
# Begin Source File

SOURCE=.\UserName.cpp
# End Source File
# Begin Source File

SOURCE=.\ViehcleMovePath.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ACDATA.H
# End Source File
# Begin Source File

SOURCE=.\AllCarSchedule.h
# End Source File
# Begin Source File

SOURCE=.\AllGreetingProcessors.h
# End Source File
# Begin Source File

SOURCE=.\AllPaxTypeFlow.h
# End Source File
# Begin Source File

SOURCE=.\AllProcessUnitInteralFlow.h
# End Source File
# Begin Source File

SOURCE=.\ASSIGN.H
# End Source File
# Begin Source File

SOURCE=.\ASSIGNDB.H
# End Source File
# Begin Source File

SOURCE=.\BAGDATA.H
# End Source File
# Begin Source File

SOURCE=.\BagGateAssign.h
# End Source File
# Begin Source File

SOURCE=.\CATEGORY.H
# End Source File
# Begin Source File

SOURCE=.\CON_DB.H
# End Source File
# Begin Source File

SOURCE=.\CONSTRNT.H
# End Source File
# Begin Source File

SOURCE=.\DestProcArcInfo.h
# End Source File
# Begin Source File

SOURCE=.\Direction.h
# End Source File
# Begin Source File

SOURCE=.\DirectionUtil.h
# End Source File
# Begin Source File

SOURCE=.\DISTLIST.H
# End Source File
# Begin Source File

SOURCE=.\FLIGHT.H
# End Source File
# Begin Source File

SOURCE=.\FlightConWithProcIDDatabase.h
# End Source File
# Begin Source File

SOURCE=.\FlightPriorityInfo.h
# End Source File
# Begin Source File

SOURCE=.\FlightScheduleCriteriaDB.h
# End Source File
# Begin Source File

SOURCE=.\FlowItemEx.h
# End Source File
# Begin Source File

SOURCE=.\FlowPair.h
# End Source File
# Begin Source File

SOURCE=.\FLT_CNST.H
# End Source File
# Begin Source File

SOURCE=.\FLT_DB.H
# End Source File
# Begin Source File

SOURCE=.\FLTDATA.H
# End Source File
# Begin Source File

SOURCE=.\FLTDIST.H
# End Source File
# Begin Source File

SOURCE=.\FLTLIST.H
# End Source File
# Begin Source File

SOURCE=.\GageLagTimeData.h
# End Source File
# Begin Source File

SOURCE=.\GateAssignmentMgr.h
# End Source File
# Begin Source File

SOURCE=.\GateLagTimeDB.h
# End Source File
# Begin Source File

SOURCE=.\GateMappingDB.h
# End Source File
# Begin Source File

SOURCE=.\GatePriorityInfo.h
# End Source File
# Begin Source File

SOURCE=.\GreetingKey.h
# End Source File
# Begin Source File

SOURCE=.\GreetingProcessors.h
# End Source File
# Begin Source File

SOURCE=.\HistogramManager.h
# End Source File
# Begin Source File

SOURCE=.\HUBBING.H
# End Source File
# Begin Source File

SOURCE=.\HubbingData.h
# End Source File
# Begin Source File

SOURCE=.\HubbingDatabase.h
# End Source File
# Begin Source File

SOURCE=.\IN_TERM.H
# End Source File
# Begin Source File

SOURCE=.\Inputs.h
# End Source File
# Begin Source File

SOURCE=.\InputsDll.h
# End Source File
# Begin Source File

SOURCE=.\MISCPROC.H
# End Source File
# Begin Source File

SOURCE=.\MobileElemConstraint.h
# End Source File
# Begin Source File

SOURCE=.\MobileElemConstraintDatabase.h
# End Source File
# Begin Source File

SOURCE=.\MobileElemTypeStrDB.h
# End Source File
# Begin Source File

SOURCE=.\MOVRDATA.H
# End Source File
# Begin Source File

SOURCE=.\MultiMobConstraint.h
# End Source File
# Begin Source File

SOURCE=.\PassengerFlowByPaxDatabase.h
# End Source File
# Begin Source File

SOURCE=.\PAX_CNST.H
# End Source File
# Begin Source File

SOURCE=.\PAX_DB.H
# End Source File
# Begin Source File

SOURCE=.\PaxBulkInfo.h
# End Source File
# Begin Source File

SOURCE=.\PAXDATA.H
# End Source File
# Begin Source File

SOURCE=.\PaxDirecton.h
# End Source File
# Begin Source File

SOURCE=.\PAXDIST.H
# End Source File
# Begin Source File

SOURCE=.\PAXFLOW.H
# End Source File
# Begin Source File

SOURCE=.\PaxFlowConvertor.h
# End Source File
# Begin Source File

SOURCE=.\PaxFlowTree.h
# End Source File
# Begin Source File

SOURCE=.\PaxFlowTreeNode.h
# End Source File
# Begin Source File

SOURCE=.\PaxToResourcePoolMap.h
# End Source File
# Begin Source File

SOURCE=.\PaxVisitorGreetingPlace.h
# End Source File
# Begin Source File

SOURCE=.\Pipe.h
# End Source File
# Begin Source File

SOURCE=.\PipeDataSet.h
# End Source File
# Begin Source File

SOURCE=.\PipeGraph.h
# End Source File
# Begin Source File

SOURCE=.\PipeInterSectionPoint.h
# End Source File
# Begin Source File

SOURCE=.\PROBAB.H
# End Source File
# Begin Source File

SOURCE=.\ProbDistEntry.h
# End Source File
# Begin Source File

SOURCE=.\ProbDistHelper.h
# End Source File
# Begin Source File

SOURCE=.\ProbDistManager.h
# End Source File
# Begin Source File

SOURCE=.\PROCDATA.H
# End Source File
# Begin Source File

SOURCE=.\PROCDB.H
# End Source File
# Begin Source File

SOURCE=.\PROCDIST.H
# End Source File
# Begin Source File

SOURCE=.\ProcessorDistributionWithPipe.h
# End Source File
# Begin Source File

SOURCE=.\ProcessorSwitchTimeDB.h
# End Source File
# Begin Source File

SOURCE=.\ProcessUnitInternalFlow.h
# End Source File
# Begin Source File

SOURCE=.\PROCHRCH.H
# End Source File
# Begin Source File

SOURCE=.\PROCID.H
# End Source File
# Begin Source File

SOURCE=.\ProcToResource.h
# End Source File
# Begin Source File

SOURCE=.\RailWayData.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ResourcePool.h
# End Source File
# Begin Source File

SOURCE=.\SCHEDULE.H
# End Source File
# Begin Source File

SOURCE=.\SECTLIST.H
# End Source File
# Begin Source File

SOURCE=.\SideMoveWalkDataSet.h
# End Source File
# Begin Source File

SOURCE=.\SideMoveWalkPaxDist.h
# End Source File
# Begin Source File

SOURCE=.\SideMoveWalkProcData.h
# End Source File
# Begin Source File

SOURCE=.\SimParameter.h
# End Source File
# Begin Source File

SOURCE=.\SingleCarSchedule.h
# End Source File
# Begin Source File

SOURCE=.\SinglePaxTypeFlow.h
# End Source File
# Begin Source File

SOURCE=.\SRVCTIME.H
# End Source File
# Begin Source File

SOURCE=.\StationLayout.h
# End Source File
# Begin Source File

SOURCE=.\TimePointOnSideWalk.h
# End Source File
# Begin Source File

SOURCE=.\TYPELIST.H
# End Source File
# Begin Source File

SOURCE=.\UserName.h
# End Source File
# Begin Source File

SOURCE=.\ViehcleMovePath.h
# End Source File
# End Group
# End Target
# End Project
