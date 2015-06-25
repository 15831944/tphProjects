#include "StdAfx.h"
#include "resource.h"
#include ".\replistviewsinglereportoperator.h"
#include "ARCReportManager.h"
#include "TermPlanDoc.h"
#include "../Reports/ReportParameter.h"
#include "../common/UnitsManager.h"
#include "../MFCExControl/SortableHeaderCtrl.h"
#include "RepListView.h"
#include "../MFCExControl/ARCGridCtrl.h"
#include "Common/SimAndReportManager.h"


#define NO_THRESHOLD 0
#define INT_THRESHOLD 1
#define FLOAT_THRESHOLD 2
#define TIME_THRESHOLD 3

#define MAX_NUM_COLUMN  17

CRepListViewSingleReportOperator::CRepListViewSingleReportOperator( CListCtrl *pListCtrl, CARCReportManager* pARCReportManager,CRepListView *pListView )
:CRepListViewBaseOperator(pListCtrl,pARCReportManager,pListView)
{
	m_nColumnCount = 0;
	m_sExtension = _T("");
}
CRepListViewSingleReportOperator::~CRepListViewSingleReportOperator(void)
{
}

void CRepListViewSingleReportOperator::LoadReport(LPARAM lHint, CObject* pHint)
{
	if(GetARCReportManager() == NULL)
		return;
	GetFileReportTypeAndExtension();
	LoadListData();
}

CARCReportManager* CRepListViewSingleReportOperator::GetARCReportManager()
{
	return m_pReportManager;
}

void CRepListViewSingleReportOperator::GetFileReportTypeAndExtension()
{

	switch(GetARCReportManager()->GetReportType())
	{
	case ENUM_BAGDELIVTIME_REP:
		m_nReportFileType = BagDeliveryTimeReportFile;
		m_sExtension = "bdt";	
		break;

	case ENUM_BAGDISTRIBUTION_REP:
		m_nReportFileType = BagDistReportFile;
		m_sExtension = "bdr";
		break;

	case ENUM_ACOPERATION_REP:
		m_nReportFileType = AcOperationsReportFile;
		m_sExtension = "aor";
		break;
	case ENUM_PAXLOG_REP:
		m_nReportFileType=PaxLogReportFile;
		m_sExtension = "pld";
		break;

	case ENUM_UTILIZATION_REP:
		m_nReportFileType=UtilizationReportFile;
		m_sExtension = "utd";
		break;
	case ENUM_DISTANCE_REP:
		m_nReportFileType=DistanceReportFile;
		m_sExtension = "dsd";
		break;
	case ENUM_PAXCOUNT_REP:
		m_nReportFileType=PassengerCountReportFile;
		m_sExtension = "pcd";
		break;
	case ENUM_QUEUETIME_REP:
		m_nReportFileType=QueueTimeReportFile;
		m_sExtension = "qtd";
		break;
	case ENUM_CONVEYOR_WAIT_TIME_REP:
		m_nReportFileType=ConveyorWaitTimeReport;
		m_sExtension = "cwtd";
		break;
	case ENUM_THROUGHPUT_REP:
		m_nReportFileType=ThroughputReportFile;
		m_sExtension = "thd";
		break;
	case ENUM_QUEUELENGTH_REP:
		m_nReportFileType=QueueLengthReportFile;
		m_sExtension = "qld";
		break;
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		m_nReportFileType=ConveyorWaitLengthReport;
		m_sExtension = "cwld";
		break;

	case ENUM_AVGQUEUELENGTH_REP:
		m_nReportFileType=AverageQueueLengthReportFile;
		m_sExtension = "qad";
		break;

	case ENUM_DURATION_REP:
		m_nReportFileType=DurationReportFile;
		m_sExtension = "drd";
		break;
	case ENUM_SERVICETIME_REP:
		m_nReportFileType=ServiceTimeReportFile;
		m_sExtension = "srd";
		break;
	case ENUM_ACTIVEBKDOWN_REP:
		m_nReportFileType=ActivityBreakdownReportFile;
		m_sExtension = "acd";
		break;
	case ENUM_BAGWAITTIME_REP:
		m_nReportFileType=BagWaitTimeReportFile;
		m_sExtension = "bgd";
		break;
	case ENUM_BAGCOUNT_REP:
		m_nReportFileType=BagCountReportFile;
		m_sExtension = "bcd";
		break;
	case ENUM_PAXDENS_REP:			//not defined in the "Filelist", temp extension used: "txt"
		m_nReportFileType=PassengerDensityReportFile;
		m_sExtension = "pdd";
		break;
	case ENUM_SPACETHROUGHPUT_REP:
		m_nReportFileType = SpaceThroughputReportFile;
		m_sExtension = "std";
		break;
	case ENUM_COLLISIONS_REP:
		m_nReportFileType = CollisionReportFile;
		m_sExtension = "crf";
		break;
	case ENUM_FIREEVACUTION_REP:
		m_nReportFileType = FireEvacuationReportFile;
		m_sExtension = "fir";
		break;
	case ENUM_MISSFLIGHT_REP:
		m_nReportFileType = MissFlightReportFile;
		m_sExtension = "missflt";
		break;
	case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
		m_nReportFileType = BillboardExposureIncidenceFile;
		m_sExtension = "bil";
		break;
	case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
		m_nReportFileType = BillboardLinkedProcIncrementlVisitFile;
		m_sExtension = "bil";
		break;
	case ENUM_RETAIL_REP:
		m_nReportFileType = RetailReportFile;
		m_sExtension = "ret";
		break;
	}
}

