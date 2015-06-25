#include "StdAfx.h"
#include ".\operatingdoorspec.h"
#include "../Database/ADODatabase.h"
#include "../InputAirside/ALTObjectGroup.h"
#include "../Common/AirportDatabase.h"
#include <algorithm>


StandOperatingDoorData::StandOperatingDoorData( void )
:m_nID(-1)
,m_nStandID(-2)
,m_pAirportDB(NULL)
{
	m_vOpDoors.clear();
}

StandOperatingDoorData::~StandOperatingDoorData( void )
{

}

int StandOperatingDoorData::GetID()
{
	return m_nID;
}

void StandOperatingDoorData::SetID( int nID )
{
	m_nID = nID;
}

const ALTObjectID& StandOperatingDoorData::GetStandName()
{
	return m_standName;
}

int StandOperatingDoorData::GetOpDoorCount()
{
	return m_vOpDoors.size();
}

//int StandOperatingDoorData::GetOpDoorID(int nIdx)
//{
//	if(nIdx<0 || nIdx >= GetOpDoorCount())
//		return -1;
//
////	return m_vOpDoors.at(nIdx);
//	return 0;
//}

//void StandOperatingDoorData::AddOpDoor( int nID )
//{
////	m_vOpDoors.push_back(nID);
//}


ACTypeDoor* StandOperatingDoorData::GetAcTypeDoor( int nID,ACTYPEDOORLIST* pDoorList ) const
{
	for (unsigned i = 0; i < pDoorList->size(); i++)
	{
		ACTypeDoor* pDoor = pDoorList->at(i);
		if (pDoor && pDoor->GetID() == nID)
		{
			return pDoor;
		}
	}
	return NULL;
}
void StandOperatingDoorData::AddDoorOperation( ACTypeDoor* pDoor )
{
	if (pDoor)
	{
		switch (pDoor->m_enumDoorDir)
		{
		case ACTypeDoor::LeftHand:
			{
				OperationDoor leftDoor;
				leftDoor.SetDoorName(pDoor->GetDoorName());
				leftDoor.SetHandType(ACTypeDoor::LeftHand);
				m_vOpDoors.push_back(leftDoor);
			}
			break;
		case ACTypeDoor::RightHand:
			{
				OperationDoor rightDoor;
				rightDoor.SetDoorName(pDoor->GetDoorName());
				rightDoor.SetHandType(ACTypeDoor::RightHand);
				m_vOpDoors.push_back(rightDoor);
			}
			break;
		case ACTypeDoor::BothSide:
			{
				OperationDoor leftDoor;
				leftDoor.SetDoorName(pDoor->GetDoorName());
				leftDoor.SetHandType(ACTypeDoor::LeftHand);
				m_vOpDoors.push_back(leftDoor);

				OperationDoor rightDoor;
				rightDoor.SetDoorName(pDoor->GetDoorName());
				rightDoor.SetHandType(ACTypeDoor::RightHand);
				m_vOpDoors.push_back(rightDoor);
			}
			break;
		default:
			break;
		}
	}
}

