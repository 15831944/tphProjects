#include "StdAfx.h"
#include "IntersectItem.h"

#include "Runway.h"
#include "Taxiway.h"
#include "Stand.h"
#include "DeicePad.h"
#include "Stretch.h"
#include "CircleStretch.h"
#include "DeicePad.h"
//////////////////////////////////////////////////////////////////////////
int IntersectItem::GetObjectID()const
{
	if(m_pObject.get())
		return m_pObject->getID();
	return m_nObjectID;
}

void IntersectItem::SaveData( int nParentID )
{	
	if( GetUID() >=0 )
	{
		CString strSQL;
		GetUpdateSQL(strSQL);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
	else // insert data
	{	
		CString strSQL;
		GetInsertSQL(nParentID, strSQL);
		m_nUID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}	
}

void IntersectItem::DeleteData()
{
	CString strSQL;
	GetDeleteSQL(strSQL);
	CADODatabase::ExecuteSQLStatement(strSQL);	
}

std::vector<IntersectItem*> IntersectItem::ReadData(int nParentID)
{

	std::vector<IntersectItem*> vReslt;
	CString strSQL;
	GetSelectSQL(nParentID, strSQL);
	long nCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		vReslt.push_back( ReadData(adoRecordset) );		
		adoRecordset.MoveNextData();
	} 

	return vReslt;
}


IntersectItem* IntersectItem::ReadData( CADORecordset& recordset )
{
	int nID = -1;
	recordset.GetFieldValue("ID", nID);
	int tType;
	recordset.GetFieldValue("TYPE", tType);
	int iObjID = -1;
	recordset.GetFieldValue("OBJECTID", iObjID);
	double distInItem;
	recordset.GetFieldValue("DISTINITEM", distInItem);
	int leadInOutIndex;
	recordset.GetFieldValue("LEADINOUTLINEIDX", leadInOutIndex);
	int nodeID;
	recordset.GetFieldValue("INTERSECTIONNODEID", nodeID);


	IntersectItem * newItem = NULL;
	if( tType == IntersectItem::RUNWAY_INTESECT )
	{
		RunwayIntersectItem* rwItem = new RunwayIntersectItem;
		newItem = rwItem;
	}
	else if(tType == IntersectItem::TAXIWAY_INTERSECT)
	{
		TaxiwayIntersectItem * taxiItem = new TaxiwayIntersectItem;
		newItem = taxiItem;
	}
	else if(tType == IntersectItem::STAND_LEADINLINE_INTERSECT)
	{
		StandLeadInLineIntersectItem* leadinItem = new StandLeadInLineIntersectItem;
		leadinItem->SetLeadInLineID(leadInOutIndex);
		newItem = leadinItem;

	}
	else if( tType == IntersectItem::STAND_LEADOUT_LINEINTERSECT)
	{
		StandLeadOutLineIntersectItem* leadoutItem = new StandLeadOutLineIntersectItem;
		leadoutItem->SetLeadOutLineID(leadInOutIndex);
		newItem = leadoutItem;
	}
	else if (tType == IntersectItem::STRETCH_INTERSECT)
	{
		StretchIntersectItem* stretchItem = new StretchIntersectItem;
		newItem = stretchItem;
	}
	else if (tType == IntersectItem::CIRCLESTRETCH_INTERSECT)
	{
		CircleStretchIntersectItem* circleStretchItem = new CircleStretchIntersectItem;
		newItem = circleStretchItem;
	}
	else if( tType == IntersectItem::DEICE_INCONSTRAIN )
	{
		DeicePadInContrainItem * deicePadInItem = new DeicePadInContrainItem;
		newItem = deicePadInItem;
	}
	else if(tType== IntersectItem::DEICE_OUTCONSTRAIN)
	{
		DeicePadOutConstrainItem * deicePadOutItem = new DeicePadOutConstrainItem;
		newItem = deicePadOutItem;
	}

	if(newItem)
	{
		newItem->SetObjectID(iObjID);
		newItem->SetDistInItem(distInItem);
		newItem->SetUID(nID);
	}

	return newItem;
}

