// PaxTags.cpp: implementation of the CPaxTags class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "PaxTags.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaxTags::CPaxTags() : DataSet( PaxTagsFile, 2.3f )
{
	m_vPaxTagItemsVec.resize(1);
	m_vPTSetNamesVec.reserve(1);
//	m_vDefaultItemsVec.reserve(1);
//	m_vOverlapItemsVec.reserve(1);
	m_vPTSetNamesVec.push_back("DEFAULT");
	m_vOverlapItemsVec.addItem(new CPaxTagItem("OVERLAP"));
	m_vDefaultItemsVec.addItem(new CPaxTagItem("DEFAULT"));
	m_nPTSetIdx = 0;

}

CPaxTags::~CPaxTags()
{
	clear();
}

int CPaxTags::GetCount()
{
	return m_vPaxTagItemsVec[m_nPTSetIdx].getCount();
}

CPaxTagItem* CPaxTags::GetPaxTagItem( int _nIdx )
{
	if(_nIdx>=0)
		return m_vPaxTagItemsVec[m_nPTSetIdx][_nIdx];
	else if(_nIdx==-1)
		return m_vDefaultItemsVec[m_nPTSetIdx];
	else if(_nIdx==-2)
		return m_vDefaultItemsVec[m_nPTSetIdx];

	return NULL;
}

void CPaxTags::DeleteItem(int _nIdx)
{
	delete m_vPaxTagItemsVec[m_nPTSetIdx][_nIdx];
	m_vPaxTagItemsVec[m_nPTSetIdx][_nIdx]=NULL;
	m_vPaxTagItemsVec[m_nPTSetIdx].deleteItem( _nIdx );
}


void CPaxTags::clear()
{
	int nCount, i;

	m_vPTSetNamesVec.clear();
	
	nCount = m_vOverlapItemsVec.getCount();
	for(i=0; i<nCount; i++) {
		delete m_vOverlapItemsVec[i];
		m_vOverlapItemsVec[i]=NULL;
	}
	m_vOverlapItemsVec.clear();

	nCount = m_vDefaultItemsVec.getCount();
	for(i=0; i<nCount; i++) {
		delete m_vDefaultItemsVec[i];
		m_vDefaultItemsVec[i]=NULL;
	}
	m_vDefaultItemsVec.clear();
	
	nCount = m_vPaxTagItemsVec.size();
	for( i=0; i<nCount; i++ ) {
		int nCount2 = m_vPaxTagItemsVec[i].size();
		for(int j=0; j<nCount2; j++) {
			delete m_vPaxTagItemsVec[i][j];
			m_vPaxTagItemsVec[i][j]=NULL;
		}
		m_vPaxTagItemsVec[i].clear();
	}
	m_vPaxTagItemsVec.clear();
}

void CPaxTags::initDefaultValues()
{
	m_vPaxTagItemsVec.resize(1);
	m_vPTSetNamesVec.reserve(1);
	//m_vDefaultItemsVec.reserve(1);
	//m_vOverlapItemsVec.reserve(1);
	m_vPTSetNamesVec.push_back("DEFAULT");
	m_vOverlapItemsVec.addItem(new CPaxTagItem("OVERLAP"));
	m_vDefaultItemsVec.addItem(new CPaxTagItem("DEFAULT"));
	m_nPTSetIdx = 0;
}

void CPaxTags::readObsoleteData (ArctermFile& p_file)
{
	ASSERT(m_vPaxTagItemsVec.size()==0);
	ASSERT(m_vPTSetNamesVec.size()==0);
	ASSERT(m_vDefaultItemsVec.getCount()==0);
	ASSERT(m_vOverlapItemsVec.getCount()==0);
	
	m_vPaxTagItemsVec.resize(1);
	m_vPTSetNamesVec.reserve(1);
	//m_vDefaultItemsVec.reserve(1);
	//m_vOverlapItemsVec.reserve(1);
	m_vPTSetNamesVec.push_back("old data");
	
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
		CPaxTagItem* pItem = new CPaxTagItem(s);
		if(pItem->GetPaxType()->GetName() == "DEFAULT" || pItem->GetPaxType()->GetName() == "OVERLAP") {
			p_file.skipField(17);
		}
		else {
			CMobileElemConstraint paxCon(m_pInTerm);
			//paxCon.SetInputTerminal(m_pInTerm);
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
		pItem->SetShowTags((BOOL) n);

		p_file.getInteger(l);
		pItem->SetTagInfo((DWORD) l);
		
		if(pItem->GetPaxType()->GetName() == "DEFAULT") {
			m_vDefaultItemsVec.addItem(pItem);
		}
		else if(pItem->GetPaxType()->GetName() == "OVERLAP") {
			m_vOverlapItemsVec.addItem(pItem);
		}
		else {
			m_vPaxTagItemsVec[0].push_back(pItem);
		}

		p_file.getLine();
	}

}

