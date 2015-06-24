// PaxDispProps.cpp: implementation of the CPaxDispProps class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PaxDispProps.h"
#include "assert.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

enum OLD_SHAPE_TYPE { 
	ENUM_SHAPE_CUBE=0,
	ENUM_SHAPE_SPHERE,
	ENUM_SHAPE_PYRAMID,
	ENUM_SHAPE_PERSON,
	ENUM_SHAPE_DETAILMAN,
	ENUM_SHAPE_SUMST1,
	ENUM_SHAPE_SUMW1,
	ENUM_SHAPE_SUMW2,
	ENUM_SHAPE_SUMW3,
	ENUM_SHAPE_SHMW1,
	ENUM_SHAPE_SHMW2,
	ENUM_SHAPE_SHM_WHL,
	ENUM_SHAPE_SUM_CRUT,
	ENUM_SHAPE_SLW1,
	ENUM_SHAPE_SLW2,
	ENUM_SHAPE_DLW1,
	ENUM_SHAPE_DLW2,
	ENUM_SHAPE_SL_WHL,
	ENUM_SHAPE_GUY1W,
	ENUM_SHAPE_GUY1WB,
	ENUM_SHAPE_GUY1WC,
	ENUM_SHAPE_GUY3W,
	ENUM_SHAPE_GUY4W,
	ENUM_SHAPE_GUY5W,
	ENUM_SHAPE_GUY1SIT,
	ENUM_SHAPE_GAL1W,
	ENUM_SHAPE_GAL1WB,
	ENUM_SHAPE_GAL2WB,
	ENUM_SHAPE_GAL3W,
	ENUM_SHAPE_GAL4W,
	ENUM_SHAPE_GAL5W,
	ENUM_SHAPE_GAL8W,
	ENUM_SHAPE_GAL1SIT,
	ENUM_SHAPE_B747,
	ENUM_SHAPE_JAGUAR,
	ENUM_SHAPE_LARGECASE,
	ENUM_SHAPE_BRIEFCASERAISED,
};

static const int TABLE_OLD_SHAPE_TO_SET[] = {
	2,
	2,
	2,
	2,
	2,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	1,
	1,
	1,
	1,
	1,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	4,
	2, // 5 (Jaguar)
	3,
	3,
	3,
	3,
	3,
};

