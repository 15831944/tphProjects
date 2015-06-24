# Microsoft Developer Studio Project File - Name="TermPlan" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TERMPLAN - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TermPlan.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TermPlan.mak" CFG="TERMPLAN - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TermPlan - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TermPlan - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ARCTERM/main", MNAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TermPlan - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "..\..\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /I "..\\" /I "..\..\include" /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXEXT" /FD /Zm200 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 vfw32.lib opengl32.lib glu32.lib adv6nd.lib ../../obj/common.lib ../../obj/inputs.lib ../../obj/engine.lib ../../obj/results.lib ../../obj/reports.lib msimg32.lib ../../obj/chart.lib htmlhelp.lib netapi32.lib Iphlpapi.lib winmm.lib wmvcore.lib Shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /out:"../../bin/ARCTerm.exe" /libpath:"..\common\release" /libpath:"..\inputs\release" /libpath:"..\engine\release" /libpath:"..\reports\release" /libpath:"..\results\release" /libpath:"..\chart\release" /libpath:"..\..\lib\openDWG" /libpath:"..\..\lib\WMSDK9"
# SUBTRACT LINK32 /profile /incremental:yes /map /nodefaultlib

!ELSEIF  "$(CFG)" == "TermPlan - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "..\..\objd"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include" /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXEXT" /Fr /YX /FD /GZ /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 VFW32.lib opengl32.lib glu32.lib adv6nd.lib ../../objd/common.lib ../../objd/inputs.lib ../../objd/engine.lib ../../objd/results.lib ../../objd/reports.lib msimg32.lib ../../objd/chart.lib htmlhelp.lib netapi32.lib Iphlpapi.lib winmm.lib wmvcore.lib Shlwapi.lib /nologo /subsystem:windows /profile /map:"../../bin/ARCTerm.map" /debug /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"msvcrt" /out:"../../ARCTermD.exe" /libpath:"..\common\debug" /libpath:"..\inputs\debug" /libpath:"..\engine\debug" /libpath:"..\reports\debug" /libpath:"..\results\debug" /libpath:"..\chart\debug" /libpath:"..\..\lib\openDWG" /libpath:"..\..\lib\WMSDK9"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "TermPlan - Win32 Release"
# Name "TermPlan - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\24bitBMP.cpp
# End Source File
# Begin Source File

SOURCE=.\3DCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\3DGridOptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\3DObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\3DTextManager.cpp
# End Source File
# Begin Source File

SOURCE=.\3DView.cpp
# End Source File
# Begin Source File

SOURCE=.\ACCatPropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ACDetails.cpp
# End Source File
# Begin Source File

SOURCE=.\ActivityDensityData.cpp
# End Source File
# Begin Source File

SOURCE=.\ActivityDensityParams.cpp
# End Source File
# Begin Source File

SOURCE=.\AFTEMSBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\AirportMachineExe.cpp
# End Source File
# Begin Source File

SOURCE=.\AirSideMSVImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\AllProcessorTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimationBar.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimationTimeManager.cpp
# End Source File
# Begin Source File

SOURCE=.\AnnualActivityDeductionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AreaDensityOfPaxtype.cpp
# End Source File
# Begin Source File

SOURCE=.\areaintersectpoint.cpp
# End Source File
# Begin Source File

SOURCE=.\AreasPortals.cpp
# End Source File
# Begin Source File

SOURCE=.\AssignFlightDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AttachWalkDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoRosterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BagDevDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BmpBtnDropList.cpp
# End Source File
# Begin Source File

SOURCE=.\BoardingCallDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\ButtonXP.cpp
# End Source File
# Begin Source File

SOURCE=.\CADFile.cpp
# End Source File
# Begin Source File

SOURCE=.\CameraBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Cameras.cpp
# End Source File
# Begin Source File

SOURCE=.\CapitalBDDecisionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CarFlowNextStation.cpp
# End Source File
# Begin Source File

SOURCE=.\CarScheduleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CarScheduleSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\CFIDSFrameView.cpp
# End Source File
# Begin Source File

SOURCE=.\CFIDSView.cpp
# End Source File
# Begin Source File

SOURCE=.\CFlight.cpp
# End Source File
# Begin Source File

SOURCE=.\ChangeVerticalProfileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrameDensityAnim.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrameSplit.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\CJColorPicker.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\CJColorPopup.cpp
# End Source File
# Begin Source File

SOURCE=.\CLicense.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\ComboBoxXP.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\ComparativeList.cpp
# End Source File
# Begin Source File

SOURCE=.\ComparativePlot.cpp
# End Source File
# Begin Source File

SOURCE=.\CompareReportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CompareReportDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\CompareReportLogView.cpp
# End Source File
# Begin Source File

SOURCE=.\CompareReportResDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CompareReportResDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\CompareReportResFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\CompareReportResTBXP.cpp
# End Source File
# Begin Source File

SOURCE=.\CompareReportResultView.cpp
# End Source File
# Begin Source File

SOURCE=.\CompareReportResView.cpp
# End Source File
# Begin Source File

SOURCE=.\CompareReportView.cpp
# End Source File
# Begin Source File

SOURCE=.\CompRepLogBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ConDBExListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ConDBListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ConDBListCtrlWithCheckBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ConWithIDDBListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolBtn.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolTree.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolTreeEditSpin.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolTreeEx.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\DefaultDisplayProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\DensityAnimView.cpp
# End Source File
# Begin Source File

SOURCE=.\DensityGLView.cpp
# End Source File
# Begin Source File

SOURCE=.\DFVCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\DFVDocTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\DFVDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\DFVFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\DIB.CPP
# End Source File
# Begin Source File

SOURCE=.\DisplayPropOverrides.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgActivityDensity.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgACTypeProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAirlineProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAirportDBNameInput.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAirportProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAnimationData.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAreaPortal.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgArrayCopies.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAttrOfLink.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBookmarkedCameras.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgComments.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDBAircraftTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDBAirline.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDBAirports.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFireImpact.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFloorAdjust.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFloorThickness.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGetString.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLayerOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLiveRecordParams.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMathSimSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMovie.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMovieWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPaxTags.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPickTime.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProbDist.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProbDistHistData.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProcDisplayProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProcDispOverrides.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProcessorSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProcessorTags.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProjectName.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRecordOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRetailPackFee.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSectorProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgShapeSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSubairlineProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTracerDensity.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgVisibleRegions.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWalkthrough.cpp
# End Source File
# Begin Source File

SOURCE=.\DockBarEx.cpp
# End Source File
# Begin Source File

SOURCE=.\DotNetTabWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\Draw.cpp
# End Source File
# Begin Source File

