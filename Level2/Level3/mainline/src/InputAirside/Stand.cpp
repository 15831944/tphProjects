#include "StdAfx.h"
#include ".\stand.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"
#include "Taxiway.h"
#include "../Common/GetIntersection.h"
#include "../Common/DistanceLineLine.h"
#include "ALTObjectDisplayProp.h"


Stand::Stand(void)
{
	//m_bUsePushback = false;
}

Stand::~Stand(void)
{
}

const CPoint2008& Stand::GetServicePoint() const
{
	return m_servicePoint;
}

//const Path& Stand::GetInContrain() const
//{
//	return m_inContrain;
//}
//
//const Path& Stand::GetOutConstrain() const
//{
//	return m_outConstrain;
//}


void Stand::SetServicePoint( const CPoint2008& _pt )
{
	m_servicePoint = _pt;
}

//void Stand::SetInContrain( const Path& _path )
//{
//	m_inContrain = _path;
//}
//
//void Stand::SetOutConstrain( const Path& _path )
//{
//	m_outConstrain = _path;
//}
//
//bool Stand::IsUsePushBack() const
//{
//	return m_bUsePushback;
//}
//
//bool Stand::UsePushBack( bool _b )
//{
//	return m_bUsePushback = _b;
//}
//SELECT NAME_L1, NAME_L2, NAME_L3, NAME_L4, USEPB, TSTAMP, SLOCATION, 
//INCONS, OUTCONS, INCONSCOUNT, OUTCONSCOUNT, SVCOUNT, SVCLOCT, 
//SVCLOCTID, INCONSID, OUTCONSID
//FROM GATE_VIEW

void Stand::ReadObject(int nObjID)
{

	m_nObjID = nObjID;
	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{	
		
		ALTObject::ReadObject(adoRecordset);


		double ptx,pty,ptz;
		adoRecordset.GetFieldValue(_T("LCTX"),ptx);

		adoRecordset.GetFieldValue(_T("LCTY"),pty);

		adoRecordset.GetFieldValue(_T("LCTZ"),ptz);
		m_servicePoint.setPoint(ptx,pty,ptz);
		
		//adoRecordset.GetFieldValue(_T("INCONSID"),m_nInconsID);
		//int nInCount = -1;
		//adoRecordset.GetFieldValue(_T("INCONSCOUNT"),nInCount);
		//adoRecordset.GetFieldValue(_T("INCONS"),nInCount,m_inContrain);

		//adoRecordset.GetFieldValue(_T("OUTCONSID"),m_nOutconsID);
		//int nOutCount = -1;
		//adoRecordset.GetFieldValue(_T("OUTCONSCOUNT"),nOutCount);
		//adoRecordset.GetFieldValue(_T("OUTCONS"),nOutCount,m_outConstrain);


		//adoRecordset.GetFieldValue(_T("USEPB"),m_bUsePushback);
	}

	m_vLeadInLines.ReadData(getID());
	m_vLeadOutLines.ReadData(getID());

}
int Stand::SaveObject(int nAirportID)
{

	int nObjID = ALTObject::SaveObject(nAirportID,ALT_STAND);

	//m_nInconsID = CADODatabase::SavePathIntoDatabase(m_inContrain);
	//m_nOutconsID = CADODatabase::SavePathIntoDatabase(m_outConstrain);

	m_vLeadInLines.SaveData(getID());
	m_vLeadOutLines.SaveData(getID());

	CADODatabase::ExecuteSQLStatement(GetInsertScript(nObjID));

	return nObjID;

}
void Stand::UpdateServicePoint()
{
	CString strSQL = _T("");

	strSQL.Format(_T("UPDATE STANDPROP SET LCTX =%f, LCTY =%f, LCTZ =%f \
					 WHERE OBJID = %d"),
					 m_servicePoint.getX(),m_servicePoint.getY(),m_servicePoint.getZ(),m_nObjID);
	CADODatabase::ExecuteSQLStatement(strSQL);	
}
//void Stand::UpdateInConstraint()
//{
//	if (m_nInconsID == -1)
//		m_nInconsID = CADODatabase::SavePathIntoDatabase(m_inContrain);
//	else
//		CADODatabase::UpdatePathInDatabase(m_inContrain, m_nInconsID);
//}
//void Stand::UpdateOutConstraint()
//{
//	if (m_nOutconsID == -1)
//		m_nOutconsID = CADODatabase::SavePathIntoDatabase(m_outConstrain);
//	else
//		CADODatabase::UpdatePathInDatabase(m_outConstrain,m_nOutconsID);
//}
void Stand::UpdateObject(int nObjID)
{
	m_vLeadInLines.SaveData(nObjID);
	m_vLeadOutLines.SaveData(nObjID);
	ALTObject::UpdateObject(nObjID);
	CADODatabase::ExecuteSQLStatement(GetUpdateScript(nObjID));
}
void Stand::DeleteObject(int nObjID)
{
	m_vLeadInLines.DeleteData();
	m_vLeadOutLines.DeleteData();
	ALTObject::DeleteObject(nObjID);
	//CADODatabase::DeletePathFromDatabase(m_nInconsID);
	//CADODatabase::DeletePathFromDatabase(m_nOutconsID);

	CADODatabase::ExecuteSQLStatement(GetDeleteScript(nObjID));

}


