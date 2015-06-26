#include "stdafx.h"
#include "CParkingStandBuffer.h"
#include "Inputs\flight.h"

PSBArrivingFltType::PSBArrivingFltType(CAirportDatabase *  _pAirportDatabase):m_pAirportDatabase(_pAirportDatabase)
{
	
	m_lBufferTime = 0L;
}

PSBArrivingFltType::~PSBArrivingFltType(void)
{
	m_pAirportDatabase = 0;
}

FlightConstraint PSBArrivingFltType::GetArrivingFltType(void)
{
	FlightConstraint flightTypeArr;
	if(!m_pAirportDatabase)
	{
		throw(_T("null pointer for airport database"));
		return (flightTypeArr);
	}
	flightTypeArr.SetAirportDB(m_pAirportDatabase);
	flightTypeArr.setConstraintWithVersion(m_strFltTypeArriving);
	return (flightTypeArr);
}

PSBDepartingFltType::PSBDepartingFltType(CAirportDatabase *  _pAirportDatabase):m_pAirportDatabase(_pAirportDatabase)
{
}

PSBDepartingFltType::~PSBDepartingFltType(void)
{
	m_pAirportDatabase = 0;
	Clear();
}

void PSBDepartingFltType::Clear(void)
{
	for (std::vector < PSBArrivingFltType *>::iterator itrArrivingFltType = m_vrArrivingFltType.begin();\
	  itrArrivingFltType != m_vrArrivingFltType.end();++itrArrivingFltType)
	{		
		if(*itrArrivingFltType)
			delete (*itrArrivingFltType);
	}
	m_vrArrivingFltType.clear();
}
void PSBDepartingFltType::AddArrivingFltType(FlightConstraint ArrFltType,long lBufferTime)
{
	for (std::vector < PSBArrivingFltType *>::iterator itrArrivingFltType = m_vrArrivingFltType.begin();\
		itrArrivingFltType != m_vrArrivingFltType.end();++itrArrivingFltType)
	{	
		if((*itrArrivingFltType)->m_fltconstrain == ArrFltType)
		{
			if(lBufferTime >= 0)
				(*itrArrivingFltType)->m_lBufferTime = lBufferTime;
			return;
		}
	}
	PSBArrivingFltType * pPSBArrivingFltType = 0;
	pPSBArrivingFltType = new PSBArrivingFltType(m_pAirportDatabase);
	if(!pPSBArrivingFltType)return;
	if(lBufferTime >= 0)
		pPSBArrivingFltType->m_lBufferTime = lBufferTime;
	pPSBArrivingFltType->m_fltconstrain = ArrFltType;
	m_vrArrivingFltType.push_back(pPSBArrivingFltType);
}

int PSBDepartingFltType::UpdateArrivingFltType(FlightConstraint OldArrFltType,FlightConstraint NewArrFltType)
{
	if(OldArrFltType == NewArrFltType)return (5);//5,same
//	if(strNewArrFltType.IsEmpty())return (1);//1,new string is empty.
	for (std::vector < PSBArrivingFltType *>::iterator itrArrivingFltType = m_vrArrivingFltType.begin();\
		itrArrivingFltType != m_vrArrivingFltType.end();++itrArrivingFltType)
	{	
		if((*itrArrivingFltType)->m_fltconstrain == OldArrFltType)
		{
			for (std::vector < PSBArrivingFltType *>::iterator itrArrivingFltTypeForNew = m_vrArrivingFltType.begin();\
				itrArrivingFltTypeForNew != m_vrArrivingFltType.end();++itrArrivingFltTypeForNew)
			{
				if((*itrArrivingFltTypeForNew)->m_fltconstrain == NewArrFltType)
					return (2);//2,new arriving flight type had been existed.
			}
			if(*itrArrivingFltType)
			{
				(*itrArrivingFltType)->m_fltconstrain = NewArrFltType;
				return (0);//0,update successful.
			}
			else
				return (3);//3,element invalid.
		}
	}
	return (4);//4,arriving flight type to be update not find.
}

void PSBDepartingFltType::DeleteArrivingFltType(FlightConstraint ArrFltType)
{
	for (std::vector < PSBArrivingFltType *>::iterator itrArrivingFltType = m_vrArrivingFltType.begin();\
		itrArrivingFltType != m_vrArrivingFltType.end();++itrArrivingFltType)
	{	
		if((*itrArrivingFltType)->m_fltconstrain == ArrFltType)
		{
			delete (*itrArrivingFltType);
			m_vrArrivingFltType.erase(itrArrivingFltType);
			return;
		}
	}
}

long PSBDepartingFltType::GetArrivingFltTypeCount(void)
{
	return ((long)(m_vrArrivingFltType.size()) );
}