SOURCE=.\DWGFile.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicCreatedProcDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EconListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Economic2Dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\EconomicaDataTypeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EconomicDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\EconRepDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ExceptionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FindDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FixedNode.cpp
# End Source File
# Begin Source File

SOURCE=..\Inputs\flight.cpp
# End Source File
# Begin Source File

SOURCE=.\FlightDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FlightGateSelection.cpp
# End Source File
# Begin Source File

SOURCE=.\FlightPriorityDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FlightScheduleAddNewDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FlightScheduleAddUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\FlightScheduleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Floor2.cpp
# End Source File
# Begin Source File

SOURCE=.\FloorMapMatch.cpp
# End Source File
# Begin Source File

SOURCE=.\FloorPropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Floors.cpp
# End Source File
# Begin Source File

SOURCE=.\FlowChartPane.cpp
# End Source File
# Begin Source File

SOURCE=.\FlowChartWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\FltPaxDataDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FltSchedDefineDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameCompareReport.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameCompareReportResult.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameCtrlDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameInputData.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameProcessDefine.cpp
# End Source File
# Begin Source File

SOURCE=.\GannChartFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GannChartWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GannRowWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GanntChartOneLineWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GateAssignDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GateAssignSummaryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GateMapDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GatePriorityDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GateSelectionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\GfxGroupEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\GfxOutBarCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalDBManDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalDBSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\gtchart.cpp
# End Source File
# Begin Source File

SOURCE=.\HeaderCtrlDrag.cpp
# End Source File
# Begin Source File

SOURCE=.\HubDataDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\hypercam.cpp
# End Source File
# Begin Source File

SOURCE=.\IconComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportFlightFromFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportIntoNewProject.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportIntoOldProjectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\INI.cpp
# End Source File
# Begin Source File

SOURCE=.\InPlaceComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\InPlaceComboBoxEx.cpp
# End Source File
# Begin Source File

SOURCE=.\InPlaceDateTimeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\InPlaceEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\InPlaceEdit2.cpp
# End Source File
# Begin Source File

SOURCE=.\InPlaceList.cpp
# End Source File
# Begin Source File

SOURCE=.\InputDataView.cpp
# End Source File
# Begin Source File

SOURCE=.\InputRepDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InputValueDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InterestOnCapitalDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\LayerListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\LayoutBar.cpp
# End Source File
# Begin Source File

SOURCE=.\LeadLagTimeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LeafNode.cpp
# End Source File
# Begin Source File

SOURCE=.\LinkageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ListBoxEx.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\ListBoxXP.cpp
# End Source File
# Begin Source File

SOURCE=.\ListCtrlDrag.cpp
# End Source File
# Begin Source File

SOURCE=.\ListCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\ListCtrlEx2.cpp
# End Source File
# Begin Source File

SOURCE=.\ListWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\MacroCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\MainBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MaintenanceDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MapFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MathematicFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\MathematicView.cpp
# End Source File
# Begin Source File

SOURCE=.\MathFlow.cpp
# End Source File
# Begin Source File

SOURCE=.\MathFlowDataSet.cpp
# End Source File
# Begin Source File

SOURCE=.\MathFlowDefineDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MathProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\MathProcessDataSet.cpp
# End Source File
# Begin Source File

SOURCE=.\MathProcessDefine.cpp
# End Source File
# Begin Source File

SOURCE=.\MathProcessEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\MathProcessEngineFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\MathProcessEngineMGN.cpp
# End Source File
# Begin Source File

SOURCE=.\MathProcessEngineMMN.cpp
# End Source File
# Begin Source File

SOURCE=.\MathResultDataSet.cpp
# End Source File
# Begin Source File

SOURCE=.\MathResultManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MathSimulation.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\MenuCH.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\MenuXP.cpp
# End Source File
# Begin Source File

SOURCE=..\Reports\MissFlightReport.cpp
# End Source File
# Begin Source File

SOURCE=.\MobileCountDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MoblieElemDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MoblieElemTips.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelSelectionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyLatLong.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveProcessorsCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveSideWalkPaxDistDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveSideWalkPropDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveSideWalkSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\Movie.cpp
# End Source File
# Begin Source File

SOURCE=.\Movies.cpp
# End Source File
# Begin Source File

SOURCE=.\MovieWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\MovingSideWorkTree.cpp
# End Source File
# Begin Source File

SOURCE=.\mschart.cpp
# End Source File
# Begin Source File

SOURCE=.\MSVImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\MuliSelTreeCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=..\Reports\MutiRunReportAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\MySpin.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\MyTrace.cpp
# End Source File
# Begin Source File

SOURCE=.\NameListMappingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\NewMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Node.cpp
# End Source File
# Begin Source File

SOURCE=.\NodeView.cpp
# End Source File
# Begin Source File

SOURCE=..\Engine\onvergePair.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenComparativeReportsGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenDWGToolkit.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenSqlDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PassengerTypeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxBulkEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxBulkListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxBulkListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxDispListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxDispPropDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxDispPropItem.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxDispProps.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxDistDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxFilterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxFlowAvoidDensityAddAreasDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxFlowAvoidDensityDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxFlowByProcDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxFlowDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxFlowSelectPipes.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxFlowTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxShapeDefs.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxShapes.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxTags.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxTrackDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxTransferDBDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxType.cpp
# End Source File
# Begin Source File

SOURCE=.\PaxTypeDefDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\PickConveyorTree.cpp
# End Source File
# Begin Source File

SOURCE=.\PipeBar.cpp
# End Source File
# Begin Source File

SOURCE=.\PipeDefinitionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PipeDropWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\PipeShowListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\PipeShowWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Placement.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\Popup.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintableListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintableTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ProbDistDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Proc2DataForClipBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcAssignDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcAssignEventDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcDataPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcDataPageDepSink.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcDataSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessDefineDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcesserDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessFlowView.cpp
# End Source File
# Begin Source File

SOURCE=.\Processor2.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessorTags.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessorTagWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessorTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcPropDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcToResPoolDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectCommentDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectControlDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectSelectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PtLineChart.cpp
# End Source File
# Begin Source File

SOURCE=.\RecordedCameras.cpp
# End Source File
# Begin Source File

SOURCE=.\RecordTimerManager.cpp
# End Source File
# Begin Source File

SOURCE=.\RepControlView.cpp
# End Source File
# Begin Source File

SOURCE=.\RepGraphView.cpp
# End Source File
# Begin Source File

SOURCE=.\RepListView.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportChildFrameSplit.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportParamDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportParameter2.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\ResDispListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ResDispPropDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizePS\ResizableGrip.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizePS\ResizableLayout.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizePS\ResizableMinMax.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizePS\ResizablePage.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizePS\ResizableSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizePS\ResizableState.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceDispPropItem.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceDispProps.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourcePoolDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RosterWizardDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RotateProcessorsCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\RotateShapesCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\RunwayTextures.cpp
# End Source File
# Begin Source File

