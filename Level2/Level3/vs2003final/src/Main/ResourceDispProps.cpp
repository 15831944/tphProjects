// ResourceDispProps.cpp: implementation of the CResourceDispProps class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResourceDispProps.h"

#include "inputs\ResourcePool.h"
#include "common\exeption.h"
#include "../Inputs/IN_TERM.H"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResourceDispProps::CResourceDispProps() : DataSet( ResourceDisplayPropertiesFile, 2.3f )
{
	m_pResPoolDB = NULL;
	m_nRDPSetIdx = 0;
	
	m_vRDPItemsVec.resize(1);
	m_vRDPSetNamesVec.reserve(1);
	m_vRDPSetNamesVec.push_back("DEFAULT");
	m_nRDPSetIdx = 0;
	
}

CResourceDispProps::~CResourceDispProps()
{
	clear();
}

void CResourceDispProps::clear()
{
	int nCount, i;

	m_vRDPSetNamesVec.clear();
	
	nCount = m_vRDPItemsVec.size();
	for( i=0; i<nCount; i++ ) {
		int nCount2 = m_vRDPItemsVec[i].size();
		for(int j=0; j<nCount2; j++) {
			delete m_vRDPItemsVec[i][j];
		}
		m_vRDPItemsVec[i].clear();
	}
	m_vRDPItemsVec.clear();
}

int CResourceDispProps::GetCount()
{
	if( m_vRDPItemsVec.size() == 0 )
		return 0;

	ASSERT( m_nRDPSetIdx>=0 && m_nRDPSetIdx < static_cast<int>(m_vRDPSetNamesVec.size()) );
	
	return m_vRDPItemsVec[m_nRDPSetIdx].size();
}

CResourceDispPropItem* CResourceDispProps::GetDispPropItem( int _nIdx )
{
	return m_vRDPItemsVec[m_nRDPSetIdx][_nIdx];
}

void CResourceDispProps::initDefaultValues()
{
	m_nRDPSetIdx = AddRDPSet(_T("DEFAULT"));
	/*m_vRDPItemsVec.resize(1);
	m_vRDPSetNamesVec.reserve(1);
	m_vRDPSetNamesVec.push_back("DEFAULT");
	m_nRDPSetIdx = 0;*/
}

void CResourceDispProps::writeRDPI(CResourceDispPropItem* pRDPI, ArctermFile& p_file ) const
{
	ASSERT(pRDPI);
	//write resource pool idx
	p_file.writeInt(pRDPI->GetResourcePoolIndex());
	//write is visible
	p_file.writeInt((int) pRDPI->IsVisible());
	//write color
	p_file.writeInt((long) pRDPI->GetColor());
	//write shape
	p_file.writeInt((int) pRDPI->GetShape().first);
	p_file.writeInt((int) pRDPI->GetShape().second);

	p_file.writeLine();
}

void CResourceDispProps::writeData (ArctermFile& p_file) const
{
	//write RDP set count
	int nRDPSetCount = m_vRDPSetNamesVec.size();
	p_file.writeInt(nRDPSetCount);
	p_file.writeInt(m_nRDPSetIdx);
	p_file.writeLine();

	CResourceDispPropItem* pItem = NULL;

	for(int i=0; i<nRDPSetCount; i++) {
		p_file.writeField("RDPISETNAME");
		p_file.writeField(m_vRDPSetNamesVec[i]);
		p_file.writeLine();
		int nCount = m_vRDPItemsVec[i].size();
	    for( int j = 0; j < nCount; j++ ) {
			pItem = m_vRDPItemsVec[i][j];
			writeRDPI( pItem, p_file );
	    }
	}
}