bool StandOperatingDoorData::GetDoorOperation(ACTypeDoor* pDoor,std::vector<OperationDoor>& vDoorOp)
{
	if (pDoor)
	{
		switch (pDoor->m_enumDoorDir)
		{
		case ACTypeDoor::LeftHand:
			{
				OperationDoor leftDoor;
				leftDoor.SetDoorName(pDoor->GetDoorName());
				leftDoor.SetHandType(ACTypeDoor::LeftHand);
				vDoorOp.push_back(leftDoor);
			}
			break;
		case ACTypeDoor::RightHand:
			{
				OperationDoor rightDoor;
				rightDoor.SetDoorName(pDoor->GetDoorName());
				rightDoor.SetHandType(ACTypeDoor::RightHand);
				vDoorOp.push_back(rightDoor);
			}
			break;
		case ACTypeDoor::BothSide:
			{
				OperationDoor leftDoor;
				leftDoor.SetDoorName(pDoor->GetDoorName());
				leftDoor.SetHandType(ACTypeDoor::LeftHand);
				vDoorOp.push_back(leftDoor);

				OperationDoor rightDoor;
				rightDoor.SetDoorName(pDoor->GetDoorName());
				rightDoor.SetHandType(ACTypeDoor::RightHand);
				vDoorOp.push_back(rightDoor);
			}
			break;
		default:
			break;
		}
	}

	if (vDoorOp.empty())
	{
		return false;
	}
	return true;
}
void StandOperatingDoorData::DoCompatible( const CString& strDoors,ACTYPEDOORLIST* pDoorList )
{
	int nDoorID;
	int nIdx0 =0;
	int nIdx =0;
	while(nIdx >=0 && nIdx <strDoors.GetLength()-1)
	{
		nIdx = strDoors.Find(',',nIdx0);
		nDoorID = atoi(strDoors.Mid(nIdx0,nIdx));
		if (nDoorID >=0)
		{
			ACTypeDoor* pDoor = GetAcTypeDoor(nDoorID,pDoorList);
			AddDoorOperation(pDoor);
			//if (pDoor)
			//{
			//	switch (pDoor->m_enumDoorDir)
			//	{
			//	case ACTypeDoor::LeftHand:
			//		{
			//			OperationDoor leftDoor;
			//			leftDoor.m_nID = nDoorID;
			//			leftDoor.m_iHandType = ACTypeDoor::LeftHand;
			//			m_vOpDoors.push_back(leftDoor);
			//		}
			//		break;
			//	case ACTypeDoor::RightHand:
			//		{
			//			OperationDoor rightDoor;
			//			rightDoor.m_nID = nDoorID;
			//			rightDoor.m_iHandType = ACTypeDoor::RightHand;
			//			m_vOpDoors.push_back(rightDoor);
			//		}
			//		break;
			//	case ACTypeDoor::BothSide:
			//		{
			//			OperationDoor leftDoor;
			//			leftDoor.m_nID = nDoorID;
			//			leftDoor.m_iHandType = ACTypeDoor::LeftHand;
			//			m_vOpDoors.push_back(leftDoor);

			//			OperationDoor rightDoor;
			//			rightDoor.m_nID = nDoorID;
			//			rightDoor.m_iHandType = ACTypeDoor::RightHand;
			//			m_vOpDoors.push_back(rightDoor);
			//		}
			//		break;
			//	default:
			//		break;
			//	}
			//}
		}
		nIdx0 = nIdx+1;
	}
}
void StandOperatingDoorData::ReadData( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	int nParentID;
	recordset.GetFieldValue(_T("PARENTID"),nParentID);
	recordset.GetFieldValue(_T("STANDID"),m_nStandID);
	CString strDoors;
	recordset.GetFieldValue(_T("OPDOORS"),strDoors);

	ALTObjectGroup standGroup;
	standGroup.ReadData(m_nStandID);
	m_standName = standGroup.getName();

	if (strDoors.IsEmpty())
		return;


    ACTYPEDOORLIST* pAcDoors = m_pAirportDB->getAcDoorMan()->GetAcTypeDoorList(m_strActype);
    if (pAcDoors == NULL)
        return;

    ArctermFile p_file;
    p_file.InitDataFromString(strDoors.GetBuffer());
    char buf[128] = {0};
    p_file.getSubField(buf, ';');
    if(strcmp(buf, "VERSION") != 0)
    {
        int nLeftPos = strDoors.Find('L');
        int nRightPos = strDoors.Find('R');
        if (nLeftPos == -1 && nRightPos == -1)//need do compatible
        {
            DoCompatible(strDoors,pAcDoors);
            return;
        }
        int nPos = strDoors.Find(',');
        CString strLeft;
        CString strRight = strDoors;
        while(nPos != -1)
        {
            strLeft = strRight.Left(nPos);
            OperationDoor doorOp;
            doorOp.parseString(strLeft, pAcDoors);
            m_vOpDoors.push_back(doorOp);
            strRight = strRight.Right(strRight.GetLength() - nPos - 1);
            nPos = strRight.Find(',');
        }

        if (strRight.IsEmpty() == false)
        {
            OperationDoor doorOp;
            doorOp.parseString(strRight, pAcDoors);
            m_vOpDoors.push_back(doorOp);
        }
    }
    else
    {
        p_file.getSubField(buf, ';');
        int nVersion = atoi(buf);
        switch(nVersion)
        {
        case 1: // version 1
            {
                int nDoorCount;
                p_file.getInteger(nDoorCount);
                for(int i=0; i<nDoorCount; i++)
                {
                    OperationDoor doorOp;
                    doorOp.readDataVersion1(p_file);

                    ACTYPEDOORLIST::iterator itor = pAcDoors->begin();
                    while(itor != pAcDoors->end())
                    {
                        if ((*itor)->GetDoorName() == doorOp.GetDoorName())
                            break;
                        itor++;
                    }
                    if (itor != pAcDoors->end())
                        m_vOpDoors.push_back(doorOp);
                }
            }
            break;
        default:
            break;
        }
    }

}

