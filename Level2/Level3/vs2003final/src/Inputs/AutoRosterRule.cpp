#include "StdAfx.h"
#include ".\autorosterrule.h"
#include "IN_TERM.H"
#include "../Database/ADODatabase.h"
#include "assign.h"
#include "../Engine/proclist.h"
#include "schedule.h"
#include "assigndb.h"
#include "procidlist.h"
#define  TY_RULE   2 


CPriorityRule::CPriorityRule( InputTerminal* pInputTerm )
: m_bAdjacent(false)
, m_nMinNum(1)
{

}

CPriorityRule::~CPriorityRule()
{

}

void CPriorityRule::ReadData( CADORecordset& recordSet )
{
	CString strProcList;
	int nValue;

	recordSet.GetFieldValue(_T("ID"),m_nID) ;
	recordSet.GetFieldValue(_T("PRIORITYID"),m_nPriorityID) ;
	recordSet.GetFieldValue(_T("PROCLIST"),strProcList) ;
	recordSet.GetFieldValue(_T("ISADJACENT"),nValue) ;
	recordSet.GetFieldValue(_T("MINNUM"),m_nMinNum) ;

	m_bAdjacent = nValue >0 ? true:false;
	SetProcList(strProcList); 
}

void CPriorityRule::SaveData( int nParentID )
{
	CString  StrSQL ;
	CString strProcList;
	GetProcListName(strProcList);
	
	StrSQL.Format(_T("INSERT INTO TB_ROSTERRULEPRIORITY (RULEID, PRIORITYID, PROCLIST, ISADJACENT, MINNUM) \
						VALUES (%d, %d, '%s', %d, %d)"),
						nParentID, m_nPriorityID, strProcList, m_bAdjacent?1:0, m_nMinNum) ;
	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(StrSQL,DATABASESOURCE_TYPE_ACCESS) ;
}

void CPriorityRule::UpdateData()
{
	CString  StrSQL ;
	CString strProcList;
	GetProcListName(strProcList);

	StrSQL.Format(_T("UPDATE TB_ROSTERRULEPRIORITY SET PRIORITYID = %d, PROCLIST = '%s', ISADJACENT = %d, MINNUM = %d, \
					  WHERE ID = %d"),
					 m_nPriorityID, strProcList, m_bAdjacent?1:0, m_nMinNum,m_nID) ;
	CADODatabase::ExecuteSQLStatement(StrSQL,DATABASESOURCE_TYPE_ACCESS) ;

}

void CPriorityRule::DeleteData()
{
	CString strSQL ;
	strSQL.Format(_T("DELETE FROM TB_AUTOROSTERRULE WHERE ID = %d"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL,DATABASESOURCE_TYPE_ACCESS);
}

void CPriorityRule::SetPriorityID( int nID )
{
	m_nPriorityID = nID;
}

int CPriorityRule::GetPriorityID()
{
	return m_nPriorityID;
}

void CPriorityRule::AddProcID( const CString& sProcID )
{
	m_vProcList.push_back(sProcID);
}

const std::vector<CString>& CPriorityRule::GetProcList()
{
	return m_vProcList;
}

void CPriorityRule::SetAdjacent( bool bAdjacent )
{
	m_bAdjacent = bAdjacent;
}

bool CPriorityRule::IsAdjacent()
{
	return m_bAdjacent;
}

void CPriorityRule::SetMinNum( int nNum )
{
	m_nMinNum = nNum;
}

int CPriorityRule::GetMinNum()
{
	return m_nMinNum;
}

void CPriorityRule::GetProcListName(CString& strName)
{
	strName = "";

	size_t nSize = m_vProcList.size();
	for (size_t i =0; i < nSize; i++)
	{
		strName += m_vProcList.at(i);
		strName += ",";
	}
	
}

void CPriorityRule::SetProcList( const CString& strName )
{
	CString strContent = strName;
	CString strLeft ="";
	int nPos =0;
	while (strContent.GetLength() >0)
	{
		nPos = strContent.Find(",");
		strLeft = strContent.Left(nPos);
		m_vProcList.push_back(strLeft);
		strContent = strContent.Right(strContent.GetLength() - nPos -1); 

	}
}

void CPriorityRule::DelProcID( const CString& sProcID )
{
	std::vector<CString>::iterator iter = std::find(m_vProcList.begin(), m_vProcList.end(), sProcID);

	if (iter != m_vProcList.end())
		m_vProcList.erase(iter);
}
///////////////////////////////////////////////////////////////////////
CAutoRosterRule::CAutoRosterRule(InputTerminal* pInputTerm)
:m_pInputTerm(pInputTerm)
, m_bBySchedule(false)
, m_bForEachFlight(false)
, m_bNumProc(false)
, m_nNumProc(1)
{
}

CAutoRosterRule::~CAutoRosterRule(void)
{
	size_t nCount = m_vDelPriorityRules.size();
	for (size_t i =0; i < nCount; i++)
	{
		delete m_vDelPriorityRules.at(i);
	}
	m_vDelPriorityRules.clear();

	nCount = m_vPriorityRules.size();
	for (size_t i =0; i < nCount; i++)
	{
		delete m_vPriorityRules.at(i);
	}
	m_vPriorityRules.clear();
}

const CMultiMobConstraint& CAutoRosterRule::GetPaxType()
{
	return m_paxType;
}

void CAutoRosterRule::SetPaxType( const CMultiMobConstraint& paxType )
{
	m_paxType = paxType;
}

void CAutoRosterRule::SetForEachFlight( bool bForEachFlight )
{
	m_bForEachFlight = bForEachFlight;
}

bool CAutoRosterRule::IsForEachFlight()
{
	return m_bForEachFlight;
}

void CAutoRosterRule::SetDefNumProc( bool bDef )
{
	m_bNumProc = bDef;
}

bool CAutoRosterRule::IsDefNumProc()
{
	return m_bNumProc;
}

void CAutoRosterRule::SetNumOfProc( int nNum )
{
	m_nNumProc = nNum;
}

int CAutoRosterRule::GetNumOfProc()
{
	return m_nNumProc;
}

const std::vector<CPriorityRule*>& CAutoRosterRule::GetPriorityRules()
{
	return m_vPriorityRules;
}

void CAutoRosterRule::AddPriorityRule( CPriorityRule* pRule )
{
	m_vPriorityRules.push_back(pRule);
}

void CAutoRosterRule::RemovePriorityRule( CPriorityRule* pRule )
{
	std::vector<CPriorityRule*>::iterator iter = std::find(m_vPriorityRules.begin(), m_vPriorityRules.end(), pRule);

	if (iter != m_vPriorityRules.end())
	{
		m_vDelPriorityRules.push_back(*iter);
		m_vPriorityRules.erase(iter);
	}
}

void CAutoRosterRule::ReadData( CADORecordset& recordSet )
{
	m_ProcID.SetStrDict( m_pInputTerm->inStrDict );

	CString ProID ;
	CString  paxTy ;
	int nBySched;
	int nForEachFlight ;
	int nDefNumProc;

	recordSet.GetFieldValue(_T("ID"),m_nID) ;
	recordSet.GetFieldValue(_T("PROCESSORID"),ProID);
	recordSet.GetFieldValue(_T("PAXTYPE"),paxTy) ;
	recordSet.GetFieldValue(_T("ISBYSCHEDTIME"),nBySched) ;
	recordSet.GetFieldValue(_T("ISFOREACHFLIGHT"),nForEachFlight) ;
	recordSet.GetFieldValue(_T("ISDEFNUMPROC"),nDefNumProc) ;
	recordSet.GetFieldValue(_T("NUMOFPROC"),m_nNumProc) ;


	m_ProcID.setID(ProID) ;

	CMobileElemConstraint paxType(m_pInputTerm);
	paxType.setConstraintWithVersion(paxTy);
	m_paxType.addConstraint(paxType);


	m_bBySchedule = nBySched >0 ? true:false;
	m_bForEachFlight = nForEachFlight >0 ? true:false;
	m_bNumProc = nDefNumProc >0 ? true:false;

	if (m_bNumProc)
	{
		long nRecordCount = -1;
		CADORecordset adoRecordset;
		CString  strSelectSQL ;
		strSelectSQL.Format(_T("SELECT * FROM TB_ROSTERRULEPRIORITY WHERE RULEID = %d"),m_nID) ;
		try
		{
			CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset,DATABASESOURCE_TYPE_ACCESS);
		}
		catch (CADOException e)
		{
			CString str = e.ErrorMessage() ;
			return ;
		}

		CPriorityRule* pPriority = NULL;
		while (!adoRecordset.IsEOF())
		{
			pPriority = new CPriorityRule(m_pInputTerm);
			pPriority->ReadData(adoRecordset);
			m_vPriorityRules.push_back(pPriority);
			adoRecordset.MoveNextData() ;
		}
	}

}