void CResourceDispProps::readData (ArctermFile& p_file)
{
	ASSERT(m_vRDPItemsVec.size()==0);
	ASSERT(m_vRDPSetNamesVec.size()==0);
	
	p_file.getLine();

	int nRDPSetCount;
	p_file.getInteger(nRDPSetCount);
	p_file.getInteger(m_nRDPSetIdx);

	//SHOULD BE REMOVED after April 2004
	if(m_nRDPSetIdx>=nRDPSetCount)
		m_nRDPSetIdx=nRDPSetCount-1;
	/////
	if(m_nRDPSetIdx<0 || nRDPSetCount<0)
		throw new FileFormatError("Resource Display Parameters Data (ResourceDisplay.txt) ");

	m_vRDPItemsVec.resize(nRDPSetCount);
	m_vRDPSetNamesVec.reserve(nRDPSetCount);
	int nCurSet = -1;

	p_file.getLine();

    while (!p_file.isBlank ())
	{
		CString sz;
		char s[256];
		int n;
		long l;

		p_file.getField(s,255); //rdpi name or rdp set name
		if(stricmp(s, "RDPISETNAME") == 0) {
			nCurSet++;
			//rdp set name
			p_file.getField(s,255);
			//s is name of rdpi set
			m_vRDPSetNamesVec.push_back(s);
			ASSERT(nCurSet == m_vRDPSetNamesVec.size()-1);
		}
		else {

			CResourceDispPropItem* pItem = new CResourceDispPropItem();
			n = atoi(s);
			pItem->SetResourcePoolIndex(n);
			p_file.getInteger(n);
			pItem->IsVisible((BOOL) n);
			p_file.getInteger(l);
			pItem->SetColor((COLORREF) l);
			p_file.getInteger(n);
			pItem->SetShape(std::make_pair(0, n));
			
			m_vRDPItemsVec[nCurSet].push_back(pItem);
		}

		p_file.getLine();
	}
}

void CResourceDispProps::readObsoleteData(ArctermFile& p_file)
{
	ASSERT(m_vRDPItemsVec.size()==0);
	ASSERT(m_vRDPSetNamesVec.size()==0);

	p_file.getLine();

	int nRDPSetCount;
	p_file.getInteger(nRDPSetCount);
	p_file.getInteger(m_nRDPSetIdx);

	//SHOULD BE REMOVED after April 2004
	if(m_nRDPSetIdx>=nRDPSetCount)
		m_nRDPSetIdx=nRDPSetCount-1;
	/////
	if(m_nRDPSetIdx<0 || nRDPSetCount<0)
		throw new FileFormatError("Resource Display Parameters Data (ResourceDisplay.txt) ");

	m_vRDPItemsVec.resize(nRDPSetCount);
	m_vRDPSetNamesVec.reserve(nRDPSetCount);
	int nCurSet = -1;

	p_file.getLine();

	while (!p_file.isBlank ())
	{
		CString sz;
		char s[256];
		int n, m;
		long l;

		p_file.getField(s,255); //rdpi name or rdp set name
		if(stricmp(s, "RDPISETNAME") == 0) {
			nCurSet++;
			//rdp set name
			p_file.getField(s,255);
			//s is name of rdpi set
			m_vRDPSetNamesVec.push_back(s);
			ASSERT(nCurSet == m_vRDPSetNamesVec.size()-1);
		}
		else {

			CResourceDispPropItem* pItem = new CResourceDispPropItem();
			n = atoi(s);
			pItem->SetResourcePoolIndex(n);
			p_file.getInteger(n);
			pItem->IsVisible((BOOL) n);
			p_file.getInteger(l);
			pItem->SetColor((COLORREF) l);
			p_file.getInteger(n);
			p_file.getInteger(m);
			pItem->SetShape(std::make_pair(n, m));

			m_vRDPItemsVec[nCurSet].push_back(pItem);
		}

		p_file.getLine();
	}
}

