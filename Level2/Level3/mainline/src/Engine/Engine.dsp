# Microsoft Developer Studio Project File - Name="engine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=engine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Engine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Engine.mak" CFG="engine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "engine - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "engine - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ARCTERM/Engine", KDAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "engine - Win32 Release"

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
# ADD LIB32 /nologo /out:"..\..\obj\Engine.lib"

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

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
# ADD LIB32 /nologo /out:"..\..\objd\Engine.lib"

!ENDIF 

# Begin Target

# Name "engine - Win32 Release"
# Name "engine - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\accessq.cpp
# End Source File
# Begin Source File

SOURCE=.\AdjacentStation.cpp
# End Source File
# Begin Source File

SOURCE=.\AirfieldProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\anServeProcList.cpp
# End Source File
# Begin Source File

SOURCE=.\Arp.cpp
# End Source File
# Begin Source File

SOURCE=.\ASSEVENT.CPP
# End Source File
# Begin Source File

SOURCE=.\AutoCreateProcessorContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoRoster.cpp
# End Source File
# Begin Source File

SOURCE=.\AvoidDensityEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\BAGENTER.CPP
# End Source File
# Begin Source File

SOURCE=.\BAGGAGE.CPP
# End Source File
# Begin Source File

SOURCE=.\BaggageProcUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\BARRIER.CPP
# End Source File
# Begin Source File

SOURCE=.\BOARDCAL.CPP
# End Source File
# Begin Source File

SOURCE=.\Car.cpp
# End Source File
# Begin Source File

SOURCE=.\conveyor.cpp
# End Source File
# Begin Source File

SOURCE=.\ConveyorStepItEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\DeiceBayProc.cpp
# End Source File
# Begin Source File

SOURCE=.\DEP_SINK.CPP
# End Source File
# Begin Source File

SOURCE=.\DEP_SRCE.CPP
# End Source File
# Begin Source File

SOURCE=.\ElevatorMoveEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\ElevatorProc.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine.cpp
# End Source File
# Begin Source File

SOURCE=.\ENTRY.CPP
# End Source File
# Begin Source File

SOURCE=.\Escalator.cpp
# End Source File
# Begin Source File

SOURCE=.\EVENT.CPP
# End Source File
# Begin Source File

SOURCE=.\ExceedMaxTimeCheckEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\FetchPersonInQueueEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\FireEvacuationEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\FIXEDQ.CPP
# End Source File
# Begin Source File

SOURCE=.\FixProc.cpp
# End Source File
# Begin Source File

SOURCE=.\FlightArrDepEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\FLOORCHG.CPP
# End Source File
# Begin Source File

SOURCE=.\FlowBelt.cpp
# End Source File
# Begin Source File

SOURCE=.\FLowChannelList.cpp
# End Source File
# Begin Source File

SOURCE=.\GATE.CPP
# End Source File
# Begin Source File

SOURCE=.\GREET.CPP
# End Source File
# Begin Source File

SOURCE=.\GroupFollowerInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupLeaderInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\HOLD.CPP
# End Source File
# Begin Source File

SOURCE=.\IntegratedStation.cpp
# End Source File
# Begin Source File

SOURCE=.\LINEPROC.CPP
# End Source File
# Begin Source File

SOURCE=.\LINEQ.CPP
# End Source File
# Begin Source File

SOURCE=.\Loader.cpp
# End Source File
# Begin Source File

SOURCE=.\MOBILE.CPP
# End Source File
# Begin Source File

SOURCE=.\Monitor.cpp
# End Source File
# Begin Source File

SOURCE=.\MOVEVENT.CPP
# End Source File
# Begin Source File

SOURCE=.\MovingSideWalk.cpp
# End Source File
# Begin Source File

SOURCE=.\NFIXEDQ.CPP
# End Source File
# Begin Source File

SOURCE=.\NodeProc.cpp
# End Source File
# Begin Source File

SOURCE=.\onvergePair.cpp
# End Source File
# Begin Source File

SOURCE=.\PAX.CPP
# End Source File
# Begin Source File

SOURCE=.\PERSON.CPP
# End Source File
# Begin Source File

SOURCE=.\PersonOnStation.cpp
# End Source File
# Begin Source File

SOURCE=.\PointAndBagsInBagProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\PROCESS.CPP
# End Source File
# Begin Source File

