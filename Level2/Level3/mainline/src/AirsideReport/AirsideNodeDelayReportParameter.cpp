#include "StdAfx.h"
#include ".\airsidenodedelayreportparameter.h"

CAirsideNodeDelayReportParameter::CAirsideNodeDelayReportParameter(void)
{
	
}

CAirsideNodeDelayReportParameter::~CAirsideNodeDelayReportParameter(void)
{
}
void CAirsideNodeDelayReportParameter::LoadParameter()
{
}

//void  CAirsideNodeDelayReportParameter::InitParameter(CXTPPropertyGrid * pGrid)
//{
//	//time range
//	InitTimeRange(pGrid);
//
//	//interval
//	InitInterval(pGrid);
//}

void  CAirsideNodeDelayReportParameter::UpdateParameter()
{
}

bool CAirsideNodeDelayReportParameter::IsReportNodeExist(CAirsideReportNode& rpNode)
{
	std::vector<CAirsideReportNode>::iterator iter = m_vReportNode.begin();
	for (;iter != m_vReportNode.end(); ++iter)
	{
		if( rpNode.EqualTo(*iter))
			return true;
	}
	return false;
}
void CAirsideNodeDelayReportParameter::AddReportNode(CAirsideReportNode& rpNode)
{
	m_vReportNode.push_back(rpNode);

}
bool CAirsideNodeDelayReportParameter::DelReportNode(size_t nIndex)
{
	if (nIndex < m_vReportNode.size())
	{
		m_vReportNode.erase(m_vReportNode.begin()+nIndex);
		return true;
	}
	return false;
}
size_t CAirsideNodeDelayReportParameter::getReportNodeCount()
{
	return m_vReportNode.size();
}
CAirsideReportNode CAirsideNodeDelayReportParameter::getReportNode(size_t nIndex)
{
	CAirsideReportNode rpNode;

	if (nIndex < m_vReportNode.size())
	{
		rpNode = m_vReportNode.at(nIndex);
	}
	return rpNode;

}
void CAirsideNodeDelayReportParameter::ClearReportNode()
{
	m_vReportNode.clear();
}
BOOL CAirsideNodeDelayReportParameter::ExportFile(ArctermFile& _file)
{
	if(!CParameters::ExportFile(_file))
		return FALSE ;
	_file.writeInt(m_nSubType);
	int size = 0 ;
	size = (int)m_vReportNode.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;

	for (int i = 0 ; i < size ;i++)
	{
		m_vReportNode[i].ExportFile(_file) ;
	}
	return TRUE ;
}
BOOL CAirsideNodeDelayReportParameter::ImportFile(ArctermFile& _file)
{
	if(!CParameters::ImportFile(_file))
		return FALSE ;
	_file.getInteger(m_nSubType) ;
	int size = 0 ;
	if(!_file.getInteger(size))
		return FALSE ;
	for (int i = 0; i < size ;i++)
	{
		_file.getLine() ;
		CAirsideReportNode  reportnode ;
		reportnode.ImportFile(_file) ;
        m_vReportNode.push_back(reportnode) ;
	}
	return TRUE ;
}

