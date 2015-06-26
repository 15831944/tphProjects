#include "StdAfx.h"
#include "FlightTypeServiceLoactions.h"
#include "AirsideImportExportManager.h"
#include "VehicleSpecifications.h"
#include "..\Database\DBElementCollection_Impl.h"


FlightTypeServiceLoactions::FlightTypeServiceLoactions(void)
{
}

FlightTypeServiceLoactions::~FlightTypeServiceLoactions(void)
{
}

//----------------------------------------------------------------------
//Interfaces

BOOL FlightTypeServiceLoactions::GetRelativeElements(ElementList& vect, int nTypeID)
{
	for (size_t i=0; i<m_listElement.size(); i++)
	{
		if(m_listElement.at(i).GetTypeID() == nTypeID)
			vect.push_back(m_listElement.at(i));
	}

	return (vect.size() > 0);
}

void FlightTypeServiceLoactions::ConvertPointIntoLocalCoordinate(const CPoint2008& pt,const double& dDegree)
{
	for (size_t i=0; i<m_listElement.size(); i++)
	{
		m_listElement[i].ConvertPointIntoLocalCoordinate(pt,dDegree);
	}
}

int FlightTypeServiceLoactions::GetElementCount(int nTypeID)
{
	int nCount = 0;


	for (size_t i=0; i<m_listElement.size(); i++)
	{
		if(m_listElement.at(i).GetTypeID() == nTypeID)
			nCount++;
	}
	return nCount;
}



void  FlightTypeServiceLoactions::AddItem(FlightTypeRelativeElement item)
{
	m_listElement.push_back(item);
}



void  FlightTypeServiceLoactions::DeleteItem(CPoint2008& point)
{
	for (ElementIter iter = m_listElement.begin(); iter != m_listElement.end();
		iter++)
	{
		FlightTypeRelativeElement& curItem = *iter;
		if(curItem.GetServiceLocation().distance(point) < 0.01)
		{
			m_listNeedDelete.push_back(*iter);
			m_listElement.erase(iter);
			return;
		}
	}

	ASSERT(FALSE);
}

void FlightTypeServiceLoactions::UpdateItem(CPoint2008& oldPoint, CPoint2008& newPoint)
{
	for (ElementIter iter = m_listElement.begin(); iter != m_listElement.end();
		iter++)
	{
		FlightTypeRelativeElement& curItem = *iter;
		if(curItem.GetServiceLocation().distance(oldPoint) < 0.01)
		{
			iter->SetServiceLocation(newPoint);
			return;
		}
	}

	ASSERT(FALSE);
}


void FlightTypeServiceLoactions::ResetAllElements()
{
	for (ElementIter iter = m_listElement.begin(); iter != m_listElement.end(); iter++)
	{
		m_listNeedDelete.push_back(*iter);
	}
	m_listElement.clear();
}