SOURCE=.\ScaleProcessorsCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\scbarg.cpp
# End Source File
# Begin Source File

SOURCE=..\Inputs\Schedule.cpp
# End Source File
# Begin Source File

SOURCE=.\ServiceTimeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetProjectDBDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Shape.cpp
# End Source File
# Begin Source File

SOURCE=.\ShapesBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ShapesBarPFlow.cpp
# End Source File
# Begin Source File

SOURCE=.\ShapesListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ShapesManager.cpp
# End Source File
# Begin Source File

SOURCE=.\SimEngineDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SimEngSettingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SimTimeRangeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\singleEventsDataDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\sizecbar.cpp
# End Source File
# Begin Source File

SOURCE=.\SmartEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\SortableHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SortHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SortListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Reports\SpaceThroughtoutResult.cpp
# End Source File
# Begin Source File

SOURCE=.\SplitTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SplitterControl.cpp
# End Source File
# Begin Source File

SOURCE=.\SplitterWndEx.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\StartDate.cpp
# End Source File
# Begin Source File

SOURCE=.\StationLinkerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StationLinkerPopDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StationSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\StatusBarXP.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TempProcInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\TempTracerData.cpp
# End Source File
# Begin Source File

SOURCE=.\TerminalMSVImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\TermPlan.cpp
# End Source File
# Begin Source File

SOURCE=.\TermPlanDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\TermPlanView.cpp
# End Source File
# Begin Source File

SOURCE=.\TessellationManager.cpp

!IF  "$(CFG)" == "TermPlan - Win32 Release"

!ELSEIF  "$(CFG)" == "TermPlan - Win32 Debug"

# ADD CPP /Gd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tga.cpp
# End Source File
# Begin Source File

SOURCE=.\ThumbnailsList.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolBar24X24.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\ToolBarXP.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolBarXP24.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\Tools.cpp
# End Source File
# Begin Source File

SOURCE=.\XPStyle\tooltip2.cpp
# End Source File
# Begin Source File

SOURCE=.\TrackWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\TrueColorToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\TVN.cpp
# End Source File
# Begin Source File

SOURCE=.\TVNode.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\UnitBar.cpp
# End Source File
# Begin Source File

SOURCE=.\UnitsManager.cpp
# End Source File
# Begin Source File

SOURCE=.\UnselectableListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\UnselectableTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\UsedProcInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\UsedProcInfoBox.cpp
# End Source File
# Begin Source File

SOURCE=.\UtilitiesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\VideoParams.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Walkthrough.cpp
# End Source File
# Begin Source File

SOURCE=.\Walkthroughs.cpp
# End Source File
# Begin Source File

SOURCE=.\XListCtrl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\24bitBMP.h
# End Source File
# Begin Source File

SOURCE=.\3DCamera.h
# End Source File
# Begin Source File

SOURCE=.\3DGridOptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\3DObjects.h
# End Source File
# Begin Source File

SOURCE=.\3DTextManager.h
# End Source File
# Begin Source File

SOURCE=.\3DView.h
# End Source File
# Begin Source File

SOURCE=.\ACCatPropertiesDlg.h
# End Source File
# Begin Source File

SOURCE=.\ACDetails.h
# End Source File
# Begin Source File

SOURCE=..\Common\ActermExcption.h
# End Source File
# Begin Source File

SOURCE=.\ActivityDensityData.h
# End Source File
# Begin Source File

SOURCE=.\ActivityDensityParams.h
# End Source File
# Begin Source File

SOURCE=.\AFTEMSBitmap.h
# End Source File
# Begin Source File

SOURCE=.\AfxDefs.h
# End Source File
# Begin Source File

SOURCE=.\AirportMachineExe.h
# End Source File
# Begin Source File

SOURCE=.\AirSideMSVImpl.h
# End Source File
# Begin Source File

SOURCE=.\AllDistributions.h
# End Source File
# Begin Source File

SOURCE=.\AllProcessorTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\AnimationBar.h
# End Source File
# Begin Source File

SOURCE=.\AnimationData.h
# End Source File
# Begin Source File

SOURCE=.\AnimationTimeManager.h
# End Source File
# Begin Source File

SOURCE=.\AnnualActivityDeductionDlg.h
# End Source File
# Begin Source File

SOURCE=.\AreaDensityOfPaxtype.h
# End Source File
# Begin Source File

SOURCE=.\AreaIntersectPoint.h
# End Source File
# Begin Source File

SOURCE=.\AreasPortals.h
# End Source File
# Begin Source File

SOURCE=.\AssignFlightDlg.h
# End Source File
# Begin Source File

SOURCE=.\AttachWalkDlg.h
# End Source File
# Begin Source File

SOURCE=.\AutoRosterDlg.h
# End Source File
# Begin Source File

SOURCE=.\BagDevDlg.h
# End Source File
# Begin Source File

SOURCE=.\BmpBtnDropList.h
# End Source File
# Begin Source File

SOURCE=.\BoardingCallDlg.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\BRIEFCASERAISED.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\ButtonXP.h
# End Source File
# Begin Source File

SOURCE=.\CADFile.h
# End Source File
# Begin Source File

SOURCE=.\CameraBar.h
# End Source File
# Begin Source File

SOURCE=.\Cameras.h
# End Source File
# Begin Source File

SOURCE=.\CapitalBDDecisionDlg.h
# End Source File
# Begin Source File

SOURCE=.\CarFlowNextStation.h
# End Source File
# Begin Source File

SOURCE=.\CarScheduleDlg.h
# End Source File
# Begin Source File

SOURCE=.\CarScheduleSheet.h
# End Source File
# Begin Source File

SOURCE=.\CFIDSFrameView.h
# End Source File
# Begin Source File

SOURCE=.\CFIDSView.h
# End Source File
# Begin Source File

SOURCE=.\CFlight.h
# End Source File
# Begin Source File

SOURCE=.\ChangeVerticalProfileDlg.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrameDensityAnim.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrameSplit.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\CJColorPicker.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\CJColorPopup.h
# End Source File
# Begin Source File

SOURCE=.\CLicense.h
# End Source File
# Begin Source File

SOURCE=.\color.h
# End Source File
# Begin Source File

SOURCE=.\ColorBox.h
# End Source File
# Begin Source File

SOURCE=.\ColorTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\ComboBoxXP.h
# End Source File
# Begin Source File

SOURCE=.\Command.h
# End Source File
# Begin Source File