void CAutoRosterRule::SaveData()
{
	CString  StrSQL ;
	TCHAR paxty[1024] = {0} ;
	m_paxType.getConstraint(0)->WriteSyntaxStringWithVersion(paxty) ;	

	//if(m_nID >0)
	//	return UpdateData();

	StrSQL.Format(_T("INSERT INTO TB_AUTOROSTERRULE (PROCESSORID, PAXTYPE, ISBYSCHEDTIME, ISFOREACHFLIGHT, ISDEFNUMPROC, NUMOFPROC) \
					 VALUES ('%s', '%s', %d, %d, %d, %d)"),
					 m_ProcID.GetIDString(), paxty, m_bBySchedule?1:0, m_bForEachFlight?1:0, m_bNumProc?1:0, m_nNumProc) ;
	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(StrSQL,DATABASESOURCE_TYPE_ACCESS) ;

	size_t nCount = m_vDelPriorityRules.size();
	for (size_t i =0; i < nCount; i++)
	{
		CPriorityRule* pRule = m_vDelPriorityRules.at(i);
		pRule->DeleteData();
	}

	nCount = m_vPriorityRules.size();
	for (size_t i =0; i < nCount; i++)
	{
		CPriorityRule* pRule = m_vPriorityRules.at(i);
		pRule->SaveData(m_nID);
	}
}

void CAutoRosterRule::UpdateData()
{
	CString  StrSQL ;
	TCHAR paxty[1024] = {0} ;
	m_paxType.getConstraint(0)->WriteSyntaxStringWithVersion(paxty) ;	

	StrSQL.Format(_T("UPDATE TB_AUTOROSTERRULE SET PROCESSORID = '%s', PAXTYPE = '%s', ISBYSCHEDTIME = %d, ISFOREACHFLIGHT = %d, ISDEFNUMPROC = %d, NUMOFPROC = %d,\
					 WHERE ID = %d"),
					  m_ProcID.GetIDString(), paxty, m_bBySchedule?1:0, m_bForEachFlight?1:0, m_bNumProc?1:0, m_nNumProc, m_nID) ;
	CADODatabase::ExecuteSQLStatement(StrSQL,DATABASESOURCE_TYPE_ACCESS) ;
}