void IntersectItem::GetInsertSQL( int nParentID,CString& strSQL )
{
	strSQL.Format("INSERT INTO INTERSECTIONNODEITEM ( OBJECTID,DISTINITEM,LEADINOUTLINEIDX,TYPE,INTERSECTIONNODEID) VALUES ( %d, %f, %d, %d,%d)",
		GetObjectID(), GetDistInItem(), GetStandLeadInOutLineID(), GetType(), nParentID);
}

void IntersectItem::GetUpdateSQL( CString& strSQL )
{
	ASSERT( GetUID() >=0 );
	strSQL.Format("UPDATE INTERSECTIONNODEITEM SET OBJECTID = %d, DISTINITEM = %f, LEADINOUTLINEIDX = %d, TYPE = %d WHERE ID = %d",
		GetObjectID(), GetDistInItem(), GetStandLeadInOutLineID(), GetType(), GetUID());
}

void IntersectItem::GetDeleteSQL( CString& strSQL )
{
	ASSERT( GetUID() >=0);
	strSQL.Format("DELETE FROM INTERSECTIONNODEITEM WHERE ID = %d", GetUID());
}

void IntersectItem::GetSelectSQL( int nParentID,CString& strSQL )
{
	strSQL.Format(_T("SELECT * FROM INTERSECTIONNODEITEM WHERE (INTERSECTIONNODEID = %d)"), nParentID);
}

bool IntersectItem::IsIdent( IntersectItem* pItem )
{
	if(pItem && pItem->GetObjectID() == this->GetObjectID() && pItem->GetType() == this->GetType() && pItem->GetStandLeadInOutLineID() == this->GetStandLeadInOutLineID() )
		return true;
	return false;
}

ALTObject* IntersectItem::GetObject()
{
	if(! m_pObject.get() )
		m_pObject = ALTObject::ReadObjectByID(GetObjectID());
	return m_pObject.get();
}

void IntersectItem::SetObject( ALTObject* pObj )
{
	m_pObject = pObj;
	m_nObjectID = pObj->getID();
}

bool IntersectItem::CompareLess( IntersectItem* pItem1, IntersectItem*pItem2 )
{
	ASSERT( pItem1 && pItem2 );
	if( pItem1->GetType() < pItem2->GetType() )
		return true;
	if(pItem1->GetType() > pItem2->GetType() )
		return false;

	if( pItem1->GetObjectID() < pItem2->GetObjectID() )
		return true;
	if( pItem1->GetObjectID() > pItem2->GetObjectID() )
		return false;

	if( pItem1->GetStandLeadInOutLineID() < pItem2->GetStandLeadInOutLineID() )
		return true;
	if( pItem1->GetStandLeadInOutLineID() > pItem2->GetStandLeadInOutLineID() )
		return false;

	ASSERT(FALSE);
	return pItem1 < pItem2;
}

CString IntersectItem::GetNameString()
{
	ALTObject *pObj = GetObject();
	ASSERT(pObj);
	return pObj->GetObjNameString();
}

int IntersectItem::GetStandLeadInOutLineID() const
{
	return -1;
}

DistanceUnit IntersectItem::GetDistInItem() const
{
	return m_dDistInItem;
}

int IntersectItem::GetUID() const
{
	return m_nUID;
}

IntersectItem::IntersectItem()
{
	m_nUID = -1;
	m_nObjectID =-1;
	m_dDistInItem = 0;
}



//////////////////////////////////////////////////////////////////////////

void RunwayIntersectItem::SetRunway( Runway*pRunway )
{
	m_pObject = pRunway;
}

Runway* RunwayIntersectItem::GetRunway()
{
	ALTObject * pObj = GetObject();
	ASSERT(pObj->GetType() == ALT_RUNWAY);
	return (Runway*)pObj;
}

CPath2008 RunwayIntersectItem::GetPath()
{
	return GetRunway()->GetPath();
}

bool RunwayIntersectItem::operator<( const RunwayIntersectItem& otherItem ) const
{
	return GetObjectID() < otherItem.GetObjectID();
}

IntersectItem* RunwayIntersectItem::Clone() const
{
	RunwayIntersectItem* pNewItem = new RunwayIntersectItem();
	*pNewItem = *this;
	return pNewItem;
}
//////////////////////////////////////////////////////////////////////////
void TaxiwayIntersectItem::SetTaxiway( Taxiway* pTaxiway )
{
	m_pObject = pTaxiway;
}