SOURCE=.\CommandHistory.h
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\ComparativeList.h
# End Source File
# Begin Source File

SOURCE=.\ComparativePlot.h
# End Source File
# Begin Source File

SOURCE=.\CompareReportDlg.h
# End Source File
# Begin Source File

SOURCE=.\CompareReportDoc.h
# End Source File
# Begin Source File

SOURCE=.\CompareReportLogView.h
# End Source File
# Begin Source File

SOURCE=.\CompareReportResDlg.h
# End Source File
# Begin Source File

SOURCE=.\CompareReportResDoc.h
# End Source File
# Begin Source File

SOURCE=.\CompareReportResFrame.h
# End Source File
# Begin Source File

SOURCE=.\CompareReportResTBXP.h
# End Source File
# Begin Source File

SOURCE=.\CompareReportResultView.h
# End Source File
# Begin Source File

SOURCE=.\CompareReportResView.h
# End Source File
# Begin Source File

SOURCE=.\CompareReportView.h
# End Source File
# Begin Source File

SOURCE=.\CompRepLogBar.h
# End Source File
# Begin Source File

SOURCE=.\ConDBExListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ConDBListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ConDBListCtrlWithCheckBox.h
# End Source File
# Begin Source File

SOURCE=.\ConWithIDDBListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\CoolBtn.h
# End Source File
# Begin Source File

SOURCE=.\CoolTree.h
# End Source File
# Begin Source File

SOURCE=.\CoolTreeEditSpin.h
# End Source File
# Begin Source File

SOURCE=.\CoolTreeEx.h
# End Source File
# Begin Source File

SOURCE=.\CtrlToolBar.h
# End Source File
# Begin Source File

SOURCE=.\DefaultDisplayProperties.h
# End Source File
# Begin Source File

SOURCE=.\DensityAnimView.h
# End Source File
# Begin Source File

SOURCE=.\DensityGLView.h
# End Source File
# Begin Source File

SOURCE=.\DFVCtrl.h
# End Source File
# Begin Source File

SOURCE=.\DFVDocTemplate.h
# End Source File
# Begin Source File

SOURCE=.\DFVDocument.h
# End Source File
# Begin Source File

SOURCE=.\DFVFrameWnd.h
# End Source File
# Begin Source File

SOURCE=.\DIB.H
# End Source File
# Begin Source File

SOURCE=.\DisplayPropOverrides.h
# End Source File
# Begin Source File

SOURCE=.\DlgActivityDensity.h
# End Source File
# Begin Source File

SOURCE=.\DlgACTypeProperties.h
# End Source File
# Begin Source File

SOURCE=.\DlgAirlineProperties.h
# End Source File
# Begin Source File

SOURCE=.\DlgAirportDBNameInput.h
# End Source File
# Begin Source File

SOURCE=.\DlgAirportProperties.h
# End Source File
# Begin Source File

SOURCE=.\DlgAnimationData.h
# End Source File
# Begin Source File

SOURCE=.\DlgAreaPortal.h
# End Source File
# Begin Source File

SOURCE=.\DlgArrayCopies.h
# End Source File
# Begin Source File

SOURCE=.\DlgAttrOfLink.h
# End Source File
# Begin Source File

SOURCE=.\DlgBookmarkedCameras.h
# End Source File
# Begin Source File

SOURCE=.\DlgComments.h
# End Source File
# Begin Source File

SOURCE=.\DlgDBAircraftTypes.h
# End Source File
# Begin Source File

SOURCE=.\DlgDBAirline.h
# End Source File
# Begin Source File

SOURCE=.\DlgDBAirports.h
# End Source File
# Begin Source File

SOURCE=.\DlgFireImpact.h
# End Source File
# Begin Source File

SOURCE=.\DlgFloorAdjust.h
# End Source File
# Begin Source File

SOURCE=.\DlgFloorThickness.h
# End Source File
# Begin Source File

SOURCE=.\DlgGetString.h
# End Source File
# Begin Source File

SOURCE=.\DlgLayerOptions.h
# End Source File
# Begin Source File

SOURCE=.\DlgLiveRecordParams.h
# End Source File
# Begin Source File

SOURCE=.\DlgMathSimSetting.h
# End Source File
# Begin Source File

SOURCE=.\DlgMovie.h
# End Source File
# Begin Source File

SOURCE=.\DlgMovieWriter.h
# End Source File
# Begin Source File

SOURCE=.\DlgPaxTags.h
# End Source File
# Begin Source File

SOURCE=.\DlgPickTime.h
# End Source File
# Begin Source File

SOURCE=.\DlgProbDist.h
# End Source File
# Begin Source File

SOURCE=.\DlgProbDistHistData.h
# End Source File
# Begin Source File

SOURCE=.\DlgProcDisplayProperties.h
# End Source File
# Begin Source File

SOURCE=.\DlgProcDispOverrides.h
# End Source File
# Begin Source File

SOURCE=.\DlgProcessorSelect.h
# End Source File
# Begin Source File

SOURCE=.\DlgProcessorTags.h
# End Source File
# Begin Source File

SOURCE=.\DlgProgress.h
# End Source File
# Begin Source File

SOURCE=.\DlgProjectName.h
# End Source File
# Begin Source File

SOURCE=.\DlgRecordOptions.h
# End Source File
# Begin Source File

SOURCE=.\DlgRetailPackFee.h
# End Source File
# Begin Source File

SOURCE=.\DlgSectorProperties.h
# End Source File
# Begin Source File

SOURCE=.\DlgShapeSelect.h
# End Source File
# Begin Source File

SOURCE=.\DlgSubairlineProperties.h
# End Source File
# Begin Source File

SOURCE=.\DlgTracerDensity.h
# End Source File
# Begin Source File

SOURCE=.\DlgVisibleRegions.h
# End Source File
# Begin Source File

SOURCE=.\DlgWalkthrough.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\DLW1.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\DLW2.h
# End Source File
# Begin Source File

SOURCE=.\DockBarEx.h
# End Source File
# Begin Source File

SOURCE=.\DotNetTabWnd.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\Draw.h
# End Source File
# Begin Source File

SOURCE=.\DWGFile.h
# End Source File
# Begin Source File

SOURCE=.\DynamicCreatedProcDlg.h
# End Source File
# Begin Source File

SOURCE=.\EconListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Economic2Dialog.h
# End Source File
# Begin Source File

SOURCE=.\EconomicaDataTypeDlg.h
# End Source File
# Begin Source File

SOURCE=.\economicdialog.h
# End Source File
# Begin Source File

SOURCE=.\EconRepDlg.h
# End Source File
# Begin Source File

SOURCE=.\ExceptionDlg.h
# End Source File
# Begin Source File