void CAutoRosterRule::DeleteData()
{
	CString strSQL ;
	strSQL.Format(_T("DELETE FROM TB_AUTOROSTERRULE WHERE ID = %d"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL,DATABASESOURCE_TYPE_ACCESS);
}

void CAutoRosterRule::SetProcessorID( const ProcessorID& procID )
{
	m_ProcID = procID;
}

const ProcessorID& CAutoRosterRule::GetProcessorID()
{
	return m_ProcID;
}

void CAutoRosterRule::SetBySchedule( bool bBySchedule )
{
	m_bBySchedule = bBySchedule;
}

bool CAutoRosterRule::IsBySchedule()
{
	return m_bBySchedule;
}

void CAutoRosterRule::CopyData( CAutoRosterRule* pRule )
{
	m_nID = pRule->m_nID;
	m_ProcID = pRule->m_ProcID;
	m_paxType = pRule->m_paxType;
	m_bForEachFlight = pRule->m_bForEachFlight;
	m_bNumProc = pRule->m_bNumProc;
	m_nNumProc = pRule->m_nNumProc;
	m_vPriorityRules.assign(pRule->m_vPriorityRules.begin(), pRule->m_vPriorityRules.end());
}

//bool CAutoRosterRule::AddProcRosterToProcAssignSchedules( std::vector<ProcAssignEntry*>& vProAssignEntries, const Flight* pFlight)
//{
//	if(vProAssignEntries.empty())
//		return false;
//
//	if (!m_bForEachFlight && !m_bBySchedule)  //for group flight at absolute time
//	{
//		ElapsedTime tOpen = GetRosterOpenTiem(NULL, 'A');
//		ElapsedTime tClose = GetRosterCloseTime(NULL, 'A');
//
//		ProcessorRosterSchedule* pProcSched = NULL;
//		ProcessorRoster* pRoster = NULL;	
//		if (!m_bNumProc)	//all
//		{
//			size_t nProcSize = vProAssignEntries.size();
//			for (size_t j =0; j < nProcSize; j++)
//			{
//				pProcSched = vProAssignEntries.at(j)->getSchedule();
//				if (!pProcSched->IsTimeRangeAvailable(tOpen, tClose, false))
//					continue;
//
//				pRoster = new ProcessorRoster;
//
//				pRoster->setOpenTime(tOpen);
//				pRoster->setCloseTime(tClose);
//				pRoster->setAbsOpenTime(tOpen);
//				pRoster->setAbsCloseTime(tClose);
//				pRoster->setAssignment(m_paxType) ;
//				pRoster->setFlag(1) ;
//				pRoster->setAssRelation(Inclusive) ;
//				pRoster->IsDaily(IsDaily());
//
//				pProcSched->addItem(pRoster);
//			}
//		}
//		else			//priority
//		{
//			AssignProcessorRosterAccordingToPrioirties(vProAssignEntries, m_paxType, tOpen, tClose, false);
//		}
//	}
//	else		//by schedule time
//	{
//			if (pFlight->isArriving())
//			{
//				FlightConstraint arrType = pFlight->getType('A');
//
//				if (((FlightConstraint*)m_paxType.getConstraint(0))->fits(arrType))
//					CreateProcessorRoster( vProAssignEntries, pFlight, 'A' );
//			}
//
//			if (pFlight->isDeparting())
//			{
//				FlightConstraint depType = pFlight->getType('D') ;
//				if(((FlightConstraint*)m_paxType.getConstraint(0))->fits(depType))
//					CreateProcessorRoster( vProAssignEntries, pFlight, 'D' );
//			}
//	}
//}

bool CAutoRosterRule::CreateProcessorRoster( std::vector<ProcAssignEntry*>& vProAssignEntries, Flight* pFlight, char mode ,std::vector<ProcessorRoster *>& vCreateRoster)
{
	ElapsedTime tOpen = GetRosterOpenTiem(pFlight, mode);
	ElapsedTime tClose = GetRosterCloseTime(pFlight, mode);

	ElapsedTime tFlightTime;
	if (mode == 'A')
		tFlightTime = pFlight->getArrTime();
	else
		tFlightTime =  pFlight->getDepTime();

	if (!m_bBySchedule)
	{
		if (tOpen < tFlightTime && tClose< tFlightTime)		//flight time not in time range
			return false;

		if (tOpen > tFlightTime && tClose > tFlightTime)		//flight time not in time range
			return false;
	}
	
	ProcessorRosterSchedule* pProcSched = NULL;
	ProcessorRoster* pRoster = NULL;	

	CMobileElemConstraint fltType = *m_paxType.getConstraint(0);
	fltType.MergeFlightConstraint(&(pFlight->getType(mode)));

	CMultiMobConstraint mobType;
	mobType.addConstraint(fltType);

	size_t nProcSize = vProAssignEntries.size();
	if(!m_bForEachFlight && m_bBySchedule)	//inclusive by schedule time
	{
		if (!m_bNumProc)	//all
		{
			bool bAssigned = false;
			for (size_t j =0; j < nProcSize; j++)
			{
				pProcSched = vProAssignEntries.at(j)->getSchedule();
				if (!pProcSched->IsTimeRangeAvailable(tOpen, tClose, false))
					continue;

				pRoster = new ProcessorRoster;

				pRoster->setOpenTime(tOpen);
				pRoster->setCloseTime(tClose);
				pRoster->setAbsOpenTime(tOpen);
				pRoster->setAbsCloseTime(tClose);
				pRoster->setAssignment(m_paxType) ;
				pRoster->setFlag(1) ;
				pRoster->setAssRelation(Inclusive) ;
				pRoster->IsDaily(IsDaily());

				pProcSched->addItem(pRoster);
				
				vCreateRoster.push_back(pRoster);

				bAssigned = true;
			}
			return bAssigned;

		}
		else			//priority
		{
			return AssignProcessorRosterAccordingToPrioirties(vProAssignEntries, m_paxType, tOpen, tClose, false, vCreateRoster);
		}
	}
	else			//exclusive
	{
		if (!m_bNumProc)	//all
		{
			bool bAssigned = false;
			for (size_t j =0; j < nProcSize; j++)
			{
				pProcSched = vProAssignEntries.at(j)->getSchedule();
				if (!pProcSched->IsTimeRangeAvailable(tOpen, tClose, true))
					continue;

				pRoster = new ProcessorRoster;

				pRoster->setOpenTime(tOpen);
				pRoster->setCloseTime(tClose);
				pRoster->setAbsOpenTime(tOpen);
				pRoster->setAbsCloseTime(tClose);
				pRoster->setAssignment(mobType) ;
				pRoster->setFlag(1) ;
				pRoster->setAssRelation(Exclusive) ;
				pRoster->IsDaily(IsDaily());

				pProcSched->addItem(pRoster);
				bAssigned = true;

				vCreateRoster.push_back(pRoster);
			}

			return bAssigned;
		}
		else			//priority
		{
			return AssignProcessorRosterAccordingToPrioirties(vProAssignEntries, mobType, tOpen, tClose, true, vCreateRoster);
		}
	}

	return false;
}

bool ProcAssignEntryCompare(ProcAssignEntry* pEntry1, ProcAssignEntry* pEntry2)
{
	CString strNum1 = pEntry1->getID()->GetLeafName();
	CString strNum2 = pEntry2->getID()->GetLeafName();

	// ensure strNum id is Numeric
	for( int i=0; i<strNum1.GetLength(); i++ )
	{
		if (!isNumeric( strNum1[i] ))
			return pEntry1->getID()->GetIDString() <= pEntry2->getID()->GetIDString();
	}

	for( int i=0; i<strNum2.GetLength(); i++ )
	{
		if (!isNumeric( strNum2[i] ))
			return pEntry1->getID()->GetIDString() <= pEntry2->getID()->GetIDString();
	}

	if (atoi( strNum1 ) > atoi( strNum2 ))
		return false;

	return true;
}

bool CAutoRosterRule::AssignProcessorRosterAccordingToPrioirties( std::vector<ProcAssignEntry*>& vProAssignEntries, 
																	const CMultiMobConstraint& mobType, 
																	const ElapsedTime& tOpen, 
																	const ElapsedTime& tClose, 
																	bool bExclusive, 
																	std::vector<ProcessorRoster *>& vCreateRoster)
{
	int nLeftProc = m_nNumProc;
	size_t nSize = m_vPriorityRules.size();
	size_t nEntrySize = vProAssignEntries.size();

	std::vector<ProcAssignEntry*> vFitEntries;

	ProcessorRosterSchedule* pProcSched = NULL;
	ProcessorRoster* pRoster = NULL;	

	std::vector<ProcessorRosterSchedule*> vSchedules;
	for (size_t i =0; i < nSize; i++)
	{
		CPriorityRule* pPriority = m_vPriorityRules.at(i);
		std::vector<CString> vProcList = pPriority->GetProcList();

		int nMin = pPriority->GetMinNum(); 
		size_t nProc = vProcList.size();

		std::vector<ProcessorRosterSchedule*> vPriSched;
		int nProcCount = 0;

		if (pPriority->IsAdjacent())
		{
			for (size_t j =0; j < nProc; j++)
			{
				ProcessorID procID;
				procID.SetStrDict(m_pInputTerm->inStrDict);
				procID.setID(vProcList.at(j));

				vFitEntries.clear();
				for (size_t m =0; m < nEntrySize; m++)
				{
					ProcAssignEntry* pEntry = vProAssignEntries.at(m);
					if (procID.idFits(*(pEntry->getID())))
						vFitEntries.push_back(pEntry);
				}

				std::sort(vFitEntries.begin(), vFitEntries.end(), ProcAssignEntryCompare);
				nProcCount += GetAvailableAdjacentProcessorSchedules(vFitEntries,vPriSched, tOpen, tClose, bExclusive, pPriority->GetMinNum(), nLeftProc);

				if (vPriSched.empty())
					continue;

				if (nLeftProc ==nProcCount)
					break;
			}
		}
		else
		{
			for (size_t j =0; j < nProc; j++)
			{
				ProcessorID procID;
				procID.SetStrDict(m_pInputTerm->inStrDict);
				procID.setID(vProcList.at(j));

				vFitEntries.clear();
				for (size_t m =0; m < nEntrySize; m++)
				{
					ProcAssignEntry* pEntry = vProAssignEntries.at(m);
					if (procID.idFits(*(pEntry->getID())))
						vFitEntries.push_back(pEntry);
				}

				size_t nCount = vFitEntries.size();
				for (size_t m =0; m < nCount; m++)
				{
					pProcSched = vFitEntries.at(m)->getSchedule();
					if (!pProcSched->IsTimeRangeAvailable(tOpen, tClose,bExclusive))
						continue;

					vPriSched.push_back(pProcSched);
					nMin--;
					nProcCount++;

					if (nProcCount ==nLeftProc)
						break;
					
				}
			}

			if (nMin >0)		//not satisfy the min condition
			{
				vPriSched.clear();		
				continue;;
			}
		}

		nLeftProc -= nProcCount;

		vSchedules.insert(vSchedules.end(),vPriSched.begin(), vPriSched.end());

		if (nLeftProc ==0)
			break;
	}

	if (nLeftProc >0)		//not satisfied
	{
		vSchedules.clear();
		return false;
	}

	size_t nSched = vSchedules.size();
	for (size_t k =0; k <nSched; k++)
	{
		pProcSched = vSchedules.at(k);

		pRoster = new ProcessorRoster;

		pRoster->setOpenTime(tOpen);
		pRoster->setCloseTime(tClose);
		pRoster->setAbsOpenTime(tOpen);
		pRoster->setAbsCloseTime(tClose);
		pRoster->setAssignment(mobType) ;
		pRoster->setFlag(1) ;
		if (bExclusive)
			pRoster->setAssRelation(Exclusive) ;
		else
			pRoster->setAssRelation(Inclusive) ;
		pRoster->IsDaily(IsDaily());

		pProcSched->addItem(pRoster);

		vCreateRoster.push_back(pRoster);
	}
	return true;
}

int CAutoRosterRule::GetAvailableAdjacentProcessorSchedules( const std::vector<ProcAssignEntry*>& vProEntries, 
															 std::vector<ProcessorRosterSchedule*>& vSchedules, 
															 const ElapsedTime& tOpen, 
															 const ElapsedTime& tClose, 
															 bool bExclusive,
															 int nMinAdjacent,
															 int nMaxSize)
{
	int nMin = nMinAdjacent;

	size_t nCount = vProEntries.size();
	if ((int)nCount < nMin)		//not satisfied the min condition
		return 0;

	ProcessorRosterSchedule* pSched = NULL;
	int nProcCount =0;
	for (size_t m =0; m < nCount; m++)
	{
		pSched = vProEntries.at(m)->getSchedule();
		if (!pSched->IsTimeRangeAvailable(tOpen, tClose,bExclusive))
		{
			if (nMin >0)
			{
				vSchedules.clear();
				nMin = nMinAdjacent;
				nProcCount = 0;
				continue;
			}
		}

		vSchedules.push_back(pSched);
		nMin--;
		nProcCount++;

		if (nProcCount == nMaxSize)
			return nProcCount;
	}

	if (nMin >0)
	{
		vSchedules.clear();
		nProcCount = 0;
	}

	return nProcCount;
}

bool CAutoRosterRule::IsDaily()
{
	return false;
}

bool CAutoRosterRule::operator<( const CAutoRosterRule& rosterRule ) const
{
	if(rosterRule.m_paxType.getCount() <= 0)
		return true;
	if (m_paxType.getCount() <= 0)
		return true;
	
	const CMobileElemConstraint* pRosterPaxType = rosterRule.m_paxType.getConstraint(0); 
	const CMobileElemConstraint* pIterPaxType = m_paxType.getConstraint(0);

	if (!pRosterPaxType || !pIterPaxType)
		return true;
	
	if (*pIterPaxType < *pRosterPaxType)
	{
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////

CRosterRulesBySchedTime::CRosterRulesBySchedTime( InputTerminal* pInputTerm )
:CAutoRosterRule(pInputTerm)
,m_eOpenType(TIMEINTERVALTYPE::Before)
,m_eCloseType(TIMEINTERVALTYPE::Before)
{
	m_tOpenTime = ElapsedTime(2*60*60L);
	m_tCloseTime = ElapsedTime(30*60L);
}

CRosterRulesBySchedTime::~CRosterRulesBySchedTime()
{

}

void CRosterRulesBySchedTime::ReadData(CADORecordset& recordSet)
{
	CAutoRosterRule::ReadData(recordSet);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CString  strSelectSQL ;
	strSelectSQL.Format(_T("SELECT * FROM TB_ROSTERRULEBYSCHEDTIME WHERE RULEID = %d"),m_nID) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset,DATABASESOURCE_TYPE_ACCESS);
	}
	catch (CADOException e)
	{
		CString str = e.ErrorMessage() ;
		return ;
	}

	if (!adoRecordset.IsEOF())
	{
		long lOpenTime;
		long lCloseTime;
		int nOpenType;
		int nCloseType;

		adoRecordset.GetFieldValue(_T("OPENTIME"),lOpenTime) ;
		adoRecordset.GetFieldValue(_T("OPENINTERVALTYPE"),nOpenType) ;
		adoRecordset.GetFieldValue(_T("CLOSETIME"),lCloseTime) ;
		adoRecordset.GetFieldValue(_T("CLOSEINTERVALTYPE"),nCloseType) ;

		m_tOpenTime = ElapsedTime(lOpenTime);
		m_tCloseTime = ElapsedTime(lCloseTime);
		m_eOpenType = (TIMEINTERVALTYPE)nOpenType;
		m_eCloseType = (TIMEINTERVALTYPE)nCloseType;
	}


}

void CRosterRulesBySchedTime::SaveData()
{
	//if (m_nID > -1)
	//{
	//	return UpdateData();
	//}

	CAutoRosterRule::SaveData();

	CString StrSQL;
	StrSQL.Format(_T("INSERT INTO TB_ROSTERRULEBYSCHEDTIME (RULEID, OPENTIME, OPENINTERVALTYPE, CLOSETIME, CLOSEINTERVALTYPE) \
					 VALUES (%d, %d, %d, %d, %d)"),
					 m_nID, m_tOpenTime.asSeconds(),(int)m_eOpenType, m_tCloseTime.asSeconds(), (int)m_eCloseType) ;
	CADODatabase::ExecuteSQLStatementAndReturnScopeID(StrSQL,DATABASESOURCE_TYPE_ACCESS) ;
}

void CRosterRulesBySchedTime::DeleteData()
{
	CString strSQL ;
	strSQL.Format(_T("DELETE FROM TB_ROSTERRULEBYSCHEDTIME WHERE RULEID = %d"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL,DATABASESOURCE_TYPE_ACCESS);

	CAutoRosterRule::DeleteData();
}

void CRosterRulesBySchedTime::UpdateData()
{
	CAutoRosterRule::UpdateData();

	CString StrSQL;
	StrSQL.Format(_T("UPDATE TB_ROSTERRULEBYSCHEDTIME SET OPENTIME = %d, OPENINTERVALTYPE = %d, CLOSETIME = %d, CLOSEINTERVALTYPE = %d,\
					 WHERE RULEID = %d"),
					m_tOpenTime.asSeconds(),(int)m_eOpenType, m_tCloseTime.asSeconds(), (int)m_eCloseType, m_nID) ;
	CADODatabase::ExecuteSQLStatement(StrSQL,DATABASESOURCE_TYPE_ACCESS) ;
}

void CRosterRulesBySchedTime::SetOpenTime( const ElapsedTime& tTime )
{
	m_tOpenTime = tTime;
}

ElapsedTime CRosterRulesBySchedTime::GetOpenTime()
{
	return m_tOpenTime;
}

void CRosterRulesBySchedTime::SetCloseTime( const ElapsedTime& tTime )
{
	m_tCloseTime = tTime;
}

ElapsedTime CRosterRulesBySchedTime::GetCloseTime()
{
	return m_tCloseTime;
}

void CRosterRulesBySchedTime::SetOpenIntervalTimeType( TIMEINTERVALTYPE eType )
{
	m_eOpenType = eType;
}

CRosterRulesBySchedTime::TIMEINTERVALTYPE CRosterRulesBySchedTime::GetOpenIntervalTimeType()
{
	return m_eOpenType;
}

void CRosterRulesBySchedTime::SetCloseIntervalTimeType( TIMEINTERVALTYPE eType )
{
	m_eCloseType = eType;
}

CRosterRulesBySchedTime::TIMEINTERVALTYPE CRosterRulesBySchedTime::GetCloseIntervalTimeType()
{
	return m_eCloseType;
}

void CRosterRulesBySchedTime::ReadRulesFromOldDB( CADORecordset& recordSet )
{
	m_ProcID.SetStrDict( m_pInputTerm->inStrDict );

	int  id = -1;
	CString  paxTy ;
	CString strStartTime ;
	CString strEndTime ;
	int nForEachFlight ;
	int nDaily ;
	int nProcNum ;
	CString ProID ;
	int nOpenType = 0;
	int nCloseType = 0;

	recordSet.GetFieldValue(_T("ID"),m_nID) ;
	recordSet.GetFieldValue(_T("PAXTY"),paxTy) ;
	recordSet.GetFieldValue(_T("STARTTIME"),strStartTime) ;
	recordSet.GetFieldValue(_T("ENDTIME"),strEndTime) ;
	recordSet.GetFieldValue(_T("RELATION"),nForEachFlight) ;
	recordSet.GetFieldValue(_T("ISDAILY"),nDaily) ;
	recordSet.GetFieldValue(_T("PROCESSORNUM"),nProcNum) ;
	recordSet.GetFieldValue(_T("PROCESSID"),ProID) ;
	recordSet.GetFieldValue(_T("OPEN_ADVANCE_OR_NOT"),nOpenType) ;
	recordSet.GetFieldValue(_T("CLOSE_ADVANCE_OR_NOT"),nCloseType) ;

	m_ProcID.setID(ProID) ;

	CMobileElemConstraint paxType(m_pInputTerm);
	paxType.setConstraintWithVersion(paxTy);
	m_paxType.addConstraint(paxType);

	ElapsedTime opentime ;
	ElapsedTime closetime ;
	m_tCloseTime.SetTime(strEndTime) ;
	m_tOpenTime.SetTime(strStartTime) ;

	m_bForEachFlight = nForEachFlight>0?true:false ;
	m_bNumProc = false;
	m_eOpenType = (TIMEINTERVALTYPE)nOpenType;
	m_eCloseType = (TIMEINTERVALTYPE)nCloseType;

}

ElapsedTime CRosterRulesBySchedTime::GetRosterOpenTiem( Flight* pflight, char mode )
{
	ElapsedTime tTime;
	if (mode == 'D')
		tTime = pflight->getDepTime();
	else
		tTime = pflight->getArrTime();

	if (m_eOpenType == TIMEINTERVALTYPE::Before)
		return tTime - m_tOpenTime;
	
	return tTime + m_tOpenTime;
}

ElapsedTime CRosterRulesBySchedTime::GetRosterCloseTime( Flight* pflight, char mode )
{
	ElapsedTime tTime;
	if (mode == 'D')
		tTime = pflight->getDepTime();
	else
		tTime = pflight->getArrTime();

	if (m_eCloseType == TIMEINTERVALTYPE::Before)
		return tTime - m_tCloseTime;

	return tTime + m_tCloseTime;
}
///////////////////////////////////////////////////////////////////////////////////////////////
CRosterRulesByAbsoluteTime::CRosterRulesByAbsoluteTime( InputTerminal* pInputTerm )
:CAutoRosterRule(pInputTerm)
,m_bDaily(true)
{
	m_tOpenTime = 0L;
	m_tCloseTime = 24*3600L -1L;
}

CRosterRulesByAbsoluteTime::~CRosterRulesByAbsoluteTime()
{

}

void CRosterRulesByAbsoluteTime::ReadData(CADORecordset& recordSet)
{
	CAutoRosterRule::ReadData(recordSet);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CString  strSelectSQL ;
	strSelectSQL.Format(_T("SELECT * FROM TB_ROSTERRULEBYABSOLUTETIME WHERE RULEID = %d"),m_nID) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset,DATABASESOURCE_TYPE_ACCESS);
	}
	catch (CADOException e)
	{
		CString str = e.ErrorMessage() ;
		return ;
	}

	if (!adoRecordset.IsEOF())
	{
		long lOpenTime;
		long lCloseTime;
		int nDaily;

		adoRecordset.GetFieldValue(_T("OPENTIME"),lOpenTime) ;
		adoRecordset.GetFieldValue(_T("CLOSETIME"),lCloseTime) ;
		adoRecordset.GetFieldValue(_T("ISDAILY"),nDaily) ;

		m_tOpenTime = ElapsedTime(lOpenTime);
		m_tCloseTime = ElapsedTime(lCloseTime);
		m_bDaily = nDaily >0 ? true:false;

	}

}