void CPaxTags::readData (ArctermFile& p_file)
{
	ASSERT(m_vPaxTagItemsVec.size()==0);
	ASSERT(m_vPTSetNamesVec.size()==0);
	ASSERT(m_vDefaultItemsVec.getCount()==0);
	ASSERT(m_vOverlapItemsVec.getCount()==0);
	
	p_file.getLine();

	int nPTSetCount;
	p_file.getInteger(nPTSetCount);
	p_file.getInteger(m_nPTSetIdx);
	ASSERT(m_nPTSetIdx<nPTSetCount);

	m_vPaxTagItemsVec.resize(nPTSetCount);
	m_vPTSetNamesVec.reserve(nPTSetCount);
	//m_vDefaultItemsVec.reserve(nPTSetCount);
	//m_vOverlapItemsVec.reserve(nPTSetCount);
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
			m_vPTSetNamesVec.push_back(s);
			m_vDefaultItemsVec.addItem(NULL);
			m_vOverlapItemsVec.addItem(NULL);
			ASSERT(nCurSet == m_vPTSetNamesVec.size()-1);
			ASSERT(nCurSet == m_vDefaultItemsVec.getCount()-1);
			ASSERT(nCurSet == m_vOverlapItemsVec.getCount()-1);
		}
		else {

			CPaxTagItem* pItem = new CPaxTagItem(s);
			if(pItem->GetPaxType()->GetName() == "DEFAULT" || pItem->GetPaxType()->GetName() == "OVERLAP") {
				p_file.skipField(17);
			}
			else {
				CMobileElemConstraint paxCon(m_pInTerm);
				paxCon.SetInputTerminal(m_pInTerm);
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
			pItem->SetShowTags((BOOL) n);

			p_file.getInteger(l);
			pItem->SetTagInfo((DWORD) l);
			
			if(pItem->GetPaxType()->GetName() == "DEFAULT") {
				m_vDefaultItemsVec[nCurSet] = pItem;
			}
			else if(pItem->GetPaxType()->GetName() == "OVERLAP") {
				m_vOverlapItemsVec[nCurSet] = pItem;
			}
			else {
				m_vPaxTagItemsVec[nCurSet].addItem(pItem);
			}

		}

		p_file.getLine();
	}
}

void CPaxTags::writeData (ArctermFile& p_file) const
{
	//write PT set count
	int nPTSetCount = m_vPTSetNamesVec.size();
	p_file.writeInt(nPTSetCount);
	p_file.writeInt(m_nPTSetIdx);
	p_file.writeLine();

	CPaxTagItem* pItem = NULL;

	for(int i=0; i<nPTSetCount; i++) {
		p_file.writeField("PDPISETNAME");
		p_file.writeField(m_vPTSetNamesVec[i]);
		p_file.writeLine();
		pItem = m_vDefaultItemsVec[i]; writePTI( pItem, p_file );
		pItem = m_vOverlapItemsVec[i]; writePTI( pItem, p_file );
		int nCount = m_vPaxTagItemsVec[i].getCount();
	    for( int j = 0; j < nCount; j++ ) {
			pItem = m_vPaxTagItemsVec[i][j];
			writePTI( pItem, p_file );
	    }
	}
}

void CPaxTags::writePTI(CPaxTagItem* pPTI, ArctermFile& p_file ) const
{
	ASSERT(pPTI);
	//write pax type
//	char s[256]; MATT
	CHAR s[2560];
	CPaxType* pPT = pPTI->GetPaxType();
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

	//write showTags
	p_file.writeInt((int) pPTI->IsShowTags());

	DWORD dwInfo = pPTI->GetTagInfo();
	//write tag info
	p_file.writeInt((long) dwInfo);

	p_file.writeLine();
}