int CResourceDispProps::AddRDPSet(const CString& _s)
{
	ASSERT(GetInputTerminal()->m_pResourcePoolDB);
	const CResourcePoolDataSet* pResPoolDB = GetInputTerminal()->m_pResourcePoolDB;
	m_vRDPSetNamesVec.push_back(_s);
	m_vRDPItemsVec.resize(m_vRDPItemsVec.size()+1);

	ASSERT(m_vRDPSetNamesVec.size() == m_vRDPItemsVec.size());

	int nSetIdx = m_vRDPSetNamesVec.size()-1;
	int nResPoolCount = pResPoolDB->getResourcePoolCount();
	for(int i=0; i<nResPoolCount; i++) {
		m_vRDPItemsVec[nSetIdx].push_back(new CResourceDispPropItem(i));
	}

	return nSetIdx;
}

void CResourceDispProps::DeleteItem(int _idx)
{
	ASSERT(_idx < static_cast<int>(m_vRDPItemsVec[m_nRDPSetIdx].size()));
	CResourceDispPropItemPtrVector& v = m_vRDPItemsVec[m_nRDPSetIdx];

	for(int i=_idx+1; i<static_cast<int>(v.size()); i++) {
		v[i]->SetResourcePoolIndex(i-1);
	}

	delete v[_idx];
	v.erase(v.begin()+_idx);
}

void CResourceDispProps::DeletRDPSet(int _idx)
{
	m_vRDPSetNamesVec.erase(m_vRDPSetNamesVec.begin()+_idx);

	int nCount = m_vRDPItemsVec[_idx].size();
	for(int i=0; i<nCount; i++) {
		delete m_vRDPItemsVec[_idx][i];
	}
	m_vRDPItemsVec[_idx].clear();
	m_vRDPItemsVec.erase(m_vRDPItemsVec.begin()+_idx);

	ASSERT(m_vRDPSetNamesVec.size() == m_vRDPItemsVec.size());

	if(m_nRDPSetIdx >= static_cast<int>(m_vRDPSetNamesVec.size()))
		m_nRDPSetIdx--;
	ASSERT(m_nRDPSetIdx >= 0);
}

const CString& CResourceDispProps::GetRDPSetName(int _idx) const
{
	ASSERT(_idx >= 0);
	ASSERT(_idx < static_cast<int>(m_vRDPSetNamesVec.size()));
	return m_vRDPSetNamesVec[_idx];
}

void CResourceDispProps::RenameCurrentRDPSet(const CString& _s)
{
	m_vRDPSetNamesVec[m_nRDPSetIdx] = _s;
}

void CResourceDispProps::SetCurrentRDPSet(int _idx)
{
	ASSERT(_idx >= 0);
	ASSERT(_idx < static_cast<int>(m_vRDPSetNamesVec.size()));
	ASSERT(_idx < static_cast<int>(m_vRDPItemsVec.size()));
	
	m_nRDPSetIdx = _idx;
}

void CResourceDispProps::loadInputs( const CString& _csProjPath, InputTerminal* _pInTerm )
{
	this->SetInputTerminal( _pInTerm );
	this->loadDataSet( _csProjPath );
}

void CResourceDispProps::saveInputs( const CString& _csProjPath, bool _bUndo )
{
	this->saveDataSet(_csProjPath, _bUndo);
}

void CResourceDispProps::AddItemToAllSets( int _nResPoolIdx )
{
	int nRDPSetCount = m_vRDPItemsVec.size();
	for(int i=0; i<nRDPSetCount; i++) {
		m_vRDPItemsVec[i].push_back( new CResourceDispPropItem(_nResPoolIdx) );
	}
}

void CResourceDispProps::DeleteItemFromAllSets(int _idx)
{
	int nRDPSetCount = m_vRDPItemsVec.size();
	for(int nRDPSetIdx=0; nRDPSetIdx<nRDPSetCount; nRDPSetIdx++) {
		ASSERT(_idx < static_cast<int>(m_vRDPItemsVec[nRDPSetIdx].size()));
		CResourceDispPropItemPtrVector& v = m_vRDPItemsVec[nRDPSetIdx];
		for(int i=_idx+1; i<static_cast<int>(v.size()); i++) {
			v[i]->SetResourcePoolIndex(i-1);
		}

		delete v[_idx];
		v.erase(v.begin()+_idx);
	}
}