void CRepListViewSingleReportOperator::LoadListData()
{
	CWaitCursor wCursor;
	char str[1024] = {0} ;
	//PROJMANAGER->getReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path, m_nReportFileType, str );
	CString sFileName;
	if( GetARCReportManager()->GetLoadReportType() == load_by_user )
	{
		sFileName = GetARCReportManager()->GetUserLoadReportFile();
	}
	else	// load_by_syetem
	{
		sFileName = GetTermPlanDoc()->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( GetTermPlanDoc()->m_ProjInfo.path );
	}

	CSVFile csvFile( sFileName, READ );
	// title should be on second line

	char title[128];
	if (csvFile.getLine ())
		csvFile.getField( title, 128 );
	
	ResetAllContent();
	GetListCtrl().DeleteAllItems();
	GetListCtrl().SetRedraw( FALSE );
	SetListHeader();


	// skip column headings line
	csvFile.skipLine ();
	int nDBIdx = 0;
	double dLength;
	m_vFlag.clear();
	while (csvFile.getLine () != -1 && !csvFile.eof())
	{
		csvFile.getField( str, 128 );
		CString strIndex;
		strIndex.Format(_T("%d"),nDBIdx+1);
		GetListCtrl().InsertItem(nDBIdx,strIndex);
		GetListCtrl().SetItemText(nDBIdx,1,str);
		//GetListCtrl().InsertItem( nDBIdx, str );
		int i;	
		//********************************************************
		//Modified by Luo Xiaobo 2002.4.29
		CTermPlanDoc * pDoc=GetTermPlanDoc();
		ENUM_REPORT_TYPE enumReportType = pDoc->GetARCReportManager().GetReportType();
		CUnitsManager* pUM = CUnitsManager::GetInstance();

		if( enumReportType == ENUM_PAXLOG_REP )
		{
			CString ColStr;
			for( i=2; i<7; i++ )
			{
				csvFile.getField( str, 128 );
				if( 2 == i || 3 == i)
				{
					ColStr = GetAbsDateTime(str);
					GetListCtrl().SetItemText( nDBIdx, i,  ColStr );
				}
				else
					GetListCtrl().SetItemText( nDBIdx, i,  str );
			}
			float fSpeed;
			csvFile.getFloat( fSpeed );
			fSpeed = static_cast<float>(UNITSMANAGER->ConvertLength( (double)fSpeed ));
			sprintf( str, "%.2f", fSpeed );
			GetListCtrl().SetItemText( nDBIdx, 7,  str );
			csvFile.getField( str, 128 );
			GetListCtrl().SetItemText( nDBIdx, 8,  str );


		}
		else if( enumReportType == ENUM_DISTANCE_REP )
		{
			//CReportSpecs *prepSpecs=((CTermPlanDoc *)GetDocument())->m_prepSpecs;	
			int iDetailed;
			GetReportParameter()->GetReportType( iDetailed );
			if( iDetailed != 0 ) //Summary
			{
				csvFile.getFloat( dLength );
				dLength = pUM->ConvertLength( dLength * SCALE_FACTOR );
				sprintf( str,"%d",(int)dLength );
				GetListCtrl().SetItemText( nDBIdx,2,str );
				csvFile.getFloat( dLength );
				dLength = pUM->ConvertLength( dLength * SCALE_FACTOR );
				sprintf( str,"%d",(int)dLength );
				GetListCtrl().SetItemText( nDBIdx,3,str );
				csvFile.getFloat( dLength );
				dLength = pUM->ConvertLength( dLength * SCALE_FACTOR );
				sprintf( str,"%d",(int)dLength );
				GetListCtrl().SetItemText( nDBIdx,4,str );
				csvFile.getField( str,128 );
				GetListCtrl().SetItemText( nDBIdx,5,str );

				for( int i=6; i<=15; ++i )
				{
					csvFile.getFloat( dLength );
					dLength = pUM->ConvertLength( dLength * SCALE_FACTOR );
					sprintf( str,"%d",(int)dLength );
					GetListCtrl().SetItemText( nDBIdx,i,str );
				}				
			}
			else   //Detailed
			{
				csvFile.getFloat( dLength );
				dLength = pUM->ConvertLength( dLength * SCALE_FACTOR);
				sprintf( str,"%d",(int)dLength );
				GetListCtrl().SetItemText( nDBIdx,2,str );
				csvFile.getField( str,128 );
				GetListCtrl().SetItemText( nDBIdx,3,str );
				csvFile.getField( str, 128 );
				GetListCtrl().SetItemText( nDBIdx,4,str );
				csvFile.getField( str, 128 );
				GetListCtrl().SetItemText( nDBIdx,5,str );
			}
		}
		else if (enumReportType == ENUM_RETAIL_REP)
		{
			int iDetailed;
			GetReportParameter()->GetReportType( iDetailed );
			if (0 == iDetailed)//Detail
			{
				int nValue = 0;
				csvFile.getInteger(nValue);
				if (nValue)
					sprintf( str,"yes");
				else
					sprintf( str,"bypass");
				GetListCtrl().SetItemText( nDBIdx,2,str );
			
				if (nValue)
				{
					csvFile.getField( str, 128 );
					GetListCtrl().SetItemText( nDBIdx,3,str );
					//enter store time
					CString ColStr;
					csvFile.getField( str, 128 );
					ElapsedTime eEnterStore;
					eEnterStore.SetTime(str);
					ColStr = GetAbsDateTime(str,TRUE);
					GetListCtrl().SetItemText( nDBIdx,4,ColStr );

					//leave store time
					csvFile.getField( str, 128 );
					ElapsedTime eLeaveStore;
					eLeaveStore.SetTime(str);
					ColStr = GetAbsDateTime(str,TRUE);
					GetListCtrl().SetItemText( nDBIdx,5,ColStr );

					//Time in store
					ElapsedTime eStoreTime;
					eStoreTime = eLeaveStore - eEnterStore;
					CString sDurationTime;
					sDurationTime.Format(_T("%02d:%02d"),eStoreTime.GetMinute(),eStoreTime.GetSecond());
					GetListCtrl().SetItemText(nDBIdx,6,sDurationTime);

					//potentially bought
					csvFile.getInteger(nValue);
					sprintf(str,"%d",nValue);
					GetListCtrl().SetItemText( nDBIdx,7,str );

					//check out
					csvFile.getInteger(nValue);
					if (nValue)
						sprintf( str,"yes");
					else
						sprintf( str,"no");

					GetListCtrl().SetItemText( nDBIdx,8,str );

					//enter queue time
					csvFile.getField( str, 128 );
					ColStr = GetAbsDateTime(str,TRUE);
					GetListCtrl().SetItemText( nDBIdx,9,ColStr );

					//check out queue time
					csvFile.getField( str, 128 );
					ElapsedTime eCheckOutQueueTime;
					eCheckOutQueueTime.SetTime(str);
					CString sCheckOutQueueTime;
					sCheckOutQueueTime.Format(_T("%02d:%02d"),eCheckOutQueueTime.GetMinute(),eCheckOutQueueTime.GetSecond());
					GetListCtrl().SetItemText(nDBIdx,10,sCheckOutQueueTime);

					//time in service
					csvFile.getField( str, 128 );
					ElapsedTime eServiceTime;
					eServiceTime.SetTime(str);
					CString sServiceTime;
					sServiceTime.Format(_T("%02d:%02d"),eServiceTime.GetMinute(),eServiceTime.GetSecond());
					GetListCtrl().SetItemText(nDBIdx,11,sServiceTime);
					
					//sales
					csvFile.getFloat(dLength);
					sprintf( str,"%.2f",dLength );
					GetListCtrl().SetItemText( nDBIdx,12,str );

					//apt
					csvFile.getFloat(dLength);
					sprintf( str,"%.2f",dLength );
					GetListCtrl().SetItemText( nDBIdx,13,str );
					
					//inventory
// 					csvFile.getInteger(nValue);
// 					if (nValue)
// 						sprintf( str,"yes");
// 					else
// 						sprintf( str,"no");
// 					GetListCtrl().SetItemText( nDBIdx,14,str );

					csvFile.getField(str,1024);
					GetListCtrl().SetItemText( nDBIdx,14,str );

					csvFile.getField(str,1024);
					GetListCtrl().SetItemText( nDBIdx,15,str );

				}
			}
			else //Summary
			{
				//store
				csvFile.getField( str, 128 );
				GetListCtrl().SetItemText( nDBIdx,2,str );

				for (int i = 0; i < 14; i++)
				{
					csvFile.getFloat(dLength);
					sprintf(str,"%.2f",dLength);
					GetListCtrl().SetItemText( nDBIdx,i + 3,str );
				}
			}
		}
		else // in the follow statement,should concern the detail and summary model
		{	 //maybe ,the row of the models are not the same in the same position
			//like the following report type ENUM_THROUGHPUT_REP
			for( i=2; i<m_nColumnCount+1; i++ )
			{
				csvFile.getField( str, 128 );				

				//if( (enumReportType == ENUM_ACOPERATION_REP)&& (5 == i || 9 == i || 12 == i) )
				//{
				//	CString strTemp = GetAbsDateTime(str);
				//	strcpy(str, strTemp.GetBuffer(0));
				//}

				if( (enumReportType == ENUM_MISSFLIGHT_REP) && ( 5 == i) )
				{
					CString strTemp = GetAbsDateTime(str);
					strcpy(str, strTemp.GetBuffer(0));
				}

				if ( (enumReportType == ENUM_THROUGHPUT_REP) &&	( (i == 2) || (i == 3) ))
				{
					int iDetailed=-1;
					GetReportParameter()->GetReportType( iDetailed );
					if (iDetailed == 0)
					{
						CString strTemp = GetAbsDateTime(str, FALSE);
						strcpy(str, strTemp.GetBuffer(0));
					}
				}
				else if ( enumReportType == ENUM_CONVEYOR_WAIT_LENGTH_REP)	
				{
					int iDetailed=-1;
					GetReportParameter()->GetReportType( iDetailed );
					if (iDetailed == 0)
					{
						if (i == 1)
						{
							CString strTemp = GetAbsDateTime(str, FALSE);
							strcpy(str, strTemp.GetBuffer(0));
						}
					}
					else
					{
						if ( i == 3)
						{
							CString strTemp = GetAbsDateTime(str, FALSE);
							strcpy(str, strTemp.GetBuffer(0));
						}
					}
				}
				else if ((enumReportType == ENUM_QUEUELENGTH_REP)&& ( i == 3 ))
				{
					int iDetailed=-1;
					GetReportParameter()->GetReportType( iDetailed );
					if (iDetailed == 1)
					{
						CString strTemp = GetAbsDateTime(str, FALSE);
						strcpy(str, strTemp.GetBuffer(0));
					}
				}
				if ( (enumReportType == ENUM_PAXDENS_REP) && (i == 2))
				{
					CString strTemp = GetAbsDateTime(str, FALSE);
					strcpy(str, strTemp.GetBuffer(0));
				}

				if ( (enumReportType == ENUM_SPACETHROUGHPUT_REP) &&
					( (i == 2) || (i == 3) ) )
				{
					CString strTemp = GetAbsDateTime(str, FALSE);
					strcpy(str, strTemp.GetBuffer(0));
				}
				//if (enumReportType == ENUM_UTILIZATION_REP && i == 7 )
				//{
				//	double dPercent = atof(str);
				//	dPercent =dPercent*100;
				//	sprintf(str,"%.0f",dPercent);
				//}
				//acoperation report load factor need .2f 
				if (enumReportType == ENUM_ACOPERATION_REP && i == 8)
				{
					double dPercent = atof(str);
					dPercent =dPercent;
					sprintf(str,"%.2f",dPercent);
				}

				GetListCtrl().SetItemText( nDBIdx, i,  str );
			}
		}

		//********************************************************
		//Modified by Xie Bo 2002.4.19
		//Setting Color because of threshold 
		//		CTermPlanDoc * pDoc=(CTermPlanDoc *)GetDocument();
		//		CReportManager repManager=pDoc->m_repManager;
		//		ENUM_REPORT_TYPE enumReportType= repManager.GetReportType();
		int thresholdField,thresholdType;
		float value;
		ElapsedTime valuetime;
		char temp[20];

		bool bFlag = false;
		if( enumReportType != ENUM_PAXLOG_REP && enumReportType != ENUM_UTILIZATION_REP &&
			enumReportType != ENUM_ACTIVEBKDOWN_REP && enumReportType != ENUM_PAXDENS_REP )
		{
			switch(enumReportType)
			{
			case ENUM_DISTANCE_REP:
				thresholdType=FLOAT_THRESHOLD;
				int iDetailed;
				GetReportParameter()->GetReportType( iDetailed );
				if( iDetailed ==0)//detailed
				{
					thresholdField=1;
				}
				else
				{
					thresholdField=3;
				}

				long lThreahold;				
				if(GetReportParameter()->GetThreshold( lThreahold ))
				{
					csvFile.setToField(thresholdField);
					csvFile.getFloat(value);
					value = static_cast<float>(pUM->ConvertLength( value * SCALE_FACTOR ));
					if (value >= lThreahold)
						bFlag = true;
				}
				break;
			case ENUM_AVGQUEUELENGTH_REP:

				GetReportParameter()->GetReportType( iDetailed );

				if(iDetailed==0)//detailed
				{
					thresholdField=2;
				}
				else
				{
					thresholdField=3;
				}

				if(GetReportParameter()->GetThreshold( lThreahold ))
				{
					csvFile.setToField(thresholdField);
					csvFile.getInteger(i);
					value=static_cast<float>(i);
					if (value >= lThreahold)
					{
						bFlag = true;
					}
				}
				break;
			case ENUM_QUEUELENGTH_REP:
			case ENUM_BAGCOUNT_REP:
			case ENUM_CONVEYOR_WAIT_LENGTH_REP:

				GetReportParameter()->GetReportType( iDetailed );
				if(iDetailed==0)//detailed
				{
					thresholdField=2;
				}
				else
				{
					thresholdField=5;
				}

				if(GetReportParameter()->GetThreshold( lThreahold ))
				{
					csvFile.setToField(thresholdField);
					csvFile.getInteger(i);
					value=static_cast<float>(i);
					if (value >= lThreahold)
					{
						bFlag = true;
					}
				}
				break;
			case ENUM_THROUGHPUT_REP:
				GetReportParameter()->GetReportType( iDetailed );
				if(iDetailed==0)//detailed
				{
					thresholdField=2;
				}
				else
				{
					thresholdField=4;
				}

				if(GetReportParameter()->GetThreshold( lThreahold ))
				{
					csvFile.setToField(thresholdField);
					csvFile.getFloat(value);
					if (value >= lThreahold)
						bFlag = true;
				}
				break;
			case ENUM_PAXCOUNT_REP:

				GetReportParameter()->GetReportType( iDetailed );
				if(iDetailed==0)//detailed
				{
					thresholdField=2;
				}
				else
				{
					thresholdField=4;
				}

				if(GetReportParameter()->GetThreshold( lThreahold ))
				{
					csvFile.setToField(thresholdField);
					csvFile.getInteger(i);
					value=static_cast<float>(i);
					if (value >= lThreahold)
						bFlag = true;
				}
				break;

				//Threshold type is Time
			case ENUM_BAGWAITTIME_REP:
			case ENUM_QUEUETIME_REP:
			case ENUM_CONVEYOR_WAIT_TIME_REP:
				GetReportParameter()->GetReportType( iDetailed );
				if(iDetailed==0)//detailed
				{
					thresholdField=2;
				}
				else
				{
					thresholdField=3;
				}

				if(GetReportParameter()->GetThreshold( lThreahold ))
				{
					csvFile.setToField(thresholdField);
					csvFile.getField(temp, 19);
					valuetime.SetTime (temp);
					if (valuetime.asSeconds() >= lThreahold)
						bFlag = true;
				}
				break;
			case ENUM_DURATION_REP:
			case ENUM_SERVICETIME_REP:

				GetReportParameter()->GetReportType( iDetailed );
				if(iDetailed==0)//detailed
				{
					thresholdField=1;
				}
				else
				{
					thresholdField=3;
				}

				if(GetReportParameter()->GetThreshold( lThreahold ))
				{
					csvFile.setToField(thresholdField);
					csvFile.getField(temp, 19);
					valuetime.SetTime (temp);
					if (valuetime.asSeconds() >= lThreahold)
						bFlag = true;
				}
				break;

			case ENUM_FIREEVACUTION_REP:
				GetReportParameter()->GetReportType( iDetailed );
				if( iDetailed == 0)	// detailed
					thresholdField = 4;
				else
					thresholdField = 2;
				if( GetReportParameter()->GetThreshold( lThreahold ))
				{
					csvFile.setToField( thresholdField );
					csvFile.getField(temp,19);
					valuetime.SetTime( temp );
					if( valuetime.asMinutes() >= lThreahold )
						bFlag = true;
				}
				break;
			}	
		}

		m_vFlag.push_back( bFlag );

		//****************************************************************

		GetListCtrl().SetItemData( nDBIdx, nDBIdx );
		nDBIdx++;
	}

	//GetListCtrl().SetFlags(m_vFlag);
	GetListCtrl().SetRedraw( TRUE );
	Invalidate();
	if(m_pListView)
		m_pListView->UpdateWindow();

	if (strlen (title) && csvFile.getLine () != -1)
	{
		strcat (title, ": ");
		csvFile.copyLine (title + strlen (title));
	}
	
	if(GetListCtrl().GetItemCount()==0)
	{	
		m_pReportManager->SetShowGraph(FALSE);
	}
	else
	{
		m_pReportManager->SetShowGraph(TRUE);
	}

	csvFile.closeOut();
}

