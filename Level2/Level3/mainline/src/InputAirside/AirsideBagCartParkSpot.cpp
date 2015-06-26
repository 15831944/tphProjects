#include "StdAfx.h"
#include ".\airsidebagcartparkspot.h"
#include "Database\AirsideParkingSpotDBDefine.h"
#include "Database\DBElementCollection_Impl.h"

CString AirsideBagCartParkSpot::GetTypeName() const
{
	return _T("BaggageCart ParkingSpot");
}

ALTOBJECT_TYPE AirsideBagCartParkSpot::GetType() const
{
	return ALT_ABAGCARTSPOT;
}



ALTObject * AirsideBagCartParkSpot::NewCopy()
{
	AirsideBagCartParkSpot* newCopy = new AirsideBagCartParkSpot();
	newCopy->CopyData(this);
	return newCopy;
}

CString AirsideBagCartParkSpot::getPropTable() const
{
	return TB_BAGCARTSPOT_PROP;
}

CString AirsideBagCartParkSpot::getViewTable() const
{
	return TBVIEW_BAGCARTSPOT;
}



//////////////////////////////////////////////////////////////////////////
//AirsideBagCartParkSpotList
AirsideBagCartParkSpotList::AirsideBagCartParkSpotList()
{

}

AirsideBagCartParkSpotList::~AirsideBagCartParkSpotList()
{
	CleanData();
}

void AirsideBagCartParkSpotList::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM %s "),TBVIEW_BAGCARTSPOT);
}

size_t AirsideBagCartParkSpotList::GetItemCount()
{
	return m_vParkingSpot.size();
}
AirsideBagCartParkSpot *AirsideBagCartParkSpotList::GetItemByIndex(size_t nIndex)
{
	if (nIndex < m_vParkingSpot.size())
	{
		return m_vParkingSpot.at(nIndex);
	}

	return NULL;
}

void AirsideBagCartParkSpotList::ReadData( int nAirportID )
{

	CString strSelectSQL;
	GetSelectElementSQL(nAirportID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		AirsideBagCartParkSpot* element = new AirsideBagCartParkSpot;
		element->InitFromDBRecordset(adoRecordset);
		m_vParkingSpot.push_back(element);
		adoRecordset.MoveNextData();
	}
}

void AirsideBagCartParkSpotList::CleanData()
{
	std::vector<AirsideBagCartParkSpot *>::iterator iter = m_vParkingSpot.begin();
	for ( ; iter != m_vParkingSpot.end(); ++ iter)
	{
		delete *iter;
	}
	m_vParkingSpot.clear();

}

