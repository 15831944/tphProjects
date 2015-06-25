#include "StdAfx.h"
#include ".\airsidetakeoffprocessparameter.h"
#include <algorithm>
CAirsideTakeoffProcessParameter::CAirsideTakeoffProcessParameter(void)
{
}

CAirsideTakeoffProcessParameter::~CAirsideTakeoffProcessParameter(void)
{
}

void CAirsideTakeoffProcessParameter::LoadParameter()
{

}

void CAirsideTakeoffProcessParameter::UpdateParameter()
{

}

CString CAirsideTakeoffProcessParameter::GetReportParamName()
{
	return _T("TakeoffProcess\\TakeoffProcess.prm");
}

void CAirsideTakeoffProcessParameter::WriteParameter( ArctermFile& _file )
{
	CParameters::WriteParameter(_file);

	int nCount = (int)m_vTakeoffPosition.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		CString sTakeoff = m_vTakeoffPosition[nItem];
		_file.writeField(sTakeoff.GetBuffer(1024));
		sTakeoff.ReleaseBuffer(1024);
		_file.writeLine();
	}

	int size = m_vFlightConstraint.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;

	TCHAR th[1024] = {0} ;
	for (int i = 0 ; i < size ;i++)
	{
		getFlightConstraint(i).WriteSyntaxStringWithVersion(th) ;
		_file.writeField(th) ;
		_file.writeLine() ;
	}
}

void CAirsideTakeoffProcessParameter::ReadParameter( ArctermFile& _file )
{
	CParameters::ReadParameter(_file);

	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		CString sTakeoff;
		_file.getField(sTakeoff.GetBuffer(1024),1024);
		sTakeoff.ReleaseBuffer(1024);
		m_vTakeoffPosition.push_back(sTakeoff);
		_file.getLine();
	}

	int size = 0 ;
	if(!_file.getInteger(size) )
		return ;

	_file.getLine() ;
	TCHAR th[1024] = {0} ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.getField(th,1024) ;
		FlightConstraint constrain(m_AirportDB) ;
		constrain.setConstraintWithVersion(th) ;
		m_vFlightConstraint.push_back(constrain) ;
		_file.getLine();
	}
}

BOOL CAirsideTakeoffProcessParameter::ExportFile( ArctermFile& _file )
{
	if(!CParameters::ExportFile(_file))
		return FALSE;
	int nCount = (int)m_vTakeoffPosition.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		CString sTakeoff = m_vTakeoffPosition[nItem];
		_file.writeField(sTakeoff.GetBuffer(1024));
		sTakeoff.ReleaseBuffer(1024);
		_file.writeLine();
	}
	_file.writeLine();
	return TRUE;
}

BOOL CAirsideTakeoffProcessParameter::ImportFile( ArctermFile& _file )
{
	if(!CParameters::ImportFile(_file))
		return FALSE;

	_file.getLine();
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		CString sTakeoff;
		_file.getField(sTakeoff.GetBuffer(1024),1024);
		sTakeoff.ReleaseBuffer(1024);
		m_vTakeoffPosition.push_back(sTakeoff);
		_file.getLine();
	}
	return TRUE;
}

bool CAirsideTakeoffProcessParameter::fit(const CString& sTakeoff,const ElapsedTime& time)
{
	//check time
	if (time < m_startTime || time > m_endTime)
	{
		return false;
	}

	//check take off position
	for (size_t i = 0; i < m_vTakeoffPosition.size(); i++)
	{
		CString& sTakeoffPosition = m_vTakeoffPosition[i];
		//find equal or have default value "All"
		if (sTakeoff.CompareNoCase(sTakeoffPosition) == 0 \
			|| sTakeoffPosition.CompareNoCase(_T("All")) == 0)
		{
			return true;
		}
	}

	return false;
}

int CAirsideTakeoffProcessParameter::GetPositionCount() const
{
	return (int)m_vTakeoffPosition.size();
}

void CAirsideTakeoffProcessParameter::AddPosition( const CString& sPosition )
{
	m_vTakeoffPosition.push_back(sPosition);
}

void CAirsideTakeoffProcessParameter::ClearPosition()
{
	m_vTakeoffPosition.clear();
}

void CAirsideTakeoffProcessParameter::DeletePosition( int nIdx )
{
	ASSERT(nIdx >= 0 && nIdx < GetPositionCount());
	m_vTakeoffPosition.erase(m_vTakeoffPosition.begin() + nIdx);
}

CString CAirsideTakeoffProcessParameter::GetPosition( int nIdx ) const
{
	ASSERT(nIdx >= 0 && nIdx < GetPositionCount());
	return m_vTakeoffPosition[nIdx];
}