//----------------------------------------------------------------------
//Database operation
void FlightTypeServiceLoactions::ReadData(int nFlightTypeDetailsItemID)
{
	CString strSQL;
	strSQL.Format(_T("SELECT * \
					 FROM FlightTypeRelativeElement\
					 WHERE (FlightTypeDetailsItemID = %d)"),nFlightTypeDetailsItemID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{
		int nID;
		adoRecordset.GetFieldValue(_T("ID"),nID);

		int nTypeID;
		adoRecordset.GetFieldValue(_T("ElementTypeID"),nTypeID);
		double x,y,z;
		adoRecordset.GetFieldValue(_T("location_x"),x);
		adoRecordset.GetFieldValue(_T("location_y"),y);
		adoRecordset.GetFieldValue(_T("location_z"),z);
		CPoint2008 svrLocation(x,y,z);

		FlightTypeRelativeElement newItem;
		newItem.SetID(nID);
		newItem.SetTypeID(nTypeID);
		newItem.SetServiceLocation(svrLocation);

		m_listElement.push_back(newItem);

		adoRecordset.MoveNextData();
	}

}

void FlightTypeServiceLoactions::SaveData(int nFltTypeDetailsItemID)
{
	for (size_t i=0; i<m_listElement.size(); i++)
	{
		if(m_listElement.at(i).GetID() == -1)
			m_listElement.at(i).SaveData(nFltTypeDetailsItemID);
		else
			m_listElement.at(i).UpdateData(nFltTypeDetailsItemID);

	}

	for (ElementIter iter = m_listNeedDelete.begin(); iter != m_listNeedDelete.end();
		iter++)
	{
		if((*iter).GetID() != -1)
			(*iter).DeleteData();
	}

}

void FlightTypeServiceLoactions::UpdateData(int nFltTypeDetailsItemID)
{
	for (size_t i=0; i<m_listElement.size(); i++)
	{
		if(m_listElement.at(i).GetID() == -1)
			m_listElement.at(i).SaveData(nFltTypeDetailsItemID);
		else 
			m_listElement.at(i).UpdateData(nFltTypeDetailsItemID);
	}

	for (ElementIter iter = m_listNeedDelete.begin(); iter != m_listNeedDelete.end();
		iter++)
	{
		if((*iter).GetID() != -1)
			(*iter).DeleteData();
	}
}

void FlightTypeServiceLoactions::DeleteData(int nFltTypeDetailsItemID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM FlightTypeRelativeElement\
					 WHERE (FlightTypeDetailsItemID = %d)"),nFltTypeDetailsItemID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}


void FlightTypeServiceLoactions::InitDefault(int nProjID, int nID)
{
	CVehicleSpecifications vehicleSpecifications;
	vehicleSpecifications.ReadData(nProjID);
    
	//
	std::map<CString, CPoint2008> mapDefaultLocation;
	
	mapDefaultLocation.insert( std::make_pair(_T("Fuel truck"), CPoint2008(-5.03,-5.13,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Fuel Browser"), CPoint2008(-7.22,-5.13,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Catering truck"), CPoint2008(-4.80,-1.1,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Baggage tug"), CPoint2008(-8.93, 13.38,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Baggage cart"), CPoint2008(6.90,12.88,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Maintenance truck"), CPoint2008(10.80,-3.20,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Lift"), CPoint2008(2.93,-7.0,0)) );
	mapDefaultLocation.insert( std::make_pair(_T("Cargo tug"), CPoint2008(4.88,-1.09,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Cargo ULD cart"), CPoint2008(11.04,3.20,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Conveyor"), CPoint2008(4.95,15.07,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Stairs"), CPoint2008(-3.39,14.81,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Staff car"), CPoint2008(11.19,15.15,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Crew bus"), CPoint2008(-13.0,15.40,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Tow truck"), CPoint2008(0.00,-8.0,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Deicing truck"), CPoint2008(-11.04,3.11,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Pax bus A"), CPoint2008(-11.04,25.17,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Pax bus B"), CPoint2008(-8.23,25.17,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Pax bus C"), CPoint2008(-5.27,25.17,0) ) );
	mapDefaultLocation.insert( std::make_pair(_T("Follow me car"), CPoint2008(9.79,19.19,0) ) );

	size_t nVehicleCount = vehicleSpecifications.GetElementCount();

	CString strValue;
	for (size_t i = 0; i < nVehicleCount; i++)
	{
		CVehicleSpecificationItem* pItem = vehicleSpecifications.GetItem(i);
		CString strName = pItem->getName();
		std::map<CString, CPoint2008>::iterator iter = mapDefaultLocation.begin();
		for ( ; iter != mapDefaultLocation.end() ;iter++)
		{
			if(strName.CompareNoCase((*iter).first) == 0)
				break ;
		}
		if(iter == mapDefaultLocation.end())
			continue;

		CPoint2008 svrPt = iter->second;
		FlightTypeRelativeElement newElemItem;
		newElemItem.SetFlightTypeDetailsItemID(nID);
		newElemItem.SetTypeID(pItem->GetID());
		newElemItem.SetServiceLocation(svrPt);

		m_listElement.push_back(newElemItem);
	}

	SaveData(nID);

}


void FlightTypeServiceLoactions::ExportData(CAirsideExportFile& exportFile, int nFltTypeDetailsItemID)
{
	exportFile.getFile().writeInt((int)m_listElement.size());

	ElementIter iter = m_listElement.begin();
	for (; iter!=m_listElement.end(); iter++)
	{
		iter->ExportData(exportFile, nFltTypeDetailsItemID);
	}
}

void FlightTypeServiceLoactions::ImportData(CAirsideImportFile& importFile, int nFltTypeDetailsItemID)
{
	int nCount = 0;
	importFile.getFile().getInteger(nCount);

	for (int i=0; i< nCount; i++)
	{
		FlightTypeRelativeElement data;
		data.ImportData(importFile, nFltTypeDetailsItemID);
	}
}