static const int TABLE_OLD_SHAPE_TO_ITEM[] = {
	0,
	1,
	2,
	3,
	3,
	6,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	0,
	1,
	2,
	3,
	12,
	0,
	1,
	2,
	3,
	4,
	5,
	13,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	0,
	0, // Jaguar
	1,
	0,
	2,
	1,
	3,
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaxDispProps::CPaxDispProps() : DataSet( PaxDisplayPropertiesFile, 2.5f)
{
	m_vPDPItemsVec.resize(1);
	m_vPDPSetNamesVec.reserve(1);
	//m_vDefaultItemsVec.reserve(1);
	//m_vOverlapItemsVec.reserve(1);
	m_vPDPSetNamesVec.push_back("DEFAULT");
	m_vOverlapItemsVec.push_back(new CPaxDispPropItem("OVERLAP"));
	m_vDefaultItemsVec.push_back(new CPaxDispPropItem("DEFAULT"));
	m_nPDPSetIdx = 0;

}

CPaxDispProps::~CPaxDispProps()
{
	clear();
}

void CPaxDispProps::clear()
{
	int nCount, i;

	m_vPDPSetNamesVec.clear();
	
	nCount = m_vOverlapItemsVec.getCount();
	for(i=0; i<nCount; i++) {
		delete m_vOverlapItemsVec[i];
	}
	m_vOverlapItemsVec.clear();

	nCount = m_vDefaultItemsVec.getCount();
	for(i=0; i<nCount; i++) {
		delete m_vDefaultItemsVec[i];
	}
	m_vDefaultItemsVec.clear();
	
	nCount = m_vPDPItemsVec.size();
	for( i=0; i<nCount; i++ ) {
		int nCount2 = m_vPDPItemsVec[i].size();
		for(int j=0; j<nCount2; j++) {
			delete m_vPDPItemsVec[i][j];
		}
		m_vPDPItemsVec[i].clear();
	}
	m_vPDPItemsVec.clear();
}

CPaxDispPropItem* CPaxDispProps::GetDefaultDispProp()
{
	return m_vDefaultItemsVec[m_nPDPSetIdx];
}

CPaxDispPropItem* CPaxDispProps::GetOverLapDispProp()
{
	return m_vOverlapItemsVec[m_nPDPSetIdx];
}

int CPaxDispProps::GetCount()
{
	return m_vPDPItemsVec[m_nPDPSetIdx].getCount();
}

CPaxDispPropItem* CPaxDispProps::GetDispPropItem( int _nIdx )
{
	if(m_nPDPSetIdx<(int)m_vPDPItemsVec.size())
	{
		if(_nIdx>=0 &&  _nIdx < m_vPDPItemsVec[m_nPDPSetIdx].getCount() )
			return m_vPDPItemsVec[m_nPDPSetIdx][_nIdx];
		else if(_nIdx==-1)
			return m_vDefaultItemsVec[m_nPDPSetIdx];
		else if(_nIdx==-2)
			return m_vOverlapItemsVec[m_nPDPSetIdx];
	}

	return NULL;
}

void CPaxDispProps::DeleteItem(int _nIdx)
{
//	delete m_vPDPItemsVec[m_nPDPSetIdx][_nIdx];
	m_vPDPItemsVec[m_nPDPSetIdx].deleteItem( _nIdx );
}
void CPaxDispProps::DeleteAndReplaceItem( int _iIdx, CPaxDispPropItem* _pPaxDisp)
{
	assert( _iIdx>= 0 && _iIdx < m_vPDPItemsVec[m_nPDPSetIdx].getCount() );
	delete m_vPDPItemsVec[m_nPDPSetIdx][_iIdx];
	m_vPDPItemsVec[m_nPDPSetIdx][_iIdx] = _pPaxDisp;
	
}
void CPaxDispProps::initDefaultValues()
{
	m_vPDPItemsVec.resize(1);
	m_vPDPSetNamesVec.reserve(1);
	//m_vDefaultItemsVec.reserve(1);
	//m_vOverlapItemsVec.reserve(1);
	m_vPDPSetNamesVec.push_back("DEFAULT");
	m_vOverlapItemsVec.addItem(new CPaxDispPropItem("OVERLAP"));
	m_vDefaultItemsVec.addItem(new CPaxDispPropItem("DEFAULT"));
	m_nPDPSetIdx = 0;
}

// Description: Read Data From Default File.
// Exception:	FileVersionTooNewError
void CPaxDispProps::loadInputs( const CString& _csProjPath, InputTerminal* _pInTerm )
{
	this->SetInputTerminal( _pInTerm );
	this->loadDataSet( _csProjPath );
}

void CPaxDispProps::saveInputs( const CString& _csProjPath, bool _bUndo )
{
	this->saveDataSet(_csProjPath, _bUndo);
}

void CPaxDispProps::readData(ArctermFile& p_file)
{
	ASSERT(m_vPDPItemsVec.size()==0);
	ASSERT(m_vPDPSetNamesVec.size()==0);
	ASSERT(m_vDefaultItemsVec.getCount()==0);
	ASSERT(m_vOverlapItemsVec.getCount()==0);

	p_file.getLine();

	int nPDPSetCount;
	p_file.getInteger(nPDPSetCount);
	p_file.getInteger(m_nPDPSetIdx);
	ASSERT(m_nPDPSetIdx<nPDPSetCount);

	m_vPDPItemsVec.resize(nPDPSetCount);
	m_vPDPSetNamesVec.reserve(nPDPSetCount);
	//	m_vDefaultItemsVec.reserve(nPDPSetCount);
	//	m_vOverlapItemsVec.reserve(nPDPSetCount);
	int nCurSet = -1;

	p_file.getLine();

	while (!p_file.isBlank ())
	{
		CString sz;
		char s[256];
		int n, m;
		long l;
		double d;
		COleDateTime dt;
		int nHour, nMin, nSec;

		p_file.getField(s,255); //pdpi name or pdp set name
		if(_stricmp(s, "PDPISETNAME") == 0) {
			nCurSet++;
			//pdp set name
			p_file.getField(s,255);
			//s is name of pdpi set
			m_vPDPSetNamesVec.push_back(s);
			m_vDefaultItemsVec.addItem(NULL);
			m_vOverlapItemsVec.addItem(NULL);
			ASSERT(nCurSet == m_vPDPSetNamesVec.size()-1);
			ASSERT(nCurSet == m_vDefaultItemsVec.getCount()-1);
			ASSERT(nCurSet == m_vOverlapItemsVec.getCount()-1);
		}
		else {

			CPaxDispPropItem* pItem = new CPaxDispPropItem(s);
			if(pItem->GetPaxType()->GetName() == "DEFAULT" || pItem->GetPaxType()->GetName() == "OVERLAP") {
				p_file.skipField(17);
			}
			else {
				//p_file.getField(s,255);
				CMobileElemConstraint paxCon(m_pInTerm);
				//paxCon.SetInputTerminal(m_pInTerm);
				//paxCon.setConstraint(s);
				paxCon.readConstraint(p_file);
				pItem->GetPaxType()->SetPaxCon(paxCon);
				p_file.getInteger(n);
				pItem->GetPaxType()->SetIsPaxConAdd((BOOL) n);
				p_file.getInteger(n);
				pItem->GetPaxType()->SetAreaportalApplyType((AREA_PORTAL_APPLY_TYPE) n);
				if(pItem->GetPaxType()->GetAreaportalApplyType() != ENUM_APPLY_NONE) {
					if(p_file.isBlankField()) 
						p_file.skipField(1);
					else {
						p_file.getField(s,255);
						pItem->GetPaxType()->GetArea().name = s;
					}
					if(p_file.isBlankField()) 
						p_file.skipField(1);
					else {
						p_file.getField(s,255);
						pItem->GetPaxType()->GetPortal().name = s;
					}
					p_file.getInteger(n);
					pItem->GetPaxType()->SetIsTimeApplied((BOOL) n);
					if(pItem->GetPaxType()->GetIsTimeApplied()) {
						p_file.getField(s,255);
						sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
						dt.SetTime(nHour, nMin, nSec);
						pItem->GetPaxType()->SetStartTime(dt);
						p_file.getField(s,255);
						sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
						dt.SetTime(nHour, nMin, nSec);
						pItem->GetPaxType()->SetEndTime(dt);
					}
					else {
						p_file.skipField(2);
					}
					p_file.getInteger(n);
					pItem->GetPaxType()->SetIsAreaPortalAdd((BOOL) n);
				}
				else {
					p_file.skipField(5);
				}
				p_file.getInteger(n);
				pItem->GetPaxType()->SetIsBirthTimeApplied((BOOL) n);
				if(pItem->GetPaxType()->GetIsBirthTimeApplied()) {
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetPaxType()->SetMinBirthTime(dt);
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetPaxType()->SetMaxBirthTime(dt);
					p_file.getInteger(n);
					pItem->GetPaxType()->SetIsBirthTimeAdd((BOOL) n);
				}
				else {
					p_file.skipField(3);
				}
				p_file.getInteger(n);
				pItem->GetPaxType()->SetIsDeathTimeApplied((BOOL) n);
				if(pItem->GetPaxType()->GetIsDeathTimeApplied()) {
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetPaxType()->SetMinDeathTime(dt);
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetPaxType()->SetMaxDeathTime(dt);
					p_file.getInteger(n);
					pItem->GetPaxType()->SetIsDeathTimeAdd((BOOL) n);
				}
				else {
					p_file.skipField(3);
				}
			}

			p_file.getInteger(n);
			p_file.getInteger(m);
			pItem->SetShape( std::pair<int, int>(n, m) );

			p_file.getInteger(n);
			pItem->SetLineType((CPaxDispPropItem::ENUM_LINE_TYPE) n);

			p_file.getInteger(l);
			pItem->SetColor((COLORREF) l);

			p_file.getInteger(n);
			pItem->SetLeaveTrail((BOOL) n);

			p_file.getInteger(n);
			pItem->SetVisible((BOOL) n);

			p_file.getFloat(d);
			pItem->SetDensity(d);

			if(pItem->GetPaxType()->GetName() == "DEFAULT") {
				m_vDefaultItemsVec[nCurSet] = pItem;
			}
			else if(pItem->GetPaxType()->GetName() == "OVERLAP") {
				m_vOverlapItemsVec[nCurSet] = pItem;
			}
			else {
				m_vPDPItemsVec[nCurSet].addItem(pItem);
			}
		}

		p_file.getLine();
	}
}

void CPaxDispProps::readObsolete24(ArctermFile& p_file)
{
	ASSERT(m_vPDPItemsVec.size()==0);
	ASSERT(m_vPDPSetNamesVec.size()==0);
	ASSERT(m_vDefaultItemsVec.getCount()==0);
	ASSERT(m_vOverlapItemsVec.getCount()==0);
	
	p_file.getLine();

	int nPDPSetCount;
	p_file.getInteger(nPDPSetCount);
	p_file.getInteger(m_nPDPSetIdx);
	ASSERT(m_nPDPSetIdx<nPDPSetCount);

	m_vPDPItemsVec.resize(nPDPSetCount);
	m_vPDPSetNamesVec.reserve(nPDPSetCount);
//	m_vDefaultItemsVec.reserve(nPDPSetCount);
//	m_vOverlapItemsVec.reserve(nPDPSetCount);
	int nCurSet = -1;

	p_file.getLine();

    while (!p_file.isBlank ())
	{
		CString sz;
		char s[256];
		int n;
		long l;
		double d;
		COleDateTime dt;
		int nHour, nMin, nSec;

		p_file.getField(s,255); //pdpi name or pdp set name
		if(_stricmp(s, "PDPISETNAME") == 0) {
			nCurSet++;
			//pdp set name
			p_file.getField(s,255);
			//s is name of pdpi set
			m_vPDPSetNamesVec.push_back(s);
			m_vDefaultItemsVec.addItem(NULL);
			m_vOverlapItemsVec.addItem(NULL);
			ASSERT(nCurSet == m_vPDPSetNamesVec.size()-1);
			ASSERT(nCurSet == m_vDefaultItemsVec.getCount()-1);
			ASSERT(nCurSet == m_vOverlapItemsVec.getCount()-1);
		}
		else {

			CPaxDispPropItem* pItem = new CPaxDispPropItem(s);
			if(pItem->GetPaxType()->GetName() == "DEFAULT" || pItem->GetPaxType()->GetName() == "OVERLAP") {
				p_file.skipField(17);
			}
			else {
				//p_file.getField(s,255);
				CMobileElemConstraint paxCon(m_pInTerm);
				//paxCon.SetInputTerminal(m_pInTerm);
				//paxCon.setConstraint(s);
				paxCon.readConstraint(p_file);
				pItem->GetPaxType()->SetPaxCon(paxCon);
				p_file.getInteger(n);
				pItem->GetPaxType()->SetIsPaxConAdd((BOOL) n);
				p_file.getInteger(n);
				pItem->GetPaxType()->SetAreaportalApplyType((AREA_PORTAL_APPLY_TYPE) n);
				if(pItem->GetPaxType()->GetAreaportalApplyType() != ENUM_APPLY_NONE) {
					if(p_file.isBlankField()) 
						p_file.skipField(1);
					else {
						p_file.getField(s,255);
						pItem->GetPaxType()->GetArea().name = s;
					}
					if(p_file.isBlankField()) 
						p_file.skipField(1);
					else {
						p_file.getField(s,255);
						pItem->GetPaxType()->GetPortal().name = s;
					}
					p_file.getInteger(n);
					pItem->GetPaxType()->SetIsTimeApplied((BOOL) n);
					if(pItem->GetPaxType()->GetIsTimeApplied()) {
						p_file.getField(s,255);
						sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
						dt.SetTime(nHour, nMin, nSec);
						pItem->GetPaxType()->SetStartTime(dt);
						p_file.getField(s,255);
						sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
						dt.SetTime(nHour, nMin, nSec);
						pItem->GetPaxType()->SetEndTime(dt);
					}
					else {
						p_file.skipField(2);
					}
					p_file.getInteger(n);
					pItem->GetPaxType()->SetIsAreaPortalAdd((BOOL) n);
				}
				else {
					p_file.skipField(5);
				}
				p_file.getInteger(n);
				pItem->GetPaxType()->SetIsBirthTimeApplied((BOOL) n);
				if(pItem->GetPaxType()->GetIsBirthTimeApplied()) {
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetPaxType()->SetMinBirthTime(dt);
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetPaxType()->SetMaxBirthTime(dt);
					p_file.getInteger(n);
					pItem->GetPaxType()->SetIsBirthTimeAdd((BOOL) n);
				}
				else {
					p_file.skipField(3);
				}
				p_file.getInteger(n);
				pItem->GetPaxType()->SetIsDeathTimeApplied((BOOL) n);
				if(pItem->GetPaxType()->GetIsDeathTimeApplied()) {
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetPaxType()->SetMinDeathTime(dt);
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetPaxType()->SetMaxDeathTime(dt);
					p_file.getInteger(n);
					pItem->GetPaxType()->SetIsDeathTimeAdd((BOOL) n);
				}
				else {
					p_file.skipField(3);
				}
			}

			p_file.getInteger(n);
			pItem->SetShape( std::pair<int, int>(TABLE_OLD_SHAPE_TO_SET[n], TABLE_OLD_SHAPE_TO_ITEM[n]) );

			p_file.getInteger(n);
			pItem->SetLineType((CPaxDispPropItem::ENUM_LINE_TYPE) n);

			p_file.getInteger(l);
			pItem->SetColor((COLORREF) l);

			p_file.getInteger(n);
			pItem->SetLeaveTrail((BOOL) n);

			p_file.getInteger(n);
			pItem->SetVisible((BOOL) n);

			p_file.getFloat(d);
			pItem->SetDensity(d);

			if(pItem->GetPaxType()->GetName() == "DEFAULT") {
				m_vDefaultItemsVec[nCurSet] = pItem;
			}
			else if(pItem->GetPaxType()->GetName() == "OVERLAP") {
				m_vOverlapItemsVec[nCurSet] = pItem;
			}
			else {
				m_vPDPItemsVec[nCurSet].addItem(pItem);
			}
		}

		p_file.getLine();
	}
}

void CPaxDispProps::readObsoletePre23(ArctermFile& p_file)
{
	ASSERT(m_vPDPItemsVec.size()==0);
	ASSERT(m_vPDPSetNamesVec.size()==0);
	ASSERT(m_vDefaultItemsVec.getCount()==0);
	ASSERT(m_vOverlapItemsVec.getCount()==0);
	
	//m_vPDPItemsVec.resize(1);
	m_vPDPSetNamesVec.reserve(1);
	//m_vDefaultItemsVec.reserve(1);
	//m_vOverlapItemsVec.reserve(1);
	m_vPDPSetNamesVec.push_back("old data");

	p_file.getLine();

    while (!p_file.isBlank ())
	{
		CString sz;
		char s[256];
		int n;
		long l;
		COleDateTime dt;
		int nHour, nMin, nSec;

		p_file.getField(s,255); //name
		CPaxDispPropItem* pItem = new CPaxDispPropItem(s);
		if(pItem->GetPaxType()->GetName() == "DEFAULT" || pItem->GetPaxType()->GetName() == "OVERLAP") {
			p_file.skipField(17);
		}
		else {
			//p_file.getField(s,255);
			CMobileElemConstraint paxCon(m_pInTerm);
			//paxCon.SetInputTerminal(m_pInTerm);
			//paxCon.setConstraint(s);
			paxCon.readConstraint(p_file);
			pItem->GetPaxType()->SetPaxCon(paxCon);
			p_file.getInteger(n);
			pItem->GetPaxType()->SetIsPaxConAdd((BOOL) n);
			p_file.getInteger(n);
			pItem->GetPaxType()->SetAreaportalApplyType((AREA_PORTAL_APPLY_TYPE) n);
			if(pItem->GetPaxType()->GetAreaportalApplyType() != ENUM_APPLY_NONE) {
				if(p_file.isBlankField()) 
					p_file.skipField(1);
				else {
					p_file.getField(s,255);
					pItem->GetPaxType()->GetArea().name = s;
				}
				if(p_file.isBlankField()) 
					p_file.skipField(1);
				else {
					p_file.getField(s,255);
					pItem->GetPaxType()->GetPortal().name = s;
				}
				p_file.getInteger(n);
				pItem->GetPaxType()->SetIsTimeApplied((BOOL) n);
				if(pItem->GetPaxType()->GetIsTimeApplied()) {
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetPaxType()->SetStartTime(dt);
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetPaxType()->SetEndTime(dt);
				}
				else {
					p_file.skipField(2);
				}
				p_file.getInteger(n);
				pItem->GetPaxType()->SetIsAreaPortalAdd((BOOL) n);
			}
			else {
				p_file.skipField(5);
			}
			p_file.getInteger(n);
			pItem->GetPaxType()->SetIsBirthTimeApplied((BOOL) n);
			if(pItem->GetPaxType()->GetIsBirthTimeApplied()) {
				p_file.getField(s,255);
				sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
				dt.SetTime(nHour, nMin, nSec);
				pItem->GetPaxType()->SetMinBirthTime(dt);
				p_file.getField(s,255);
				sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
				dt.SetTime(nHour, nMin, nSec);
				pItem->GetPaxType()->SetMaxBirthTime(dt);
				p_file.getInteger(n);
				pItem->GetPaxType()->SetIsBirthTimeAdd((BOOL) n);
			}
			else {
				p_file.skipField(3);
			}
			p_file.getInteger(n);
			pItem->GetPaxType()->SetIsDeathTimeApplied((BOOL) n);
			if(pItem->GetPaxType()->GetIsDeathTimeApplied()) {
				p_file.getField(s,255);
				sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
				dt.SetTime(nHour, nMin, nSec);
				pItem->GetPaxType()->SetMinDeathTime(dt);
				p_file.getField(s,255);
				sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
				dt.SetTime(nHour, nMin, nSec);
				pItem->GetPaxType()->SetMaxDeathTime(dt);
				p_file.getInteger(n);
				pItem->GetPaxType()->SetIsDeathTimeAdd((BOOL) n);
			}
			else {
				p_file.skipField(3);
			}
		}

		p_file.getInteger(n);
		pItem->SetShape( std::pair<int, int>(TABLE_OLD_SHAPE_TO_SET[n], TABLE_OLD_SHAPE_TO_ITEM[n]) );

		p_file.getInteger(n);
		pItem->SetLineType((CPaxDispPropItem::ENUM_LINE_TYPE) n);

		p_file.getInteger(l);
		pItem->SetColor((COLORREF) l);

		p_file.getInteger(n);
		pItem->SetLeaveTrail((BOOL) n);

		p_file.getInteger(n);
		pItem->SetVisible((BOOL) n);

		if(pItem->GetPaxType()->GetName() == "DEFAULT") {
			m_vDefaultItemsVec.addItem(pItem);
		}
		else if(pItem->GetPaxType()->GetName() == "OVERLAP") {
			m_vOverlapItemsVec.addItem(pItem);
		}
		else {
			m_vPDPItemsVec[0].addItem(pItem);
		}

		p_file.getLine();
	}
}

void CPaxDispProps::readObsolete23(ArctermFile& p_file)
{
	ASSERT(m_vPDPItemsVec.size()==0);
	ASSERT(m_vPDPSetNamesVec.size()==0);
	ASSERT(m_vDefaultItemsVec.getCount()==0);
	ASSERT(m_vOverlapItemsVec.getCount()==0);
	
	p_file.getLine();

	int nPDPSetCount;
	p_file.getInteger(nPDPSetCount);
	p_file.getInteger(m_nPDPSetIdx);
	ASSERT(m_nPDPSetIdx<nPDPSetCount);

	m_vPDPItemsVec.resize(nPDPSetCount);
	m_vPDPSetNamesVec.reserve(nPDPSetCount);
	//m_vDefaultItemsVec.reserve(nPDPSetCount);
	//m_vOverlapItemsVec.reserve(nPDPSetCount);
	int nCurSet = -1;

	p_file.getLine();

    while (!p_file.isBlank ())
	{
		CString sz;
		char s[256];
		int n;
		long l;
		COleDateTime dt;
		int nHour, nMin, nSec;

		p_file.getField(s,255); //pdpi name or pdp set name
		if(_stricmp(s, "PDPISETNAME") == 0) {
			nCurSet++;
			//pdp set name
			p_file.getField(s,255);
			//s is name of pdpi set
			m_vPDPSetNamesVec.push_back(s);
			m_vDefaultItemsVec.addItem(NULL);
			m_vOverlapItemsVec.addItem(NULL);
			ASSERT(nCurSet == m_vPDPSetNamesVec.size()-1);
			ASSERT(nCurSet == m_vDefaultItemsVec.size()-1);
			ASSERT(nCurSet == m_vOverlapItemsVec.size()-1);
		}
		else {

			CPaxDispPropItem* pItem = new CPaxDispPropItem(s);
			if(pItem->GetPaxType()->GetName() == "DEFAULT" || pItem->GetPaxType()->GetName() == "OVERLAP") {
				p_file.skipField(17);
			}
			else {
				//p_file.getField(s,255);
				CMobileElemConstraint paxCon(m_pInTerm);
				//paxCon.SetInputTerminal(m_pInTerm);
				//paxCon.setConstraint(s);
				paxCon.readConstraint(p_file);
				pItem->GetPaxType()->SetPaxCon(paxCon);
				p_file.getInteger(n);
				pItem->GetPaxType()->SetIsPaxConAdd((BOOL) n);
				p_file.getInteger(n);
				pItem->GetPaxType()->SetAreaportalApplyType((AREA_PORTAL_APPLY_TYPE) n);
				if(pItem->GetPaxType()->GetAreaportalApplyType() != ENUM_APPLY_NONE) {
					if(p_file.isBlankField()) 
						p_file.skipField(1);
					else {
						p_file.getField(s,255);
						pItem->GetPaxType()->GetArea().name = s;
					}
					if(p_file.isBlankField()) 
						p_file.skipField(1);
					else {
						p_file.getField(s,255);
						pItem->GetPaxType()->GetPortal().name = s;
					}
					p_file.getInteger(n);
					pItem->GetPaxType()->SetIsTimeApplied((BOOL) n);
					if(pItem->GetPaxType()->GetIsTimeApplied()) {
						p_file.getField(s,255);
						sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
						dt.SetTime(nHour, nMin, nSec);
						pItem->GetPaxType()->SetStartTime(dt);
						p_file.getField(s,255);
						sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
						dt.SetTime(nHour, nMin, nSec);
						pItem->GetPaxType()->SetEndTime(dt);
					}
					else {
						p_file.skipField(2);
					}
					p_file.getInteger(n);
					pItem->GetPaxType()->SetIsAreaPortalAdd((BOOL) n);
				}
				else {
					p_file.skipField(5);
				}
				p_file.getInteger(n);
				pItem->GetPaxType()->SetIsBirthTimeApplied((BOOL) n);
				if(pItem->GetPaxType()->GetIsBirthTimeApplied()) {
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetPaxType()->SetMinBirthTime(dt);
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetPaxType()->SetMaxBirthTime(dt);
					p_file.getInteger(n);
					pItem->GetPaxType()->SetIsBirthTimeAdd((BOOL) n);
				}
				else {
					p_file.skipField(3);
				}
				p_file.getInteger(n);
				pItem->GetPaxType()->SetIsDeathTimeApplied((BOOL) n);
				if(pItem->GetPaxType()->GetIsDeathTimeApplied()) {
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetPaxType()->SetMinDeathTime(dt);
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetPaxType()->SetMaxDeathTime(dt);
					p_file.getInteger(n);
					pItem->GetPaxType()->SetIsDeathTimeAdd((BOOL) n);
				}
				else {
					p_file.skipField(3);
				}
			}

			p_file.getInteger(n);
			pItem->SetShape( std::pair<int, int>(TABLE_OLD_SHAPE_TO_SET[n], TABLE_OLD_SHAPE_TO_ITEM[n]) );

			p_file.getInteger(n);
			pItem->SetLineType((CPaxDispPropItem::ENUM_LINE_TYPE) n);

			p_file.getInteger(l);
			pItem->SetColor((COLORREF) l);

			p_file.getInteger(n);
			pItem->SetLeaveTrail((BOOL) n);

			p_file.getInteger(n);
			pItem->SetVisible((BOOL) n);

			if(pItem->GetPaxType()->GetName() == "DEFAULT") {
				m_vDefaultItemsVec[nCurSet] = pItem;
			}
			else if(pItem->GetPaxType()->GetName() == "OVERLAP") {
				m_vOverlapItemsVec[nCurSet] = pItem;
			}
			else {
				m_vPDPItemsVec[nCurSet].addItem(pItem);
			}
		}

		p_file.getLine();
	}
}

void CPaxDispProps::readObsoleteData (ArctermFile& p_file)
{
	if(p_file.getVersion() < 2.3f) {
		readObsoletePre23(p_file);
	}
	else if(p_file.getVersion() == 2.3f) {
		readObsolete23(p_file);
	}
	else if(p_file.getVersion() == 2.4f) {
		readObsolete24(p_file);
	}
	else if(p_file.getVersion() > 2.4f) {
		readData(p_file);
	}
	else {
		ASSERT(FALSE);
	}
}


void CPaxDispProps::writeData (ArctermFile& p_file) const
{
	//write PDP set count
	int nPDPSetCount = m_vPDPSetNamesVec.size();
	p_file.writeInt(nPDPSetCount);
	p_file.writeInt(m_nPDPSetIdx);
	p_file.writeLine();

	CPaxDispPropItem* pItem = NULL;

	for(int i=0; i<nPDPSetCount; i++) {
		p_file.writeField("PDPISETNAME");
		p_file.writeField(m_vPDPSetNamesVec[i]);
		p_file.writeLine();
		pItem = m_vDefaultItemsVec[i]; writePDPI( pItem, p_file );
		pItem = m_vOverlapItemsVec[i]; writePDPI( pItem, p_file );
		int nCount = m_vPDPItemsVec[i].size();
	    for( int j = 0; j < nCount; j++ ) {
			pItem = m_vPDPItemsVec[i][j];
			writePDPI( pItem, p_file );
	    }
	}
}

void CPaxDispProps::writePDPI(CPaxDispPropItem* pPDPI, ArctermFile& p_file) const
{
	ASSERT(pPDPI);
	//write pax type
	//char s[256]; MATT
	char s[2560];
	CPaxType* pPT = pPDPI->GetPaxType();
	p_file.writeField(pPT->GetName());

	if(pPT->GetName() == "DEFAULT" || pPT->GetName() == "OVERLAP") {
		p_file.writeField("DEFAULT");
		p_file.writeBlankField();
		p_file.writeBlankField();
		p_file.writeBlankField();
		p_file.writeBlankField();
		p_file.writeBlankField();
		p_file.writeBlankField();
		p_file.writeBlankField();
		p_file.writeBlankField();
		p_file.writeBlankField();
		p_file.writeBlankField();
		p_file.writeBlankField();
		p_file.writeBlankField();
		p_file.writeBlankField();
		p_file.writeBlankField();
		p_file.writeBlankField();
		p_file.writeBlankField();
	}
	else {
		pPT->GetPaxCon().WriteSyntaxStringWithVersion(s);
		p_file.writeField(s);
		p_file.writeInt((int) pPT->GetIsPaxConAdd());

		p_file.writeInt((int) pPT->GetAreaportalApplyType());
		if(pPT->GetAreaportalApplyType() != ENUM_APPLY_NONE) {
			if(pPT->GetArea().name.IsEmpty())
				p_file.writeBlankField();
			else
				p_file.writeField(pPT->GetArea().name);
			if(pPT->GetPortal().name.IsEmpty())
				p_file.writeBlankField();
			else
				p_file.writeField(pPT->GetPortal().name);
			p_file.writeInt((int)pPT->GetIsTimeApplied()); //area/portal time
			if(pPT->GetIsTimeApplied()) {
				CString sTime = pPT->GetStartTime().Format("%H:%M:%S");
				p_file.writeField(sTime);
				sTime = pPT->GetEndTime().Format("%H:%M:%S");
				p_file.writeField(sTime);
			}
			else {
				p_file.writeBlankField();
				p_file.writeBlankField(); 
			}
			p_file.writeInt((int) pPT->GetIsAreaPortalAdd());
		}
		else {
			p_file.writeBlankField();
			p_file.writeBlankField();
			p_file.writeBlankField();
			p_file.writeBlankField();
			p_file.writeBlankField();
		}

		p_file.writeInt((int) pPT->GetIsBirthTimeApplied());
		if(pPT->GetIsBirthTimeApplied()) {
			CString sTime = pPT->GetMinBirthTime().Format("%H:%M:%S");
			p_file.writeField(sTime);
			sTime = pPT->GetMaxBirthTime().Format("%H:%M:%S");
			p_file.writeField(sTime);
			p_file.writeInt((int) pPT->GetIsBirthTimeAdd());
		}
		else {
			p_file.writeBlankField();
			p_file.writeBlankField();
			p_file.writeBlankField();
		}
		
		p_file.writeInt((int) pPT->GetIsDeathTimeApplied());
		if(pPT->GetIsDeathTimeApplied()) {
			CString sTime = pPT->GetMinDeathTime().Format("%H:%M:%S");
			p_file.writeField(sTime);
			sTime = pPT->GetMaxDeathTime().Format("%H:%M:%S");
			p_file.writeField(sTime);
			p_file.writeInt((int) pPT->GetIsDeathTimeAdd());
		}
		else {
			p_file.writeBlankField();
			p_file.writeBlankField(); 
			p_file.writeBlankField(); 
		}
	}

	//write shape
	p_file.writeInt((int) pPDPI->GetShape().first);
	p_file.writeInt((int) pPDPI->GetShape().second);

	//write line type
	p_file.writeInt((int) pPDPI->GetLineType());
	//write color
	p_file.writeInt((long) pPDPI->GetColor());
	//write isLeaveTrail
	p_file.writeInt((int) pPDPI->IsLeaveTrail());
	//write isVisible
	p_file.writeInt((int) pPDPI->IsVisible());
	//write density
	p_file.writeFloat(static_cast<float>(pPDPI->GetDensity()));

	p_file.writeLine();
	
}


int CPaxDispProps::AddPDPSet(const CString& _s, CPaxDispPropItem* _pDefaultItem, CPaxDispPropItem* _pOverlapItem)
{
	m_vPDPSetNamesVec.push_back(_s);
	m_vDefaultItemsVec.addItem(_pDefaultItem);
	m_vOverlapItemsVec.addItem(_pOverlapItem);

	ASSERT(m_vPDPSetNamesVec.size() == m_vDefaultItemsVec.getCount());
	ASSERT(m_vPDPSetNamesVec.size() == m_vOverlapItemsVec.getCount());

	m_vPDPItemsVec.resize(m_vPDPItemsVec.size()+1);

	ASSERT(m_vPDPSetNamesVec.size() == m_vPDPItemsVec.size());

	return m_vPDPSetNamesVec.size()-1;
}

void CPaxDispProps::DeletPDPSet(int _idx)
{
	m_vPDPSetNamesVec.erase(m_vPDPSetNamesVec.begin()+_idx);
	delete m_vDefaultItemsVec[_idx];
	//m_vDefaultItemsVec.deleteItem(_idx);
	m_vDefaultItemsVec.removeItem(_idx);
	delete m_vOverlapItemsVec[_idx];
//	m_vOverlapItemsVec.deleteItem(_idx);
    m_vOverlapItemsVec.removeItem(_idx);
	int nCount = m_vPDPItemsVec[_idx].getCount();
	for(int i=0; i<nCount; i++) {
		delete m_vPDPItemsVec[_idx][i];
	}
	m_vPDPItemsVec[_idx].clear();
	m_vPDPItemsVec.erase(m_vPDPItemsVec.begin()+_idx);

	ASSERT(m_vPDPSetNamesVec.size() == m_vDefaultItemsVec.getCount());
	ASSERT(m_vPDPSetNamesVec.size() == m_vOverlapItemsVec.getCount());
	ASSERT(m_vPDPSetNamesVec.size() == m_vPDPItemsVec.size());

	if(m_nPDPSetIdx >= static_cast<int>(m_vPDPSetNamesVec.size()))
		m_nPDPSetIdx--;
	ASSERT(m_nPDPSetIdx >= 0);
}

const CString& CPaxDispProps::GetPDPSetName(int _idx) const
{
	ASSERT(_idx >= 0);
	ASSERT(_idx < static_cast<int>(m_vPDPSetNamesVec.size()));
	return m_vPDPSetNamesVec[_idx];
}

void CPaxDispProps::RenameCurrentPDPSet(const CString& _s)
{
	m_vPDPSetNamesVec[m_nPDPSetIdx] = _s;
}

void CPaxDispProps::SetCurrentPDPSet(int _idx)
{
	ASSERT(_idx >= 0);
	ASSERT(_idx < static_cast<int>(m_vPDPSetNamesVec.size()));
	ASSERT(_idx < static_cast<int>(m_vPDPItemsVec.size()));
	ASSERT(_idx < m_vDefaultItemsVec.getCount());
	ASSERT(_idx < m_vOverlapItemsVec.getCount());

	
	m_nPDPSetIdx = _idx;
}

// find Best Match Type from the sortedContainer
// int CPaxDispProps::FindBestMatch(const MobDescStruct& pds, Terminal* pTerminal)
// {
// 	for(int j=0; j<GetCount(); j++) 
// 	{
// 		CPaxDispPropItem* pPDPI = GetDispPropItem(j);
// 		CPaxDispPropItemPtrVector paxItemVector;
// 		if(pPDPI->GetPaxType()->Matches(pds, pTerminal)) 
// 			if (Overlap(paxItemVector,pPDPI))
// 			{
// 				return -2;
// 			}
// 			else return j;
// 	}
// 
// 		return -1;
// }

bool CPaxDispProps::Overlap(std::vector<int> paxItemVector,CPaxDispPropItem* pPDPI)
{
	for (unsigned i = 0; i < paxItemVector.size(); i++)
	{
		int nIdx = paxItemVector.at(i);
		CPaxDispPropItem* pItem  = GetDispPropItem(nIdx);
		if (!pPDPI->GetPaxType()->fit(*pItem->GetPaxType()) && !pItem->GetPaxType()->fit(*pPDPI->GetPaxType()))
		{
			return true;
		}
			
	}
	return false;
}
int CPaxDispProps::FindBestMatch(const MobDescStruct& pds, Terminal* pTerminal)
{
	std::vector<int> vIndex;
	for(int j=0; j<GetCount(); j++) 
	{
		CPaxDispPropItem* pPDPI = GetDispPropItem(j);
		if(pPDPI->GetPaxType()->Matches(pds, pTerminal)) 
		{
			if (vIndex.empty())
			{
				vIndex.push_back(j);
			}
			else
			{
				if (Overlap(vIndex,pPDPI))
				{
					return -2;
				}
				else
				{
					vIndex.push_back(j);
				}
			}
		}
	}

	if (vIndex.empty())
	{
		return -1;
	}

	return vIndex.front();
}