SOURCE=.\Fallback.h
# End Source File
# Begin Source File

SOURCE=.\FindDlg.h
# End Source File
# Begin Source File

SOURCE=.\FixedNode.h
# End Source File
# Begin Source File

SOURCE=.\FlightDialog.h
# End Source File
# Begin Source File

SOURCE=.\FlightGateSelection.h
# End Source File
# Begin Source File

SOURCE=.\FlightPriorityDlg.h
# End Source File
# Begin Source File

SOURCE=.\FlightScheduleAddNewDlg.h
# End Source File
# Begin Source File

SOURCE=.\FlightScheduleAddUtil.h
# End Source File
# Begin Source File

SOURCE=.\FlightScheduleDlg.h
# End Source File
# Begin Source File

SOURCE=.\Floor2.h
# End Source File
# Begin Source File

SOURCE=.\FloorLayer.h
# End Source File
# Begin Source File

SOURCE=.\FloorMapMatch.h
# End Source File
# Begin Source File

SOURCE=.\FloorPropertiesDlg.h
# End Source File
# Begin Source File

SOURCE=.\Floors.h
# End Source File
# Begin Source File

SOURCE=..\Engine\FLowChannelList.h
# End Source File
# Begin Source File

SOURCE=.\FlowChartPane.h
# End Source File
# Begin Source File

SOURCE=.\FlowChartWnd.h
# End Source File
# Begin Source File

SOURCE=.\FlowGeometryHelpers.h
# End Source File
# Begin Source File

SOURCE=.\FltPaxDataDlg.h
# End Source File
# Begin Source File

SOURCE=.\FltSchedDefineDlg.h
# End Source File
# Begin Source File

SOURCE=.\FrameCompareReport.h
# End Source File
# Begin Source File

SOURCE=.\FrameCompareReportResult.h
# End Source File
# Begin Source File

SOURCE=.\FrameCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FrameCtrlDlg.h
# End Source File
# Begin Source File

SOURCE=.\FrameInputData.h
# End Source File
# Begin Source File

SOURCE=.\FrameProcessDefine.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GAL1SIT.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GAL1W.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GAL1WB.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GAL2WB.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GAL3W.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GAL4W.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GAL5W.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GAL8W.h
# End Source File
# Begin Source File

SOURCE=.\GannChartFrameWnd.h
# End Source File
# Begin Source File

SOURCE=.\GannChartWnd.h
# End Source File
# Begin Source File

SOURCE=.\GannCommon.h
# End Source File
# Begin Source File

SOURCE=.\GannRowWnd.h
# End Source File
# Begin Source File

SOURCE=.\GanntChartOneLineWnd.h
# End Source File
# Begin Source File

SOURCE=.\gateassignctrl.h
# End Source File
# Begin Source File

SOURCE=.\GateAssignDlg.h
# End Source File
# Begin Source File

SOURCE=.\GateAssignmentDlg.h
# End Source File
# Begin Source File

SOURCE=.\GateAssignSummaryDlg.h
# End Source File
# Begin Source File

SOURCE=.\GateMapDlg.h
# End Source File
# Begin Source File

SOURCE=.\GatePriorityDlg.h
# End Source File
# Begin Source File

SOURCE=.\GateSelectionDlg.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\GfxGroupEdit.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\GfxOutBarCtrl.h
# End Source File
# Begin Source File

SOURCE=.\GlobalDBManDlg.h
# End Source File
# Begin Source File

SOURCE=.\GlobalDBSheet.h
# End Source File
# Begin Source File

SOURCE=.\gtchart.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GUY1SIT.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GUY1W.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GUY1WB.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GUY1WC.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GUY3W.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GUY4W.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\GUY5W.h
# End Source File
# Begin Source File

SOURCE=.\HeaderCtrlDrag.h
# End Source File
# Begin Source File

SOURCE=.\HubDataDlg.h
# End Source File
# Begin Source File

SOURCE=.\hypercam.h
# End Source File
# Begin Source File

SOURCE=.\IconComboBox.h
# End Source File
# Begin Source File

SOURCE=.\ImportFlightFromFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\ImportIntoNewProject.h
# End Source File
# Begin Source File

SOURCE=.\ImportIntoOldProjectDlg.h
# End Source File
# Begin Source File

SOURCE=.\INI.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceComboBox.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceComboBoxEx.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceCtrl.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceDateTimeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceEdit.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceEdit2.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceList.h
# End Source File
# Begin Source File

SOURCE=.\InputDataView.h
# End Source File
# Begin Source File

SOURCE=.\InputRepDlg.h
# End Source File
# Begin Source File

SOURCE=.\InputValueDlg.h
# End Source File
# Begin Source File

SOURCE=.\InterestOnCapitalDlg.h
# End Source File
# Begin Source File

SOURCE=.\KeyFrame.h
# End Source File
# Begin Source File

SOURCE=.\LayerListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\LayoutBar.h
# End Source File
# Begin Source File

SOURCE=.\LeadLagTimeDlg.h
# End Source File
# Begin Source File

SOURCE=.\LeafNode.h
# End Source File
# Begin Source File

SOURCE=.\LinkageDlg.h
# End Source File
# Begin Source File

SOURCE=.\ListBoxEx.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\ListBoxXP.h
# End Source File
# Begin Source File

SOURCE=.\ListCtrlDrag.h
# End Source File
# Begin Source File

SOURCE=.\ListCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\ListCtrlEx2.h
# End Source File
# Begin Source File

SOURCE=.\ListWnd.h
# End Source File
# Begin Source File

SOURCE=.\MacroCommand.h
# End Source File
# Begin Source File

SOURCE=.\MainBar.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MaintenanceDialog.h
# End Source File
# Begin Source File

SOURCE=.\MapFile.h
# End Source File
# Begin Source File

SOURCE=.\MathematicFrame.h
# End Source File
# Begin Source File

SOURCE=.\MathematicView.h
# End Source File
# Begin Source File

SOURCE=.\MathFlow.h
# End Source File
# Begin Source File

SOURCE=.\MathFlowDataSet.h
# End Source File
# Begin Source File

SOURCE=.\MathFlowDefineDlg.h
# End Source File
# Begin Source File

SOURCE=.\MathProcess.h
# End Source File
# Begin Source File

SOURCE=.\MathProcessDataSet.h
# End Source File
# Begin Source File

SOURCE=.\MathProcessDefine.h
# End Source File
# Begin Source File

SOURCE=.\MathProcessEngine.h
# End Source File
# Begin Source File

SOURCE=.\MathProcessEngineFactory.h
# End Source File
# Begin Source File

SOURCE=.\MathProcessEngineMGN.h
# End Source File
# Begin Source File