void CRosterRulesByAbsoluteTime::SaveData()
{
	//if (m_nID > -1)
	//{
	//	return UpdateData();
	//}

	CAutoRosterRule::SaveData();

	CString StrSQL;
	StrSQL.Format(_T("INSERT INTO TB_ROSTERRULEBYABSOLUTETIME (RULEID, OPENTIME, CLOSETIME, ISDAILY) \
					 VALUES (%d, %d, %d, %d)"),
					 m_nID, m_tOpenTime.asSeconds(),m_tCloseTime.asSeconds(), m_bDaily?1:0) ;
	CADODatabase::ExecuteSQLStatementAndReturnScopeID(StrSQL,DATABASESOURCE_TYPE_ACCESS) ;
}

void CRosterRulesByAbsoluteTime::DeleteData()
{
	CString strSQL ;
	strSQL.Format(_T("DELETE FROM TB_ROSTERRULEBYABSOLUTETIME WHERE RULEID = %d"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL,DATABASESOURCE_TYPE_ACCESS);

	CAutoRosterRule::DeleteData();
}

void CRosterRulesByAbsoluteTime::UpdateData()
{
	CAutoRosterRule::UpdateData();

	CString StrSQL;
	StrSQL.Format(_T("UPDATE TB_ROSTERRULEBYABSOLUTETIME SET OPENTIME = %d, CLOSETIME = %d, ISDAILY = %d,\
					 WHERE RULEID = %d"),
					 m_tOpenTime.asSeconds(),m_tCloseTime.asSeconds(), m_bDaily?1:0, m_nID) ;
	CADODatabase::ExecuteSQLStatement(StrSQL,DATABASESOURCE_TYPE_ACCESS) ;
}

void CRosterRulesByAbsoluteTime::SetOpenTime( const ElapsedTime& tTime )
{
	m_tOpenTime = tTime;
}

ElapsedTime CRosterRulesByAbsoluteTime::GetOpenTime()
{
	return m_tOpenTime;
}

void CRosterRulesByAbsoluteTime::SetCloseTime( const ElapsedTime& tTime )
{
	m_tCloseTime = tTime;
}

ElapsedTime CRosterRulesByAbsoluteTime::GetCloseTime()
{
	return m_tCloseTime;
}

void CRosterRulesByAbsoluteTime::SetDaily( bool bDaily )
{
	m_bDaily = bDaily;
}

bool CRosterRulesByAbsoluteTime::IsDaily()
{
	return m_bDaily;
}

void CRosterRulesByAbsoluteTime::ReadDataFromFile( ArctermFile& p_file)
{
	int nDaily;
	p_file.getInteger(nDaily) ;
	m_bDaily = nDaily >0?true:false;

	p_file.getTime(m_tOpenTime) ;
	p_file.getTime(m_tCloseTime) ;
	m_paxType.readConstraints(p_file,m_pInputTerm) ;

}

void CRosterRulesByAbsoluteTime::ReadRulesFromOldDB( CADORecordset& recordSet )
{
	m_ProcID.SetStrDict( m_pInputTerm->inStrDict );

	int  id = -1;
	CString  paxTy ;
	CString strStartTime ;
	CString strEndTime ;
	int nForEachFlight ;
	int nDaily ;
	int nProcNum ;
	CString ProID ;

	recordSet.GetFieldValue(_T("ID"),m_nID) ;
	recordSet.GetFieldValue(_T("PAXTY"),paxTy) ;
	recordSet.GetFieldValue(_T("STARTTIME"),strStartTime) ;
	recordSet.GetFieldValue(_T("ENDTIME"),strEndTime) ;
	recordSet.GetFieldValue(_T("RELATION"),nForEachFlight) ;
	recordSet.GetFieldValue(_T("ISDAILY"),nDaily) ;
	recordSet.GetFieldValue(_T("PROCESSORNUM"),nProcNum) ;
	recordSet.GetFieldValue(_T("PROCESSID"),ProID) ;

	m_ProcID.setID(ProID) ;

	CMobileElemConstraint paxType(m_pInputTerm);
	paxType.setConstraintWithVersion(paxTy);
	m_paxType.addConstraint(paxType);

	ElapsedTime opentime ;
	ElapsedTime closetime ;
	m_tCloseTime.SetTime(strEndTime) ;
	m_tOpenTime.SetTime(strStartTime) ;

	m_bForEachFlight = nForEachFlight>0?true:false ;
	m_bNumProc = false;
	m_bDaily = nDaily>0?true:false;

}

ElapsedTime CRosterRulesByAbsoluteTime::GetRosterOpenTiem( Flight* pflight, char mode )
{
	return m_tOpenTime;
}

ElapsedTime CRosterRulesByAbsoluteTime::GetRosterCloseTime( Flight* pflight, char mode )
{
	return m_tCloseTime;
}
///////////////////////////////////////////////////////////////////////////////////////////
CProcRosterRules::CProcRosterRules( InputTerminal* pInputTerm )
:DataSet(RosterRulesFile,2.4f)
{
   SetInputTerminal(pInputTerm) ;
}

CProcRosterRules::~CProcRosterRules()
{
	size_t nCount = m_vDelData.size();
	for (size_t i =0; i < nCount; i++)
	{
		delete m_vDelData.at(i);
	}
	m_vDelData.clear();

	nCount = m_vData.size();
	for (size_t i =0; i < nCount; i++)
	{
		delete m_vData.at(i);
	}
	m_vData.clear();
}

void CProcRosterRules::readData( ArctermFile& p_file )
{
	ProcessorID id;
	id.SetStrDict(m_pInTerm->inStrDict);

	ReadDataFromDB(id);
}

bool RosterRulePaxTypeCompare(CAutoRosterRule* pRosterRule1,CAutoRosterRule* pRosterRule2)
{
	if (!pRosterRule1 || !pRosterRule2)
		return true;

	return *pRosterRule1 < *pRosterRule2;
}

void CProcRosterRules::readObsoleteData( ArctermFile& p_file )
{
	assert( m_pInTerm );
	clear();

	CAutoRosterRule *pRule = NULL; 

	ProcessorIDList idList;
	idList.ownsElements( 1 );//cause idList.readIDList (p_file, m_pInTerm ); will alloc memory 
	ProcessorRoster procAssign( m_pInTerm );
	StringList grouplist ;
	m_pInTerm->GetProcessorList()->getAllGroupNames(grouplist,-1) ;

	CString name ;
	ProcessorID id;
	id.SetStrDict( GetInputTerminal()->inStrDict );

	while (p_file.getLine())
	{
		idList.readIDList (p_file, m_pInTerm );
		name = idList.getID(0)->GetIDString() ;
		id.setID(name) ;

		GroupIndex index =  m_pInTerm->GetProcessorList()->getGroupIndex(id) ;
		if (index.end == -1 || index.start == -1 )
			continue ;

		pRule = new CRosterRulesByAbsoluteTime(GetInputTerminal()) ;
		((CRosterRulesByAbsoluteTime*)pRule)->ReadDataFromFile(p_file) ;
		pRule->SetProcessorID(id);
		m_vData.push_back(pRule);
	}
	
	std::sort(m_vData.begin(),m_vData.end(),RosterRulePaxTypeCompare);
}

void CProcRosterRules::writeData( ArctermFile& p_file ) const
{

	CProcRosterRules* pData =  const_cast<CProcRosterRules*>(this) ;
	pData->SaveDataToDB();
}


void CProcRosterRules::ReadDataFromDB(const ProcessorID& procID)
{

	ReadDataFromOldDB(procID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CString  strSelectSQL ;
	strSelectSQL.Format(_T("SELECT * FROM TB_AUTOROSTERRULE WHERE PROCESSORID = '%s'"), procID.GetIDString()) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset,DATABASESOURCE_TYPE_ACCESS);
	}
	catch (CADOException e)
	{
		CString str = e.ErrorMessage() ;
		return ;
	}

	CAutoRosterRule* pRule = NULL;
	while (!adoRecordset.IsEOF())
	{
		int nBySched = 0;
		adoRecordset.GetFieldValue(_T("ISBYSCHEDTIME"),nBySched);
		if (nBySched ==0)		//not by schedule time
		{
			pRule = new CRosterRulesByAbsoluteTime(m_pInTerm);
			pRule->ReadData(adoRecordset);			
		}
		else
		{
			pRule = new CRosterRulesBySchedTime(m_pInTerm);
			pRule->ReadData(adoRecordset);
		}
		m_vData.push_back(pRule);
		adoRecordset.MoveNextData() ;
	}
	std::sort(m_vData.begin(),m_vData.end(),RosterRulePaxTypeCompare);
}