PSBArrivingFltType * PSBDepartingFltType::GetArrivingFltType(long lIndex)
{
	long lCurIndex = 0;
	for (std::vector < PSBArrivingFltType *>::iterator itrArrivingFltType = m_vrArrivingFltType.begin();\
		itrArrivingFltType != m_vrArrivingFltType.end();++itrArrivingFltType)
	{		
		if(lCurIndex++ == lIndex)
		{
			return (*itrArrivingFltType);
		}
	}
	return (0);
}
FlightConstraint PSBDepartingFltType::GetDepartingFltType(void)
{
	FlightConstraint flightTypeDep;
	if(!m_pAirportDatabase)
	{
		throw(_T("null pointer for airport database"));
		return (flightTypeDep);
	}
	flightTypeDep.SetAirportDB(m_pAirportDatabase);
	flightTypeDep.setConstraintWithVersion(m_strFltTypeDeparting);
	return (flightTypeDep);
}

PSBStdGrp::PSBStdGrp(CAirportDatabase *  _pAirportDatabase)	:m_pAirportDatabase(_pAirportDatabase)							
{
}

PSBStdGrp::PSBStdGrp(int nStdGrp,CAirportDatabase *  _pAirportDatabase):m_nStandGroupID(nStdGrp),m_pAirportDatabase(_pAirportDatabase)	
{
}

PSBStdGrp::~PSBStdGrp(void)
{
	m_pAirportDatabase = 0;
	Clear();
}
void PSBStdGrp::Clear(void)
{
	for (std::vector < PSBDepartingFltType *>::iterator itrDepartingFltType = m_vrDepartingFltType.begin();\
	  itrDepartingFltType != m_vrDepartingFltType.end();++itrDepartingFltType) 
	{
		if(*itrDepartingFltType)
			delete (*itrDepartingFltType);
	}
	m_vrDepartingFltType.clear();
}

void PSBStdGrp::AddDepartingFltType(FlightConstraint DepFltType,FlightConstraint ArrFltType,long lBufferTime)
{
	for (std::vector < PSBDepartingFltType *>::iterator itrDepartingFltType = m_vrDepartingFltType.begin();\
		itrDepartingFltType != m_vrDepartingFltType.end();++itrDepartingFltType) 
	{
		if((*itrDepartingFltType)->m_fltconstrain == DepFltType)
		{
			(*itrDepartingFltType)->AddArrivingFltType(ArrFltType,lBufferTime);
			return;
		}
	}
	PSBDepartingFltType * pPSBDepartingFltType  = 0;
	pPSBDepartingFltType = new PSBDepartingFltType(m_pAirportDatabase);
	if(!pPSBDepartingFltType)return;
	pPSBDepartingFltType->m_fltconstrain = DepFltType;
	pPSBDepartingFltType->AddArrivingFltType(ArrFltType,lBufferTime);
	m_vrDepartingFltType.push_back(pPSBDepartingFltType);
}

int PSBStdGrp::UpdateDepartingFltType(FlightConstraint OldDepFltType,FlightConstraint DepFltType)
{
	if(OldDepFltType == DepFltType)return (5);//5,same
	long nDepartingFltType = 0;
	for (std::vector < PSBDepartingFltType *>::iterator itrDepartingFltType = m_vrDepartingFltType.begin();\
		itrDepartingFltType != m_vrDepartingFltType.end();++itrDepartingFltType,++nDepartingFltType) 
	{
		if((*itrDepartingFltType)->m_fltconstrain == OldDepFltType)
		{
			for (std::vector < PSBDepartingFltType *>::iterator itrDepartingFltTypeForNew = m_vrDepartingFltType.begin();\
				itrDepartingFltTypeForNew != m_vrDepartingFltType.end();++itrDepartingFltTypeForNew) 
			{
				if((*itrDepartingFltTypeForNew)->m_fltconstrain == DepFltType)
					return (2);//2,new departing flight type to be update had been existed.
			}
			PSBDepartingFltType * pDepFltTypeUpdate = GetDepartingFltType(nDepartingFltType);
			if(pDepFltTypeUpdate)
			{
				pDepFltTypeUpdate->m_fltconstrain = DepFltType; 
				return (0);//0,update successful.
			}
			else
				return (3);//3,element invalid.
		}
	}
	return (4);//4,departing flight type to be update not find.
}

void PSBStdGrp::DeleteDepartingFltType(FlightConstraint DepFltType)
{
	for (std::vector < PSBDepartingFltType *>::iterator itrDepartingFltType = m_vrDepartingFltType.begin();\
		itrDepartingFltType != m_vrDepartingFltType.end();++itrDepartingFltType) 
	{
		if((*itrDepartingFltType)->m_fltconstrain == DepFltType)
		{
			(*itrDepartingFltType)->Clear();
			delete (*itrDepartingFltType);
			m_vrDepartingFltType.erase(itrDepartingFltType);
			return;
		}
	}
}

ALTObjectGroup PSBStdGrp::GetStandGroup(void)
{
	ALTObjectGroup altObjGroup;
	altObjGroup.ReadData(m_nStandGroupID);	
	return (altObjGroup);
}

long PSBStdGrp::GetDepartingFltTypeCount(void)
{
	return ((long)(m_vrDepartingFltType.size()) );
}

