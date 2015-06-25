#include "StdAfx.h"
#include ".\taxiwaydelayparameters.h"
#include <algorithm>
#include "../Results/AirsideFlightEventLog.h"
#include "../InputAirside/IntersectionNode.h"

BOOL CTaxiwaySelect::IsSegInParaDefine(double delayFromDis,double delayToDis)
{
	if(m_vData.empty())
		return TRUE ;

	size_t nSize = m_vData.size();
	for (size_t i = 0 ; i < nSize ; i++)
	{
		CReportTaxiwaySegment _seg = m_vData.at(i);
		if(_seg.m_FormNodeID == -1 && _seg.m_ToNodeID == -1)
			return TRUE ;

		if (_seg.m_dToDist == -1)		//from a node to end of taxiway
		{
			if (min(delayFromDis,delayToDis) >= _seg.m_dFromDist)
				return TRUE;
		}
		else			
		{
			if (_seg.m_dToDist > _seg.m_dFromDist)		//positive direction
			{
				if (min(delayFromDis,delayToDis) >= _seg.m_dFromDist && max(delayFromDis, delayToDis) <= _seg.m_dToDist)
					return TRUE;

			}
			else		//negative direction
			{
				if (min(delayFromDis,delayToDis) >= _seg.m_dToDist && max(delayFromDis, delayToDis) <= _seg.m_dFromDist)
					return TRUE;
			}
		}

		//if( (min(fromDis,toDis) < delayFromDis &&  delayFromDis< max(fromDis,toDis)) || ( (delayFromDis == fromDis || delayFromDis== toDis)&& (min(fromDis,toDis) < delayToDis &&  delayToDis< max(fromDis,toDis) ) ))
		//	return TRUE ;
	}
	return FALSE ;
}


CTaxiwaySelect::CTaxiwaySelect():m_TaxiwayID(-1)
{

}
void CTaxiwaySelect::AddIntersectionPair(int _fromID , const CString& _from ,int _toId , const CString& _to)
{
	for ( int i = 0 ; i < (int)m_vData.size() ;i++)
	{
		if(m_vData[i].m_FormNodeID == _fromID && m_vData[i].m_ToNodeID == _toId)
			return ;
	}
	CReportTaxiwaySegment intersectionNode(_fromID,_toId) ;
	intersectionNode.m_strFromName = _from ;
	intersectionNode.m_strToName = _to ;
	intersectionNode.InitNodeDist(m_TaxiwayID);
	m_vData.push_back(intersectionNode) ;
}
CTaxiwayDelayParameters::CTaxiwayDelayParameters(void):CParameters()
{

}
void CTaxiwayDelayParameters::cleardata()
{
	for (int i = 0 ; i < (int)m_TaxiwayData.size() ;i++)
	{
		delete m_TaxiwayData[i] ;
	}

	for (int ii = 0; ii < (int)m_vTaxiwayDataForShow.size(); ii++)
	{
		delete m_vTaxiwayDataForShow[ii];
	}
	m_TaxiwayData.clear() ;
	m_vTaxiwayDataForShow.clear();
}
CTaxiwaySelect* CTaxiwayDelayParameters::AddTaxiway(int _taxiwayid)
{
	
	TY_TAXIWAY_DATA_ITER iter = m_TaxiwayData.begin() ;
	for ( ; iter != m_TaxiwayData.end() ;iter++)
	{
		if( (*iter)->GetTaxiwayID() == _taxiwayid)
			return *iter ;
	}
	CTaxiwaySelect* taxiwaysel = new CTaxiwaySelect ;
	taxiwaysel->SetTaxiwayID(_taxiwayid) ;
	m_TaxiwayData.push_back(taxiwaysel) ;
	return taxiwaysel ;
}