Taxiway * TaxiwayIntersectItem::GetTaxiway()
{
	ALTObject* pObj = GetObject();
	ASSERT(pObj->GetType() == ALT_TAXIWAY);
	return (Taxiway*)pObj;		 
}

CString TaxiwayIntersectItem::GetNameString()
{
	Taxiway* pTaxiway = GetTaxiway();
	ASSERT(pTaxiway);
	return pTaxiway->GetMarking().c_str();
}

CPath2008 TaxiwayIntersectItem::GetPath()
{
	return GetTaxiway()->GetPath();
}

bool TaxiwayIntersectItem::operator<( const TaxiwayIntersectItem& otherItem ) const
{
	return GetObjectID() < otherItem.GetObjectID();
}

IntersectItem* TaxiwayIntersectItem::Clone() const
{
	TaxiwayIntersectItem* pNewItem = new TaxiwayIntersectItem;
	*pNewItem = *this;
	return pNewItem;
}
//////////////////////////////////////////////////////////////////////////
void CircleStretchIntersectItem::SetCircleStretch(CircleStretch* pCirStretch)
{
	m_pObject = pCirStretch;
}

CircleStretch* CircleStretchIntersectItem::GetCircleStretch()
{
	ALTObject* pObj = GetObject();
	ASSERT(pObj->GetType() == ALT_CIRCLESTRETCH);
	return (CircleStretch*)pObj;
}

CPath2008 CircleStretchIntersectItem::GetPath()
{
	return GetCircleStretch()->GetPath();
}

bool CircleStretchIntersectItem::operator<( const CircleStretchIntersectItem& otherItem ) const
{
	return GetObjectID() < otherItem.GetObjectID();
}

IntersectItem* CircleStretchIntersectItem::Clone() const
{
	CircleStretchIntersectItem* pnewItem = new CircleStretchIntersectItem();
	*pnewItem = *this;
	return pnewItem;
}
//////////////////////////////////////////////////////////////////////////
void StretchIntersectItem::SetStretch(Stretch* pStretch)
{
	m_pObject = pStretch;
}

Stretch* StretchIntersectItem::GetStretch()
{
	ALTObject* pObj = GetObject();
	ASSERT(pObj->GetType() == ALT_STRETCH);
	return (Stretch*)pObj;
}

CPath2008 StretchIntersectItem::GetPath()
{
	return GetStretch()->GetPath();
}

bool StretchIntersectItem::operator<( const StretchIntersectItem& otherItem ) const
{
	return GetObjectID() < otherItem.GetObjectID();
}

IntersectItem* StretchIntersectItem::Clone() const
{
	StretchIntersectItem* pnewItem = new StretchIntersectItem();
	*pnewItem = *this;
	return pnewItem;
}
//////////////////////////////////////////////////////////////////////////
void StandLeadInLineIntersectItem::SetStand( Stand* pStand )
{
	m_pObject = pStand;
}

Stand * StandLeadInLineIntersectItem::GetStand()
{
	ALTObject* pObj = GetObject();
	ASSERT(pObj->GetType()==ALT_STAND );
	return (Stand*)pObj;
}

CString StandLeadInLineIntersectItem::GetNameString()
{
	Stand* pStand = GetStand();
	ASSERT(pStand);
	StandLeadInLine*leadInLine = pStand->GetLeadInLineList().GetItemByID( GetStandLeadInOutLineID() );
	CString strFt;
	if(leadInLine)
		strFt.Format("%s-%s(In)", pStand->GetObjNameString(),leadInLine->GetName() );
	return strFt;
}

CPath2008 StandLeadInLineIntersectItem::GetPath()
{
	StandLeadInLine* inLine = GetStand()->GetLeadInLineList().GetItemByID(GetStandLeadInOutLineID());
	if(inLine)
		return inLine->GetPath();
	
	ASSERT(FALSE);	
	return CPath2008();
}

bool StandLeadInLineIntersectItem::operator<( const StandLeadInLineIntersectItem& otherItem ) const
{
	if( GetObjectID() < otherItem.GetObjectID() )
		return true;
	if( GetStandLeadInOutLineID() < otherItem.GetStandLeadInOutLineID() )
		return true;
	return false;
}