CString CRepListViewSingleReportOperator::GetAbsDateTime( LPCTSTR elaptimestr, BOOL needsec /*= TRUE*/ )
{
	CSimAndReportManager *ptSim = GetTermPlanDoc()->GetTerminal().GetSimReportManager();
	CStartDate tstartdate = ptSim->GetStartDate();
	ElapsedTime etime;
	etime.SetTime(elaptimestr);

	CString retstr, tstr;
	bool bAbsDate;
	COleDateTime _tdate, _ttime;
	int _nDtIdx;
	tstartdate.GetDateTime(etime, bAbsDate, _tdate, _nDtIdx, _ttime);
	if(!needsec)
		tstr = _ttime.Format(" %H:%M");
	else
		tstr = _ttime.Format(" %H:%M:%S");
	if(bAbsDate)
	{
		retstr = _tdate.Format("%Y-%m-%d");
	}
	else
	{
		retstr.Format("Day%d", _nDtIdx + 1 );
	}
	retstr += tstr;

	return retstr;
}

void CRepListViewSingleReportOperator::SetListHeader()
{


	CString csColumnLabel[MAX_NUM_COLUMN];
	int nDefaultColumnWidth[MAX_NUM_COLUMN];
	int nFormat[MAX_NUM_COLUMN];
	EDataType type[MAX_NUM_COLUMN];
	// remove all the columns for the previous view.

	int nHeaderCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for( int i=0; i<nHeaderCount; i++ )
	{
		GetListCtrl().DeleteColumn( 0 );
	}

	//	CReportSpecs *prepSpecs=((CTermPlanDoc *)GetDocument())->m_prepSpecs;	
	int i = 0;
	int iDetailed=-1;
	GetReportParameter()->GetReportType( iDetailed );

	switch( m_nReportFileType )
	{
	case MissFlightReportFile:
		{
			m_nColumnCount = 7;
			csColumnLabel[0] = CString("");
			csColumnLabel[1] = CString("Passenger ID");
			csColumnLabel[2] = CString("Flight ID");
			csColumnLabel[3] = CString("Last Processor");
			csColumnLabel[4] = CString("Time");
			csColumnLabel[5] = CString("Dep Time");
			csColumnLabel[6] = CString("Passenger Type");

			nDefaultColumnWidth[0] = 30;
			nDefaultColumnWidth[1] = 70;
			nDefaultColumnWidth[2] = 70;
			nDefaultColumnWidth[3] = 70;
			nDefaultColumnWidth[4] = 70;
			nDefaultColumnWidth[5] = 70;
			nDefaultColumnWidth[6] = 70;

			for( int i=0; i<6; i++ )
			{
				nFormat[i] = LVCFMT_CENTER; 
			}
			nFormat[3] = LVCFMT_LEFT;
			nFormat[6] = LVCFMT_LEFT;
			
			type[0] = dtINT;
			type[1] = dtSTRING;
			type[2] = dtSTRING;
			type[3] = dtSTRING;
			type[4] = dtSTRING;
			type[5] = dtINT;
			type[6] = dtSTRING;

			break;
		}
	case BagDeliveryTimeReportFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 5;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Bag #");
				csColumnLabel[2] = CString("Pax #");
				csColumnLabel[3] = CString("Passenger Type");
				csColumnLabel[4] = CString("Delivery Time");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 70;
				nDefaultColumnWidth[2] = 70;
				nDefaultColumnWidth[3] = 70;
				nDefaultColumnWidth[4] = 70;

				for( int i=0; i<5; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtINT;
				type[2] = dtINT;
				type[3] = dtSTRING;
				type[4] = dtDATETIME;
				break;						
			}
			else
			{
				m_nColumnCount = 16;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger Type");
				csColumnLabel[2] = CString("Minimum");
				csColumnLabel[3] = CString("Average");
				csColumnLabel[4] = CString("Maximum");
				csColumnLabel[5] = CString("Count");

				CString str = " (s)";
				csColumnLabel[6] = CString("Q1") + str;
				csColumnLabel[7] = CString("Q2") + str;
				csColumnLabel[8] = CString("Q3") + str;
				csColumnLabel[9] = CString("P1") + str;
				csColumnLabel[10] = CString("P5") + str;
				csColumnLabel[11] = CString("P10") + str;
				csColumnLabel[12] = CString("P90") + str;
				csColumnLabel[13] = CString("P95") + str;
				csColumnLabel[14] = CString("P99") + str;

				csColumnLabel[15] = CString("Sigma") + str;


				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 180;
				nDefaultColumnWidth[2] = 60;
				nDefaultColumnWidth[3] = 60;
				nDefaultColumnWidth[4] = 60;
				nDefaultColumnWidth[5] = 60;
				nDefaultColumnWidth[6] = 60;
				nDefaultColumnWidth[7] = 60;
				nDefaultColumnWidth[8] = 60;
				nDefaultColumnWidth[9] = 60;
				nDefaultColumnWidth[10] = 60;
				nDefaultColumnWidth[11] = 60;
				nDefaultColumnWidth[12] = 60;
				nDefaultColumnWidth[13] = 60;
				nDefaultColumnWidth[14] = 60;
				nDefaultColumnWidth[15] = 60;


				for( int i=0; i<16; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
 				type[1] = dtSTRING;
 				type[2] = dtDATETIME;
 				type[3] = dtDATETIME;
 				type[4] = dtDATETIME;
 				type[5] = dtINT;
 
 				type[6] = dtDATETIME;
 				type[7] = dtDATETIME;
 				type[8] = dtDATETIME;
 				type[9] = dtDATETIME;
 				type[10] = dtDATETIME;
 				type[11] = dtDATETIME;
 				type[12] = dtDATETIME;
 				type[13] = dtDATETIME;
 				type[14] = dtDATETIME;				
 				type[15] = dtDATETIME;	
				break;		
			}
		}
	case BagDistReportFile:
		{
			m_nColumnCount = 7;
			csColumnLabel[0] = CString("");
			csColumnLabel[1] = CString("Flight ID(Arr)");
			csColumnLabel[2] = CString("Flight ID(Dep)");
			csColumnLabel[3] = CString("Scheduled Time(Arr)");
			csColumnLabel[4] = CString("Scheduled Time(Dep)");
			csColumnLabel[5] = CString("Bag Num(Arr)");
			csColumnLabel[6] = CString("Bag Num(Dep)");

			nDefaultColumnWidth[0] = 30;
			nDefaultColumnWidth[1] = 70;
			nDefaultColumnWidth[2] = 70;
			nDefaultColumnWidth[3] = 70;
			nDefaultColumnWidth[4] = 70;
			nDefaultColumnWidth[5] = 70;
			nDefaultColumnWidth[6] = 70;

			for( int i=0; i<7; i++ )
			{
				nFormat[i] = LVCFMT_CENTER; 
			}

			type[0] = dtINT;
			type[1] = dtSTRING;
			type[2] = dtSTRING;
			type[3] = dtSTRING;
			type[4] = dtSTRING;
			type[5] = dtINT;
			type[6] = dtINT;
			break;			
		}
	case PaxLogReportFile:
		{
			m_nColumnCount = 9;
			csColumnLabel[0] = CString("");
			csColumnLabel[1] = CString("ID");
			csColumnLabel[2] = CString("Entry");
			csColumnLabel[3] = CString("Exit");
			csColumnLabel[4] = CString("Group");
			csColumnLabel[5] = CString("Bags");
			csColumnLabel[6] = CString("Carts");
			CString csSpeed;
			csSpeed.Format( "Speed (%s/Sec)", UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) );
			csColumnLabel[7] = csSpeed;
			csColumnLabel[8] = CString("Pax Type");

			nDefaultColumnWidth[0] = 30;
			nDefaultColumnWidth[1] = 70;
			nDefaultColumnWidth[2] = 90;
			nDefaultColumnWidth[3] = 90;
			nDefaultColumnWidth[4] = 70;
			nDefaultColumnWidth[5] = 70;
			nDefaultColumnWidth[6] = 70;
			nDefaultColumnWidth[7] = 120;
			nDefaultColumnWidth[8] = 220;

			for( int i=0; i<8; i++ )
			{
				nFormat[i] = LVCFMT_CENTER; 
			}
			nFormat[7] = LVCFMT_LEFT;

			type[0] = dtINT;
			type[1] = dtINT;
			type[2] = dtDATETIME;
			type[3] = dtDATETIME;
			type[4] = dtINT;
			type[5] = dtINT;
			type[6] = dtINT;
			type[7] = dtDEC;
			type[8] = dtSTRING;
			break;
		}

	case DistanceReportFile:
		{
			if(iDetailed==0)//Detailed
			{
				//AfxMessageBox("Detailed!");
				m_nColumnCount = 6;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger ID");
				CUnitsManager* pUM = CUnitsManager::GetInstance();
				int nUnit = pUM->GetLengthUnits();				
				CString str = " (" + pUM->GetLengthUnitString( nUnit ) + ")";
				csColumnLabel[2] = CString("Distance") + str;
				csColumnLabel[3] = CString("Passenger Type");

				csColumnLabel[4] = CString("From Processor");
				csColumnLabel[5] = CString("To Processor");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 100;
				nDefaultColumnWidth[2] = 100;
				nDefaultColumnWidth[3] = 180;
				nDefaultColumnWidth[4] = 100;
				nDefaultColumnWidth[5] = 100;

				for( int i=0; i<3; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}
				nFormat[3] = LVCFMT_LEFT;
				nFormat[4] = LVCFMT_CENTER;
				nFormat[5] = LVCFMT_CENTER;

				type[0] = dtINT;
				type[1] = dtINT;
				type[2] = dtDEC;
				type[3] = dtSTRING;
				type[4] = dtSTRING;
				type[5] = dtSTRING;
				break;
			}
			else
			{
				//AfxMessageBox("Summary!");
				m_nColumnCount = 16;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger Type");
				CUnitsManager* pUM = CUnitsManager::GetInstance();
				int nUnit = pUM->GetLengthUnits();				
				CString str = " (" + pUM->GetLengthUnitString( nUnit ) + ")";
				csColumnLabel[2] = CString("Minimum") + str;
				csColumnLabel[3] = CString("Average") + str;
				csColumnLabel[4] = CString("Maximum") + str;
				csColumnLabel[5] = CString("Count");

				csColumnLabel[6] = CString("Q1") + str;
				csColumnLabel[7] = CString("Q2") + str;
				csColumnLabel[8] = CString("Q3") + str;
				csColumnLabel[9] = CString("P1") + str;
				csColumnLabel[10] = CString("P5") + str;
				csColumnLabel[11] = CString("P10") + str;
				csColumnLabel[12] = CString("P90") + str;
				csColumnLabel[13] = CString("P95") + str;
				csColumnLabel[14] = CString("P99") + str;

				csColumnLabel[15] = CString("Sigma") + str;


				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 180;
				nDefaultColumnWidth[2] = 60;
				nDefaultColumnWidth[3] = 60;
				nDefaultColumnWidth[4] = 60;
				nDefaultColumnWidth[5] = 60;
				nDefaultColumnWidth[6] = 60;
				nDefaultColumnWidth[7] = 60;
				nDefaultColumnWidth[8] = 60;
				nDefaultColumnWidth[9] = 60;
				nDefaultColumnWidth[10] = 60;
				nDefaultColumnWidth[11] = 60;
				nDefaultColumnWidth[12] = 60;
				nDefaultColumnWidth[13] = 60;
				nDefaultColumnWidth[14] = 60;
				nDefaultColumnWidth[15] = 60;


				for( int i=0; i<16; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDEC;
				type[3] = dtDEC;
				type[4] = dtDEC;
				type[5] = dtINT;
				type[6] = dtDEC;
				type[7] = dtDEC;
				type[8] = dtDEC;
				type[9] = dtDEC;
				type[10] = dtDEC;
				type[11] = dtDEC;
				type[12] = dtDEC;
				type[13] = dtDEC;
				type[14] = dtDEC;				
				type[15] = dtDEC;	
				break;
			}
			break;
		}

	case FireEvacuationReportFile:
		{
			if( iDetailed == 0 )	// detailed
			{
				m_nColumnCount = 6;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Pax ID");
				csColumnLabel[2] = CString("Pax Type");
				csColumnLabel[3] = CString("Exit Processor");
				csColumnLabel[4] = CString("Exit Time");
				csColumnLabel[5] = CString("Exit Duration");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 80;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;
				nDefaultColumnWidth[4] = 80;
				nDefaultColumnWidth[5] = 80;

				for( int i=0; i<m_nColumnCount; i++ )
				{
					nFormat[i] = LVCFMT_CENTER;
				}

				type[0] = dtINT;
				type[1] = dtINT;
				type[2] = dtSTRING;
				type[3] = dtSTRING;
				type[4] = dtDATETIME;
				type[5] = dtDATETIME;
				break;
			}
			else	//summary
			{
				m_nColumnCount = 16;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger Type");
				csColumnLabel[2] = CString("Minimum");
				csColumnLabel[3] = CString("Average");
				csColumnLabel[4] = CString("Maximum");
				csColumnLabel[5] = CString("Count");

				CString str = " (s)";
				csColumnLabel[6] = CString("Q1") + str;
				csColumnLabel[7] = CString("Q2") + str;
				csColumnLabel[8] = CString("Q3") + str;
				csColumnLabel[9] = CString("P1") + str;
				csColumnLabel[10] = CString("P5") + str;
				csColumnLabel[11] = CString("P10") + str;
				csColumnLabel[12] = CString("P90") + str;
				csColumnLabel[13] = CString("P95") + str;
				csColumnLabel[14] = CString("P99") + str;
				csColumnLabel[15] = CString("Sigma") + str;

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 180;
				nDefaultColumnWidth[2] = 60;
				nDefaultColumnWidth[3] = 60;
				nDefaultColumnWidth[4] = 60;
				nDefaultColumnWidth[5] = 60;
				nDefaultColumnWidth[6] = 60;
				nDefaultColumnWidth[7] = 60;
				nDefaultColumnWidth[8] = 60;
				nDefaultColumnWidth[9] = 60;
				nDefaultColumnWidth[10] = 60;
				nDefaultColumnWidth[11] = 60;
				nDefaultColumnWidth[12] = 60;
				nDefaultColumnWidth[13] = 60;
				nDefaultColumnWidth[14] = 60;
				nDefaultColumnWidth[15] = 60;

				for( int i=0; i<m_nColumnCount; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}
				
				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDATETIME;
				type[3] = dtDATETIME;
				type[4] = dtDATETIME;
				type[5] = dtINT;

				type[6] = dtDATETIME;
				type[7] = dtDATETIME;
				type[8] = dtDATETIME;
				type[9] = dtDATETIME;
				type[10] = dtDATETIME;
				type[11] = dtDATETIME;
				type[12] = dtDATETIME;
				type[13] = dtDATETIME;
				type[14] = dtDATETIME;	
				type[15] = dtDATETIME;
				break;	
			}
		}

	case QueueTimeReportFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 6;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Pax #");
				csColumnLabel[2] = CString("Total Time");
				csColumnLabel[3] = CString("Average Time");
				csColumnLabel[4] = CString("Processors");
				csColumnLabel[5] = CString("Passenger Type");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 80;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;
				nDefaultColumnWidth[4] = 80;
				nDefaultColumnWidth[5] = 200;

				for( int i=0; i<5; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}
				nFormat[5] = LVCFMT_LEFT;

				type[0] = dtINT;
				type[1] = dtINT;
				type[2] = dtDATETIME;
				type[3] = dtDATETIME;
				type[4] = dtINT;
				type[5] = dtSTRING;
				break;
			}
			else//Summary
			{
				m_nColumnCount = 16;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger Type");
				csColumnLabel[2] = CString("Minimum");
				csColumnLabel[3] = CString("Average");
				csColumnLabel[4] = CString("Maximum");
				csColumnLabel[5] = CString("Count");

				CString str = " (s)";
				csColumnLabel[6] = CString("Q1") + str;
				csColumnLabel[7] = CString("Q2") + str;
				csColumnLabel[8] = CString("Q3") + str;
				csColumnLabel[9] = CString("P1") + str;
				csColumnLabel[10] = CString("P5") + str;
				csColumnLabel[11] = CString("P10") + str;
				csColumnLabel[12] = CString("P90") + str;
				csColumnLabel[13] = CString("P95") + str;
				csColumnLabel[14] = CString("P99") + str;

				csColumnLabel[15] = CString("Sigma") + str;


				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 180;
				nDefaultColumnWidth[2] = 60;
				nDefaultColumnWidth[3] = 60;
				nDefaultColumnWidth[4] = 60;
				nDefaultColumnWidth[5] = 60;
				nDefaultColumnWidth[6] = 60;
				nDefaultColumnWidth[7] = 60;
				nDefaultColumnWidth[8] = 60;
				nDefaultColumnWidth[9] = 60;
				nDefaultColumnWidth[10] = 60;
				nDefaultColumnWidth[11] = 60;
				nDefaultColumnWidth[12] = 60;
				nDefaultColumnWidth[13] = 60;
				nDefaultColumnWidth[14] = 60;
				nDefaultColumnWidth[15] = 60;


				for( int i=0; i<16; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDATETIME;
				type[3] = dtDATETIME;
				type[4] = dtDATETIME;
				type[5] = dtINT;

				type[6] = dtDATETIME;
				type[7] = dtDATETIME;
				type[8] = dtDATETIME;
				type[9] = dtDATETIME;
				type[10] = dtDATETIME;
				type[11] = dtDATETIME;
				type[12] = dtDATETIME;
				type[13] = dtDATETIME;
				type[14] = dtDATETIME;				
				type[15] = dtDATETIME;	
				break;	
			}//end of else
		}//end of case
	case ConveyorWaitTimeReport:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 6;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Pax #");
				csColumnLabel[2] = CString("Total Time");
				csColumnLabel[3] = CString("Average Time");
				csColumnLabel[4] = CString("Processors");
				csColumnLabel[5] = CString("Passenger Type");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 80;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;
				nDefaultColumnWidth[4] = 80;
				nDefaultColumnWidth[5] = 200;

				for( int i=0; i<5; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}
				nFormat[5] = LVCFMT_LEFT;

				type[0] = dtINT;
				type[1] = dtINT;
				type[2] = dtDATETIME;
				type[3] = dtDATETIME;
				type[4] = dtINT;
				type[5] = dtSTRING;
				break;
			}
			else//Summary
			{
				m_nColumnCount = 16;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger Type");
				csColumnLabel[2] = CString("Minimum");
				csColumnLabel[3] = CString("Average");
				csColumnLabel[4] = CString("Maximum");
				csColumnLabel[5] = CString("Count");

				CString str = " (s)";
				csColumnLabel[6] = CString("Q1") + str;
				csColumnLabel[7] = CString("Q2") + str;
				csColumnLabel[8] = CString("Q3") + str;
				csColumnLabel[9] = CString("P1") + str;
				csColumnLabel[10] = CString("P5") + str;
				csColumnLabel[11] = CString("P10") + str;
				csColumnLabel[12] = CString("P90") + str;
				csColumnLabel[13] = CString("P95") + str;
				csColumnLabel[14] = CString("P99") + str;

				csColumnLabel[15] = CString("Sigma") + str;


				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 180;
				nDefaultColumnWidth[2] = 60;
				nDefaultColumnWidth[3] = 60;
				nDefaultColumnWidth[4] = 60;
				nDefaultColumnWidth[5] = 60;
				nDefaultColumnWidth[6] = 60;
				nDefaultColumnWidth[7] = 60;
				nDefaultColumnWidth[8] = 60;
				nDefaultColumnWidth[9] = 60;
				nDefaultColumnWidth[10] = 60;
				nDefaultColumnWidth[11] = 60;
				nDefaultColumnWidth[12] = 60;
				nDefaultColumnWidth[13] = 60;
				nDefaultColumnWidth[14] = 60;
				nDefaultColumnWidth[15] = 60;


				for( int i=0; i<16; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDATETIME;
				type[3] = dtDATETIME;
				type[4] = dtDATETIME;
				type[5] = dtINT;

				type[6] = dtDATETIME;
				type[7] = dtDATETIME;
				type[8] = dtDATETIME;
				type[9] = dtDATETIME;
				type[10] = dtDATETIME;
				type[11] = dtDATETIME;
				type[12] = dtDATETIME;
				type[13] = dtDATETIME;
				type[14] = dtDATETIME;				
				type[15] = dtDATETIME;	
				break;	
			}//end of else
		}//end of case

	case DurationReportFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 6;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger #");
				csColumnLabel[2] = CString("Duration");
				csColumnLabel[3] = CString("Passenger Type");
				csColumnLabel[4] = CString("From Processor");
				csColumnLabel[5] = CString("To Processor");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 80;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 200;
				nDefaultColumnWidth[4] = 100;
				nDefaultColumnWidth[5] = 100;


				nFormat[0] = LVCFMT_CENTER;
				nFormat[1] = LVCFMT_LEFT;
				nFormat[2] = LVCFMT_CENTER; 
				nFormat[3] = LVCFMT_LEFT;
				nFormat[4] = LVCFMT_CENTER;
				nFormat[5] = LVCFMT_CENTER;

				type[0] = dtINT;
				type[1] = dtINT;
				type[2] = dtDATETIME;
				type[3] = dtSTRING;
				type[4] = dtSTRING;
				type[5] = dtSTRING;
				break;
			}
			else//Summary
			{
				m_nColumnCount = 16;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger Type");
				csColumnLabel[2] = CString("Minimum");
				csColumnLabel[3] = CString("Average");
				csColumnLabel[4] = CString("Maximum");
				csColumnLabel[5] = CString("Count");

				CString str = " (s)";
				csColumnLabel[6] = CString("Q1") + str;
				csColumnLabel[7] = CString("Q2") + str;
				csColumnLabel[8] = CString("Q3") + str;
				csColumnLabel[9] = CString("P1") + str;
				csColumnLabel[10] = CString("P5") + str;
				csColumnLabel[11] = CString("P10") + str;
				csColumnLabel[12] = CString("P90") + str;
				csColumnLabel[13] = CString("P95") + str;
				csColumnLabel[14] = CString("P99") + str;

				csColumnLabel[15] = CString("Sigma") + str;


				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 190;
				nDefaultColumnWidth[2] = 70;
				nDefaultColumnWidth[3] = 70;
				nDefaultColumnWidth[4] = 70;
				nDefaultColumnWidth[5] = 70;
				nDefaultColumnWidth[6] = 60;
				nDefaultColumnWidth[7] = 60;
				nDefaultColumnWidth[8] = 60;
				nDefaultColumnWidth[9] = 60;
				nDefaultColumnWidth[10] = 60;
				nDefaultColumnWidth[11] = 60;
				nDefaultColumnWidth[12] = 60;
				nDefaultColumnWidth[13] = 60;
				nDefaultColumnWidth[14] = 60;
				nDefaultColumnWidth[15] = 60;

				for( int i=0; i<16; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDATETIME;
				type[3] = dtDATETIME;
				type[4] = dtDATETIME;
				type[5] = dtINT;

				type[6] = dtDATETIME;
			
				type[7] = dtDATETIME;
				type[8] = dtDATETIME;
				type[9] = dtDATETIME;
				type[10] = dtDATETIME;
				type[11] = dtDATETIME;
				type[12] = dtDATETIME;
				type[13] = dtDATETIME;
				type[14] = dtDATETIME;				
				type[15] = dtDATETIME;	
				break;	
			}//end of else
		}//end of case

	case ServiceTimeReportFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 6;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Pax #");
				csColumnLabel[2] = CString("Group Service");
				csColumnLabel[3] = CString("Single Service");
				csColumnLabel[4] = CString("Processors");
				csColumnLabel[5] = CString("Passenger Type");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 80;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;
				nDefaultColumnWidth[4] = 80;
				nDefaultColumnWidth[5] = 200;

				for( int i=0; i<5; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}
				nFormat[5] = LVCFMT_LEFT;

				type[0] = dtINT;
				type[1] = dtINT;
				type[2] = dtDATETIME;
				type[3] = dtDATETIME;
				type[4] = dtINT;
				type[5] = dtSTRING;
				break;
			}
			else//Summary
			{
				m_nColumnCount = 16;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger Type");
				csColumnLabel[2] = CString("Minimum");
				csColumnLabel[3] = CString("Average");
				csColumnLabel[4] = CString("Maximum");
				csColumnLabel[5] = CString("Count");

				CString str = " (s)";
				csColumnLabel[6] = CString("Q1") + str;
				csColumnLabel[7] = CString("Q2") + str;
				csColumnLabel[8] = CString("Q3") + str;
				csColumnLabel[9] = CString("P1") + str;
				csColumnLabel[10] = CString("P5") + str;
				csColumnLabel[11] = CString("P10") + str;
				csColumnLabel[12] = CString("P90") + str;
				csColumnLabel[13] = CString("P95") + str;
				csColumnLabel[14] = CString("P99") + str;
				csColumnLabel[15] = CString("Sigma") + str;

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 190;
				nDefaultColumnWidth[2] = 70;
				nDefaultColumnWidth[3] = 70;
				nDefaultColumnWidth[4] = 70;
				nDefaultColumnWidth[5] = 70;
				nDefaultColumnWidth[6] = 60;
				nDefaultColumnWidth[7] = 60;
				nDefaultColumnWidth[8] = 60;
				nDefaultColumnWidth[9] = 60;
				nDefaultColumnWidth[10] = 60;
				nDefaultColumnWidth[11] = 60;
				nDefaultColumnWidth[12] = 60;
				nDefaultColumnWidth[13] = 60;
				nDefaultColumnWidth[14] = 60;
				nDefaultColumnWidth[15] = 60;

				for( int i=0; i<16; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDATETIME;
				type[3] = dtDATETIME;
				type[4] = dtDATETIME;
				type[5] = dtINT;
				type[6] = dtDATETIME;
				type[7] = dtDATETIME;
				type[8] = dtDATETIME;
				type[9] = dtDATETIME;
				type[10] = dtDATETIME;
				type[11] = dtDATETIME;
				type[12] = dtDATETIME;
				type[13] = dtDATETIME;
				type[14] = dtDATETIME;				
				type[15] = dtDATETIME;	
				break;	
			}//end of else
		}//end of case

	case ActivityBreakdownReportFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 8;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("ID");
				csColumnLabel[2] = CString("Moving Time");
				csColumnLabel[3] = CString("Queuing Time");
				csColumnLabel[4] = CString("Bag Wait Time");
				csColumnLabel[5] = CString("Hold Area Time");
				csColumnLabel[6] = CString("Service Time");
				csColumnLabel[7] = CString("Passenger Type");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 80;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;
				nDefaultColumnWidth[4] = 80;
				nDefaultColumnWidth[5] = 80;
				nDefaultColumnWidth[6] = 80;
				nDefaultColumnWidth[7] = 200;

				for( int i=0; i<7; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}
				nFormat[7] = LVCFMT_LEFT;
				
				type[0] = dtINT;
				type[1] = dtINT;
				type[2] = dtDATETIME;
				type[3] = dtDATETIME;
				type[4] = dtDATETIME;
				type[5] = dtDATETIME;
				type[6] = dtDATETIME;
				type[7] = dtSTRING;
				break;
			}
			else//Summary
			{
				m_nColumnCount = 8;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger Type");
				csColumnLabel[2] = CString("Moving Time");
				csColumnLabel[3] = CString("Queuing Time");
				csColumnLabel[4] = CString("Bag Wait Time");
				csColumnLabel[5] = CString("Hold Area Time");
				csColumnLabel[6] = CString("Service Time");
				csColumnLabel[7] = CString("Count");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 200;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;
				nDefaultColumnWidth[4] = 80;
				nDefaultColumnWidth[5] = 80;
				nDefaultColumnWidth[6] = 80;
				nDefaultColumnWidth[7] = 80;

				for( int i=0; i<8; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDATETIME;
				type[3] = dtDATETIME;
				type[4] = dtDATETIME;
				type[5] = dtDATETIME;
				type[6] = dtDATETIME;
				type[7] = dtINT;
				break;	
			}
		}//end of case

	case UtilizationReportFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 8;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor");
				csColumnLabel[2] = CString("Scheduled");
				csColumnLabel[3] = CString("Overtime");
				csColumnLabel[4] = CString("Actual");
				csColumnLabel[5] = CString("Service");
				csColumnLabel[6] = CString("Idle");
				csColumnLabel[7] = CString("Utilization(%)");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 200;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;
				nDefaultColumnWidth[4] = 80;
				nDefaultColumnWidth[5] = 80;
				nDefaultColumnWidth[6] = 80;
				nDefaultColumnWidth[7] = 80;

				for( int i=0; i<8; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDATETIME;
				type[3] = dtDATETIME;
				type[4] = dtDATETIME;
				type[5] = dtDATETIME;
				type[6] = dtDATETIME;
				type[7] = dtDEC;
				break;
			}
			else//Summary
			{
				m_nColumnCount = 9;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor");
				csColumnLabel[2] = CString("Group Size");
				csColumnLabel[3] = CString("Scheduled");
				csColumnLabel[4] = CString("Overtime");
				csColumnLabel[5] = CString("Actual");
				csColumnLabel[6] = CString("Service");
				csColumnLabel[7] = CString("Idle");
				csColumnLabel[8] = CString("Utilization(%)");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 190;
				nDefaultColumnWidth[2] = 70;
				nDefaultColumnWidth[3] = 70;
				nDefaultColumnWidth[4] = 70;
				nDefaultColumnWidth[5] = 70;
				nDefaultColumnWidth[6] = 70;
				nDefaultColumnWidth[7] = 70;
				nDefaultColumnWidth[8] = 70;

				for( int i=0; i<9; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtINT;
				type[3] = dtDATETIME;
				type[4] = dtDATETIME;
				type[5] = dtDATETIME;
				type[6] = dtDATETIME;
				type[7] = dtDATETIME;
				type[8] = dtDEC;
				break;	
			}
		}

	case ThroughputReportFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 5;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor");
				csColumnLabel[2] = CString("Start Time");
				csColumnLabel[3] = CString("End Time");
				csColumnLabel[4] = CString("Pax Served");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 200;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;
				nDefaultColumnWidth[4] = 80;

				for( int i=0; i<5; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDATETIME;
				type[3] = dtDATETIME;
				type[4] = dtDEC;
				break;
			}
			else//Summary
			{
				m_nColumnCount = 7;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor");
				csColumnLabel[2] = CString("Group Size");
				csColumnLabel[3] = CString("Pax/Group");
				csColumnLabel[4] = CString("Pax/Proc");
				csColumnLabel[5] = CString("Group Thrpt/Hr");
				csColumnLabel[6] = CString("Avg/Hr");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 190;
				nDefaultColumnWidth[2] = 70;
				nDefaultColumnWidth[3] = 70;
				nDefaultColumnWidth[4] = 70;
				nDefaultColumnWidth[5] = 70;
				nDefaultColumnWidth[6] = 70;

				for( int i=0; i<7; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}
				
				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtINT;
				type[3] = dtINT;
				type[4] = dtDEC;
				type[5] = dtDEC;
				type[6] = dtDEC;
				break;	
			}//end of else
		}//end of case

	case QueueLengthReportFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 4;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor");
				csColumnLabel[2] = CString("Time");
				csColumnLabel[3] = CString("Queue Length");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 200;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;

				for( int i=0; i<4; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDATETIME;
				type[3] = dtINT;
				break;
			}
			else//Summary
			{
				m_nColumnCount = 8;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor Group");
				csColumnLabel[2] = CString("Group Size");
				csColumnLabel[3] = CString("Time");
				csColumnLabel[4] = CString("Min");
				csColumnLabel[5] = CString("Avg");
				csColumnLabel[6] = CString("Max");
				csColumnLabel[7] = CString("Total");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 190;
				nDefaultColumnWidth[2] = 70;
				nDefaultColumnWidth[3] = 70;
				nDefaultColumnWidth[4] = 70;
				nDefaultColumnWidth[5] = 70;
				nDefaultColumnWidth[6] = 70;
				nDefaultColumnWidth[7] = 70;

				for( int i=0; i<8; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtINT;
				type[3] = dtDATETIME;
				type[4] = dtINT;
				type[5] = dtDEC;
				type[6] = dtINT;
				type[7] = dtINT;
				break;	
			}//end of else
		}//end of case

	case ConveyorWaitLengthReport:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 4;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor");
				csColumnLabel[2] = CString("Time");
				csColumnLabel[3] = CString("Conveyour Wait Length");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 200;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;

				for( int i=0; i<4; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDATETIME;
				type[3] = dtINT;
				break;
			}
			else//Summary
			{
				m_nColumnCount = 8;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor Group");
				csColumnLabel[2] = CString("Group Size");
				csColumnLabel[3] = CString("Time");
				csColumnLabel[4] = CString("Min");
				csColumnLabel[5] = CString("Avg");
				csColumnLabel[6] = CString("Max");
				csColumnLabel[7] = CString("Total");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 190;
				nDefaultColumnWidth[2] = 70;
				nDefaultColumnWidth[3] = 70;
				nDefaultColumnWidth[4] = 70;
				nDefaultColumnWidth[5] = 70;
				nDefaultColumnWidth[6] = 70;
				nDefaultColumnWidth[7] = 70;

				for( int i=0; i<8; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtINT;
				type[3] = dtDATETIME;
				type[4] = dtINT;
				type[5] = dtDEC;
				type[6] = dtINT;
				type[7] = dtINT;

// 				type[0] = dtSTRING;
// 				type[1] = dtINT;
// 				type[2] = dtDATETIME;
// 				type[3] = dtINT;
// 				type[4] = dtDEC;
// 				type[5] = dtINT;
// 				type[6] = dtINT;
				break;	
			}//end of else
		}//end of case			

	case AverageQueueLengthReportFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 4;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor");
				/*csColumnLabel[1] = CString("Avg Queue");*/
				csColumnLabel[2] = CString("Max Queue");
				csColumnLabel[3] = CString("Max Queue Time");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 200;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;
				//nDefaultColumnWidth[3] = 80;

				for( int i=0; i<4; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDEC;
				type[3] = dtINT;
				type[4] = dtDATETIME;

// 				type[0] = dtSTRING;
// 				type[1] = dtDEC;
// 				type[2] = dtINT;
// 				type[3] = dtDATETIME;
				break;
			}
			else//Summary
			{
				m_nColumnCount = 5;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor");
				csColumnLabel[2] = CString("Group Size");
				//csColumnLabel[2] = CString("Avg Queue");
				csColumnLabel[3] = CString("Max Queue");
				csColumnLabel[4] = CString("Max Queue Time");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 190;
				nDefaultColumnWidth[2] = 70;
				nDefaultColumnWidth[3] = 70;
				nDefaultColumnWidth[4] = 70;
				//nDefaultColumnWidth[4] = 70;

				for( int i=0; i<5; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtINT;
				type[3] = dtDEC;
				type[4] = dtINT;
				type[5] = dtDATETIME;
				break;	
			}//end of else
		}//end of case

	case PassengerCountReportFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 4;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger Type");
				csColumnLabel[2] = CString("Time");
				csColumnLabel[3] = CString("Count");


				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 200;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;


				for( int i=0; i<4; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDATETIME;
				type[3] = dtINT;

				break;
			}
			else
			{
				m_nColumnCount = 10;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger Type");
				csColumnLabel[2] = CString("Min Pax");
				csColumnLabel[3] = CString("Min Time");
				csColumnLabel[4] = CString("Avg/Period");
				csColumnLabel[5] = CString("Max Pax");
				csColumnLabel[6] = CString("Max Time");
				csColumnLabel[7] = CString("Total Pax");
				csColumnLabel[8] = CString("First Entry");
				csColumnLabel[9] = CString("Last Exit");


				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 200;
				nDefaultColumnWidth[2] = 90;
				nDefaultColumnWidth[3] = 90;
				nDefaultColumnWidth[4] = 70;
				nDefaultColumnWidth[5] = 70;
				nDefaultColumnWidth[6] = 70;
				nDefaultColumnWidth[7] = 70;
				nDefaultColumnWidth[8] = 70;
				nDefaultColumnWidth[9] = 80;

				for( int i=0; i<10; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtINT;
				type[3] = dtDATETIME;
				type[4] = dtDEC;
				type[5] = dtINT;
				type[6] = dtDATETIME;
				type[7] = dtINT;
				type[8] = dtDATETIME;
				type[9] = dtDATETIME;
				break;
			}
			break;
		}
	case PassengerDensityReportFile:
		{
			m_nColumnCount = 6;
			csColumnLabel[0] = CString("");
			csColumnLabel[1] = CString("Passenger Type");
			csColumnLabel[2] = CString("Time");
			csColumnLabel[3] = CString("Count");
			csColumnLabel[4] = CString("Pax/m2");
			csColumnLabel[5] = CString("m2/Pax");

			nDefaultColumnWidth[0] = 30;
			nDefaultColumnWidth[1] = 200;
			nDefaultColumnWidth[2] = 80;
			nDefaultColumnWidth[3] = 80;
			nDefaultColumnWidth[4] = 80;
			nDefaultColumnWidth[5] = 80;

			for( int i=0; i<6; i++ )
			{
				nFormat[i] = LVCFMT_CENTER; 
			}

			type[0] = dtINT;
			type[1] = dtSTRING;
			type[2] = dtDATETIME;
			type[3] = dtINT;
			type[4] = dtDEC;
			type[5] = dtDEC;
			break;
		}//end of case

	case SpaceThroughputReportFile:
		{
			m_nColumnCount = 5;
			csColumnLabel[0] = CString("");
			csColumnLabel[1] = CString("Passenger Type");
			csColumnLabel[2] = CString("Start Time");
			csColumnLabel[3] = CString("End Time");
			csColumnLabel[4] = CString("Count");

			nDefaultColumnWidth[0] = 30;
			nDefaultColumnWidth[1] = 200;
			nDefaultColumnWidth[2] = 80;
			nDefaultColumnWidth[3] = 80;
			nDefaultColumnWidth[4] = 80;

			for(int i=0; i<5; i++)
			{
				nFormat[i] = LVCFMT_CENTER;
			}

			type[0] = dtINT;
			type[1] = dtSTRING;
			type[2] = dtDATETIME;
			type[3] = dtDATETIME;
			type[4] = dtDEC;
			break;
		}

	case CollisionReportFile:
		{
			m_nColumnCount = 5;
			csColumnLabel[0] = CString("");
			csColumnLabel[1] = CString("Pax Type");
			csColumnLabel[2] = CString("Time");
			csColumnLabel[3] = CString("Pax ID 1");
			csColumnLabel[4] = CString("Pax ID 2");

			nDefaultColumnWidth[0] = 30;
			nDefaultColumnWidth[1] = 100;
			nDefaultColumnWidth[2] = 60;
			nDefaultColumnWidth[3] = 100;
			nDefaultColumnWidth[4] = 100;

			for(int i = 0; i < 5; i++)
			{
				nFormat[i] = LVCFMT_CENTER;
			}

			type[0] = dtINT;
			type[1] = dtSTRING;
			type[2] = dtDATETIME;
			type[3] = dtDEC;
			type[4] = dtDEC;
			break;
		}

	case AcOperationsReportFile:
		{
			m_nColumnCount = 15;
			csColumnLabel[0] = CString("");
			csColumnLabel[1] = CString("Flight ID");
			csColumnLabel[2] = CString("Operation");
			csColumnLabel[3] = CString("Aircraft Type");
			csColumnLabel[4] = CString("Capacity");
			csColumnLabel[5] = CString("Scheduled Arr Time");
			csColumnLabel[6] = CString("Scheduled Dep Time");
			csColumnLabel[7] = CString("Load Factor(%)");
			csColumnLabel[8] = CString("Load");
			csColumnLabel[9] = CString("Actual Arr Time");
			csColumnLabel[10] = CString("Actual Dep Time");
			csColumnLabel[11] = CString("Delay Arr");
			csColumnLabel[12] = CString("Delay Dep");
			csColumnLabel[13] = CString("Gate Occupancy");
			csColumnLabel[14] = CString("Flight Type");

			nDefaultColumnWidth[0] = 30;
			nDefaultColumnWidth[1] = 70;
			nDefaultColumnWidth[2] = 70;
			nDefaultColumnWidth[3] = 70;
			nDefaultColumnWidth[4] = 70;
			nDefaultColumnWidth[5] = 70;
			nDefaultColumnWidth[6] = 70;
			nDefaultColumnWidth[7] = 70;
			nDefaultColumnWidth[8] = 70;
			nDefaultColumnWidth[9] = 70;
			nDefaultColumnWidth[10] = 70;
			nDefaultColumnWidth[11] = 70;
			nDefaultColumnWidth[12] = 70;
			nDefaultColumnWidth[13] = 70;
			nDefaultColumnWidth[14] = 100;

			for( int i=0; i<15; i++ )
			{
				nFormat[i] = LVCFMT_CENTER; 
			}

			
			type[0] = dtINT;
 			type[1] = dtSTRING;
 			type[2] = dtSTRING;
 			type[3] = dtSTRING;
 			type[4] = dtDEC;
 			type[5] = dtSTRING;
 			type[6] = dtSTRING;
 			//			type[4] = dtDATETIME;
 			//			type[5] = dtDATETIME;
 			type[7] = dtDEC;
 			type[8] = dtDEC;
 			type[9] = dtSTRING;
 			type[10] = dtSTRING;
 			//			type[8] = dtDATETIME;
 			//			type[9] = dtDATETIME;
 			type[11] = dtSTRING;
 			type[12] = dtSTRING;
 			//			type[10] = dtDEC;
 			//			type[11] = dtDEC;
 			type[13] = dtDATETIME;
 			type[14] = dtSTRING;
			break;
		}

	case BagCountReportFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 4;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor");
				csColumnLabel[2] = CString("Time");
				csColumnLabel[3] = CString("Bag Count");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 100;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;

				for( int i=0; i<4; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDATETIME;
				type[3] = dtINT;
			}
			else//Summary
			{
				m_nColumnCount = 8;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger Group");
				csColumnLabel[2] = CString("Group Size");
				csColumnLabel[3] = CString("Time");
				csColumnLabel[4] = CString("Min");
				csColumnLabel[5] = CString("Avg");
				csColumnLabel[6] = CString("Max");
				csColumnLabel[7] = CString("Total");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 200;
				nDefaultColumnWidth[2] = 70;
				nDefaultColumnWidth[3] = 70;
				nDefaultColumnWidth[4] = 70;
				nDefaultColumnWidth[5] = 70;
				nDefaultColumnWidth[6] = 70;
				nDefaultColumnWidth[7] = 70;

				for( int i=0; i<8; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtINT;
				type[3] = dtDATETIME;
				type[4] = dtINT;
				type[5] = dtDEC;
				type[6] = dtINT;
				type[7] = dtINT;
			}
			break;
		}//end of case
	case BagWaitTimeReportFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 5;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Pax #");
				csColumnLabel[2] = CString("Bag Count");
				csColumnLabel[3] = CString("Wait Time");
				csColumnLabel[4] = CString("Passenger Type");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 80;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;
				nDefaultColumnWidth[4] = 200;

				for( int i=0; i<4; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}
				nFormat[4] = LVCFMT_LEFT;


				type[0] = dtINT;
 				type[0] = dtINT;
 				type[1] = dtINT;
 				type[2] = dtDATETIME;
 				type[3] = dtSTRING;
			}
			else//Summary
			{								
				m_nColumnCount = 16;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger Type");
				csColumnLabel[2] = CString("Minimum");
				csColumnLabel[3] = CString("Average");
				csColumnLabel[4] = CString("Maximum");
				csColumnLabel[5] = CString("Count");

				CString str = " (s)";
				csColumnLabel[6] = CString("Q1") + str;
				csColumnLabel[7] = CString("Q2") + str;
				csColumnLabel[8] = CString("Q3") + str;
				csColumnLabel[9] = CString("P1") + str;
				csColumnLabel[10] = CString("P5") + str;
				csColumnLabel[11] = CString("P10") + str;
				csColumnLabel[12] = CString("P90") + str;
				csColumnLabel[13] = CString("P95") + str;
				csColumnLabel[14] = CString("P99") + str;

				csColumnLabel[15] = CString("Sigma") + str;


				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 180;
				nDefaultColumnWidth[2] = 60;
				nDefaultColumnWidth[3] = 60;
				nDefaultColumnWidth[4] = 60;
				nDefaultColumnWidth[5] = 60;
				nDefaultColumnWidth[6] = 60;
				nDefaultColumnWidth[7] = 60;
				nDefaultColumnWidth[8] = 60;
				nDefaultColumnWidth[9] = 60;
				nDefaultColumnWidth[10] = 60;
				nDefaultColumnWidth[11] = 60;
				nDefaultColumnWidth[12] = 60;
				nDefaultColumnWidth[13] = 60;
				nDefaultColumnWidth[14] = 60;
				nDefaultColumnWidth[15] = 60;


				for( int i=0; i<16; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDATETIME;
				type[3] = dtDATETIME;
				type[4] = dtDATETIME;
				type[5] = dtINT;

				type[6] = dtDATETIME;
				type[7] = dtDATETIME;
				type[8] = dtDATETIME;
				type[9] = dtDATETIME;
				type[10] = dtDATETIME;
				type[11] = dtDATETIME;
				type[12] = dtDATETIME;
				type[13] = dtDATETIME;
				type[14] = dtDATETIME;				
				type[15] = dtDATETIME;	

			}
			break;
		}//end of case

	case BillboardExposureIncidenceFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 6;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor");
				csColumnLabel[2] = CString("Start Time");
				csColumnLabel[3] = CString("End Time");
				csColumnLabel[4] = CString("Exposure Served");
				csColumnLabel[5] = CString("Total Pax Passed");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 200;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;
				nDefaultColumnWidth[4] = 120;
				nDefaultColumnWidth[5] = 120;
				for( int i=0; i<6; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[0] = dtSTRING;
				type[1] = dtDATETIME;
				type[2] = dtDATETIME;
				type[3] = dtDEC;
				type[4] = dtDEC;
				break;
			}
			else//Summary
			{
				m_nColumnCount = 4;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor");
				//csColumnLabel[1] = CString("Group Size");
				//csColumnLabel[2] = CString("Pax/Group");
				//csColumnLabel[3] = CString("Pax/Proc");
				//csColumnLabel[4] = CString("Group Thrpt/Hr");
				csColumnLabel[2] = CString("Avg/Hr");
				csColumnLabel[3] = CString("Avg/Hr");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 190;
				//nDefaultColumnWidth[1] = 70;
				//nDefaultColumnWidth[2] = 70;
				//nDefaultColumnWidth[3] = 70;
				//nDefaultColumnWidth[4] = 70;
				nDefaultColumnWidth[2] = 70;
				nDefaultColumnWidth[3] = 70;
				for( int i=0; i<4; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
 				type[1] = dtSTRING;
 				//type[1] = dtINT;
 				//type[2] = dtINT;
 				//type[3] = dtDEC;
 				//type[4] = dtDEC;
 				type[2] = dtDEC;
 				type[3] = dtDEC;
				break;	
			}//end of else
		}//end of case
	case BillboardLinkedProcIncrementlVisitFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 6;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor");
				csColumnLabel[2] = CString("Start Time");
				csColumnLabel[3] = CString("End Time");
				csColumnLabel[4] = CString("Pax Severed");
				csColumnLabel[5] = CString("Incremental Pax Severed");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 200;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;
				nDefaultColumnWidth[4] = 120;
				nDefaultColumnWidth[5] = 120;
				for( int i=0; i<6; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}


				type[0] = dtINT;
				type[1] = dtSTRING;
				type[2] = dtDATETIME;
				type[3] = dtDATETIME;
				type[4] = dtDEC;
				type[5] = dtDEC;
				break;
			}
			else//Summary
			{
				m_nColumnCount = 4;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Processor");
				//csColumnLabel[1] = CString("Group Size");
				//csColumnLabel[2] = CString("Pax/Group");
				//csColumnLabel[3] = CString("Pax/Proc");
				//csColumnLabel[4] = CString("Group Thrpt/Hr");
				csColumnLabel[2] = CString("Avg/Hr");
				csColumnLabel[3] = CString("Avg/Hr");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 190;
				//nDefaultColumnWidth[1] = 70;
				//nDefaultColumnWidth[2] = 70;
				//nDefaultColumnWidth[3] = 70;
				//nDefaultColumnWidth[4] = 70;
				nDefaultColumnWidth[2] = 70;
				nDefaultColumnWidth[3] = 70;
				for( int i=0; i<4; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
 				type[1] = dtSTRING;
 				//type[1] = dtINT;
 				//type[2] = dtINT;
 				//type[3] = dtDEC;
 				//type[4] = dtDEC;
 				type[2] = dtDEC;
 				type[3] = dtDEC;
				break;	
			}//end of else
		}//end of case
	case RetailReportFile:
		{
			if(iDetailed==0)//Detailed
			{
				m_nColumnCount = 16;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Passenger ID");
				csColumnLabel[2] = CString("Shop");
				csColumnLabel[3] = CString("Store");
				csColumnLabel[4] = CString("Enter Store date/time");
				csColumnLabel[5] = CString("Leave Store date/time");
				csColumnLabel[6] = CString("Time in store");
				csColumnLabel[7] = CString("Units potentially bought");
				csColumnLabel[8] = CString("Checkout");
				csColumnLabel[9] = CString("Enter checkout Q date/time");
				csColumnLabel[10] = CString("Checkout Q time(mm:ss)");
				csColumnLabel[11] = CString("Time in service(mm:ss)");
				csColumnLabel[12] = CString("Sale($)");
				csColumnLabel[13] = CString("Apt Revenue($)");
				//csColumnLabel[13] = CString("Inventory OK");
				csColumnLabel[14] = CString("Passenger Type");
				csColumnLabel[15] = CString("Product name");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 20;
				nDefaultColumnWidth[2] = 80;
				nDefaultColumnWidth[3] = 80;
				nDefaultColumnWidth[4] = 100;
				nDefaultColumnWidth[5] = 100;
				nDefaultColumnWidth[6] = 100;
				nDefaultColumnWidth[7] = 20;
				nDefaultColumnWidth[8] = 20;
				nDefaultColumnWidth[9] = 100;
				nDefaultColumnWidth[10] = 80;
				nDefaultColumnWidth[11] = 80;
				nDefaultColumnWidth[12] = 20;
				nDefaultColumnWidth[13] = 80;
			//	nDefaultColumnWidth[13] = 50;
				nDefaultColumnWidth[14] = 120;
				nDefaultColumnWidth[15] = 120;
				for( int i=0; i<16; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtINT;
				type[2] = dtSTRING;
				type[3] = dtSTRING;
				type[4] = dtDATETIME;
				type[5] = dtDATETIME;
				type[6] = dtDATETIME;
				type[7] = dtINT;
				type[8] = dtSTRING;
				type[9] = dtDATETIME;
				type[10] = dtDATETIME;
				type[11] = dtDATETIME;
				type[12] = dtINT;
				type[13] = dtINT;
				type[14] = dtSTRING;
				type[15] = dtSTRING;
			}
			else
			{
				m_nColumnCount = 17;
				csColumnLabel[0] = CString("");
				csColumnLabel[1] = CString("Interval");
				csColumnLabel[2] = CString("Store");
				csColumnLabel[3] = CString("Min sales/pax");
				csColumnLabel[4] = CString("Avg sales/pax");
				csColumnLabel[5] = CString("Max sales/pax");
				csColumnLabel[6] = CString("Q1");
				csColumnLabel[7] = CString("Q2");
				csColumnLabel[8] = CString("Q3");
				csColumnLabel[9] = CString("P1");
				csColumnLabel[10] = CString("P5");
				csColumnLabel[11] = CString("P10");
				csColumnLabel[12] = CString("P90");
				csColumnLabel[13] = CString("P95");
				csColumnLabel[14] = CString("P99");
				csColumnLabel[15] = CString("STD");
				csColumnLabel[16] = CString("DEV");

				nDefaultColumnWidth[0] = 30;
				nDefaultColumnWidth[1] = 20;
				nDefaultColumnWidth[2] = 70;
				nDefaultColumnWidth[3] = 30;
				nDefaultColumnWidth[4] = 30;
				nDefaultColumnWidth[5] = 30;
				nDefaultColumnWidth[6] = 20;
				nDefaultColumnWidth[7] = 20;
				nDefaultColumnWidth[8] = 20;
				nDefaultColumnWidth[9] = 20;
				nDefaultColumnWidth[10] = 20;
				nDefaultColumnWidth[11] = 20;
				nDefaultColumnWidth[12] = 20;
				nDefaultColumnWidth[13] = 20;
				nDefaultColumnWidth[14] = 20;
				nDefaultColumnWidth[15] = 20;
				nDefaultColumnWidth[16] = 20;
				for( int i=0; i<17; i++ )
				{
					nFormat[i] = LVCFMT_CENTER; 
				}

				type[0] = dtINT;
				type[1] = dtINT;
				type[2] = dtSTRING;
				type[3] = dtDEC;
				type[4] = dtDEC;
				type[5] = dtDEC;
				type[6] = dtDEC;
				type[7] = dtDEC;
				type[8] = dtDEC;
				type[9] = dtDEC;
				type[10] = dtDEC;
				type[11] = dtDEC;
				type[12] = dtDEC;
				type[13] = dtDEC;
				type[14] = dtDEC;
				type[15] = dtDEC;
				type[16] = dtDEC;

			}
		}
		break;
	}//end of switch
	for(int i=0; i<m_nColumnCount; i++ )
	{
		DWORD dwStyle = nFormat[i];
		dwStyle &= ~HDF_OWNERDRAW;
		GetListCtrl().InsertColumn( i, csColumnLabel[i],/* nFormat[i]*/dwStyle, nDefaultColumnWidth[i] );
		if(getListView())
			getListView()->GetSortableHeaderCtrl().SetDataType( i, type[i] );
	}
}

