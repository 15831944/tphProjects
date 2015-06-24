// AircraftTags.cpp: implementation of the CAircraftTags class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AircraftTags.h"
#include "termplan.h"
#include "../Inputs/IN_TERM.H"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAircraftTags::CAircraftTags() : DataSet( AircraftTagsFile )
{
	m_vACTagItemsVec.resize(1);
	m_vSetNamesVec.reserve(1);
	m_vSetNamesVec.push_back("DEFAULT");
	m_vOverlapItemsVec.addItem(new CAircraftTagItem("OVERLAP"));
	m_vDefaultItemsVec.addItem(new CAircraftTagItem("DEFAULT"));
	m_nSetIdx = 0;

}

CAircraftTags::~CAircraftTags()
{
	clear();
}

int CAircraftTags::GetCount()
{
	return m_vACTagItemsVec[m_nSetIdx].getCount();
}

CAircraftTagItem* CAircraftTags::GetTagItem( int _nIdx )
{
	if(_nIdx>=0)
		return m_vACTagItemsVec[m_nSetIdx][_nIdx];
	else if(_nIdx==-1)
		return m_vDefaultItemsVec[m_nSetIdx];
	else if(_nIdx==-2)
		return m_vDefaultItemsVec[m_nSetIdx];

	return NULL;
}

void CAircraftTags::DeleteItem(int _nIdx)
{
	m_vACTagItemsVec[m_nSetIdx].deleteItem( _nIdx );
}


void CAircraftTags::clear()
{
	int nCount, i;

	m_vSetNamesVec.clear();
	
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
	
	nCount = m_vACTagItemsVec.size();
	for( i=0; i<nCount; i++ ) {
		int nCount2 = m_vACTagItemsVec[i].size();
		for(int j=0; j<nCount2; j++) {
			delete m_vACTagItemsVec[i][j];
		}
		m_vACTagItemsVec[i].clear();
	}
	m_vACTagItemsVec.clear();
}

void CAircraftTags::initDefaultValues()
{
	m_vACTagItemsVec.resize(1);
	m_vSetNamesVec.reserve(1);
	//m_vDefaultItemsVec.reserve(1);
	//m_vOverlapItemsVec.reserve(1);
	m_vSetNamesVec.push_back("DEFAULT");
	m_vOverlapItemsVec.addItem(new CAircraftTagItem("OVERLAP"));
	m_vDefaultItemsVec.addItem(new CAircraftTagItem("DEFAULT"));
	m_nSetIdx = 0;
}

void CAircraftTags::readObsoleteData (ArctermFile& p_file)
{
}

