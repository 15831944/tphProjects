#include "StdAfx.h"
#include "..\Database\DBElementCollection_Impl.h"
#include "PaxBusParking.h"
#include "AirsideImportExportManager.h"


CPaxBusParking::CPaxBusParking()
:m_enumGateType(depgate)
{
}

CPaxBusParking::~CPaxBusParking()
{
}

const CPath2008& CPaxBusParking::GetPath() const
{
	return m_path;
}

void CPaxBusParking::SetPath( const CPath2008& _path )
{
	m_path = _path;
}

CPath2008& CPaxBusParking::getPath()
{
	return m_path;
}
void CPaxBusParking::SetGate(CString strGate)
{
	m_strGate = strGate;
}
CString CPaxBusParking::GetGate()
{
	return m_strGate;
}

void CPaxBusParking::SetGatetype(Gatetype enumGateType)
{
	m_enumGateType = enumGateType;
}
Gatetype CPaxBusParking::GetGatetype()
{
	return m_enumGateType;
}

void CPaxBusParking::SaveData(int nParentID)
{
	m_nPathID =  CADODatabase::SavePath2008IntoDatabase(m_path);
	CString strSQL;
	if (m_nID < 0)
	{
		GetInsertSQL(nParentID,strSQL);
		if (strSQL.IsEmpty())
			return;
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		GetUpdateSQL(strSQL);
		if (strSQL.IsEmpty())
			return;
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void CPaxBusParking::UpdatePath()
{
	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
}

void CPaxBusParking::InitFromDBRecordset(CADORecordset& recordset)
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	recordset.GetFieldValue(_T("GATE"),m_strGate);
	int nGateType;
	recordset.GetFieldValue(_T("GATETYPE"),nGateType);
	m_enumGateType = (Gatetype)nGateType;
	recordset.GetFieldValue(_T("PATHID"),m_nPathID);

	CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);
	//int nPointCount = -1;
	//recordset.GetFieldValue(_T("POINTCOUNT"),nPointCount);
	//recordset.GetFieldValue(_T("PATH"),nPointCount,m_path);
}

void CPaxBusParking::GetInsertSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO IN_PAXBUSPARKING\
					 (PROJID, GATE, GATETYPE, PATHID)\
					 VALUES (%d,'%s',%d,%d)"),
					 nParentID,m_strGate,(int)m_enumGateType,m_nPathID);
}

void CPaxBusParking::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE IN_PAXBUSPARKING\
					 SET GATE ='%s', GATETYPE =%d, PATHID =%d\
					 WHERE (ID = %d)"),m_strGate,(int)m_enumGateType,m_nPathID,m_nID);

}

void CPaxBusParking::GetDeleteSQL(CString& strSQL) const
{
	CADODatabase::DeletePathFromDatabase(m_nPathID);
	strSQL.Format(_T("DELETE FROM IN_PAXBUSPARKING\
					 WHERE (ID = %d)"),m_nID);
}

void CPaxBusParking::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(exportFile.GetProjectID());
	exportFile.getFile().writeField(m_strGate);
	exportFile.getFile().writeInt((int)m_enumGateType);
	//path
	int nPathCount = m_path.getCount();	
	exportFile.getFile().writeInt(nPathCount);
	for (int i =0; i < nPathCount; ++i)
	{
		exportFile.getFile().writePoint2008(m_path.getPoint(i));
	}
	exportFile.getFile().writeLine();
}
void CPaxBusParking::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);

	importFile.getFile().getField(m_strGate.GetBuffer(1024),1024);
	
	int nGateType = -1;
	importFile.getFile().getInteger(nGateType);
	m_enumGateType = (Gatetype)nGateType;

	//path
	int nPointCount = 0;
	importFile.getFile().getInteger(nPointCount);
	m_path.init(nPointCount);
	CPoint2008 *pPoint =  m_path.getPointList();

	for (int i = 0; i < nPointCount; ++i)
	{
		importFile.getFile().getPoint2008(pPoint[i]);
	}

	importFile.getFile().getLine();

	SaveData(importFile.getNewProjectID());
}

/////////////////////////////////////////////////////////////////////////////////////////
//
CPaxBusParkingList::CPaxBusParkingList()
{
}
CPaxBusParkingList::~CPaxBusParkingList()
{

}

void CPaxBusParkingList::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("SELECT ID, GATE, GATETYPE, PATHID\
					 FROM IN_PAXBUSPARKING\
					 WHERE (PROJID = %d)"), nParentID);

}
void CPaxBusParkingList::ExportPaxBusParkings(CAirsideExportFile& exportFile)
{
	CPaxBusParkingList paxBusParkingList;
	paxBusParkingList.ReadData(exportFile.GetProjectID());

	exportFile.getFile().writeField("PaxBusParking");
	exportFile.getFile().writeLine();

	size_t itemCount = paxBusParkingList.GetElementCount();

	for (size_t i =0; i < itemCount; ++i)
	{
		paxBusParkingList.GetItem(i)->ExportData(exportFile);	
	}
	exportFile.getFile().endFile();
}
void CPaxBusParkingList::ImportPaxBusParkings(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CPaxBusParking  paxBusParking;
		paxBusParking.ImportData(importFile);
	}
}
size_t CPaxBusParkingList::GetItemCount()
{
	return m_dataList.size();
}
CPaxBusParking *CPaxBusParkingList::GetItemByIndex(size_t nIndex)
{
	if (nIndex < m_dataList.size())
	{
		return m_dataList.at(nIndex);
	}

	return NULL;
}

void CPaxBusParkingList::DoNotCall()
{
	CPaxBusParkingList cpbl;
	cpbl.AddNewItem(NULL);
}