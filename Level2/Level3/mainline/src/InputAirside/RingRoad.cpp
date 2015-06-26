#include "Stdafx.h"
#include "RingRoad.h"
#include "AirsideImportExportManager.h"
#include "../Common/NaturalCubicSpline.h"
RingRoad::RingRoad()
:m_nID(-1)
,m_nPathID(-1)
,m_nOffset(2)
{


	//default value
	std::vector<CPoint2008> vPt;
	
	vPt.push_back( CPoint2008(-20,10,0) );
	vPt.push_back( CPoint2008(0,-12,0) );
	vPt.push_back( CPoint2008(20,10,0) );
	vPt.push_back( CPoint2008(0,25,0) );

	CPoint2008 ptList[1024];
	int nCount = (int)vPt.size();
	for(size_t i=0; i<vPt.size(); i++)
	{
		ptList[i].setX(vPt.at(i).getX());
		ptList[i].setY(vPt.at(i).getY());
		ptList[i].setZ(0.0);
	}

	InitServiceLocation(nCount, ptList);
}

RingRoad::~RingRoad()
{

}

void RingRoad::SetPath(CPath2008 path)
{
	m_path = path;
}

CPath2008 RingRoad::GetPath() const
{
	return m_path;
}

CPath2008 RingRoad::GetSmoothPath() const
{
	CPath2008 sPath;
	ClosedNaturalCubicSpline::calcNaturalCubic( &m_path, sPath);
	
	//remove the duplicate point

	if (sPath.getCount()< 2)
		return sPath;

	CPath2008 pathReturn;
	pathReturn.init(1,sPath.getPointList());
	//pathReturn.insertPointAfterAt(sPath.getPoint(0),0);
	
	for (int i = 0; i< sPath.getCount(); ++i)
	{
		CPoint2008 pt = sPath.getPoint(i);

		bool bFind = false;
		for (int j=0; j < pathReturn.getCount(); ++j)
		{
			if (pathReturn.getPoint(j) == pt)
			{
				bFind = true;
				break;
			}
		}

		if (bFind == false)
		{
			pathReturn.insertPointAfterAt(pt,pathReturn.getCount() -1);
		}
	}

	return pathReturn;
}

void RingRoad::ResetPath()
{
	m_path.clear();
}

void RingRoad::ConvertPathIntoLocalCoordinate(const CPoint2008& pt,const double& dDegree)
{
	//meter ->centimeter
	int nPtCount = m_path.getCount();
	for (int i =0; i < nPtCount;++i)
	{
		m_path[i] *= 100;
		m_path[i].setY(m_path[i].getY() * (-1));//change gdi coordinate to normal
	}
	m_path.Rotate(dDegree);
	m_path.DoOffset(pt.getX(),pt.getY(),pt.getZ());
}

void RingRoad::InitServiceLocation (int pathCount, const CPoint2008 *pointList)
{
	 m_path.init (pathCount, pointList);
}

void RingRoad::SaveData(int nFltDetailsID)
{
	m_nPathID =  CADODatabase::SavePath2008IntoDatabase(m_path);

	CString strSQL;

	strSQL.Format(_T("INSERT INTO RINGROAD \
					 (FLIGHTTYPEDETAILSITEMID, PATHID, OFFSET) \
					 VALUES (%d,%d, %d)"),
					 nFltDetailsID, m_nPathID, m_nOffset);

	m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void RingRoad::ReadData(int nFltDetailsID)
{
	CString strSQL;
	strSQL.Format(_T("SELECT * \
					 FROM RINGROAD\
					 WHERE (FLIGHTTYPEDETAILSITEMID = %d)"),nFltDetailsID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	if(!adoRecordset.IsEOF())
	{
		int nFltID;
		adoRecordset.GetFieldValue(_T("FLIGHTTYPEDETAILSITEMID"),nFltID);

		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("OFFSET"),m_nOffset);
		adoRecordset.GetFieldValue(_T("PATHID"),m_nPathID);
		CADODatabase::ReadPath2008FromDatabase(m_nPathID, m_path);
	}

}

void RingRoad::UpdateData(int nFltDetailsID)
{
	if(m_nPathID != -1)
		CADODatabase::DeletePathFromDatabase(m_nPathID);
	
	m_nPathID =  CADODatabase::SavePath2008IntoDatabase(m_path);

	if(m_nID != -1)
	{
		CString strSQL;
		strSQL.Format(_T("UPDATE RINGROAD  \
					 SET FLIGHTTYPEDETAILSITEMID=%d, PATHID=%d, OFFSET=%d\
					 WHERE (ID = %d)"), nFltDetailsID, m_nPathID, m_nOffset, m_nID);

		long lRecordCount = 0;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	}
	else
	{
		SaveData(nFltDetailsID);
	}
}

void RingRoad::DeleteData()
{
	CADODatabase::DeletePathFromDatabase(m_nPathID);

	CString strSQL;
	strSQL.Format(_T("DELETE FROM RINGROAD WHERE ( ID = %d)"), m_nID);
	
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void RingRoad::ExportData(CAirsideExportFile& exportFile, int nFltTypeDetailsItemID)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nPathID);
	exportFile.getFile().writeInt(m_nOffset);
	exportFile.getFile().writeInt(nFltTypeDetailsItemID);

	int nPathCount = m_path.getCount();
	exportFile.getFile().writeInt(nPathCount);
	for (int i=0; i<nPathCount; i++)
	{
		exportFile.getFile().writePoint2008(m_path.getPoint(i));
	}

}

void RingRoad::ImportData(CAirsideImportFile& importFile, int nFltTypeDetailsItemID)
{
	importFile.getFile().getInteger(m_nID);
	importFile.getFile().getInteger(m_nPathID);
	
	if(importFile.getVersion() > 1025)
		importFile.getFile().getInteger(m_nOffset);
	else
		m_nOffset = 2;
	
	int nBlank = 0;
	importFile.getFile().getInteger(nBlank);

	int nPathCount = 0;
	importFile.getFile().getInteger(nPathCount);
	m_path.init(nPathCount);
	CPoint2008 *pPoint = m_path.getPointList();
	for (int i = 0; i < nPathCount; ++i)
	{
		importFile.getFile().getPoint2008(pPoint[i]);
	}

	SaveData(nFltTypeDetailsItemID);
}