void CAircraftTags::readData (ArctermFile& p_file)
{
	ASSERT(m_vACTagItemsVec.size()==0);
	ASSERT(m_vSetNamesVec.size()==0);
	ASSERT(m_vDefaultItemsVec.getCount()==0);
	ASSERT(m_vOverlapItemsVec.getCount()==0);
	
	p_file.getLine();

	int nSetCount;
	p_file.getInteger(nSetCount);
	p_file.getInteger(m_nSetIdx);
	ASSERT(m_nSetIdx<nSetCount);

	m_vACTagItemsVec.resize(nSetCount);
	m_vSetNamesVec.reserve(nSetCount);
	//m_vDefaultItemsVec.reserve(nSetCount);
	//m_vOverlapItemsVec.reserve(nSetCount);
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

		p_file.getField(s,255); //ati name or adp set name
		if(stricmp(s, "ADPISETNAME") == 0) {
			nCurSet++;
			//pdp set name
			p_file.getField(s,255);
			//s is name of pdpi set
			m_vSetNamesVec.push_back(s);
			m_vDefaultItemsVec.addItem(NULL);
			m_vOverlapItemsVec.addItem(NULL);
			ASSERT(nCurSet == m_vSetNamesVec.size()-1);
			ASSERT(nCurSet == m_vDefaultItemsVec.getCount()-1);
			ASSERT(nCurSet == m_vOverlapItemsVec.getCount()-1);
		}
		else {

			CAircraftTagItem* pItem = new CAircraftTagItem(s);
			if(pItem->GetFlightType()->GetName() == "DEFAULT" || pItem->GetFlightType()->GetName() == "OVERLAP") {
				p_file.skipField(10);
			}
			else {
				FlightConstraint flightCon;
				flightCon.SetAirportDB(m_pInTerm->m_pAirportDB);
				flightCon.readConstraint(p_file);
				pItem->GetFlightType()->SetFlightConstraint(flightCon);
				p_file.getInteger(n);
				pItem->GetFlightType()->SetIsFlightConstraintAdd((BOOL) n);

				p_file.getInteger(n);
				pItem->GetFlightType()->SetIsBirthTimeApplied((BOOL) n);
				if(pItem->GetFlightType()->GetIsBirthTimeApplied()) {
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetFlightType()->SetMinBirthTime(dt);
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetFlightType()->SetMaxBirthTime(dt);
					p_file.getInteger(n);
					pItem->GetFlightType()->SetIsBirthTimeAdd((BOOL) n);
				}
				else {
					p_file.skipField(3);
				}
				p_file.getInteger(n);
				pItem->GetFlightType()->SetIsDeathTimeApplied((BOOL) n);
				if(pItem->GetFlightType()->GetIsDeathTimeApplied()) {
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetFlightType()->SetMinDeathTime(dt);
					p_file.getField(s,255);
					sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
					dt.SetTime(nHour, nMin, nSec);
					pItem->GetFlightType()->SetMaxDeathTime(dt);
					p_file.getInteger(n);
					pItem->GetFlightType()->SetIsDeathTimeAdd((BOOL) n);
				}
				else {
					p_file.skipField(3);
				}
			}

			p_file.getInteger(n);
			pItem->SetShowTags((BOOL) n);

			p_file.getInteger(l);
			pItem->SetTagInfo((DWORD) l);
			
			if(pItem->GetFlightType()->GetName() == "DEFAULT") {
				m_vDefaultItemsVec[nCurSet] = pItem;
			}
			else if(pItem->GetFlightType()->GetName() == "OVERLAP") {
				m_vOverlapItemsVec[nCurSet] = pItem;
			}
			else {
				m_vACTagItemsVec[nCurSet].addItem(pItem);
			}

		}

		p_file.getLine();
	}
}

void CAircraftTags::writeData (ArctermFile& p_file) const
{
	//write PT set count
	int nSetCount = m_vSetNamesVec.size();
	p_file.writeInt(nSetCount);
	p_file.writeInt(m_nSetIdx);
	p_file.writeLine();

	CAircraftTagItem* pItem = NULL;

	for(int i=0; i<nSetCount; i++) {
		p_file.writeField("ADPISETNAME");
		p_file.writeField(m_vSetNamesVec[i]);
		p_file.writeLine();
		pItem = m_vDefaultItemsVec[i]; writeATI( pItem, p_file );
		pItem = m_vOverlapItemsVec[i]; writeATI( pItem, p_file );
		int nCount = m_vACTagItemsVec[i].getCount();
	    for( int j = 0; j < nCount; j++ ) {
			pItem = m_vACTagItemsVec[i][j];
			writeATI( pItem, p_file );
	    }
	}
}

void CAircraftTags::writeATI(CAircraftTagItem* pATI, ArctermFile& p_file ) const
{
	ASSERT(pATI);
	//write pax type
	CHAR s[2560];
	CFlightType* pAT = pATI->GetFlightType();
	p_file.writeField(pAT->GetName());

	if(pAT->GetName() == "DEFAULT" || pAT->GetName() == "OVERLAP") {
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
	}
	else {
		pAT->GetFlightConstraint().WriteSyntaxStringWithVersion(s);
		p_file.writeField(s);
		p_file.writeInt((int) pAT->GetIsFlightConstraintAdd());

		p_file.writeInt((int) pAT->GetIsBirthTimeApplied());
		if(pAT->GetIsBirthTimeApplied()) {
			CString sTime = pAT->GetMinBirthTime().Format("%H:%M:%S");
			p_file.writeField(sTime);
			sTime = pAT->GetMaxBirthTime().Format("%H:%M:%S");
			p_file.writeField(sTime);
			p_file.writeInt((int) pAT->GetIsBirthTimeAdd());
		}
		else {
			p_file.writeBlankField();
			p_file.writeBlankField();
			p_file.writeBlankField();
		}
		
		p_file.writeInt((int) pAT->GetIsDeathTimeApplied());
		if(pAT->GetIsDeathTimeApplied()) {
			CString sTime = pAT->GetMinDeathTime().Format("%H:%M:%S");
			p_file.writeField(sTime);
			sTime = pAT->GetMaxDeathTime().Format("%H:%M:%S");
			p_file.writeField(sTime);
			p_file.writeInt((int) pAT->GetIsDeathTimeAdd());
		}
		else {
			p_file.writeBlankField();
			p_file.writeBlankField(); 
			p_file.writeBlankField(); 
		}
	}

	//write showTags
	p_file.writeInt((int) pATI->IsShowTags());

	DWORD dwInfo = pATI->GetTagInfo();
	//write tag info
	p_file.writeInt((long) dwInfo);

	p_file.writeLine();
}