PSBDepartingFltType * PSBStdGrp::GetDepartingFltType(long lIndex)
{
	long lCurIndex = 0;
	for (std::vector < PSBDepartingFltType *>::iterator itrDepartingFltType = m_vrDepartingFltType.begin();\
		itrDepartingFltType != m_vrDepartingFltType.end();++itrDepartingFltType) 
	{		
		if(lCurIndex++ == lIndex)
		{
			return (*itrDepartingFltType);
		}
	}
	return (0);
}


CParkingStandBuffer::CParkingStandBuffer(CAirportDatabase *  _pAirportDatabase):m_pAirportDatabase(_pAirportDatabase)
{
	m_nProjID = -1;
}

CParkingStandBuffer::CParkingStandBuffer(int nProjID,CAirportDatabase *  _pAirportDatabase):m_nProjID(nProjID),m_pAirportDatabase(_pAirportDatabase)
{
}

CParkingStandBuffer::~CParkingStandBuffer(void)
{
	m_pAirportDatabase = 0;
	Clear();
}

void CParkingStandBuffer::Clear(void)
{
	for (std::vector < PSBStdGrp *>::iterator itrStandGroup = m_vrPushBackStandgroup.begin();\
	  itrStandGroup != m_vrPushBackStandgroup.end();++itrStandGroup) 
	{
		if(*itrStandGroup)
			delete (*itrStandGroup);
	}
	m_vrPushBackStandgroup.clear();
}

void CParkingStandBuffer::ReadData(int nProjID)
{
	CString strSQL = _T("");
	long nCount = 0L;
	CADORecordset adoRecordset;

	strSQL = GetSelectScript(nProjID);
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,adoRecordset);	
	
	int nStdGrpID = -1;
	CString strDepFltType = _T("");
	CString strArrFltType = _T("");
	long lBufferTime = 0L;

	Clear(); 
	while (!adoRecordset.IsEOF()) 
	{
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
		adoRecordset.GetFieldValue(_T("STDGRPID"),nStdGrpID);
		adoRecordset.GetFieldValue(_T("DEPFLTTYPE"),strDepFltType);
		FlightConstraint DepFltType;
		if (m_pAirportDatabase)
		{
			DepFltType.SetAirportDB(m_pAirportDatabase);
			DepFltType.setConstraintWithVersion(strDepFltType);
		}

		adoRecordset.GetFieldValue(_T("ARRFLTTYPE"),strArrFltType);
		FlightConstraint ArrFltType;
		if (m_pAirportDatabase)
		{
			ArrFltType.SetAirportDB(m_pAirportDatabase);
			ArrFltType.setConstraintWithVersion(strArrFltType);
		}

		adoRecordset.GetFieldValue(_T("BUFFERTIME"),lBufferTime);
		AddStandGroup(nStdGrpID,DepFltType,ArrFltType,lBufferTime);
		adoRecordset.MoveNextData();
	}
}

void CParkingStandBuffer::AddStandGroup(int nStdGrpID,FlightConstraint DepFltType,FlightConstraint ArrFltType,long lBufferTime)
{
	for (std::vector < PSBStdGrp *>::iterator itrStandGroup = m_vrPushBackStandgroup.begin();\
		itrStandGroup != m_vrPushBackStandgroup.end();++itrStandGroup) 
	{
		if( (*itrStandGroup)->m_nStandGroupID == nStdGrpID)
		{	
			(*itrStandGroup)->AddDepartingFltType(DepFltType,ArrFltType,lBufferTime);
			return;
		}
	}
	PSBStdGrp * pPSBStdGrp = 0;
	pPSBStdGrp = new PSBStdGrp(nStdGrpID,m_pAirportDatabase);
	if(!pPSBStdGrp)return;
	pPSBStdGrp->AddDepartingFltType(DepFltType,ArrFltType,lBufferTime);
	m_vrPushBackStandgroup.push_back(pPSBStdGrp);
}

int CParkingStandBuffer::UpdateStandGroup(int nOldStdGrpID,int nNewStrGrpID)
{
	if(nOldStdGrpID == nNewStrGrpID)return (5);//5,same
	if(nNewStrGrpID < 0)return (1);//1,new stand group id invalid.
	long lStdGrpIndex = 0;
	for (std::vector < PSBStdGrp *>::iterator itrStandGroup = m_vrPushBackStandgroup.begin();\
		itrStandGroup != m_vrPushBackStandgroup.end();++itrStandGroup,++lStdGrpIndex) 
	{
		if( (*itrStandGroup)->m_nStandGroupID == nOldStdGrpID)
		{
			for (std::vector < PSBStdGrp *>::iterator itrStandGroupForNew = m_vrPushBackStandgroup.begin();\
				itrStandGroupForNew != m_vrPushBackStandgroup.end();++itrStandGroupForNew) 
			{
				if( (*itrStandGroupForNew)->m_nStandGroupID == nNewStrGrpID)
					return (2);//2,new stand group to be update had been existed.
			}
			PSBStdGrp * pStdGrp = GetParkingStandgroup(lStdGrpIndex);
			if(pStdGrp)
			{
				pStdGrp->m_nStandGroupID = nNewStrGrpID;
				return (0);//0,update successful.
			}
			else
				return (3);//3,element invalid.
		}
	}
	return (4);//4,stand group id to be update not find.
}

