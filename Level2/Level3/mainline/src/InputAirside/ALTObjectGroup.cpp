#include "stdafx.h"
#include ".\altobjectgroup.h"
#include "../Database/ADODatabase.h"

ALTObjectGroup::ALTObjectGroup(void)
{
	m_nID = -1;
	m_nProjID = -1;
	m_enumType = ALT_UNKNOWN;
}

ALTObjectGroup::~ALTObjectGroup(void)
{
}
void ALTObjectGroup::ReadData(int nGroupID)
{
	if (nGroupID == -1)
		return;

	m_nID = nGroupID;

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * \
		FROM ALTOBJECTGROUP \
		WHERE (ID = %d)"),nGroupID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		ReadData(adoRecordset);
	}
}

void ALTObjectGroup::ReadData(CADORecordset& adoRecordset)
{
	CString strName1 = _T("");
	CString strName2 = _T("");
	CString strName3 = _T("");
	CString strName4 = _T("");

	int nType;
	adoRecordset.GetFieldValue(_T("TYPE"),nType);
	if (nType > ALT_UNKNOWN && nType < ALT_ATYPE_END)
	{
		m_enumType = (ALTOBJECT_TYPE)nType;
	}

	adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
	adoRecordset.GetFieldValue(_T("NAME_L1"),strName1);
	m_groupName.at(0) = strName1;

	adoRecordset.GetFieldValue(_T("NAME_L2"),strName2);
	m_groupName.at(1) = strName2;

	adoRecordset.GetFieldValue(_T("NAME_L3"),strName3);
	m_groupName.at(2) = strName3;	

	adoRecordset.GetFieldValue(_T("NAME_L4"),strName4);
	m_groupName.at(3) = strName4;
}

void ALTObjectGroup::SaveData(int nProjID)
{

	if (m_nID != -1)
		 return UpdateData();

	CString strSQL =_T("");
	strSQL.Format(_T("INSERT INTO ALTOBJECTGROUP \
		(PROJID,TYPE, NAME_L1, NAME_L2, NAME_L3, NAME_L4) \
		VALUES (%d,%d,'%s','%s','%s','%s')"),
		nProjID,(int)m_enumType,
		m_groupName.at(0).c_str(),m_groupName.at(1).c_str(),m_groupName.at(2).c_str(),m_groupName.at(3).c_str());

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		m_nProjID = nProjID;
}

int ALTObjectGroup::InsertData(int nProjID)
{
	m_nProjID = nProjID;

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, TYPE, NAME_L1, NAME_L2, NAME_L3, NAME_L4, TSTAMP \
					 FROM ALTOBJECTGROUP \
					 WHERE (PROJID = %d)"),nProjID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		CString strName0 = _T("");
		CString strName1 = _T("");
		CString strName2 = _T("");
		CString strName3 = _T("");
     
		int nID;
		adoRecordset.GetFieldValue(_T("ID"),nID);

		int nType;
		adoRecordset.GetFieldValue(_T("TYPE"),nType);
		ALTOBJECT_TYPE altObjType = ALT_UNKNOWN;
		if (nType > ALT_UNKNOWN && nType < ALT_ATYPE_END)
		{
			altObjType = (ALTOBJECT_TYPE)nType;
		}

		adoRecordset.GetFieldValue(_T("NAME_L1"),strName0);
		adoRecordset.GetFieldValue(_T("NAME_L2"),strName1);
		adoRecordset.GetFieldValue(_T("NAME_L3"),strName2);
		adoRecordset.GetFieldValue(_T("NAME_L4"),strName3);

		if (m_enumType==altObjType
			&& !strName0.Compare(m_groupName.at(0).c_str())
			&& !strName1.Compare(m_groupName.at(1).c_str())
			&& !strName2.Compare(m_groupName.at(2).c_str())
			&& !strName3.Compare(m_groupName.at(3).c_str()))
		{
			return nID;
		}

		adoRecordset.MoveNextData();
	}

	strSQL =_T("");
	strSQL.Format(_T("INSERT INTO ALTOBJECTGROUP \
					 (PROJID,TYPE, NAME_L1, NAME_L2, NAME_L3, NAME_L4) \
					 VALUES (%d,%d,'%s','%s','%s','%s')"),
					 nProjID,(int)m_enumType,
					 m_groupName.at(0).c_str(),m_groupName.at(1).c_str(),m_groupName.at(2).c_str(),m_groupName.at(3).c_str());

	int nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	return nID;
}

void ALTObjectGroup::UpdateData()
{
	if (m_nID == -1)
		return;


	CString strSQL =_T("");
	strSQL.Format(_T("UPDATE ALTOBJECTGROUP \
		SET TYPE =%d, NAME_L1 ='%s', NAME_L2 ='%s', NAME_L3 ='%s', NAME_L4 ='%s' \
		WHERE (ID = %d)"),(int)m_enumType,
		m_groupName.at(0).c_str(),m_groupName.at(1).c_str(),m_groupName.at(2).c_str(),m_groupName.at(3).c_str(),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}
void ALTObjectGroup::DeleteData()
{
	if (m_nID == -1)
		return;

	CString strSQL =_T("");
	strSQL.Format(_T("DELETE FROM ALTOBJECTGROUP \
		WHERE (ID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

bool ALTObjectGroup::IsFits( const ALTObjectIDList& idList ) const
{
	return m_groupName.IsFits(idList);
}

bool ALTObjectGroup::GetObjects(std::vector< ALTObject>& vObjct)
{
	if(m_nProjID == -1 || m_enumType == ALT_UNKNOWN)//the group object has not initialized
	{
		ASSERT(0);
		return false;

	}
	CString strSQL = _T("");
	
	CString strSelect = _T("");
	strSelect.Format(_T("SELECT ID, APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED\
		FROM         ALTOBJECT\
		WHERE     (TYPE = %d) AND (APTID IN\
		(SELECT     ID\
		FROM          ALTAIRPORT\
		WHERE      (PROJID = %d)))"),(int)m_enumType,m_nProjID);



	CString strWhere = _T("");
	
	for (size_t i =0; i < OBJECT_STRING_LEVEL; ++i)
	{
		if (m_groupName.at(i).length() == 0)
			break;

		CString strSub = _T("");
		strSub.Format(_T("  AND NAME_L%d = '%s'"),i+1,m_groupName.at(i).c_str());
		strWhere = strWhere + strSub;
	}

	strSQL = strSelect + strWhere;


	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		ALTObject object;
		int nObjID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nObjID);
		object.ReadObject(adoRecordset);
		object.setID(nObjID);

		vObjct.push_back(object);

		adoRecordset.MoveNextData();
	}

	return true;
}

ALTOBJECT_TYPE ALTObjectGroup::getType()
{
	return m_enumType;
}

ALTObjectID ALTObjectGroup::getName() const
{
	return m_groupName;
}

int ALTObjectGroup::getID() const
{
	return m_nID;
}

void ALTObjectGroup::setName( const ALTObjectID& groupName )
{
	m_groupName = groupName;
}

void ALTObjectGroup::setType( ALTOBJECT_TYPE ObjType )
{
	m_enumType = ObjType;
}

void ALTObjectGroup::setID( int nID )
{
	m_nID = nID;
}