CString Stand::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("SELECT APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, LCTX, LCTY, LCTZ, USEPB, \
		INCONSID, INCONSCOUNT, INCONS, OUTCONSID, OUTCONSCOUNT, OUTCONS, \
		TSTAMP\
		FROM STAND_VIEW\
		WHERE ID = %d"),nObjID);

	return strSQL;
}
CString Stand::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("INSERT INTO STANDPROP(OBJID, LCTX, LCTY, LCTZ) \
					 VALUES (%d,%f, %f, %f)"),
					 nObjID,m_servicePoint.getX(),m_servicePoint.getY(),m_servicePoint.getZ());

	return strSQL;


}
CString Stand::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("UPDATE STANDPROP SET LCTX =%f, LCTY =%f, LCTZ =%f \
		WHERE OBJID = %d"),
		m_servicePoint.getX(),m_servicePoint.getY(),m_servicePoint.getZ(),nObjID);

	return strSQL;
}
CString Stand::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("DELETE FROM STANDPROP	WHERE OBJID = %d"),nObjID);

	return strSQL;

}

ALTObject * Stand::NewCopy()
{
	Stand * reslt = new Stand;
	reslt->SetServicePoint(m_servicePoint);
	
	reslt->m_vLeadInLines.CopyData( m_vLeadInLines );
	reslt->m_vLeadOutLines.CopyData( m_vLeadOutLines );

	reslt->__super::CopyData(this);
	return reslt;
}