int CParkingStandBuffer::UpdateDepartingFltType(int nStdGrpID,FlightConstraint OldDepFltType,FlightConstraint NewDepFltType)
{
	if(OldDepFltType == NewDepFltType)return (5);//5,same
	if(nStdGrpID < 0)return (1);//1,new stand group id invalid.
	long lStdGrpIndex = 0;
	for (std::vector < PSBStdGrp *>::iterator itrStandGroup = m_vrPushBackStandgroup.begin();\
		itrStandGroup != m_vrPushBackStandgroup.end();++itrStandGroup,++lStdGrpIndex) 
	{
		if( (*itrStandGroup)->m_nStandGroupID == nStdGrpID)
		{
			PSBStdGrp * pStdGrp = GetParkingStandgroup(lStdGrpIndex);
			if(pStdGrp)
				return (pStdGrp->UpdateDepartingFltType(OldDepFltType,NewDepFltType));
			else
				return (3);//3,element invalid.
		}
	}
	return (4);//stand group id to be update not find.
}

int CParkingStandBuffer::UpdateArrivingFltType(int nStdGrpID,FlightConstraint DepFltType, FlightConstraint OldArrFltType,FlightConstraint NewArrFltType)
{
	if(OldArrFltType == NewArrFltType)return (5);//5,same
	if(nStdGrpID < 0)return (1);//1,new stand group id invalid.
	long lStdGrpIndex = 0;
	for (std::vector < PSBStdGrp *>::iterator itrStandGroup = m_vrPushBackStandgroup.begin();\
		itrStandGroup != m_vrPushBackStandgroup.end();++itrStandGroup,++lStdGrpIndex) 
	{
		if( (*itrStandGroup)->m_nStandGroupID == nStdGrpID)
		{
			PSBStdGrp * pStdGrp = GetParkingStandgroup(lStdGrpIndex);
			if(pStdGrp)
			{ 
				long lDepartingFltType = 0;
				for (std::vector < PSBDepartingFltType *>::iterator itrDepartingFltType = pStdGrp->m_vrDepartingFltType.begin();\
					itrDepartingFltType != pStdGrp->m_vrDepartingFltType.end();++itrDepartingFltType,++lDepartingFltType) 
				{
					if((*itrDepartingFltType)->m_fltconstrain == DepFltType)
					{						
						return ((*itrDepartingFltType)->UpdateArrivingFltType(OldArrFltType,NewArrFltType) );
					}
				}
				return (4);//4,departing flight type to be update not find.
			}
			else
				return (3);//3,element invalid.
		}
	}
	return (4);//stand group id to be update not find.
}

long CParkingStandBuffer::GetStandBufferTime(int nStdGrpID,FlightConstraint DepFltType,FlightConstraint ArrFltType)
{
	for (std::vector < PSBStdGrp *>::iterator itrStandGroup = m_vrPushBackStandgroup.begin();\
		itrStandGroup != m_vrPushBackStandgroup.end();++itrStandGroup) 
	{
		if( (*itrStandGroup)->m_nStandGroupID == nStdGrpID)
		{
			for (std::vector < PSBDepartingFltType *>::iterator itrDepartingFltType = (*itrStandGroup)->m_vrDepartingFltType.begin();\
				itrDepartingFltType != (*itrStandGroup)->m_vrDepartingFltType.end();++itrDepartingFltType) 
			{
				if((*itrDepartingFltType)-> m_fltconstrain == DepFltType)
				{
					for (std::vector < PSBArrivingFltType *>::iterator itrArrivingFltType = (*itrDepartingFltType)->m_vrArrivingFltType.begin();\
						itrArrivingFltType != (*itrDepartingFltType)->m_vrArrivingFltType.end();++itrArrivingFltType)
					{
						if( (*itrArrivingFltType)->m_fltconstrain == ArrFltType )
						{
							return ((*itrArrivingFltType)->m_lBufferTime);
						}
					}
				}
			}
		}
	}

	return (0L);
}

void CParkingStandBuffer::DeleteStandGroup(int nStdGrpID)
{
	for (std::vector < PSBStdGrp *>::iterator itrStandGroup = m_vrPushBackStandgroup.begin();\
		itrStandGroup != m_vrPushBackStandgroup.end();++itrStandGroup) 
	{
		if( (*itrStandGroup)->m_nStandGroupID == nStdGrpID)
		{
			(*itrStandGroup)->Clear();
			delete (*itrStandGroup);
			m_vrPushBackStandgroup.erase(itrStandGroup);
			return;
		}
	}
}