SOURCE=.\ProcGroupIdxTree.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcGroupIdxTreeNode.cpp
# End Source File
# Begin Source File

SOURCE=.\proclist.cpp
# End Source File
# Begin Source File

SOURCE=.\PROCQ.CPP
# End Source File
# Begin Source File

SOURCE=.\Pusher.cpp
# End Source File
# Begin Source File

SOURCE=.\PusherCheckMaxWaitTimeEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\PusherScheduleEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\RailwayInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\RailwayInfoKey.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceElement.cpp
# End Source File
# Begin Source File

SOURCE=.\Rule.cpp
# End Source File
# Begin Source File

SOURCE=.\RunwayProc.cpp
# End Source File
# Begin Source File

SOURCE=.\Scanner.cpp
# End Source File
# Begin Source File

SOURCE=.\SEQUENCQ.CPP
# End Source File
# Begin Source File

SOURCE=.\ServiceSlot.cpp
# End Source File
# Begin Source File

SOURCE=.\SimMobileElemList.cpp
# End Source File
# Begin Source File

SOURCE=.\SimpleConveyor.cpp
# End Source File
# Begin Source File

SOURCE=.\Sorter.cpp
# End Source File
# Begin Source File

SOURCE=.\Splitter.cpp
# End Source File
# Begin Source File

SOURCE=.\Stair.cpp
# End Source File
# Begin Source File

SOURCE=.\StandProc.cpp
# End Source File
# Begin Source File

SOURCE=.\TaxiwayProc.cpp
# End Source File
# Begin Source File

SOURCE=.\TERMINAL.CPP
# End Source File
# Begin Source File

SOURCE=.\TLOSEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\TrainTrafficGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\TURNARND.CPP
# End Source File
# Begin Source File

SOURCE=.\TurnaroundVisitor.cpp
# End Source File
# Begin Source File

SOURCE=.\VISITOR.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ACCESSQ.H
# End Source File
# Begin Source File

SOURCE=.\AdjacentStation.h
# End Source File
# Begin Source File

SOURCE=.\AirfieldProcessor.h
# End Source File
# Begin Source File

SOURCE=.\anServeProcList.h
# End Source File
# Begin Source File

SOURCE=.\Arp.h
# End Source File
# Begin Source File

SOURCE=.\ASSEVENT.H
# End Source File
# Begin Source File

SOURCE=.\AutoCreateProcessorContainer.h
# End Source File
# Begin Source File

SOURCE=.\AutoRoster.h
# End Source File
# Begin Source File

SOURCE=.\AvoidDensityEvent.h
# End Source File
# Begin Source File

SOURCE=.\BAGENTER.H
# End Source File
# Begin Source File

SOURCE=.\BAGGAGE.H
# End Source File
# Begin Source File

SOURCE=.\BaggageProcUtil.h
# End Source File
# Begin Source File

SOURCE=.\BARRIER.H
# End Source File
# Begin Source File

SOURCE=.\BOARDCAL.H
# End Source File
# Begin Source File

SOURCE=.\Car.h
# End Source File
# Begin Source File

SOURCE=.\conveyor.h
# End Source File
# Begin Source File

SOURCE=.\ConveyorStepItEvent.h
# End Source File
# Begin Source File

SOURCE=.\DeiceBayProc.h
# End Source File
# Begin Source File

SOURCE=.\DEP_SINK.H
# End Source File
# Begin Source File

SOURCE=.\DEP_SRCE.H
# End Source File
# Begin Source File

SOURCE=.\ElevatorMoveEvent.h
# End Source File
# Begin Source File

SOURCE=.\ElevatorProc.h
# End Source File
# Begin Source File

SOURCE=.\Engine.h
# End Source File
# Begin Source File

SOURCE=.\EngineDll.h
# End Source File
# Begin Source File

SOURCE=.\ENTRY.H
# End Source File
# Begin Source File

SOURCE=.\Escalator.h
# End Source File
# Begin Source File

SOURCE=.\EVENT.H
# End Source File
# Begin Source File

SOURCE=.\ExceedMaxTimeCheckEvent.h
# End Source File
# Begin Source File

SOURCE=.\FetchPersonInQueueEvent.h
# End Source File
# Begin Source File

SOURCE=.\FireEvacuationEvent.h
# End Source File
# Begin Source File

SOURCE=.\FIXEDQ.H
# End Source File
# Begin Source File