// Description: Read Data From Default File.
// Exception:	FileVersionTooNewError
void CPaxTags::loadInputs( const CString& _csProjPath, InputTerminal* _pInTerm )
{
	SetInputTerminal( _pInTerm );
	loadDataSet( _csProjPath );
}

void CPaxTags::saveInputs( const CString& _csProjPath, bool _bUndo )
{
	saveDataSet(_csProjPath, _bUndo);
}

int CPaxTags::AddPTSet(const CString& _s, CPaxTagItem* _pDefaultItem, CPaxTagItem* _pOverlapItem)
{
	m_vPTSetNamesVec.push_back(_s);
	m_vDefaultItemsVec.addItem(_pDefaultItem);
	m_vOverlapItemsVec.addItem(_pOverlapItem);

	ASSERT(m_vPTSetNamesVec.size() == m_vDefaultItemsVec.getCount());
	ASSERT(m_vPTSetNamesVec.size() == m_vOverlapItemsVec.getCount());

	m_vPaxTagItemsVec.resize(m_vPaxTagItemsVec.size()+1);

	ASSERT(m_vPTSetNamesVec.size() == m_vPaxTagItemsVec.size());

	return m_vPTSetNamesVec.size()-1;
}

void CPaxTags::DeletPTSet(int _idx)
{
	m_vPTSetNamesVec.erase(m_vPTSetNamesVec.begin()+_idx);
	delete m_vDefaultItemsVec[_idx];
	m_vDefaultItemsVec[_idx]=NULL;
	m_vDefaultItemsVec.deleteItem(_idx);
	delete m_vOverlapItemsVec[_idx];
	m_vOverlapItemsVec[_idx]=NULL;
	m_vOverlapItemsVec.deleteItem(_idx);

	int nCount = m_vPaxTagItemsVec[_idx].getCount();
	for(int i=0; i<nCount; i++) {
		delete m_vPaxTagItemsVec[_idx][i];
		m_vPaxTagItemsVec[_idx][i]=NULL;
	}
	m_vPaxTagItemsVec[_idx].clear();
	m_vPaxTagItemsVec.erase(m_vPaxTagItemsVec.begin()+_idx);

	ASSERT(m_vPTSetNamesVec.size() == m_vDefaultItemsVec.getCount());
	ASSERT(m_vPTSetNamesVec.size() == m_vOverlapItemsVec.getCount());
	ASSERT(m_vPTSetNamesVec.size() == m_vPaxTagItemsVec.size());

	if(m_nPTSetIdx >= static_cast<int>(m_vPTSetNamesVec.size()))
		m_nPTSetIdx--;
	ASSERT(m_nPTSetIdx >= 0);
}

const CString& CPaxTags::GetPTSetName(int _idx) const
{
	ASSERT(_idx >= 0);
	ASSERT(_idx < static_cast<int>(m_vPTSetNamesVec.size()));
	return m_vPTSetNamesVec[_idx];
}

void CPaxTags::RenameCurrentPTSet(const CString& _s)
{
	m_vPTSetNamesVec[m_nPTSetIdx] = _s;
}

void CPaxTags::SetCurrentPTSet(int _idx)
{
	ASSERT(_idx >= 0);
	ASSERT(_idx < static_cast<int>(m_vPTSetNamesVec.size()));
	ASSERT(_idx < static_cast<int>(m_vPaxTagItemsVec.size()));
	ASSERT(_idx < m_vDefaultItemsVec.getCount());
	ASSERT(_idx < m_vOverlapItemsVec.getCount());

	
	m_nPTSetIdx = _idx;
}
int CPaxTags::FindBestMatch(const MobDescStruct& pds, Terminal* pTerminal)
{
	
	for(int j=0; j<GetCount(); j++) 
	{
		CPaxTagItem* pPTI =GetPaxTagItem(j);
		if(pPTI->GetPaxType()->Matches(pds, pTerminal))
			return j;
	}
	return -1;
		
}

/////////////////////////////////////
//// CPaxTagItem ///////////////////
////////////////////////////////////

CPaxTagItem::CPaxTagItem(CString _csName)
{
	m_paxType.SetName( _csName );
	m_bShowTags = FALSE;
	m_dwTagInfo = PAX_NONE;
}

CPaxTagItem::~CPaxTagItem()
{
}