CTaxiwaySelect* CTaxiwayDelayParameters::AddTaxiwayForShow(int _taxiwayid)
{
	TY_TAXIWAY_DATA_ITER iter = m_vTaxiwayDataForShow.begin() ;
	for ( ; iter != m_vTaxiwayDataForShow.end() ;iter++)
	{
		if( (*iter)->GetTaxiwayID() == _taxiwayid)
			return *iter ;
	}
	CTaxiwaySelect* taxiwaysel = new CTaxiwaySelect ;
	taxiwaysel->SetTaxiwayID(_taxiwayid) ;
	m_vTaxiwayDataForShow.push_back(taxiwaysel) ;
	return taxiwaysel ;
}

BOOL CTaxiwayDelayParameters::CheckTheTaxiWayIsSelect(int _taxiID,CTaxiwaySelect** _FindResult)
{
	if(m_TaxiwayData.empty())
	{
		*_FindResult = NULL ;
		return TRUE ;
	}
	TY_TAXIWAY_DATA_ITER iter = m_TaxiwayData.begin() ;
	for ( ; iter != m_TaxiwayData.end() ;iter++)
	{
		if( ( *iter)->GetTaxiwayID() == _taxiID)
		{
			*_FindResult = *iter ;
			return TRUE ;
		}
	}
	return FALSE ;
}