// Description: Read Data From Default File.
// Exception:	FileVersionTooNewError
void CAircraftTags::loadInputs( const CString& _csProjPath, InputTerminal* _pInTerm )
{
	SetInputTerminal( _pInTerm );
	loadDataSet( _csProjPath );
}

void CAircraftTags::saveInputs( const CString& _csProjPath, bool _bUndo )
{
	saveDataSet(_csProjPath, _bUndo);
}

int CAircraftTags::AddSet(const CString& _s, CAircraftTagItem* _pDefaultItem, CAircraftTagItem* _pOverlapItem)
{
	m_vSetNamesVec.push_back(_s);
	m_vDefaultItemsVec.addItem(_pDefaultItem);
	m_vOverlapItemsVec.addItem(_pOverlapItem);

	ASSERT(m_vSetNamesVec.size() == m_vDefaultItemsVec.getCount());
	ASSERT(m_vSetNamesVec.size() == m_vOverlapItemsVec.getCount());

	m_vACTagItemsVec.resize(m_vACTagItemsVec.size()+1);

	ASSERT(m_vSetNamesVec.size() == m_vACTagItemsVec.size());

	return m_vSetNamesVec.size()-1;
}

void CAircraftTags::DeletSet(int _idx)
{
	m_vSetNamesVec.erase(m_vSetNamesVec.begin()+_idx);
	delete m_vDefaultItemsVec[_idx];
	m_vDefaultItemsVec.deleteItem(_idx);
	delete m_vOverlapItemsVec[_idx];
	m_vOverlapItemsVec.deleteItem(_idx);

	int nCount = m_vACTagItemsVec[_idx].getCount();
	for(int i=0; i<nCount; i++) {
		delete m_vACTagItemsVec[_idx][i];
	}
	m_vACTagItemsVec[_idx].clear();
	m_vACTagItemsVec.erase(m_vACTagItemsVec.begin()+_idx);

	ASSERT(m_vSetNamesVec.size() == m_vDefaultItemsVec.getCount());
	ASSERT(m_vSetNamesVec.size() == m_vOverlapItemsVec.getCount());
	ASSERT(m_vSetNamesVec.size() == m_vACTagItemsVec.size());

	if(m_nSetIdx >= static_cast<int>(m_vSetNamesVec.size()))
		m_nSetIdx--;
	ASSERT(m_nSetIdx >= 0);
}

const CString& CAircraftTags::GetSetName(int _idx) const
{
	ASSERT(_idx >= 0);
	ASSERT(_idx < static_cast<int>(m_vSetNamesVec.size()));
	return m_vSetNamesVec[_idx];
}

void CAircraftTags::RenameCurrentSet(const CString& _s)
{
	m_vSetNamesVec[m_nSetIdx] = _s;
}

void CAircraftTags::SetCurrentSet(int _idx)
{
	ASSERT(_idx >= 0);
	ASSERT(_idx < static_cast<int>(m_vSetNamesVec.size()));
	ASSERT(_idx < static_cast<int>(m_vACTagItemsVec.size()));
	ASSERT(_idx < m_vDefaultItemsVec.getCount());
	ASSERT(_idx < m_vOverlapItemsVec.getCount());

	
	m_nSetIdx = _idx;
}
int CAircraftTags::FindBestMatch(const AirsideFlightDescStruct& fds,  bool bARR,Terminal* pTerminal)
{
	
	for(int j=0; j<GetCount(); j++) 
	{
		CAircraftTagItem* pATI = GetTagItem(j);
		if(pATI->GetFlightType()->Matches(fds, bARR,pTerminal))
			return j;
	}
	return -1;
		
}

/////////////////////////////////////
//// CAircraftTagItem ///////////////////
////////////////////////////////////

CAircraftTagItem::CAircraftTagItem(const CString& _csName)
{
	m_flightType.SetName( _csName );
	m_bShowTags = FALSE;
	m_dwTagInfo = ACTAG_NONE;
}

CAircraftTagItem::~CAircraftTagItem()
{
}