void Stand::ExportStands(int nAirportID,CAirsideExportFile& exportFile)
{

	std::vector<int> vStandID;
	ALTAirport::GetStandsIDs(nAirportID,vStandID);
	int nStandCount = static_cast<int>(vStandID.size());
	for (int i =0; i < nStandCount;++i)
	{
		Stand standObj;
		standObj.ReadObject(vStandID[i]);
		standObj.ExportStand(exportFile);
	}


}
void Stand::ExportStand(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	//airport id
	exportFile.getFile().writeInt(m_nAptID);
	//service location
	exportFile.getFile().writePoint2008(m_servicePoint);

	
	{
		//lead in Lines
		size_t nLeadInLineCount = m_vLeadInLines.GetItemCount();
		exportFile.getFile().writeInt((int)nLeadInLineCount);
		for(size_t i=0; i<nLeadInLineCount;i++)
		{
			StandLeadInLine * pLine = &m_vLeadInLines.ItemAt(i);
			exportFile.getFile().writeInt(pLine->GetID());
			exportFile.getFile().writeField( pLine->GetName() );
			CPath2008& path = pLine->getPath();
			int nPtCnt = path.getCount();
			exportFile.getFile().writeInt(nPtCnt);
			for(int j=0;j<nPtCnt;j++)
			{
				exportFile.getFile().writePoint2008(path.getPoint(j));
			}
		}
		exportFile.getFile().writeLine();
		//lead out lines
		size_t nLeadoutLineCount = m_vLeadOutLines.GetItemCount();
		exportFile.getFile().writeInt((int)nLeadoutLineCount);
		for(size_t i =0; i< nLeadoutLineCount;i++)
		{
			StandLeadOutLine * pLine = &m_vLeadOutLines.ItemAt(i);
			exportFile.getFile().writeInt(pLine->GetID());
			exportFile.getFile().writeField(pLine->GetName());
			exportFile.getFile().writeInt(pLine->IsPushBack());
			CPath2008& path = pLine->getPath();
			int nPtCnt = path.getCount();
			exportFile.getFile().writeInt(nPtCnt);
			for(int j=0;j<path.getCount();j++)
			{
				exportFile.getFile().writePoint2008(path.getPoint(j));
			}
		}
		exportFile.getFile().writeLine();
	}


	StandDisplayProp dspProp;
	dspProp.ReadData(m_nObjID);
	dspProp.ExportDspProp(exportFile);

}

void ConvertInOutConstrainToLeadInOutLine(const CPath2008& inCons, const CPath2008& outCons, bool bPushback, Stand *pStand)
{
	if(inCons.getCount())
	{
		StandLeadInLine leadInline;
		leadInline.SetPath(inCons);
		leadInline.SetName("Line 1");
		pStand->m_vLeadInLines.AddNewItem(leadInline);

		if(bPushback)
		{
			StandLeadOutLine leadoutLine;
			CPath2008 outLinePath = inCons;
			std::reverse(outLinePath.getPointList(), outLinePath.getPointList() + outLinePath.getCount() );
			leadoutLine.SetPath(outLinePath);
			leadoutLine.SetPushBack(true);
			leadoutLine.SetName("Line 1");
			pStand->m_vLeadOutLines.AddNewItem(leadoutLine);
		}
	}
	if( outCons.getCount() && !bPushback )
	{
		StandLeadOutLine leadOutLine;
		leadOutLine.SetName("Line 1");
		leadOutLine.SetPath(outCons);
		leadOutLine.SetPushBack(bPushback);
		pStand->m_vLeadOutLines.AddNewItem(leadOutLine);
	}
}