SOURCE=.\MathProcessEngineMMN.h
# End Source File
# Begin Source File

SOURCE=.\MathResultDataSet.h
# End Source File
# Begin Source File

SOURCE=.\MathResultManager.h
# End Source File
# Begin Source File

SOURCE=.\MathSimulation.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\MenuCH.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\MenuXP.h
# End Source File
# Begin Source File

SOURCE=..\Reports\MissFlightReport.h
# End Source File
# Begin Source File

SOURCE=.\MobileCountDlg.h
# End Source File
# Begin Source File

SOURCE=.\MoblieElemDlg.h
# End Source File
# Begin Source File

SOURCE=.\MoblieElemTips.h
# End Source File
# Begin Source File

SOURCE=.\ModelSelectionDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModifyLatLong.h
# End Source File
# Begin Source File

SOURCE=.\MoveProcessorsCommand.h
# End Source File
# Begin Source File

SOURCE=.\MoveSideWalkPaxDistDlg.h
# End Source File
# Begin Source File

SOURCE=.\MoveSideWalkPropDlg.h
# End Source File
# Begin Source File

SOURCE=.\MoveSideWalkSheet.h
# End Source File
# Begin Source File

SOURCE=.\Movie.h
# End Source File
# Begin Source File

SOURCE=.\Movies.h
# End Source File
# Begin Source File

SOURCE=.\MovieWriter.h
# End Source File
# Begin Source File

SOURCE=.\MovingSideWorkTree.h
# End Source File
# Begin Source File

SOURCE=.\mschart.h
# End Source File
# Begin Source File

SOURCE=.\MSVImpl.h
# End Source File
# Begin Source File

SOURCE=.\MuliSelTreeCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\MySpin.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\MyTrace.h
# End Source File
# Begin Source File

SOURCE=.\NameListMappingDlg.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\NewMenu.h
# End Source File
# Begin Source File

SOURCE=.\Node.h
# End Source File
# Begin Source File

SOURCE=.\NodeView.h
# End Source File
# Begin Source File

SOURCE=.\OpenComparativeReportsGroup.h
# End Source File
# Begin Source File

SOURCE=.\OpenDWGToolkit.h
# End Source File
# Begin Source File

SOURCE=.\OpenSqlDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\PassengerTypeDialog.h
# End Source File
# Begin Source File

SOURCE=.\PaxBulkEdit.h
# End Source File
# Begin Source File

SOURCE=.\PaxBulkListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\PaxBulkListDlg.h
# End Source File
# Begin Source File

SOURCE=.\PaxDispListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\PaxDispPropDlg.h
# End Source File
# Begin Source File

SOURCE=.\PaxDispPropItem.h
# End Source File
# Begin Source File

SOURCE=.\PaxDispProps.h
# End Source File
# Begin Source File

SOURCE=.\PaxDistDlg.h
# End Source File
# Begin Source File

SOURCE=.\PaxFilterDlg.h
# End Source File
# Begin Source File

SOURCE=.\PaxFlowAvoidDensityAddAreasDlg.h
# End Source File
# Begin Source File

SOURCE=.\PaxFlowAvoidDensityDlg.h
# End Source File
# Begin Source File

SOURCE=.\PaxFlowByProcDlg.h
# End Source File
# Begin Source File

SOURCE=.\PaxFlowDlg.h
# End Source File
# Begin Source File

SOURCE=.\PaxFlowSelectPipes.h
# End Source File
# Begin Source File

SOURCE=.\PaxFlowTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\PaxProbDistDlg.h
# End Source File
# Begin Source File

SOURCE=.\PaxShapeDefs.h
# End Source File
# Begin Source File

SOURCE=.\PAXShapes.h
# End Source File
# Begin Source File

SOURCE=.\PaxTags.h
# End Source File
# Begin Source File

SOURCE=.\PaxTrackDlg.h
# End Source File
# Begin Source File

SOURCE=.\PaxTransferDBDlg.h
# End Source File
# Begin Source File

SOURCE=.\PaxType.h
# End Source File
# Begin Source File

SOURCE=.\PaxTypeDefDlg.h
# End Source File
# Begin Source File

SOURCE=.\PBuffer.h
# End Source File
# Begin Source File

SOURCE=.\PickConveyorTree.h
# End Source File
# Begin Source File

SOURCE=.\PipeBar.h
# End Source File
# Begin Source File

SOURCE=.\PipeDefinitionDlg.h
# End Source File
# Begin Source File

SOURCE=.\PipeDropWnd.h
# End Source File
# Begin Source File

SOURCE=.\PipeShowListBox.h
# End Source File
# Begin Source File

SOURCE=.\PipeShowWnd.h
# End Source File
# Begin Source File

SOURCE=.\Placement.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\Popup.h
# End Source File
# Begin Source File

SOURCE=.\PrintableListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\PrintableTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ProbDistDlg.h
# End Source File
# Begin Source File

SOURCE=.\Proc2DataForClipBoard.h
# End Source File
# Begin Source File

SOURCE=.\ProcAssignDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcAssignEventDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcDataPage.h
# End Source File
# Begin Source File

SOURCE=.\ProcDataPageDepSink.h
# End Source File
# Begin Source File

SOURCE=.\ProcDataSheet.h
# End Source File
# Begin Source File

SOURCE=.\ProcessDefineDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcesserDialog.h
# End Source File
# Begin Source File

SOURCE=.\ProcessFlowView.h
# End Source File
# Begin Source File

SOURCE=.\Processor2.h
# End Source File
# Begin Source File

SOURCE=.\ProcessorCommandUtil.h
# End Source File
# Begin Source File

SOURCE=.\ProcessorTags.h
# End Source File
# Begin Source File

SOURCE=.\ProcessorTagWnd.h
# End Source File
# Begin Source File

SOURCE=.\ProcessorTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ProcPropDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcToResPoolDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProgressBar.h
# End Source File
# Begin Source File

SOURCE=.\ProjectBar.h
# End Source File
# Begin Source File

SOURCE=.\ProjectCommentDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProjectControlDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProjectSelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\PtLineChart.h
# End Source File
# Begin Source File

SOURCE=.\RecordedCameras.h
# End Source File
# Begin Source File

SOURCE=.\RecordTimerManager.h
# End Source File
# Begin Source File

SOURCE=.\RepControlView.h
# End Source File
# Begin Source File

SOURCE=.\RepGraphView.h
# End Source File
# Begin Source File

SOURCE=.\RepListView.h
# End Source File
# Begin Source File

SOURCE=.\ReportChildFrameSplit.h
# End Source File
# Begin Source File

SOURCE=.\ReportParamDlg.h
# End Source File
# Begin Source File