void CParkingStandBuffer::DeleteDepartingFltType(int nStdGrpID,FlightConstraint DepFltType)
{
	for (std::vector < PSBStdGrp *>::iterator itrStandGroup = m_vrPushBackStandgroup.begin();\
		itrStandGroup != m_vrPushBackStandgroup.end();++itrStandGroup) 
	{
		if( (*itrStandGroup)->m_nStandGroupID == nStdGrpID)
		{
			(*itrStandGroup)->DeleteDepartingFltType(DepFltType);
			return;
		}
	}
}

void CParkingStandBuffer::DeleteArrivingFltType(int nStdGrpID,FlightConstraint DepFltType,FlightConstraint ArrFltType)
{
	for (std::vector < PSBStdGrp *>::iterator itrStandGroup = m_vrPushBackStandgroup.begin();\
		itrStandGroup != m_vrPushBackStandgroup.end();++itrStandGroup) 
	{
		if( (*itrStandGroup)->m_nStandGroupID == nStdGrpID)
		{
			for (std::vector < PSBDepartingFltType *>::iterator itrDepartingFltType = (*itrStandGroup)->m_vrDepartingFltType.begin();\
				itrDepartingFltType != (*itrStandGroup)->m_vrDepartingFltType.end();++itrDepartingFltType) 
			{
				if((*itrDepartingFltType)-> m_fltconstrain == DepFltType)
				{
					(*itrDepartingFltType)->DeleteArrivingFltType(ArrFltType);
					return;
				}
			}
		}
	}
}

long CParkingStandBuffer::GetParkingStandgroupCount(void)
{
	return ((long)(m_vrPushBackStandgroup.size()) );
}

PSBStdGrp * CParkingStandBuffer::GetParkingStandgroup(long lIndex)
{
	long lCurIndex = 0;
	for (std::vector < PSBStdGrp *>::iterator itrStandGroup = m_vrPushBackStandgroup.begin();\
		itrStandGroup != m_vrPushBackStandgroup.end();++itrStandGroup) 
	{		
		if(lCurIndex++ == lIndex)
		{
			return (*itrStandGroup);
		}
	}
	return (0);
}

void CParkingStandBuffer::SaveData(void)
{
	CString strSQL =_T("");

	strSQL = GetDeleteScript();
	CADODatabase::ExecuteSQLStatement(strSQL);

	int nStdGrpID = -1;
	CString strDepFltType = _T("");
	CString strArrFltType = _T("");
	long lBufferTime = 0L;

	long lStdGrpCount = 0;
	long lDepFltTypeCount = 0;
	long lArrFltTypeCount = 0;
	lStdGrpCount = GetParkingStandgroupCount();
	for (long lStdGrpIndex = 0;lStdGrpIndex < lStdGrpCount;lStdGrpIndex++)
	{
		nStdGrpID = -1;
		strDepFltType = _T("");
		strArrFltType = _T("");
		lBufferTime = 0L;

		PSBStdGrp * pPSBStdGrp = GetParkingStandgroup(lStdGrpIndex);
		nStdGrpID = pPSBStdGrp->m_nStandGroupID;
		lDepFltTypeCount = pPSBStdGrp->GetDepartingFltTypeCount();
		if(lDepFltTypeCount <= 0)
		{
			FlightConstraint DepFltType;
			FlightConstraint ArrFltType;
			DepFltType.WriteSyntaxStringWithVersion(strDepFltType.GetBuffer(1024));
			DepFltType.WriteSyntaxStringWithVersion(strArrFltType.GetBuffer(1024));
			strSQL.Format(_T("INSERT INTO PARKINGSTANDBUFFER (PROJID,STDGRPID,DEPFLTTYPE,ARRFLTTYPE,BUFFERTIME) VALUES (%d,%d,'%s','%s',%d); "),\
				m_nProjID,nStdGrpID,strDepFltType,strArrFltType,lBufferTime);
			CADODatabase::ExecuteSQLStatement(strSQL);
		}
		for (long lDepFltTypeIndex = 0;lDepFltTypeIndex < lDepFltTypeCount;lDepFltTypeIndex++)
		{
			strDepFltType = _T("");
			strArrFltType = _T("");
			lBufferTime = 0L;

			PSBDepartingFltType * pPSBDepartingFltType = pPSBStdGrp->GetDepartingFltType(lDepFltTypeIndex);
			pPSBDepartingFltType->m_fltconstrain.WriteSyntaxStringWithVersion(strDepFltType.GetBuffer(1024));
			lArrFltTypeCount = pPSBDepartingFltType->GetArrivingFltTypeCount();
			if(lArrFltTypeCount <= 0)
			{
				FlightConstraint ArrFltType;
				ArrFltType.WriteSyntaxStringWithVersion(strArrFltType.GetBuffer(1024));
				strSQL.Format(_T("INSERT INTO PARKINGSTANDBUFFER (PROJID,STDGRPID,DEPFLTTYPE,ARRFLTTYPE,BUFFERTIME) VALUES (%d,%d,'%s','%s',%d); "),\
					m_nProjID,nStdGrpID,strDepFltType,strArrFltType,lBufferTime);
				CADODatabase::ExecuteSQLStatement(strSQL);
			}
			for (long lArrFltTypeIndex = 0;lArrFltTypeIndex < lArrFltTypeCount;lArrFltTypeIndex++)
			{
				strArrFltType = _T("");
				lBufferTime = 0L;

				PSBArrivingFltType * pPSBArrivingFltType = pPSBDepartingFltType->GetArrivingFltType(lArrFltTypeIndex);				
				pPSBArrivingFltType->m_fltconstrain.WriteSyntaxStringWithVersion(strArrFltType.GetBuffer(1024));
				lBufferTime = pPSBArrivingFltType->m_lBufferTime;

				strSQL.Format(_T("INSERT INTO PARKINGSTANDBUFFER (PROJID,STDGRPID,DEPFLTTYPE,ARRFLTTYPE,BUFFERTIME) VALUES (%d,%d,'%s','%s',%d); "),\
					m_nProjID,nStdGrpID,strDepFltType,strArrFltType,lBufferTime);
				CADODatabase::ExecuteSQLStatement(strSQL);
			}
		}
	}
	return;
}