void StandOperatingDoorData::SaveData( int nParentID )
{
	ALTObjectGroup standGroup;

	if (m_nStandID >=0)
	{
		standGroup.ReadData(m_nStandID);
		standGroup.DeleteData();
	}
		
	standGroup.setName(m_standName);
	standGroup.setID(-1);
	standGroup.SaveData(1);
	m_nStandID = standGroup.getID();

    ArctermFile p_file;
    p_file.appendValue("VERSION");
    p_file.appendValue(";");
    p_file.appendValue("1"); // version 1
	int nCount = m_vOpDoors.size();
    p_file.writeInt(nCount);
	for (int i = 0; i < nCount; i++)
	{
		OperationDoor doorOp = m_vOpDoors.at(i);
		doorOp.writeData(p_file);
	}

    CString strDoors(p_file.getDataLine());
	if (m_nID < 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO TB_STANDDOOROPERATING\
						 (PARENTID,STANDID,OPDOORS)\
						 VALUES (%d,%d,'%s')"),nParentID,m_nStandID,strDoors);

		m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
		UpdateData(nParentID, strDoors);
}

void StandOperatingDoorData::UpdateData( int nParentID ,const CString& strDoors)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE TB_STANDDOOROPERATING\
					 SET PARENTID =%d, STANDID = %d,OPDOORS = '%s'\
					 WHERE (ID = %d)"), nParentID,  m_nStandID, strDoors, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void StandOperatingDoorData::DeleteData()
{
	ALTObjectGroup standGroup;

	if (m_nStandID >=0)
	{
		standGroup.ReadData(m_nStandID);
		standGroup.DeleteData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM TB_STANDDOOROPERATING\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

int StandOperatingDoorData::GetStandID()
{
	return m_nStandID;
}

void StandOperatingDoorData::SetStandName( const ALTObjectID& altName )
{
	m_standName = altName;
}


void StandOperatingDoorData::DelOpDoor( OperationDoor* pDoorOp )
{
	for (int i = 0; i< GetOpDoorCount(); i++)
	{
		OperationDoor doorOp = m_vOpDoors.at(i);
		if (doorOp == *pDoorOp)
		{
			m_vOpDoors.erase(m_vOpDoors.begin() + i);
			break;
		}
	}
}

bool StandOperatingDoorData::findOpDoor( OperationDoor* pDoorOp )
{
	for (int i = 0; i< GetOpDoorCount(); i++)
	{
		OperationDoor doorOp = m_vOpDoors.at(i);
		if (doorOp == *pDoorOp)
		{
			return true;
		}
	}
	return false;
}

//void StandOperatingDoorData::GetOpDoors( std::vector<int>& vDoors )
//{
////	vDoors.assign(m_vOpDoors.begin(),m_vOpDoors.end());
//}

///////////////////////////////////////////////////////////////////
FltOperatingDoorData::FltOperatingDoorData( CAirportDatabase* pAirportDatabase )
:m_nID(-1)
,m_pAirportDB(pAirportDatabase)
{
	m_vDelStandOpDoorData.clear();
	m_vStandOpDoorData.clear();
}

FltOperatingDoorData::~FltOperatingDoorData( void )
{
	std::vector<StandOperatingDoorData*>::iterator iter = m_vStandOpDoorData.begin();
	while(iter != m_vStandOpDoorData.end())
	{
		delete (*iter);
		(*iter) = NULL;
		iter++;
	}
	m_vStandOpDoorData.clear();

	iter = m_vDelStandOpDoorData.begin();
	while(iter != m_vDelStandOpDoorData.end())
	{
		delete (*iter);
		(*iter) = NULL;
		iter++;
	}
	m_vDelStandOpDoorData.clear();
}

int FltOperatingDoorData::GetID()
{
	return m_nID;
}

void FltOperatingDoorData::SetID( int nID )
{
	m_nID = nID;
}

const FlightConstraint& FltOperatingDoorData::GetFltType()
{
	return m_FltType;
}

void FltOperatingDoorData::SetFltType( const FlightConstraint& fltType )
{
	m_FltType = fltType;
}

int FltOperatingDoorData::GetStandDoorCount()
{
	return m_vStandOpDoorData.size();
}

StandOperatingDoorData* FltOperatingDoorData::GetStandDoorData( int nIdx )
{
	if(nIdx <0 || nIdx >= GetStandDoorCount())
		return NULL;

	return m_vStandOpDoorData.at(nIdx);
}

void FltOperatingDoorData::AddStandDoor( StandOperatingDoorData* pData )
{
	m_vStandOpDoorData.push_back(pData);
}

void FltOperatingDoorData::ReadData( CADORecordset& adoRecordset )
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	CString strFltType;
	adoRecordset.GetFieldValue(_T("FLTTYPE"),strFltType);

	char szBuffer[1024] = {0};
	if (m_pAirportDB)
	{
		m_FltType.SetAirportDB(m_pAirportDB);
		m_FltType.setConstraintWithVersion(strFltType);
		m_FltType.getACType(szBuffer);
	}

	CString strSQL;
	strSQL.Format(_T("SELECT *  FROM  TB_STANDDOOROPERATING\
					 WHERE (PARENTID = %d)"),m_nID);
	try
	{
		long lRecordCount = 0;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{
			StandOperatingDoorData* pData = new StandOperatingDoorData;
			pData->SetAircraftType(CString(szBuffer));
			pData->SetAirportDatabase(m_pAirportDB);
			if (pData != NULL)
				pData->ReadData(adoRecordset);

			m_vStandOpDoorData.push_back(pData);

			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void FltOperatingDoorData::SaveData()
{
	CString strSQL;
	char szFltType[1024] = {0};
	m_FltType.WriteSyntaxStringWithVersion(szFltType);
	if (m_nID < 0)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO TB_FLTDOOROPERATING ( FLTTYPE )   \
						 VALUES ('%s')"),szFltType);

		m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
		UpdateData();

	std::vector<StandOperatingDoorData*>::iterator iter = m_vStandOpDoorData.begin();

	for (;iter != m_vStandOpDoorData.end(); ++iter)
	{
		(*iter)->SaveData(m_nID);
	}

	for(iter = m_vDelStandOpDoorData.begin(); iter != m_vDelStandOpDoorData.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vDelStandOpDoorData.clear();
}

void FltOperatingDoorData::UpdateData()
{
	CString strSQL;
	char szFltType[1024] = {0};
	m_FltType.WriteSyntaxStringWithVersion(szFltType);

	strSQL = _T("");
	strSQL.Format(_T("UPDATE TB_FLTDOOROPERATING\
					 SET FLTTYPE = '%s' WHERE (ID = %d)"),szFltType,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}

void FltOperatingDoorData::DeleteData()
{
	std::vector<StandOperatingDoorData*>::iterator iter  = m_vStandOpDoorData.begin();
	for (; iter != m_vStandOpDoorData.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	for (iter = m_vDelStandOpDoorData.begin(); iter != m_vDelStandOpDoorData.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM TB_FLTDOOROPERATING\
					 WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void FltOperatingDoorData::DelStandData( StandOperatingDoorData* pData )
{
	std::vector<StandOperatingDoorData*>::iterator iter = std::find(m_vStandOpDoorData.begin(),m_vStandOpDoorData.end(),pData);
	if (iter != m_vStandOpDoorData.end())
	{
		m_vDelStandOpDoorData.push_back(*iter);
		m_vStandOpDoorData.erase(iter);
	}
}

///////////////////////////////////////////////////////////////////
OperatingDoorSpec::OperatingDoorSpec(void)
:m_pAirportDB(NULL)
{
}

OperatingDoorSpec::~OperatingDoorSpec(void)
{
	std::vector<FltOperatingDoorData*>::iterator iter = m_vFltData.begin();
	while(iter != m_vFltData.end())
	{
		delete (*iter);
		(*iter) = NULL;
		iter++;
	}
	m_vFltData.clear();

	iter = m_vDelFltData.begin();
	while(iter != m_vDelFltData.end())
	{
		delete (*iter);
		(*iter) = NULL;
		iter++;
	}
	m_vDelFltData.clear();
}

int OperatingDoorSpec::GetFltDataCount()
{
	return m_vFltData.size();
}

FltOperatingDoorData* OperatingDoorSpec::GetFltDoorData( int nIdx )
{
	if (nIdx <0 || nIdx >= GetFltDataCount())
		return NULL;
		
	return m_vFltData.at(nIdx);
}

void OperatingDoorSpec::AddFlightOpDoor( FltOperatingDoorData* pData )
{
	m_vFltData.push_back(pData);
}

void OperatingDoorSpec::ReadData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM TB_FLTDOOROPERATING"));
	try
	{
		long lRecordCount = 0;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{
			FltOperatingDoorData* pData = new FltOperatingDoorData(m_pAirportDB);
			if (pData != NULL)
				pData->ReadData(adoRecordset);

			m_vFltData.push_back(pData);

			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void OperatingDoorSpec::SaveData()
{
	std::vector<FltOperatingDoorData*>::iterator iter  = m_vFltData.begin();
	for (; iter != m_vFltData.end(); ++iter)
	{
		(*iter)->SaveData();
	}

	for (iter = m_vDelFltData.begin(); iter != m_vDelFltData.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
		*iter = NULL;
	}
	m_vDelFltData.clear();
}

void OperatingDoorSpec::DelFltData( FltOperatingDoorData* pData )
{
	std::vector<FltOperatingDoorData*>::iterator iter = std::find(m_vFltData.begin(),m_vFltData.end(),pData);
	if (iter != m_vFltData.end())
	{
		m_vDelFltData.push_back(*iter);
		m_vFltData.erase(iter);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


StandOperatingDoorData::OperationDoor::OperationDoor()
{
    m_doorName = ""; 
    m_iHandType = ACTypeDoor::BothSide;
}

StandOperatingDoorData::OperationDoor::~OperationDoor()
{

}

void StandOperatingDoorData::OperationDoor::readDataVersion1( ArctermFile& p_file )
{
    char buf[128] = {0};

    p_file.getSubField(buf, ';');
    m_doorName = buf;

    p_file.getSubField(buf, ';');
    if(strcmp(buf, "L") == 0)
    {
        m_iHandType = ACTypeDoor::LeftHand;
    }
    else
    {
        m_iHandType = ACTypeDoor::RightHand;
    }
}

void StandOperatingDoorData::OperationDoor::parseString(const CString& strOp, ACTYPEDOORLIST* pAcDoors)
{
    CString strLeft;
    CString strRight;
    strLeft = strOp.Left(1);
    strRight = strOp.Right(strOp.GetLength() - 1);
    if (strLeft.Compare("L") == 0)
    {
        m_iHandType = ACTypeDoor::LeftHand;
    }
    else
    {
        m_iHandType = ACTypeDoor::RightHand;
    }

    int nID = atoi(strRight);
    int nCount = (int)pAcDoors->size();
    for(int i=0; i<nCount; i++)
    {
        if(pAcDoors->at(i)->GetID() == nID)
        {
            m_doorName = pAcDoors->at(i)->GetDoorName();
            break;
        }
    }
}

CString StandOperatingDoorData::OperationDoor::GetHandTypeString() const
{
    if (m_iHandType == ACTypeDoor::LeftHand)
    {
        return CString("Left");
    }
    else if (m_iHandType == ACTypeDoor::RightHand)
    {
        return CString("Right");
    }

    return CString("Left");
}

bool StandOperatingDoorData::OperationDoor::operator==( const OperationDoor& doorOp ) const
{
    if(m_doorName.Compare(doorOp.m_doorName) != 0)
    {
        return false;
    }

    if (doorOp.m_iHandType != m_iHandType)
    {
        return false;
    }

    return true;
}

CString StandOperatingDoorData::OperationDoor::GetDoorString() const
{
    CString strOp;
    if (m_iHandType == ACTypeDoor::LeftHand)
    {
        strOp.Format(_T("L%s"),m_doorName);
    }
    else if (m_iHandType == ACTypeDoor::RightHand)
    {
        strOp.Format(_T("R%s"),m_doorName);
    }
    return strOp;
}

void StandOperatingDoorData::OperationDoor::writeData(ArctermFile& p_file)
{
    p_file.appendField(m_doorName.GetBuffer());

    p_file.appendValue(";");
    if(m_iHandType == ACTypeDoor::LeftHand)
    {
        p_file.appendValue("L");
    }
    else
    {
        p_file.appendValue("R");
    }
}