void Stand::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	
	int nAirportID = -1;
	importFile.getFile().getInteger(nAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nAirportID);
	//service location
	importFile.getFile().getPoint2008(m_servicePoint);	

	
	if( importFile.getVersion() < 1038 )
	{
		CPath2008 inContrain;
		CPath2008 outContrain;
		//in constrain
		int nInconsCount = 0;
		importFile.getFile().getInteger(nInconsCount);
		inContrain.init(nInconsCount);

		CPoint2008* pPointIncons = inContrain.getPointList();
		for (int i =0; i < nInconsCount; ++i)
		{
			importFile.getFile().getPoint2008(pPointIncons[i]);
		}
		
		//out cons
		int nOutConsCount = 0;
		importFile.getFile().getInteger(nOutConsCount);
		outContrain.init(nOutConsCount);
		CPoint2008* pPointOutCons = outContrain.getPointList();
		for (int i =0; i < nOutConsCount; ++i)
		{
			importFile.getFile().getPoint2008(pPointOutCons[i]);
		}
		int nUsePushBack = 0;
		importFile.getFile().getInteger(nUsePushBack);
		bool bUsePushback = (nUsePushBack == 1)?true:false ;

		ConvertInOutConstrainToLeadInOutLine(inContrain,outContrain,bUsePushback,this);
		//m_inContrain.init(0);
		//m_outConstrain.init(0);
        importFile.getFile().getLine();
		
		int nOldObjectID = m_nObjID;
		int nNewObjID = SaveObject(m_nAptID);


		importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);

	}
	else
	{
		std::vector<int> vOldLeadInLineIDs;
		std::vector<int> vOldLeadOutLineIDs;

		//lead in lines
		int nLeadInLineCount = 0;
		importFile.getFile().getInteger(nLeadInLineCount);
		for(int i=0;i<nLeadInLineCount;i++)
		{
			StandLeadInLine newInLine;
			int nLeadInLineId;
			importFile.getFile().getInteger(nLeadInLineId);
			vOldLeadInLineIDs.push_back(nLeadInLineId);

			CString strName;
			importFile.getFile().getField(strName.GetBuffer(255),255);
			int nPtCnt = 0;
			importFile.getFile().getInteger(nPtCnt);
			CPath2008 linePath;
			linePath.init(nPtCnt);
			for(int j=0;j<nPtCnt;j++)
			{
				importFile.getFile().getPoint2008(linePath[j]);
			}
			
			newInLine.SetName(strName);
			newInLine.SetPath(linePath);

			m_vLeadInLines.AddNewItem(newInLine);
		}
		importFile.getFile().getLine();
		//lead out lines
		int nLeadOutLineCount = 0;
		importFile.getFile().getInteger(nLeadOutLineCount);
		for(int i=0;i< nLeadOutLineCount;i++)
		{
			StandLeadOutLine newOutLine;
			int nLeadOutLineID;
			importFile.getFile().getInteger(nLeadOutLineID);
			vOldLeadOutLineIDs.push_back(nLeadOutLineID);

			CString strName;
			importFile.getFile().getField(strName.GetBuffer(255),255);
			BOOL bPush;
			importFile.getFile().getInteger(bPush);
			int nPtCnt = 0;
			importFile.getFile().getInteger(nPtCnt);
			CPath2008 linepath;
			linepath.init(nPtCnt);
			for(int j=0;j<nPtCnt;j++)
			{
				importFile.getFile().getPoint2008(linepath[j]);
			}
			newOutLine.SetName(strName);
			newOutLine.SetPushBack(bPush);
			newOutLine.SetPath(linepath);
			m_vLeadOutLines.AddNewItem(newOutLine);
		}
		importFile.getFile().getLine();
		
		int nOldObjectID = m_nObjID;
		int nNewObjID = SaveObject(m_nAptID);


		importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);

		ASSERT(vOldLeadInLineIDs.size() ==  m_vLeadInLines.GetItemCount() );
		ASSERT(vOldLeadOutLineIDs.size() == m_vLeadOutLines.GetItemCount() );

		for(int i=0;i< (int)m_vLeadInLines.GetItemCount();i++ )
		{
			importFile.AddStandLeadInLineItemIndexMap(vOldLeadInLineIDs.at(i), m_vLeadInLines.ItemAt(i).GetID() );
		}
		for(int i=0;i< (int)m_vLeadOutLines.GetItemCount();i++)
		{
			importFile.AddStandLeadOutLineItemIndexMap(vOldLeadOutLineIDs.at(i), m_vLeadOutLines.ItemAt(i).GetID());
		}

	}	

	StandDisplayProp dspProp;
	dspProp.ImportDspProp(importFile,m_nObjID);
}