void CProcRosterRules::SaveDataToDB()
{

	size_t nCount = m_vDelData.size();
	for (size_t i =0; i < nCount; i++)
	{
		CAutoRosterRule* pRule = m_vDelData.at(i);
		pRule->DeleteData();
	}

	nCount = m_vData.size();
	for (size_t i =0; i < nCount; i++)
	{
		CAutoRosterRule* pRule = m_vData.at(i);
		pRule->SaveData();
	}
}

void CProcRosterRules::AddData( CAutoRosterRule* pData )
{
	std::vector<CAutoRosterRule*>::iterator iter = std::find(m_vData.begin(), m_vData.end(),pData);

	if (iter == m_vData.end())
		m_vData.push_back(pData);
}

void CProcRosterRules::DelData( CAutoRosterRule* pData )
{
	std::vector<CAutoRosterRule*>::iterator iter = std::find(m_vData.begin(), m_vData.end(),pData);

	if (iter != m_vData.end())
	{
		m_vDelData.push_back(pData);
		m_vData.erase(iter);
	}
}

int CProcRosterRules::GetDataCount()
{
	return (int)m_vData.size();
}

CAutoRosterRule* CProcRosterRules::GetData( int idx )
{
	if (idx <0 || idx > GetDataCount()-1)
		return NULL;

	return m_vData.at(idx);
}