BOOL CTaxiwayDelayParameters::ExportFile(ArctermFile& _file)
{
	CParameters::ExportFile(_file);
	//write taxiway node
	_file.writeInt((int)m_TaxiwayData.size()) ;
	_file.writeLine() ;
	CTaxiwaySelect* taxiwaySel = NULL ;
	for (int i = 0 ; i < (int)m_TaxiwayData.size() ;i++)
	{
		taxiwaySel = m_TaxiwayData[i] ;
		_file.writeField(taxiwaySel->GetTaxiwayName()) ;
		_file.writeInt(taxiwaySel->GetTaxiwayID()) ;
		_file.writeLine() ;
		_file.writeInt((int)taxiwaySel->GetIntersectionSegment()->size() ) ;
		_file.writeLine() ;

		for (int j = 0 ; j < (int)(int)taxiwaySel->GetIntersectionSegment()->size() ;j++)
		{
			_file.writeField( taxiwaySel->GetIntersectionSegment()->at(j).m_strFromName);
			_file.writeInt( taxiwaySel->GetIntersectionSegment()->at(j).m_FormNodeID);

			_file.writeField( taxiwaySel->GetIntersectionSegment()->at(j).m_strToName);
			_file.writeInt( taxiwaySel->GetIntersectionSegment()->at(j).m_ToNodeID);

			_file.writeLine() ;
		}
	}

	//for show
	_file.writeInt(GetCount());
	_file.writeLine();
	CTaxiwaySelect* pTaxiwayForShow = NULL;
	for (int ii = 0; ii < GetCount(); ii++)
	{
		pTaxiwayForShow = GetTaxiwayForShowItem(ii);
		if (!pTaxiwayForShow)
			continue;

		if (pTaxiwayForShow->GetTaxiwayName().IsEmpty())
			_file.writeBlankField();
		else
			_file.writeField(pTaxiwayForShow->GetTaxiwayName());

		_file.writeInt(pTaxiwayForShow->GetTaxiwayID());
		_file.writeLine() ;
		_file.writeInt((int)pTaxiwayForShow->GetIntersectionSegment()->size() ) ;
		_file.writeLine() ;

		for (int j = 0 ; j < (int)pTaxiwayForShow->GetIntersectionSegment()->size() ;j++)
		{
			_file.writeField( pTaxiwayForShow->GetIntersectionSegment()->at(j).m_strFromName);
			_file.writeInt( pTaxiwayForShow->GetIntersectionSegment()->at(j).m_FormNodeID);

			_file.writeField( pTaxiwayForShow->GetIntersectionSegment()->at(j).m_strToName);
			_file.writeInt( pTaxiwayForShow->GetIntersectionSegment()->at(j).m_ToNodeID);

			_file.writeLine() ;
		}
	}
	return TRUE ;
}
BOOL CTaxiwayDelayParameters::ImportFile(ArctermFile& _file)
{
	CParameters::ImportFile(_file) ;
	cleardata() ;
	long taxiwayNum = 0 ;
	_file.getInteger(taxiwayNum) ;
	_file.getLine() ;
	TCHAR taxiwayname[1024] = {0} ;
	int taxiId = -1 ;
	CTaxiwaySelect* taxiwaySel = NULL ;

	TCHAR fromNode[1024] = {0} ;
	TCHAR Tonode[1024] = {0} ;
	int fromID = -1 ;
	int toID = -1 ;
	for (int i = 0 ; i < taxiwayNum ;i++)
	{
		_file.getField(taxiwayname,1024) ;
		_file.getInteger(taxiId) ;
		taxiwaySel = new CTaxiwaySelect ;
		taxiwaySel->SetTaxiwayName(taxiwayname) ;
		taxiwaySel->SetTaxiwayID(taxiId) ;
		m_TaxiwayData.push_back(taxiwaySel) ;

		//read segment 
		_file.getLine() ;

		int segmentNum = 0 ;
		_file.getInteger(segmentNum) ;
		_file.getLine() ;
		

		for (int num = 0 ; num < segmentNum ;num++)
		{
			_file.getField(fromNode,1024) ;
			_file.getInteger(fromID) ;
			_file.getField(Tonode,1024) ;
			_file.getInteger(toID) ;
			CReportTaxiwaySegment intersectionNode(fromID,toID) ;
			intersectionNode.m_FormNodeID = fromID ;
			intersectionNode.m_strFromName = fromNode ;
			intersectionNode.m_strToName = Tonode ;
			intersectionNode.m_ToNodeID = toID ;
			intersectionNode.InitNodeDist(taxiwaySel->GetTaxiwayID());
			taxiwaySel->GetIntersectionSegment()->push_back(intersectionNode) ;
			_file.getLine();
		}

	}

	//for show
	long nCount = 0 ;
	_file.getInteger(nCount) ;
	_file.getLine() ;
	TCHAR sTaxiway[1024] = {0} ;
	int nTaiwayID = -1 ;
	CTaxiwaySelect* pTaxiwayForShow = NULL ;

	TCHAR sFromNode[1024] = {0} ;
	TCHAR sToNode[1024] = {0} ;
	int nFromID = -1 ;
	int nToID = -1 ;
	for (int i = 0 ; i < nCount ;i++)
	{
		_file.getField(sTaxiway,1024) ;
		_file.getInteger(nTaiwayID) ;
		pTaxiwayForShow = new CTaxiwaySelect ;
		pTaxiwayForShow->SetTaxiwayName(sTaxiway) ;
		pTaxiwayForShow->SetTaxiwayID(nTaiwayID) ;
		m_vTaxiwayDataForShow.push_back(pTaxiwayForShow) ;

		//read segment 
		_file.getLine() ;

		int segmentNum = 0 ;
		_file.getInteger(segmentNum) ;
		for (int num = 0 ; num < segmentNum ;num++)
		{
			_file.getLine();
			_file.getField(sFromNode,1024) ;
			_file.getInteger(nFromID) ;
			_file.getField(sToNode,1024) ;
			_file.getInteger(nToID) ;
			CReportTaxiwaySegment intersectionNode(fromID,toID) ;
			intersectionNode.m_FormNodeID = nFromID ;
			intersectionNode.m_strFromName = sFromNode ;
			intersectionNode.m_strToName = sToNode ;
			intersectionNode.m_ToNodeID = nToID ;
			intersectionNode.InitNodeDist(pTaxiwayForShow->GetTaxiwayID());
			pTaxiwayForShow->GetIntersectionSegment()->push_back(intersectionNode) ;

		}

	}
	return TRUE ;
}

CString CTaxiwayDelayParameters::GetReportParamName()
{
	return _T("TaxiwayDelay\\TaxiwayDelay.prm");
}