SOURCE=.\ReportParameter2.h
# End Source File
# Begin Source File

SOURCE=.\ReportProperty.h
# End Source File
# Begin Source File

SOURCE=.\ResDispListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ResDispPropDlg.h
# End Source File
# Begin Source File

SOURCE=.\ResizePS\ResizableGrip.h
# End Source File
# Begin Source File

SOURCE=.\ResizePS\ResizableLayout.h
# End Source File
# Begin Source File

SOURCE=.\ResizePS\ResizableMinMax.h
# End Source File
# Begin Source File

SOURCE=.\ResizePS\ResizablePage.h
# End Source File
# Begin Source File

SOURCE=.\ResizePS\ResizableSheet.h
# End Source File
# Begin Source File

SOURCE=.\ResizePS\ResizableState.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ResourceDispPropItem.h
# End Source File
# Begin Source File

SOURCE=.\ResourceDispProps.h
# End Source File
# Begin Source File

SOURCE=.\ResourcePoolDlg.h
# End Source File
# Begin Source File

SOURCE=.\RosterWizardDlg.h
# End Source File
# Begin Source File

SOURCE=.\RotateProcessorsCommand.h
# End Source File
# Begin Source File

SOURCE=.\RotateShapesCommand.h
# End Source File
# Begin Source File

SOURCE=.\RunwayTextures.h
# End Source File
# Begin Source File

SOURCE=.\ScaleProcessorsCommand.h
# End Source File
# Begin Source File

SOURCE=.\scbarg.h
# End Source File
# Begin Source File

SOURCE=.\ServiceTimeDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetProjectDBDlg.h
# End Source File
# Begin Source File

SOURCE=.\Shape.h
# End Source File
# Begin Source File

SOURCE=.\ShapesBar.h
# End Source File
# Begin Source File

SOURCE=.\ShapesBarPFlow.h
# End Source File
# Begin Source File

SOURCE=.\ShapesListBox.h
# End Source File
# Begin Source File

SOURCE=.\ShapesManager.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\SHM_WHL.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\SHMW1.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\SHMW2.h
# End Source File
# Begin Source File

SOURCE=.\SimEngineDlg.h
# End Source File
# Begin Source File

SOURCE=.\SimEngSettingDlg.h
# End Source File
# Begin Source File

SOURCE=.\SimTimeRangeDlg.h
# End Source File
# Begin Source File

SOURCE=.\singleEventsDataDlg.h
# End Source File
# Begin Source File

SOURCE=.\sizecbar.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\SL_WHL.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\SLW1.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\SLW2.h
# End Source File
# Begin Source File

SOURCE=.\SmartEdit.h
# End Source File
# Begin Source File

SOURCE=.\SortableHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SortHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SortListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SplitTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SplitterControl.h
# End Source File
# Begin Source File

SOURCE=.\SplitterWndEx.h
# End Source File
# Begin Source File

SOURCE=.\StationLinkerDlg.h
# End Source File
# Begin Source File

SOURCE=.\StationLinkerPopDlg.h
# End Source File
# Begin Source File

SOURCE=.\StationSheet.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\StatusBarXP.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\SUM_CRUT.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\SUMST1.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\SUMW1.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\SUMW2.h
# End Source File
# Begin Source File

SOURCE=.\PAXSHAPES\SUMW3.h
# End Source File
# Begin Source File

SOURCE=.\TempProcInfo.h
# End Source File
# Begin Source File

SOURCE=.\TempTracerData.h
# End Source File
# Begin Source File

SOURCE=.\TerminalMSVImpl.h
# End Source File
# Begin Source File

SOURCE=.\TermPlan.h
# End Source File
# Begin Source File

SOURCE=.\TermPlanDoc.h
# End Source File
# Begin Source File

SOURCE=.\TermPlanView.h
# End Source File
# Begin Source File

SOURCE=.\TessellationManager.h
# End Source File
# Begin Source File

SOURCE=.\tga.h
# End Source File
# Begin Source File

SOURCE=.\ThumbnailsList.h
# End Source File
# Begin Source File

SOURCE=.\TimeSelector.h
# End Source File
# Begin Source File

SOURCE=.\ToolBar24X24.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\ToolbarXP.h
# End Source File
# Begin Source File

SOURCE=.\ToolBarXP24.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\Tools.h
# End Source File
# Begin Source File

SOURCE=.\XPStyle\tooltip2.h
# End Source File
# Begin Source File

SOURCE=.\TrackWnd.h
# End Source File
# Begin Source File

SOURCE=.\TreeCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\TrueColorToolBar.h
# End Source File
# Begin Source File

SOURCE=.\TVN.h
# End Source File
# Begin Source File

SOURCE=.\TVNode.h
# End Source File
# Begin Source File

SOURCE=.\UndoDialog.h
# End Source File
# Begin Source File

SOURCE=.\UnitBar.h
# End Source File
# Begin Source File

SOURCE=.\UnitsManager.h
# End Source File
# Begin Source File

SOURCE=.\UnselectableListBox.h
# End Source File
# Begin Source File

SOURCE=.\UnselectableTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\UsedProcInfo.h
# End Source File
# Begin Source File

SOURCE=.\UsedProcInfoBox.h
# End Source File
# Begin Source File

SOURCE=.\UtilitiesDialog.h
# End Source File
# Begin Source File

SOURCE=.\Version.h
# End Source File
# Begin Source File

SOURCE=.\VideoParams.h
# End Source File
# Begin Source File

SOURCE=.\ViewBar.h
# End Source File
# Begin Source File

SOURCE=.\Walkthrough.h
# End Source File
# Begin Source File

SOURCE=.\Walkthroughs.h
# End Source File
# Begin Source File

SOURCE=.\XListCtrl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RES\747ICON.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\add.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\adddel_p.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\adddeled.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\aircraftcat_filter.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\all_simu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\altitude.cur
# End Source File
# Begin Source File

SOURCE=.\Res\animtool.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\animtool_G.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\animtool_W.bmp
# End Source File
# Begin Source File

SOURCE=".\RES\animtoolbar_delta+run.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\animtoolbar_delta+run_G.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\animtoolbar_delta+run_W.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\ARC-Logo.bmp"
# End Source File
# Begin Source File

SOURCE=.\Res\areasSmallIMG.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\arrowLtoR.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ATTACHED.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\BAGICON.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bitmap5.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bitmap_s.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\bmp00004.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\bmp00005.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\bmp00006.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00007.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00008.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\bmp00009.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\bmp00010.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00011.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00012.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00013.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00014.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp_seldefined.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmpPipeBar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cameraLargeIMG.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cameraSmallIMG.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cameratoolbar24.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cameratoolbar24_G.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cameratoolbar24_W.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\camtool.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\camtool_G.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\camtool_W.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\carschedul_.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cbd.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cbu.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\CFIDSChildView.ico
# End Source File
# Begin Source File