void CProcRosterRules::clear()
{
	m_vDelData.insert(m_vDelData.end(), m_vData.begin(),m_vData.end());
	m_vData.clear();
}

void CProcRosterRules::ReadDataFromOldDB(const ProcessorID& procID)
{
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CString  strSelectSQL ;
	strSelectSQL.Format(_T("SELECT * FROM TB_ROSTER_RULES WHERE PROCESSID = '%s'"),procID.GetIDString()) ;

	try
	{
		CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset,DATABASESOURCE_TYPE_ACCESS);
	}
	catch (CADOException e)
	{
		CString str = e.ErrorMessage() ;
		return ;
	}

	CAutoRosterRule* pRule = NULL ;
	while (!adoRecordset.IsEOF())
	{
		int nType = 0;
		adoRecordset.GetFieldValue(_T("TYID"),nType);
		if(nType == TY_RULE)
		{
			pRule = new CRosterRulesByAbsoluteTime(GetInputTerminal());
			pRule->ReadRulesFromOldDB(adoRecordset);
		}
		else
		{
			pRule = new CRosterRulesBySchedTime(GetInputTerminal()) ;
			pRule->ReadRulesFromOldDB(adoRecordset);
		}

		m_vData.push_back(pRule) ;
		adoRecordset.MoveNextData() ;
	}

	//Clear old db
	strSelectSQL.Format(_T("DELETE FROM TB_ROSTER_RULES WHERE PROCESSID = '%s'"),procID.GetIDString());
	try
	{
		CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset,DATABASESOURCE_TYPE_ACCESS);
	}
	catch (CADOException e)
	{
		CString str = e.ErrorMessage() ;
		return ;
	}

}