void CTaxiwayDelayParameters::WriteParameter(ArctermFile& _file)
{
	CParameters::WriteParameter(_file);
	//write taxiway node
	_file.writeInt((int)m_TaxiwayData.size()) ;
	_file.writeLine() ;
	CTaxiwaySelect* taxiwaySel = NULL ;
	for (int i = 0 ; i < (int)m_TaxiwayData.size() ;i++)
	{
		taxiwaySel = m_TaxiwayData[i] ;
		_file.writeField(taxiwaySel->GetTaxiwayName()) ;
		_file.writeInt(taxiwaySel->GetTaxiwayID()) ;
		_file.writeLine() ;
		_file.writeInt((int)taxiwaySel->GetIntersectionSegment()->size() ) ;
		_file.writeLine() ;

		for (int j = 0 ; j < (int)(int)taxiwaySel->GetIntersectionSegment()->size() ;j++)
		{
			_file.writeField( taxiwaySel->GetIntersectionSegment()->at(j).m_strFromName);
			_file.writeInt( taxiwaySel->GetIntersectionSegment()->at(j).m_FormNodeID);

			_file.writeField( taxiwaySel->GetIntersectionSegment()->at(j).m_strToName);
			_file.writeInt( taxiwaySel->GetIntersectionSegment()->at(j).m_ToNodeID);

			_file.writeLine() ;
		}
	}
}

void CTaxiwayDelayParameters::ReadParameter(ArctermFile& _file)
{
	CParameters::ReadParameter(_file) ;
	cleardata() ;
	_file.getLine() ;
	long taxiwayNum = 0 ;
	_file.getInteger(taxiwayNum) ;
	_file.getLine() ;
	TCHAR taxiwayname[1024] = {0} ;
	int taxiId = -1 ;
	CTaxiwaySelect* taxiwaySel = NULL ;

	TCHAR fromNode[1024] = {0} ;
	TCHAR Tonode[1024] = {0} ;
	int fromID = -1 ;
	int toID = -1 ;
	for (int i = 0 ; i < taxiwayNum ;i++)
	{
		_file.getField(taxiwayname,1024) ;
		_file.getInteger(taxiId) ;
		taxiwaySel = new CTaxiwaySelect ;
		taxiwaySel->SetTaxiwayName(taxiwayname) ;
		taxiwaySel->SetTaxiwayID(taxiId) ;
		m_TaxiwayData.push_back(taxiwaySel) ;

		//read segment 
		_file.getLine() ;

		int segmentNum = 0 ;
		_file.getInteger(segmentNum) ;


		for (int num = 0 ; num < segmentNum ;num++)
		{
			_file.getLine() ;
			_file.getField(fromNode,1024) ;
			_file.getInteger(fromID) ;
			_file.getField(Tonode,1024) ;
			_file.getInteger(toID) ;
			CReportTaxiwaySegment intersectionNode(fromID,toID) ;
			intersectionNode.m_FormNodeID = fromID ;
			intersectionNode.m_strFromName = fromNode ;
			intersectionNode.m_strToName = Tonode ;
			intersectionNode.m_ToNodeID = toID ;
			intersectionNode.InitNodeDist(taxiwaySel->GetTaxiwayID());
			taxiwaySel->GetIntersectionSegment()->push_back(intersectionNode) ;
		}

	}
}

CReportTaxiwaySegment::CReportTaxiwaySegment( int _fromID , int _toID ) 
:m_FormNodeID(_fromID)
,m_ToNodeID(_toID)
, m_strFromName(_T("Start"))
, m_strToName(_T("End"))
, m_dFromDist(-1)
, m_dToDist(-1)
{

}

void CReportTaxiwaySegment::InitNodeDist(int nTaxiwayID)
{
	IntersectionNode _Node;
	if (m_FormNodeID > -1)
	{
		_Node.ReadData(m_FormNodeID) ;
		m_dFromDist = _Node.GetDistance(nTaxiwayID) ;
	}

	if (m_ToNodeID > -1)
	{
		_Node.ReadData(m_ToNodeID) ;
		m_dToDist = _Node.GetDistance(nTaxiwayID) ;
	}

}