void Stand::OnRotate( DistanceUnit dx,const ARCVector3& vCenter )
{
	//m_inContrain.DoOffset(-vCenter[VX],-vCenter[VY],-vCenter[VZ]);
	//m_outConstrain.DoOffset(-vCenter[VX],-vCenter[VY],-vCenter[VZ]);

	//m_inContrain.Rotate(dx);
	//m_outConstrain.Rotate(dx);

	//m_inContrain.DoOffset(vCenter[VX],vCenter[VY],vCenter[VZ]);
	//m_outConstrain.DoOffset(vCenter[VX],vCenter[VY],vCenter[VZ]);

	for(size_t i=0;i< m_vLeadInLines.GetItemCount();i++)
	{
		StandLeadInLine * pLine = &m_vLeadInLines.ItemAt(i);
		pLine->getPath().DoOffset(-vCenter[VX],-vCenter[VY],-vCenter[VZ]);
		pLine->getPath().Rotate(dx);
		pLine->getPath().DoOffset(vCenter[VX],vCenter[VY],vCenter[VZ]);
	}

	for(size_t i=0;i< m_vLeadOutLines.GetItemCount();i++)
	{
		StandLeadOutLine * pLine = &m_vLeadOutLines.ItemAt(i);
		pLine->getPath().DoOffset(-vCenter[VX],-vCenter[VY],-vCenter[VZ]);
		pLine->getPath().Rotate(dx);
		pLine->getPath().DoOffset(vCenter[VX],vCenter[VY],vCenter[VZ]);
	}
	
}

IntersectionNodeList Stand::GetIntersectNodes() const
{
	IntersectionNodeList vIntersectNode;
	IntersectionNode::ReadIntersecNodeList(m_nObjID,vIntersectNode);
	return vIntersectNode;
}

//Path Stand::GetLeadLine( LeadLineType ltype, int nidx ) const
//{
//	if(ltype == IN_LINE)
//		return GetInContrain();
//	else
//		return GetOutConstrain();
//}

void Stand::DoOffset( DistanceUnit dx, DistanceUnit dy, DistanceUnit dz )
{
	m_servicePoint.DoOffset(dx,dy,dz);
	//m_inContrain.DoOffset(dx,dy,dz);
	//m_outConstrain.DoOffset(dx,dy,dz);
	for(size_t i=0;i< m_vLeadInLines.GetItemCount();i++)
	{
		StandLeadInLine& pLine = m_vLeadInLines.ItemAt(i);
		pLine.getPath().DoOffset(dx,dy,dz);
	}

	for(size_t i=0;i< m_vLeadOutLines.GetItemCount();i++)
	{
		StandLeadOutLine& pLine = m_vLeadOutLines.ItemAt(i);
		pLine.getPath().DoOffset(dx,dy,dz);
	}

}

bool Stand::CopyData(const  ALTObject* pObj )
{
	if(this == pObj)
		return true;

	if( pObj->GetType() == GetType() )
	{
		Stand * otherStand = (Stand*)pObj;
		m_servicePoint = otherStand->GetServicePoint();
		
	//	m_vLeadInLines.CopyData(otherStand->m_vLeadInLines);
	//	m_vLeadOutLines.CopyData(otherStand->m_vLeadOutLines);		
		m_vLeadInLines = otherStand->m_vLeadInLines;
		m_vLeadOutLines = otherStand->m_vLeadOutLines;
	}

	return __super::CopyData(pObj);
}

StandLeadInLineList& Stand::GetLeadInLineList()
{
	return m_vLeadInLines;
}

StandLeadOutLineList& Stand::GetLeadOutLineList()
{
	return m_vLeadOutLines;
}

BOOL Stand::IsAllLinesValid() const
{
	return m_vLeadInLines.IsValid() && m_vLeadOutLines.IsValid();
}

const GUID& Stand::getTypeGUID()
{
	// {D9C96220-8197-44cf-ACA0-5199A5FD598D}
	static const GUID name = 
	{ 0xd9c96220, 0x8197, 0x44cf, { 0xac, 0xa0, 0x51, 0x99, 0xa5, 0xfd, 0x59, 0x8d } };
	
	return name;
}

ALTObjectDisplayProp* Stand::NewDisplayProp()
{
	return new StandDisplayProp();
}
