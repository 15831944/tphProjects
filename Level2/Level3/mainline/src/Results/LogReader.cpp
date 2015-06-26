#include "StdAfx.h"
#include "LogReader.h"
#include "PAXENTRY.H"
#include "PAXLOG.H"
#include "Common\FileOutPut.h"
#include "OUT_TERM.H"
#include "Common\SimAndReportManager.h"
#include "Common\STATES.H"
#include <algorithm>
#include "BridgeConnectorLog.h"
#include "CommonData\PROCID.H"
#include "Engine\TERMINAL.H"
using namespace std;


//
bool LogReader::PrintPaxLog( OutputTerminal* pTerm, const std::vector<int>& mobId,const CString& _csProjPath,  const CString& outDir)
{
	pTerm->GetSimReportManager()->SetCurrentSimResult(0);

	CString sFileName = pTerm->GetSimReportManager()->GetCurrentLogsFileName( PaxDescFile, _csProjPath );	
	PaxLog paxLog(NULL);
	paxLog.LoadDataIfNecessary(sFileName);
	

	for(int i=0;i<paxLog.getCount();i++)
	{
		MobLogEntry element;
		paxLog.getItem(element, i);
		const MobDescStruct& mobDesc = element.GetMobDesc();
		if(std::find(mobId.begin(),mobId.end(),mobDesc.id)==mobId.end())
			continue;

		CString outfilename;
		outfilename.Format(_T("%sPaxLog%d.csv"),outDir.GetString(), mobDesc.id);

		CFileOutput::Clear(outfilename);
		CFileOutput fileOut(outfilename);
		fileOut.Log("Mobile Element Logs").line();
		fileOut.Log("Descriptions:").line();

#define LOG_MOBDESC_MEMBER(x)  fileOut.Log(#x).Log(',').Log(mobDesc.##x).line()

		LOG_MOBDESC_MEMBER(id);
		LOG_MOBDESC_MEMBER(terminalStartTime);
		LOG_MOBDESC_MEMBER(terminalEndTime);
		LOG_MOBDESC_MEMBER(startTime);
		LOG_MOBDESC_MEMBER(endTime);
		LOG_MOBDESC_MEMBER(enplanTime);
		LOG_MOBDESC_MEMBER(startPos);
		LOG_MOBDESC_MEMBER(endPos);
		LOG_MOBDESC_MEMBER(fileID);
		LOG_MOBDESC_MEMBER(mobileType);
		LOG_MOBDESC_MEMBER(intrinsic);
		LOG_MOBDESC_MEMBER(paxType);
		LOG_MOBDESC_MEMBER(arrFlight);
		LOG_MOBDESC_MEMBER(depFlight);
		LOG_MOBDESC_MEMBER(groupSize);
		LOG_MOBDESC_MEMBER(bagCount);
		LOG_MOBDESC_MEMBER(ownerID);
		LOG_MOBDESC_MEMBER(indexNum);
		LOG_MOBDESC_MEMBER(speed);
		LOG_MOBDESC_MEMBER(ownStart);
		LOG_MOBDESC_MEMBER(eachPaxOwnNoPax);
		LOG_MOBDESC_MEMBER(bMissFlightDeath);
		LOG_MOBDESC_MEMBER(carryonCount);
		LOG_MOBDESC_MEMBER(nBulkIndex);
		LOG_MOBDESC_MEMBER(nFreqIndex);
		LOG_MOBDESC_MEMBER(missFlightTime);
		LOG_MOBDESC_MEMBER(flightRealDepartureTime);
		LOG_MOBDESC_MEMBER(bBulkProfileCapacity);

		LOG_MOBDESC_MEMBER(xAngle);
		LOG_MOBDESC_MEMBER(yAngle);
		LOG_MOBDESC_MEMBER(zAngle);
		LOG_MOBDESC_MEMBER(w);



		typedef std::vector< MobEventStruct > part_event_list;
		part_event_list mobData;

		{//read Event logs 
			CString sFileName = pTerm->GetSimReportManager()->GetCurrentLogsFileName( PaxEventFile, _csProjPath );	
			sFileName = getPaxEventFileName(sFileName, mobDesc.fileID);
			auto_ptr<ifstream> pLogFile (new ifstream( sFileName, std::ios::in | std::ios::binary ));
			if(pLogFile->bad())
				return false;

			long beginPos	= element.GetMobDesc().startPos;
			long endPos		= element.GetMobDesc().endPos + sizeof( MobEventStruct ) ;
			long eventNum   = ( endPos-beginPos )/sizeof( MobEventStruct );

			mobData.resize(eventNum);
			pLogFile->seekg( beginPos, std::ios::beg );
			//long lPos = logfile.tellg();
			pLogFile->read( (char*)&( mobData[0] ), endPos - beginPos );
		}

		//long	elementID;
		//long    time;               // current hit time (1/18.2 of second)
		//float   x;                  // x position of hit in nm
		//float   y;                  // x position of hit in nm
		//float  speed ;				 //the speed  
		//float   m_RealZ ;            // if m_IsRealZ is true , this value will be uesd when write log .
		//short   z;                  // z position of hit (floor)
		//short   procNumber;         // index of current processor
		//short   reason;				// ( currently, only used in conveyor system ) which processor cause me stop
		//short   state;              // state of element at this hit (states.h)
		//char    m_IsRealZ;          // if true , z is the real value for log , else z will be handled by floor when write log in 3D view .
		//char	m_motionDir;		//onboard animation mobile element motion direction(four state: forward(F),backward(B),leftsidle(L),rightsidle(R)) 
		//bool    m_bVisible:1;		//visitor walk with owner if true draw in 3dview,or hide in 3dview
		//bool    m_bWithOwner:1;  //if visitor walk with owner
		//bool	bDynamicCreatedProc:1;// ( currently ,only used in TLOS Created new processor dynamicly ) it means whether or not processor( procNumber ) is dynamicly created
		//bool	followowner:1;  // flag visitor is moveing with its owner or not; to pax,the flag is invalidate
		//bool    backup:1;    // point the direction of out constrain 
		fileOut.line();
		fileOut.Log(_T("MobileElement Events")).line();

#define LOG_MOBDEVENTDESC_MEM(x) fileOut.Log(#x).Log(',')
		fileOut.Log("index").Log(',');
		LOG_MOBDEVENTDESC_MEM(elementID);
		LOG_MOBDEVENTDESC_MEM(time);
		LOG_MOBDEVENTDESC_MEM(x);
		LOG_MOBDEVENTDESC_MEM(y);
		LOG_MOBDEVENTDESC_MEM(speed);
		LOG_MOBDEVENTDESC_MEM(m_RealZ);
        LOG_MOBDEVENTDESC_MEM(z);
        LOG_MOBDEVENTDESC_MEM(procNumber);
        CString strProcName;
        LOG_MOBDEVENTDESC_MEM(strProcName);
        CString strFlt;
        LOG_MOBDEVENTDESC_MEM(strFlt);
		LOG_MOBDEVENTDESC_MEM(reason);
		LOG_MOBDEVENTDESC_MEM(state);
		LOG_MOBDEVENTDESC_MEM(m_IsRealZ);
		LOG_MOBDEVENTDESC_MEM(m_motionDir);
		LOG_MOBDEVENTDESC_MEM(m_bVisible);
		LOG_MOBDEVENTDESC_MEM(m_bWithOwner);
		LOG_MOBDEVENTDESC_MEM(bDynamicCreatedProc);
		LOG_MOBDEVENTDESC_MEM(followowner);
		LOG_MOBDEVENTDESC_MEM(backup);
		fileOut.line();


		for(size_t i=0;i<mobData.size();i++)
		{
			MobEventStruct& evt = mobData[i];
#define LOG_MOBDEVENT_MEMBER(x) fileOut.Log(x).Log(',')

			LOG_MOBDEVENT_MEMBER(i);
			LOG_MOBDEVENT_MEMBER(evt.elementID);
			LOG_MOBDEVENT_MEMBER(evt.time);
			LOG_MOBDEVENT_MEMBER(evt.x);
			LOG_MOBDEVENT_MEMBER(evt.y);
			LOG_MOBDEVENT_MEMBER(evt.speed);
			LOG_MOBDEVENT_MEMBER(evt.m_RealZ);
            LOG_MOBDEVENT_MEMBER(evt.z);
            LOG_MOBDEVENT_MEMBER(evt.procNumber);
            Terminal* pInterm = (Terminal*)pTerm;
            if(pInterm && evt.procNumber != -1)
                strProcName = pInterm->procList->getProcessor(evt.procNumber)->getIDName();
            else
                strProcName = _T("evt.procNumber == -1");
            LOG_MOBDEVENT_MEMBER(strProcName);
            if(pInterm)
                LOG_MOBDEVENT_MEMBER(_T("flight xxx"));
            else
                LOG_MOBDEVENT_MEMBER(_T("flight xxx"));
			LOG_MOBDEVENT_MEMBER(evt.reason);
			LOG_MOBDEVENT_MEMBER(STATE_NAMES[evt.state]);
			LOG_MOBDEVENT_MEMBER( bool(evt.m_IsRealZ!=0) );
			LOG_MOBDEVENT_MEMBER(evt.m_motionDir);
			LOG_MOBDEVENT_MEMBER(evt.m_bVisible);
			LOG_MOBDEVENT_MEMBER(evt.m_bWithOwner);
			LOG_MOBDEVENT_MEMBER(evt.bDynamicCreatedProc);
			LOG_MOBDEVENT_MEMBER(evt.followowner);
			LOG_MOBDEVENT_MEMBER(evt.backup);

			fileOut.line();
		}
	}
	return true;
}