void CParkingStandBuffer::ImportData(CAirsideImportFile& importFile)
{
	m_nProjID = importFile.getNewProjectID();
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);
	
	Clear(); 

	int nStdGrpID = -1;
	CString strDepFltType = _T("");
	CString strArrFltType = _T("");
	long lBufferTime = 0L;
	char szStringData[1024] = {0};
	long lStringLen = 0L;

	long lStdGrpCount = 0;
	long lDepFltTypeCount = 0;
	long lArrFltTypeCount = 0;
	importFile.getFile().getInteger(lStdGrpCount);
	for (long lStdGrpIndex = 0;lStdGrpIndex < lStdGrpCount;lStdGrpIndex++)
	{
		nStdGrpID = -1;		
		strDepFltType = _T("");
		strArrFltType = _T("");
		lBufferTime = 0L;

		importFile.getFile().getInteger(nStdGrpID);
		ALTObjectGroup altObjGroup;	
		altObjGroup.setType(ALT_STAND);
		ALTObjectID objName;
		objName.readALTObjectID(importFile.getFile());
		altObjGroup.setName(objName); 
		altObjGroup.SaveData(m_nProjID);
		nStdGrpID = altObjGroup.getID();
		importFile.getFile().getLine();

		importFile.getFile().getInteger(lDepFltTypeCount);
// 		if(lDepFltTypeCount <= 0)
// 		{
// 			AddStandGroup(nStdGrpID,strDepFltType,strArrFltType,lBufferTime);
// 		}
		for (long lDepFltTypeIndex = 0;lDepFltTypeIndex < lDepFltTypeCount;lDepFltTypeIndex++)
		{
			strDepFltType = _T("");
			strArrFltType = _T("");
			lBufferTime = 0L;
		
			importFile.getFile().getInteger(lStringLen);
			importFile.getFile().getField(szStringData,lStringLen);
			strDepFltType = szStringData;
			importFile.getFile().getInteger(lArrFltTypeCount);
// 			if(lArrFltTypeCount <= 0)
// 			{
// 				AddStandGroup(nStdGrpID,strDepFltType,strArrFltType,lBufferTime);
// 			}
			for (long lArrFltTypeIndex = 0;lArrFltTypeIndex < lArrFltTypeCount;lArrFltTypeIndex++)
			{
				strArrFltType = _T("");
				lBufferTime = 0L;

				importFile.getFile().getInteger(lStringLen);
				importFile.getFile().getField(szStringData,lStringLen);
				strArrFltType = szStringData;
				importFile.getFile().getInteger(lBufferTime);

//				AddStandGroup(nStdGrpID,strDepFltType,strArrFltType,lBufferTime);
			}
		}
	}

	importFile.getFile().getLine();
}