void CRepListViewSingleReportOperator::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	
    switch(lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		// Request prepaint notifications for each item.
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
		
    case CDDS_ITEMPREPAINT: // Requested notification
		int nDBIdx = GetListCtrl().GetItemData(lplvcd->nmcd.dwItemSpec);
		
		if( m_vFlag.size() != 0 && m_vFlag[nDBIdx] )
		{
			lplvcd->clrText = RGB(255, 0, 0);
			//lplvcd->clrTextBk = RGB(255,0,0);
		}
		*pResult = CDRF_DODEFAULT;
		break;
	}
}

void CRepListViewSingleReportOperator::OnListviewExport()
{
	if(m_sExtension.IsEmpty())		//If no items listed, then return;
		return;
	CString upExtension = m_sExtension;
	upExtension.MakeUpper();
	CFileDialog filedlg( FALSE,m_sExtension, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report files (*." + m_sExtension + ") | *." + m_sExtension + ";*." + upExtension + "|All type (*.*)|*.*|", NULL, 0, FALSE  );
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();

	CString sFileName;
	if( GetARCReportManager()->GetLoadReportType() == load_by_user )	
	{
		sFileName = GetARCReportManager()->GetUserLoadReportFile();
	}
	else				  // load_by_system
	{
		sFileName = GetTermPlanDoc()->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( GetTermPlanDoc()->m_ProjInfo.path );
	}

	CFileFind fileFind;
	if(fileFind.FindFile(csFileName))
	{
		if(MessageBox(NULL,"File already exists, are you sure to overwrite it?", "Warning!", MB_YESNO) == IDNO)
			return;
	}
	CopyFile(sFileName, csFileName, TRUE);
}

void CRepListViewSingleReportOperator::LoadReportProcessFlow( CMathResultManager* pManager, enum ENUM_REPORT_TYPE _enumRepType )
{
/*	case ENUM_QUEUETIME_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_THROUGHPUT_REP:
	case ENUM_PASSENGERNO_REP:
	case ENUM_UTILIZATION_REP:
*/	
	if(pManager == NULL)
		return ;

	switch(_enumRepType)
	{
	case ENUM_QUEUETIME_REP:
		{
			MathResult &result = pManager->GetQTimeResult();
			ShowReportProcessFlow(result);
			break;
		}
	case ENUM_QUEUELENGTH_REP:
		{
			MathResult &result = pManager->GetQLengthResult();
			ShowReportProcessFlow(result);
			break;
		}
	case ENUM_THROUGHPUT_REP:
		{
			MathResult &result = pManager->GetThroughputResult();
			ShowReportProcessFlow(result);
			break;
		}
	case ENUM_PASSENGERNO_REP:
		{
			MathResult &result = pManager->GetPaxCountResult();
			ShowReportProcessFlow(result);
			break;
		}
	case ENUM_UTILIZATION_REP:
		{
			MathResult &result = pManager->GetUtilizationResult();
			ShowReportProcessFlow(result);
			break;
		}
	default:
		break;
	}
	
	return ;
}

void CRepListViewSingleReportOperator::ShowReportProcessFlow( MathResult& result )
{
	GetListCtrl().SetRedraw( FALSE );

	GetListCtrl().DeleteAllItems();

	m_nColumnCount = result.m_vstrProcess.size();
	//	m_nColumnCount = 3;//test
	if(m_nColumnCount == 0)
	{
		GetListCtrl().SetRedraw( TRUE );
		Invalidate();
		return ;
	}

	EDataType type = dtDATETIME;
	GetListCtrl().InsertColumn(0, "Time", LVCFMT_CENTER, 60);
	
	if(getListView() == NULL)
		return;

	getListView()->GetSortableHeaderCtrl().SetDataType(0, type);


	int nWidth = 80;
	int i = 0;
	for( i = 0; i < m_nColumnCount; i++ )
	{
		type = dtSTRING;
		GetListCtrl().InsertColumn( i+1, (CString)result.m_vstrProcess[i].c_str(), LVCFMT_CENTER, nWidth );
		getListView()->GetSortableHeaderCtrl().SetDataType( i+1, type );
	}

	const std::vector<MathResultRecord> &vRecord = result.m_vResult;
	int nRow = vRecord.size();
	//	nRow = 5;//test
	CString strProcResult = "";
	for( i = 0; i < nRow; i++ )
	{
		int nItemIdx = GetListCtrl().InsertItem( i, GetAbsDateTime(vRecord[i].time.printTime()));//test

		const std::vector<double>& vProcResults = vRecord[i].vProcResults;
		for( int j = 0; j < m_nColumnCount; j++ )
		{
			strProcResult.Format("%.2f", vProcResults[j]);
			//			strProcResult = "vProcResults[i]";//test
			GetListCtrl().SetItemText( nRow + 1, j+1, strProcResult );
		}
	}	

	GetListCtrl().SetRedraw( TRUE );
	Invalidate();
	m_pListView->UpdateWindow();
}

CRepListView * CRepListViewSingleReportOperator::getListView() const
{
	return (CRepListView *)m_pListView;
}