bool CProcRosterRules::FindData( CAutoRosterRule* pData )
{
	std::vector<CAutoRosterRule*>::iterator iter = std::find(m_vData.begin(), m_vData.end(),pData);

	if (iter != m_vData.end())
		return true;

	return false;
}

void CProcRosterRules::DoAutoRosterAssignment( std::vector<ProcAssignEntry*>& vProcAssignEntries , RosterAssignFlightList& unassignList)
{
	unassignList.RemoveAll();

	FlightSchedule* pSchedule =  m_pInTerm->flightSchedule;
	if(pSchedule == NULL)
		return ;

	std::vector<Flight*> vAssignedArrFlt;
	std::vector<Flight*> vAssignedDepFlt;

	size_t nCount = m_vData.size();
	for (size_t i = 0 ; i < nCount; i++)
	{
		CAutoRosterRule* pRule = m_vData.at(i);

		if (!pRule->IsForEachFlight() && !pRule->IsBySchedule())  //for group flight at absolute time
		{
			ElapsedTime tOpen = pRule->GetRosterOpenTiem(NULL, 'A');
			ElapsedTime tClose = pRule->GetRosterCloseTime(NULL, 'A');

			ProcessorRosterSchedule* pProcSched = NULL;
			ProcessorRoster* pRoster = NULL;	
			
			std::vector<ProcessorRoster *> vCreateRoster;
			if (!pRule->IsDefNumProc())	//all
			{	
				size_t nProcSize = vProcAssignEntries.size();
				for (size_t j =0; j < nProcSize; j++)
				{	

					pProcSched = vProcAssignEntries.at(j)->getSchedule();
					if (!pProcSched->IsTimeRangeAvailable(tOpen, tClose, false))
						continue;

					pRoster = new ProcessorRoster;

					pRoster->setOpenTime(tOpen);
					pRoster->setCloseTime(tClose);
					pRoster->setAbsOpenTime(tOpen);
					pRoster->setAbsCloseTime(tClose);
					pRoster->setAssignment(pRule->GetPaxType()) ;
					pRoster->setFlag(1) ;
					pRoster->setAssRelation(Inclusive) ;
					pRoster->IsDaily(pRule->IsDaily());

					pProcSched->addItem(pRoster);
					vCreateRoster.push_back(pRoster);
				}
			}
			else			//priority
			{
				pRule->AssignProcessorRosterAccordingToPrioirties(vProcAssignEntries, pRule->GetPaxType(), tOpen, tClose, false, vCreateRoster);
			}

			const CMultiMobConstraint& paxType = pRule->GetPaxType();
			int nFlightCount = pSchedule->getCount();
			for( int i=0; i< nFlightCount;  i++ )
			{
				Flight* pFlight = pSchedule->getItem( i );
				if (pFlight->isArriving())
				{
					if (std::find(vAssignedArrFlt.begin(), vAssignedArrFlt.end(),pFlight) == vAssignedArrFlt.end())		//unassigned
					{
						FlightConstraint arrType = pFlight->getType('A');

						if (((FlightConstraint*)paxType.getConstraint(0))->fits(arrType))
						{
							if(vCreateRoster.size() > 0)
							{
								vAssignedArrFlt.push_back(pFlight);
								RosterAssignFlight *pRosterAssFlight = unassignList.AddFlight(&paxType, pFlight, RosterAssignFlight::emOp_Arrival, RosterAssignFlight::emAss_Sucess, pRule);
								pRosterAssFlight->AddRoster(vCreateRoster);
							}
							else
							{
								unassignList.AddFlight(&paxType, pFlight, RosterAssignFlight::emOp_Arrival, RosterAssignFlight::emAss_Failed, pRule);
							}

						}
						else
						{
							unassignList.AddFlight(NULL, pFlight, RosterAssignFlight::emOp_Arrival, RosterAssignFlight::emAss_NotInclude, NULL);
						}
					}
				}
				if(pFlight->isDeparting())
				{

					if (std::find(vAssignedDepFlt.begin(), vAssignedDepFlt.end(),pFlight) == vAssignedDepFlt.end())		//unassigned
					{
						FlightConstraint depType = pFlight->getType('D') ;

						if(((FlightConstraint*)paxType.getConstraint(0))->fits(depType))
						{
							if(vCreateRoster.size() > 0)
							{
								vAssignedDepFlt.push_back(pFlight);
								RosterAssignFlight *pRosterAssFlight = unassignList.AddFlight(&paxType, pFlight, RosterAssignFlight::emOp_Departure, RosterAssignFlight::emAss_Sucess, pRule);
								pRosterAssFlight->AddRoster(vCreateRoster);		
							}
							else
							{
								unassignList.AddFlight(&paxType, pFlight, RosterAssignFlight::emOp_Departure, RosterAssignFlight::emAss_Failed, pRule);
							}
						}
						else
						{
							unassignList.AddFlight(NULL, pFlight, RosterAssignFlight::emOp_Departure, RosterAssignFlight::emAss_NotInclude, NULL);
						}
					}
				}
			}
		

		}
		else		//by schedule time or roster for each flight
		{		
			const CMultiMobConstraint& paxType = pRule->GetPaxType();
			int nFlightCount = pSchedule->getCount();
			for( int i=0; i< nFlightCount;  i++ )
			{
				Flight* pFlight = pSchedule->getItem( i );
				if (pFlight->isArriving())
				{
					if (std::find(vAssignedArrFlt.begin(), vAssignedArrFlt.end(),pFlight) == vAssignedArrFlt.end())		//unassigned
					{
						FlightConstraint arrType = pFlight->getType('A');

						if (((FlightConstraint*)paxType.getConstraint(0))->fits(arrType))
						{
							std::vector<ProcessorRoster *> vCreateRoster;
							if( pRule->CreateProcessorRoster( vProcAssignEntries, pFlight, 'A' ,vCreateRoster))
							{
								vAssignedArrFlt.push_back(pFlight);
								RosterAssignFlight *pRosterAssFlight = unassignList.AddFlight(&paxType, pFlight, RosterAssignFlight::emOp_Arrival,RosterAssignFlight::emAss_Sucess, pRule);
								pRosterAssFlight->AddRoster(vCreateRoster);
							}
							else
							{
								unassignList.AddFlight(&paxType, pFlight, RosterAssignFlight::emOp_Arrival, RosterAssignFlight::emAss_Failed, pRule);
							}
						}
						else
						{
							unassignList.AddFlight(NULL, pFlight, RosterAssignFlight::emOp_Arrival, RosterAssignFlight::emAss_NotInclude, NULL);
						}
					}
				}

				if (pFlight->isDeparting())
				{
					if (std::find(vAssignedDepFlt.begin(), vAssignedDepFlt.end(),pFlight) == vAssignedDepFlt.end())		//unassigned
					{
						FlightConstraint depType = pFlight->getType('D') ;

						if(((FlightConstraint*)paxType.getConstraint(0))->fits(depType))
						{
							std::vector<ProcessorRoster *> vCreateRoster;
							if( pRule->CreateProcessorRoster( vProcAssignEntries, pFlight, 'D' , vCreateRoster))
							{
								vAssignedDepFlt.push_back(pFlight);
								RosterAssignFlight *pRosterAssFlight = unassignList.AddFlight(&paxType, pFlight, RosterAssignFlight::emOp_Departure,RosterAssignFlight::emAss_Sucess, pRule);
								pRosterAssFlight->AddRoster(vCreateRoster);
							}
							else
							{
								RosterAssignFlight *pRosterAssFlight = unassignList.AddFlight(&paxType, pFlight, RosterAssignFlight::emOp_Departure,RosterAssignFlight::emAss_Failed, pRule);						
							}
						}
						else
						{
							RosterAssignFlight *pRosterAssFlight = unassignList.AddFlight(NULL, pFlight, RosterAssignFlight::emOp_Departure,RosterAssignFlight::emAss_NotInclude, NULL);
						}
					}
				}
			}

		}
	}
	//unassignList.removeAssignedFlight();
}

void RosterAssignFlight::DeleteRoster( ProcessorRoster* pRoster )
{
	std::vector<ProcessorRoster *>::iterator iter = std::find(m_vAssignedRoster.begin(),m_vAssignedRoster.end(),pRoster);
	if (iter != m_vAssignedRoster.end())
	{
		m_vAssignedRoster.erase(iter);
	}

	if (GetCount() == 0)
	{
		if(m_pRule == NULL)
		{
			m_emAssignResult = emAss_NotInclude;
		}
		else
		{
			m_emAssignResult = emAss_Failed;
		}
		
	}
}

void RosterAssignFlight::Clear()
{
	m_vAssignedRoster.clear();
	m_emAssignResult = emAssign_Null;
}