void CParkingStandBuffer::ImportData_Old(CAirsideImportFile& importFile)
{
	m_nProjID = importFile.getNewProjectID();
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);

	Clear(); 
	CString strStdGrp = _T("");
	CString strDepFltType = _T("");
	CString strArrFltType = _T("");
	long lBufferTime = 0L;
	char szStringData[1024] = {0};
	long lStringLen = 0L;

	long lStdGrpCount = 0;
	long lDepFltTypeCount = 0;
	long lArrFltTypeCount = 0;
	importFile.getFile().getInteger(lStdGrpCount);
	for (long lStdGrpIndex = 0;lStdGrpIndex < lStdGrpCount;lStdGrpIndex++)
	{
		strStdGrp = _T("");		
		strDepFltType = _T("");
		strArrFltType = _T("");
		lBufferTime = 0L;

		importFile.getFile().getInteger(lStringLen);
		importFile.getFile().getField(szStringData,lStringLen);
		strStdGrp = szStringData;  

		int nStdGrpID = -1;
		ALTObjectGroup altObjGroup;	
		altObjGroup.setType(ALT_STAND);
		ALTObjectID objName;

		CString strTemp = _T("");
		int nPos = -1;
		nPos = strStdGrp.Find('-');
		strTemp = strStdGrp.Left(nPos);
		strStdGrp = strStdGrp.Mid(nPos+1);
		objName.m_val[0] = strTemp;
		nPos = strStdGrp.Find('-');
		strTemp = _T("");
		strTemp = strStdGrp.Left(nPos);
		strStdGrp = strStdGrp.Mid(nPos+1);
		objName.m_val[1] = strTemp;
		nPos = strStdGrp.Find('-');
		strTemp = _T("");
		strTemp = strStdGrp.Left(nPos);
		strStdGrp = strStdGrp.Mid(nPos+1);
		objName.m_val[2] = strTemp;
		nPos = strStdGrp.Find('-');
		strTemp = _T("");
		strTemp = strStdGrp.Left(nPos);
		strStdGrp = strStdGrp.Mid(nPos+1);
		objName.m_val[3] = strTemp;

		altObjGroup.setName(objName); 
		altObjGroup.SaveData(m_nProjID);
		nStdGrpID = altObjGroup.getID();

		importFile.getFile().getInteger(lDepFltTypeCount);
		if(lDepFltTypeCount <= 0)
		{
//			AddStandGroup(nStdGrpID,strDepFltType,strArrFltType,lBufferTime);
		}
		for (long lDepFltTypeIndex = 0;lDepFltTypeIndex < lDepFltTypeCount;lDepFltTypeIndex++)
		{
			strDepFltType = _T("");
			strArrFltType = _T("");
			lBufferTime = 0L;

			importFile.getFile().getInteger(lStringLen);
			importFile.getFile().getField(szStringData,lStringLen);
			strDepFltType = szStringData;
			importFile.getFile().getInteger(lArrFltTypeCount);
			if(lArrFltTypeCount <= 0)
			{
//				AddStandGroup(nStdGrpID,strDepFltType,strArrFltType,lBufferTime);
			}
			for (long lArrFltTypeIndex = 0;lArrFltTypeIndex < lArrFltTypeCount;lArrFltTypeIndex++)
			{
				strArrFltType = _T("");
				lBufferTime = 0L;

				importFile.getFile().getInteger(lStringLen);
				importFile.getFile().getField(szStringData,lStringLen);
				strArrFltType = szStringData;
				importFile.getFile().getInteger(lBufferTime);

//				AddStandGroup(nStdGrpID,strDepFltType,strArrFltType,lBufferTime);
			}
		}
	}

	importFile.getFile().getLine();
}


void CParkingStandBuffer::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);

	int nStdGrpID = -1;
	CString strDepFltType = _T("");
	CString strArrFltType = _T("");
	long lBufferTime = 0L;

	long lStdGrpCount = 0;
	long lDepFltTypeCount = 0;
	long lArrFltTypeCount = 0;
	lStdGrpCount = GetParkingStandgroupCount();
	exportFile.getFile().writeInt(lStdGrpCount);
	for (long lStdGrpIndex = 0;lStdGrpIndex < lStdGrpCount;lStdGrpIndex++)
	{
		nStdGrpID = -1;
		strDepFltType = _T("");
		strArrFltType = _T("");
		lBufferTime = 0L;

		PSBStdGrp * pPSBStdGrp = GetParkingStandgroup(lStdGrpIndex);

		nStdGrpID = pPSBStdGrp->m_nStandGroupID;
		exportFile.getFile().writeInt(nStdGrpID);
		ALTObjectGroup altObjGroup;
		altObjGroup.ReadData(nStdGrpID);	
		ALTObjectID objName = altObjGroup.getName();
		objName.writeALTObjectID(exportFile.getFile());
		exportFile.getFile().writeLine();

		lDepFltTypeCount = pPSBStdGrp->GetDepartingFltTypeCount();
		exportFile.getFile().writeInt(lDepFltTypeCount);
		for (long lDepFltTypeIndex = 0;lDepFltTypeIndex < lDepFltTypeCount;lDepFltTypeIndex++)
		{
			strDepFltType = _T("");
			strArrFltType = _T("");
			lBufferTime = 0L;

			PSBDepartingFltType * pPSBDepartingFltType = pPSBStdGrp->GetDepartingFltType(lDepFltTypeIndex);
			strDepFltType = pPSBDepartingFltType->m_strFltTypeDeparting;
			exportFile.getFile().writeInt(strDepFltType.GetLength());
			exportFile.getFile().writeField(strDepFltType);
			lArrFltTypeCount = pPSBDepartingFltType->GetArrivingFltTypeCount();
			exportFile.getFile().writeInt(lArrFltTypeCount);
			for (long lArrFltTypeIndex = 0;lArrFltTypeIndex < lArrFltTypeCount;lArrFltTypeIndex++)
			{
				strArrFltType = _T("");
				lBufferTime = 0L;

				PSBArrivingFltType * pPSBArrivingFltType = pPSBDepartingFltType->GetArrivingFltType(lArrFltTypeIndex);
				strArrFltType = pPSBArrivingFltType->m_strFltTypeArriving;
				exportFile.getFile().writeInt(strArrFltType.GetLength());
				exportFile.getFile().writeField(strArrFltType);
				lBufferTime = pPSBArrivingFltType->m_lBufferTime;
				exportFile.getFile().writeInt(lBufferTime);
			}
		}
	}

	exportFile.getFile().writeLine();
}