IntersectItem* StandLeadInLineIntersectItem::Clone() const
{
	StandLeadInLineIntersectItem* pnewItem = new StandLeadInLineIntersectItem();
	*pnewItem = *this;
	return pnewItem;
}
//////////////////////////////////////////////////////////////////////////
void StandLeadOutLineIntersectItem::SetStand( Stand* pStand )
{
	m_pObject = pStand;
}

Stand * StandLeadOutLineIntersectItem::GetStand()
{
	ALTObject * pObj = GetObject();
	ASSERT(pObj->GetType()==ALT_STAND );
	return (Stand*)pObj;
}

CString StandLeadOutLineIntersectItem::GetNameString()
{
	Stand* pStand = GetStand();
	ASSERT(pStand);
	StandLeadOutLine* leadOutLine = pStand->GetLeadOutLineList().GetItemByID( GetStandLeadInOutLineID() );
	CString strFt;
	if( leadOutLine )
		strFt.Format("%s-%s(Out)",pStand->GetObjNameString(), leadOutLine->GetName() );
	return strFt;
}

CPath2008 StandLeadOutLineIntersectItem::GetPath()
{
	StandLeadOutLine* pOutLine = GetStand()->GetLeadOutLineList().GetItemByID(GetStandLeadInOutLineID());
	if(pOutLine)
		return pOutLine->GetPath();	

	ASSERT(FALSE);
	return CPath2008();
}

bool StandLeadOutLineIntersectItem::operator<( const StandLeadInLineIntersectItem& otherItem )
{
	if( GetObjectID() < otherItem.GetObjectID() )
		return true;
	if( GetStandLeadInOutLineID() < otherItem.GetStandLeadInOutLineID() )
		return true;
	return false;
}

IntersectItem* StandLeadOutLineIntersectItem::Clone() const
{
	StandLeadOutLineIntersectItem* pnewItem = new StandLeadOutLineIntersectItem();
	*pnewItem = *this;
	return pnewItem;
}
//////////////////////////////////////////////////////////////////////////
CString DeicePadInContrainItem::GetNameString()
{
	DeicePad* pPad = GetDeicePad();
	ASSERT(pPad);
	CString strFt;
	strFt.Format("%s(In)",pPad->GetObjNameString());
	return strFt;
}

CPath2008 DeicePadInContrainItem::GetPath()
{
	DeicePad* pPad = GetDeicePad();
	ASSERT(pPad);
	if(pPad)
		return pPad->GetInContrain();
	return CPath2008();
}

void DeicePadInContrainItem::SetDeicePad( DeicePad* pdeicepad )
{
	m_pObject = pdeicepad;
}

bool DeicePadInContrainItem::operator<( const DeicePadInContrainItem&otherItem ) const
{
	return GetObjectID() < otherItem.GetObjectID();
}

IntersectItem* DeicePadInContrainItem::Clone() const
{
	DeicePadInContrainItem* pnewItem = new DeicePadInContrainItem();
	*pnewItem = *this;
	return pnewItem;
}

//////////////////////////////////////////////////////////////////////////

CString DeicePadOutConstrainItem::GetNameString()
{
	DeicePad* pPad = GetDeicePad();
	ASSERT(pPad);
	CString strFt;
	strFt.Format("%s(Out)",pPad->GetObjNameString());
	return strFt;
}

CPath2008 DeicePadOutConstrainItem::GetPath()
{
	DeicePad* pPad = GetDeicePad();
	ASSERT(pPad);
	if(pPad)
		return pPad->GetOutConstrain();
	return CPath2008();
}

void DeicePadOutConstrainItem::SetDeicePad( DeicePad* pdeicepad )
{
	m_pObject = pdeicepad;
}

bool DeicePadOutConstrainItem::operator<( const DeicePadOutConstrainItem& otherItem ) const
{
	return GetObjectID() < otherItem.GetObjectID();
}

IntersectItem* DeicePadOutConstrainItem::Clone() const
{
	DeicePadOutConstrainItem* pnewItem = new DeicePadOutConstrainItem();
	*pnewItem = *this;
	return pnewItem;
}