SOURCE=.\Res\CFIDSFrameView.ico
# End Source File
# Begin Source File

SOURCE=.\Res\check_bo.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\color2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ComparativeChart.bmp_d.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ComparativeChart.bmp_h.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ComparativeChart_n.bmp.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ComparativeReport.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ComparativeReport_d.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ComparativeReport_h.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ComparativeReport_n.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ComparativeText_d.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ComparativeText_h.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ComparativeText_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Controls_Altitude.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Controls_BackGround.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Controls_HitTest.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Controls_Move.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Controls_PitchRoll.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Controls_Turn.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cooltree.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\RES\dash1.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\dash2.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\dash3.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\database.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\delete.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\disable_.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\dot1.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\dot2.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\dot3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\DownArrow.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\dragging.cur
# End Source File
# Begin Source File

SOURCE=.\Res\edit.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\element_.ico
# End Source File
# Begin Source File

SOURCE=.\Res\emptynode.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Expirical.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Exponential.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\export.bmp
# End Source File
# Begin Source File

SOURCE=".\Res\fang .bmp"
# End Source File
# Begin Source File

SOURCE=.\RES\femaleperson_shape.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\fire96.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\firetoolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\firetoolbar_G.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\firetoolbar_W.bmp
# End Source File
# Begin Source File

SOURCE=.\res\floorLargeIMG.bmp
# End Source File
# Begin Source File

SOURCE=.\res\floorSmallIMG.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\floortoolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\floortoolbar_G.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\floortoolbar_W.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\globaldbman.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\GRAPH08.ICO
# End Source File
# Begin Source File

SOURCE=.\Res\guyshape.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\hideactagstoolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\hideactagstoolbar_G.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\hideactagstoolbar_W.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\hideproctagstoolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\hideproctagstoolbar_G.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\hideproctagstoolbar_W.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Histogram.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_down.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_up.ico
# End Source File
# Begin Source File

SOURCE=.\res\icondur.ico
# End Source File
# Begin Source File

SOURCE=.\Res\icr_hand.cur
# End Source File
# Begin Source File

SOURCE=.\Res\idr_comp.ico
# End Source File
# Begin Source File

SOURCE=.\Res\imageList.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\import.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\in.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\infotoolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\infotoolbar_G.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\infotoolbar_W.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\JAGUARICON.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\keyframe.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\layouttool.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\layouttool_G.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\layouttool_W.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\layoutToolBar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\layoutToolBar_G.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\layoutToolBar_W.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\linkagebar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\listctrltoolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\listimag.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\main.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\maleperson_shape.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\marker.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\MDICHILD.ICO
# End Source File
# Begin Source File

SOURCE=.\Res\mob.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\mobile_e.bmp
# End Source File
# Begin Source File

SOURCE=.\res\move.cur
# End Source File
# Begin Source File

SOURCE=.\res\movex.cur
# End Source File
# Begin Source File

SOURCE=.\res\movey.cur
# End Source File
# Begin Source File

SOURCE=.\RES\movietb.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\multicol.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\newdeled.bmp
# End Source File
# Begin Source File

SOURCE=.\res\nodeimag.bmp
# End Source File
# Begin Source File

SOURCE=.\res\nodeimages.bmp
# End Source File
# Begin Source File

SOURCE=.\res\nodeimagesRGB.bmp
# End Source File
# Begin Source File

SOURCE=.\res\nodeimg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\nodesmallimagesRGB.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\nodraggi.cur
# End Source File
# Begin Source File

SOURCE=.\Res\norcdf.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\norcha.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\NOTE12.ICO
# End Source File
# Begin Source File

SOURCE=.\Res\operate.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\out.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\paxbulk.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\paxdist.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\pdparamt.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\peoplemove.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\pickspeed.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\pipe.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\pipeshow.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\pipetool.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\pipetool_G.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\pipetool_W.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Plane.ico
# End Source File
# Begin Source File

SOURCE=.\Res\portalsSmallIMG.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\PRINT.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\print2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\procassi.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\process1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\process_.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\reportto.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\save.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Scmax.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Scmin.bmp
# End Source File
# Begin Source File

SOURCE=.\sdi.ico
# End Source File
# Begin Source File

SOURCE=.\Res\shapesba.bmp
# End Source File
# Begin Source File

SOURCE=.\res\smallnod.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\solid1.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\solid2.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\solid3.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\speedtool.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\speedtool2.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\speedtool2_G.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\speedtool2_W.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\speedtool_G.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\speedtool_W.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\speedtoolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\speedtoolbar_G.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\speedtoolbar_W.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\stationlinker.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\SUMW1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\systool.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\systool2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\systool2_G.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\systool2_W.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\systool_G.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\systool_W.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TermPlan.ico
# End Source File
# Begin Source File

SOURCE=.\TermPlan.rc
# End Source File
# Begin Source File

SOURCE=.\TermPlan.rc2
# End Source File
# Begin Source File

SOURCE=.\res\TermPlanDoc.ico
# End Source File
# Begin Source File

SOURCE=.\RES\test.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar2.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\toolbar_.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\toolbar_passenger_flow.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\tra_n1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\TraN.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\treeImageList.bmp
# End Source File
# Begin Source File

SOURCE=.\res\turn.cur
# End Source File
# Begin Source File

SOURCE=.\Res\ucprocSmallIMG.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Uniform.bmp
# End Source File
# Begin Source File

SOURCE=.\res\UpArrow.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\yuan.bmp
# End Source File
# Begin Source File

SOURCE=".\Res\zhui ti.bmp"
# End Source File
# Begin Source File

SOURCE=.\Res\zoomin.ico
# End Source File
# Begin Source File

SOURCE=.\Res\zoominoutbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\zoomout.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\TermPlan.reg
# End Source File
# End Target
# End Project
# Section TermPlan : {8425B91B-6246-4AC3-BF9A-D9D5B078FE19}
# 	2:21:DefaultSinkHeaderFile:gateassignctrl.h
# 	2:16:DefaultSinkClass:CGateAssign
# End Section
# Section TermPlan : {3F212D37-7C53-4467-AFD9-F494EE79857D}
# 	2:5:Class:CGTChart
# 	2:10:HeaderFile:gtchart.h
# 	2:8:ImplFile:gtchart.cpp
# End Section
# Section TermPlan : {229AB93B-612C-401F-8254-3D38A14769D8}
# 	2:21:DefaultSinkHeaderFile:gtchart.h
# 	2:16:DefaultSinkClass:CGTChart
# End Section