void CParkingStandBuffer::ExportParkingStandBuffer(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB)
{
	CParkingStandBuffer parkingStdBuf(exportFile.GetProjectID(),pAirportDB);
	parkingStdBuf.ReadData(exportFile.GetProjectID());
	exportFile.getFile().writeField(_T("PARKINGSTANDBUFFER"));
	exportFile.getFile().writeLine();
	parkingStdBuf.ExportData(exportFile);
	exportFile.getFile().endFile();
}

void CParkingStandBuffer::ImportParkingStandBuffer(CAirsideImportFile& importFile)
{
	CParkingStandBuffer parkingStdBuf(0);
	while(!importFile.getFile().isBlank())
	{
		if(importFile.getFile().getVersion() <= 1015)
			parkingStdBuf.ImportData_Old(importFile);
		else
			parkingStdBuf.ImportData(importFile);
	}
	parkingStdBuf.SaveData();
	
}

CString CParkingStandBuffer::GetSelectScript(int nProjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM PARKINGSTANDBUFFER WHERE PROJID = %d ;"),nProjID);
	return (strSQL);
}

CString CParkingStandBuffer::GetDeleteScript() const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM PARKINGSTANDBUFFER WHERE PROJID = %d;"),m_nProjID);
	return (strSQL);
}

PSBDepartingFltType * CParkingStandBuffer::GetDepartingFltType( int nStdGrpID,FlightConstraint DepFltType )
{
	for (std::vector < PSBStdGrp *>::iterator itrStandGroup = m_vrPushBackStandgroup.begin();\
		itrStandGroup != m_vrPushBackStandgroup.end();++itrStandGroup) 
	{
		if( (*itrStandGroup)->m_nStandGroupID == nStdGrpID)
		{
			for (std::vector < PSBDepartingFltType *>::iterator itrDepartingFltType = (*itrStandGroup)->m_vrDepartingFltType.begin();\
				itrDepartingFltType != (*itrStandGroup)->m_vrDepartingFltType.end();++itrDepartingFltType) 
			{
				if((*itrDepartingFltType)-> m_fltconstrain == DepFltType)
					return *itrDepartingFltType;
			}
		}
	}
	return NULL;
}
//-----------------------------------------------------------------------------------------
//ParkingStandBufferList

ParkingStandBufferList::ParkingStandBufferList(int nProjID,CAirportDatabase *  pAirportDatabase)
:m_nProjID(nProjID)
,m_pAirportDatabase(pAirportDatabase)
,m_pParkingStandBuffer(NULL)
{

}

ParkingStandBufferList::~ParkingStandBufferList()
{
	delete m_pParkingStandBuffer;
	m_pParkingStandBuffer = NULL;
}

void ParkingStandBufferList::LoadData()
{
	if(m_pParkingStandBuffer != NULL)
		delete m_pParkingStandBuffer;
	m_pParkingStandBuffer = new CParkingStandBuffer(m_nProjID, m_pAirportDatabase);
	m_pParkingStandBuffer->ReadData(m_nProjID);
}

ElapsedTime ParkingStandBufferList::GetBufferTime(FlightDescStruct& depFlight, FlightDescStruct& arrFlight)
{
	ElapsedTime bufTime = 0L;

	int nCount = m_pParkingStandBuffer->GetParkingStandgroupCount();
	for (int i =0; i < nCount; i++)
	{				
		PSBDepartingFltType* pItem = NULL;
		for ( int j =0; j < m_pParkingStandBuffer->GetParkingStandgroup(i)->GetDepartingFltTypeCount(); j++)
		{

			FlightConstraint fltCnst = m_pParkingStandBuffer->GetParkingStandgroup(i)->GetDepartingFltType(j)->GetDepartingFltType();
			if( fltCnst.fits( depFlight ) )
			{
				pItem = m_pParkingStandBuffer->GetParkingStandgroup(i)->GetDepartingFltType(j);
				break;
			}
		}
		if(pItem == NULL)
			continue;

		for (int j =0; j < pItem->GetArrivingFltTypeCount(); j++)
		{
			FlightConstraint fltCnst = pItem->GetArrivingFltType(j)->GetArrivingFltType();
			if( fltCnst.fits( arrFlight ) )
			{
				bufTime = ElapsedTime(pItem->GetArrivingFltType(j)->m_lBufferTime * 60);
				return bufTime;
			}
		}

	}

	return bufTime;
}