CString LogReader::getPaxEventFileName( CString initName, int nFileIndex )
{
	if(nFileIndex==0)
		return initName;

	long lPos = initName.Find('.');
	CString strLeft = initName.Left(lPos);
	CString ret;
	ret.Format(_T("%s%03d.log"),strLeft,nFileIndex);
	return ret;
}


bool LogReader::PrintBridgeLog(Terminal* pTerm, const CString& _csProjPath, const CString& outDir)
{
	

	pTerm->GetSimReportManager()->SetCurrentSimResult(0);

	CString sFileName = pTerm->GetSimReportManager()->GetCurrentLogsFileName( BridgeConnectorDescFile, _csProjPath );

	EventLog<BridgeConnectorEventStruct> evetlog;
	CBridgeConnectorLog* pLog = new CBridgeConnectorLog(&evetlog);
	pLog->LoadDataIfNecessary( sFileName );

	sFileName = pTerm->GetSimReportManager()->GetCurrentLogsFileName( BridgeConnectorEventFile, _csProjPath );
	pLog->LoadEventDataIfNecessary( sFileName );	
	
	CString outFile = outDir+_T("bridgelog.csv");
	CFileOutput::Clear(outFile);
	CFileOutput fileOut(outFile);

	int nLogCount = pLog->getCount();
	for(int i=0;i<nLogCount;i++)
	{
		CBridgeConnectorLogEntry bridgeElment;
		BridgeConnectorDescStruct bds;
		pLog->getItem(bridgeElment,i);
		bridgeElment.SetOutputTerminal(pTerm);
		bridgeElment.SetEventLog(&evetlog);
		bridgeElment.initStruct(bds);

		ProcessorID id;
		id.setIDS(bds.name);
		id.SetStrDict(pTerm->inStrDict);
		fileOut.Log(id.GetIDString()).Line();

	#define TI(x) fileOut.Log(#x).Log(',')
		TI(name);TI(indexNum);TI(procType);TI(xLocation);TI(yLocation);TI(zLocation);TI(dWidth);TI(dLength);
		TI(xDirFrom);TI(yDirFrom);TI(zDirFrom);TI(xDirTo);TI(yDirTo);TI(zDirTo);
		TI(startTime);TI(endTime);TI(startPos);TI(endPos);TI(speed);
		fileOut.Line();
	#define CT(x) fileOut.Log(bds.##x).Log(',')
		CT(name);CT(indexNum);CT(procType);CT(xLocation);CT(yLocation);CT(zLocation);CT(dWidth);CT(dLength);
		CT(xDirFrom);CT(yDirFrom);CT(zDirFrom);CT(xDirTo);CT(yDirTo);CT(zDirTo);
		CT(startTime);CT(endTime);CT(startPos);CT(endPos);CT(speed);
		fileOut.Line();


		//typedef std::vector<BridgeConnectorEventStruct> EventList;
		//const  EventList& eventlist = bridgeElment.getEventList();
		int nEventCout = bridgeElment.getCount();
		for(int j=0;j<nEventCout;j++)
		{
			BridgeConnectorEventStruct e = bridgeElment.getEvent(j);

			LOG_MOBDEVENT_MEMBER(e.time);
			LOG_MOBDEVENT_MEMBER(e.x);
			LOG_MOBDEVENT_MEMBER(e.y);
			LOG_MOBDEVENT_MEMBER(e.z);
			LOG_MOBDEVENT_MEMBER(e.state);
			fileOut.Line();
		}
		fileOut.Line();
	}
	return true;
}