SOURCE=.\FixProc.h
# End Source File
# Begin Source File

SOURCE=.\FlightArrDepEvent.h
# End Source File
# Begin Source File

SOURCE=.\FLOORCHG.H
# End Source File
# Begin Source File

SOURCE=.\FlowBelt.h
# End Source File
# Begin Source File

SOURCE=.\GATE.H
# End Source File
# Begin Source File

SOURCE=.\GREET.H
# End Source File
# Begin Source File

SOURCE=.\GroupFollowerInfo.h
# End Source File
# Begin Source File

SOURCE=.\GroupInfo.h
# End Source File
# Begin Source File

SOURCE=.\GroupLeaderInfo.h
# End Source File
# Begin Source File

SOURCE=.\HOLD.H
# End Source File
# Begin Source File

SOURCE=.\IntegratedStation.h
# End Source File
# Begin Source File

SOURCE=.\LINEPROC.H
# End Source File
# Begin Source File

SOURCE=.\LINEQ.H
# End Source File
# Begin Source File

SOURCE=.\Loader.h
# End Source File
# Begin Source File

SOURCE=.\MOBILE.H
# End Source File
# Begin Source File

SOURCE=.\Monitor.h
# End Source File
# Begin Source File

SOURCE=.\MOVEVENT.H
# End Source File
# Begin Source File

SOURCE=.\MovingSideWalk.h
# End Source File
# Begin Source File

SOURCE=.\NFIXEDQ.H
# End Source File
# Begin Source File

SOURCE=.\NodeProc.h
# End Source File
# Begin Source File

SOURCE=.\onvergePair.h
# End Source File
# Begin Source File

SOURCE=.\PAX.H
# End Source File
# Begin Source File

SOURCE=.\PERSON.H
# End Source File
# Begin Source File

SOURCE=.\PersonOnStation.h
# End Source File
# Begin Source File

SOURCE=.\PointAndBagsInBagProcessor.h
# End Source File
# Begin Source File

SOURCE=.\PROCESS.H
# End Source File
# Begin Source File

SOURCE=.\ProcGroupIdxTree.h
# End Source File
# Begin Source File

SOURCE=.\ProcGroupIdxTreeNode.h
# End Source File
# Begin Source File

SOURCE=.\PROCLIST.H
# End Source File
# Begin Source File

SOURCE=.\PROCQ.H
# End Source File
# Begin Source File

SOURCE=.\Pusher.h
# End Source File
# Begin Source File

SOURCE=.\PusherCheckMaxWaitTimeEvent.h
# End Source File
# Begin Source File

SOURCE=.\PusherScheduleEvent.h
# End Source File
# Begin Source File

SOURCE=.\RailwayInfo.h
# End Source File
# Begin Source File

SOURCE=.\RailwayInfoKey.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ResourceElement.h
# End Source File
# Begin Source File

SOURCE=.\Rule.h
# End Source File
# Begin Source File

SOURCE=.\RunwayProc.h
# End Source File
# Begin Source File

SOURCE=.\Scanner.h
# End Source File
# Begin Source File

SOURCE=.\SEQUENCQ.H
# End Source File
# Begin Source File

SOURCE=.\ServiceSlot.h
# End Source File
# Begin Source File

SOURCE=.\SimMobileElemList.h
# End Source File
# Begin Source File

SOURCE=.\SimpleConveyor.h
# End Source File
# Begin Source File

SOURCE=.\Sorter.h
# End Source File
# Begin Source File

SOURCE=.\Splitter.h
# End Source File
# Begin Source File

SOURCE=.\Stair.h
# End Source File
# Begin Source File

SOURCE=.\StandProc.h
# End Source File
# Begin Source File

SOURCE=.\TaxiwayProc.h
# End Source File
# Begin Source File

SOURCE=.\TERMINAL.H
# End Source File
# Begin Source File

SOURCE=.\TLOSEvent.h
# End Source File
# Begin Source File

SOURCE=.\TracePredef.h
# End Source File
# Begin Source File

SOURCE=.\TrainTrafficGraph.h
# End Source File
# Begin Source File

SOURCE=.\TURNARND.H
# End Source File
# Begin Source File

SOURCE=.\TurnaroundVisitor.h
# End Source File
# Begin Source File

SOURCE=.\VISITOR.H
# End Source File
# End Group
# End Target
# End Project
