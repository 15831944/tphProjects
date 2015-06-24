// PaxDispProps.cpp: implementation of the CPaxDispProps class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AircraftDispProps.h"
#include "assert.h"
#include "../Inputs/IN_TERM.H"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAircraftDispProps::CAircraftDispProps() : DataSet( AircraftDisplayPropertiesFile )
{
	m_vItemsVec.resize(1);
	m_vSetNamesVec.reserve(1);
	m_vSetNamesVec.push_back("DEFAULT");
	m_vOverlapItemsVec.push_back(new CAircraftDispPropItem("OVERLAP"));
	m_vDefaultItemsVec.push_back(new CAircraftDispPropItem("DEFAULT"));
	m_nSetIdx = 0;
	m_fVersion = 2.3f;
}

CAircraftDispProps::~CAircraftDispProps()
{
	clear();
}

void CAircraftDispProps::clear()
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
	
	nCount = m_vItemsVec.size();
	for( i=0; i<nCount; i++ ) {
		int nCount2 = m_vItemsVec[i].size();
		for(int j=0; j<nCount2; j++) {
			delete m_vItemsVec[i][j];
		}
		m_vItemsVec[i].clear();
	}
	m_vItemsVec.clear();
}

CAircraftDispPropItem* CAircraftDispProps::GetDefaultDispProp()
{
	return m_vDefaultItemsVec[m_nSetIdx];
}

CAircraftDispPropItem* CAircraftDispProps::GetOverLapDispProp()
{
	return m_vOverlapItemsVec[m_nSetIdx];
}

int CAircraftDispProps::GetCount()
{
	return m_vItemsVec[m_nSetIdx].getCount();
}

CAircraftDispPropItem* CAircraftDispProps::GetDispPropItem( int _nIdx )
{
	CAircraftDispPropItemVector& vec = m_vItemsVec[m_nSetIdx];
	

	if(_nIdx>=0 && _nIdx < (int)vec.size() )
		return vec[_nIdx];
	else if(_nIdx==-1)
		return m_vDefaultItemsVec[m_nSetIdx];
	else if(_nIdx==-2)
		return m_vOverlapItemsVec[m_nSetIdx];
	else 
		return m_vDefaultItemsVec[m_nSetIdx];

	return NULL;
}

void CAircraftDispProps::DeleteItem(int _nIdx)
{
	m_vItemsVec[m_nSetIdx].deleteItem( _nIdx );
}

void CAircraftDispProps::DeleteAndReplaceItem( int _iIdx, CAircraftDispPropItem* _pPaxDisp)
{
	assert( _iIdx>= 0 && _iIdx < m_vItemsVec[m_nSetIdx].getCount() );
	delete m_vItemsVec[m_nSetIdx][_iIdx];
	m_vItemsVec[m_nSetIdx][_iIdx] = _pPaxDisp;
	
}
void CAircraftDispProps::initDefaultValues()
{
	m_vItemsVec.resize(1);
	m_vSetNamesVec.reserve(1);
	m_vSetNamesVec.push_back("DEFAULT");
	m_vOverlapItemsVec.addItem(new CAircraftDispPropItem("OVERLAP"));
	m_vDefaultItemsVec.addItem(new CAircraftDispPropItem("DEFAULT"));
	m_nSetIdx = 0;
}

// Description: Read Data From Default File.
// Exception:	FileVersionTooNewError
void CAircraftDispProps::loadInputs( const CString& _csProjPath, InputTerminal* _pInTerm )
{
	this->SetInputTerminal( _pInTerm );
	this->loadDataSet( _csProjPath );
}

void CAircraftDispProps::saveInputs( const CString& _csProjPath, bool _bUndo )
{
	this->saveDataSet(_csProjPath, _bUndo);
}

void CAircraftDispProps::readObsoleteData(ArctermFile& p_file)
{
	//ASSERT(m_vItemsVec.size()==0);
	//ASSERT(m_vSetNamesVec.size()==0);
	//ASSERT(m_vDefaultItemsVec.getCount()==0);
	//ASSERT(m_vOverlapItemsVec.getCount()==0);

	//p_file.getLine();

	//int nSetCount;
	//p_file.getInteger(nSetCount);
	//p_file.getInteger(m_nSetIdx);
	//ASSERT(m_nSetIdx<nSetCount);

	//m_vItemsVec.resize(nSetCount);
	//m_vSetNamesVec.reserve(nSetCount);

	//int nCurSet = -1;

	//p_file.getLine();

	//while (!p_file.isBlank ())
	//{
	//	CString sz;
	//	char s[256];
	//	//int n, m;
	//	//long l;
	//	//double d;
	//	//COleDateTime dt;
	//	//int nHour, nMin, nSec;

	//	p_file.getField(s,255); //adpi name or adp set name
	//	if(stricmp(s, "ADPISETNAME") == 0) {
	//		nCurSet++;
	//		//adp set name
	//		p_file.getField(s,255);
	//		//s is name of adpi set
	//		m_vSetNamesVec.push_back(s);
	//		m_vDefaultItemsVec.addItem(NULL);
	//		m_vOverlapItemsVec.addItem(NULL);
	//		ASSERT(nCurSet == m_vSetNamesVec.size()-1);
	//		ASSERT(nCurSet == m_vDefaultItemsVec.getCount()-1);
	//		ASSERT(nCurSet == m_vOverlapItemsVec.getCount()-1);
	//	}
	//	else {
	//		
	//	}

	//	p_file.getLine();
	//}


	ASSERT(m_vItemsVec.size()==0);
	ASSERT(m_vSetNamesVec.size()==0);
	ASSERT(m_vDefaultItemsVec.getCount()==0);
	ASSERT(m_vOverlapItemsVec.getCount()==0);

	p_file.getLine();

	int nSetCount;
	p_file.getInteger(nSetCount);
	p_file.getInteger(m_nSetIdx);
	ASSERT(m_nSetIdx<nSetCount);

	m_vItemsVec.resize(nSetCount);
	m_vSetNamesVec.reserve(nSetCount);
	//	m_vDefaultItemsVec.reserve(nSetCount);
	//	m_vOverlapItemsVec.reserve(nSetCount);
	int nCurSet = -1;

	p_file.getLine();

	while (!p_file.isBlank ())
	{
		CString sz;
		char s[256];
		int n, m;
		long l;
		//double d;
		//float f;

		COleDateTime dt;
		int nHour, nMin, nSec;

		p_file.getField(s,255); //adpi name or adp set name
		if(_stricmp(s, "ADPISETNAME") == 0) {
			nCurSet++;
			//adp set name
			p_file.getField(s,255);
			//s is name of adpi set
			m_vSetNamesVec.push_back(s);
			m_vDefaultItemsVec.addItem(NULL);
			m_vOverlapItemsVec.addItem(NULL);
			ASSERT(nCurSet == m_vSetNamesVec.size()-1);
			ASSERT(nCurSet == m_vDefaultItemsVec.getCount()-1);
			ASSERT(nCurSet == m_vOverlapItemsVec.getCount()-1);
		}
		else {

			CAircraftDispPropItem* pItem = new CAircraftDispPropItem(s);
			if(pItem->GetFlightType()->GetName() == "DEFAULT" || pItem->GetFlightType()->GetName() == "OVERLAP") {
				p_file.skipField(17);
			}
			else {
				//p_file.getField(s,255);
				FlightConstraint flightCon;
				flightCon.SetAirportDB(m_pInTerm->m_pAirportDB);
				//paxCon.setConstraint(s);
				flightCon.readConstraint(p_file);
				pItem->GetFlightType()->SetFlightConstraint(flightCon);
				p_file.getInteger(n);
				pItem->GetFlightType()->SetIsFlightConstraintAdd((BOOL) n);
//				p_file.getInteger(n);
/*				pItem->GetFlightType()->SetAreaportalApplyType((AREA_PORTAL_APPLY_TYPE) n);
				if(pItem->GetFlightType()->GetAreaportalApplyType() != ENUM_APPLY_NONE) {
					if(p_file.isBlankField()) 
						p_file.skipField(1);
					else {
						p_file.getField(s,255);
						pItem->GetFlightType()->GetArea().name = s;
					}
					if(p_file.isBlankField()) 
						p_file.skipField(1);
					else {
						p_file.getField(s,255);
						pItem->GetFlightType()->GetPortal().name = s;
					}
					p_file.getInteger(n);
					pItem->GetFlightType()->SetIsTimeApplied((BOOL) n);
					if(pItem->GetFlightType()->GetIsTimeApplied()) {
						p_file.getField(s,255);
						sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
						dt.SetTime(nHour, nMin, nSec);
						pItem->GetFlightType()->SetStartTime(dt);
						p_file.getField(s,255);
						sscanf(s, "%d:%d:%d", &nHour, &nMin, &nSec);
						dt.SetTime(nHour, nMin, nSec);
						pItem->GetFlightType()->SetEndTime(dt);
					}
					else {
						p_file.skipField(2);
					}
					p_file.getInteger(n);
					pItem->GetFlightType()->SetIsAreaPortalAdd((BOOL) n);
				}
				else {
					p_file.skipField(5);
				}
*/
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
			p_file.getInteger(m);
			pItem->SetShape( std::pair<int, int>(n, m) );

			p_file.getInteger(n);
			pItem->SetLineType((CAircraftDispPropItem::ENUM_LINE_TYPE) n);

			p_file.getInteger(l);
			pItem->SetColor((COLORREF) l);

			p_file.getInteger(n);
			pItem->SetLeaveTrail((BOOL) n);

			p_file.getInteger(n);
			pItem->SetVisible((BOOL) n);

//			p_file.getFloat(d);
//			pItem->SetDensity(d);
		
			

			if(pItem->GetFlightType()->GetName() == "DEFAULT") {
				m_vDefaultItemsVec[nCurSet] = pItem;
			}
			else if(pItem->GetFlightType()->GetName() == "OVERLAP") {
				m_vOverlapItemsVec[nCurSet] = pItem;
			}
			else {
				m_vItemsVec[nCurSet].addItem(pItem);
			}
		}

		p_file.getLine();
	}
}

void CAircraftDispProps::readData (ArctermFile& p_file)
{
	//ASSERT(FALSE);

	ASSERT(m_vItemsVec.size()==0);
	ASSERT(m_vSetNamesVec.size()==0);
	ASSERT(m_vDefaultItemsVec.getCount()==0);
	ASSERT(m_vOverlapItemsVec.getCount()==0);

	p_file.getLine();

	int nSetCount;
	p_file.getInteger(nSetCount);
	p_file.getInteger(m_nSetIdx);
	ASSERT(m_nSetIdx<nSetCount);

	m_vItemsVec.resize(nSetCount);
	m_vSetNamesVec.reserve(nSetCount);
	
	int nCurSet = -1;

	p_file.getLine();

	while (!p_file.isBlank ())
	{
		CString sz;
		char s[256];
		int n;
		long l;
		//double d;
		float f;

		COleDateTime dt;
		int nHour, nMin, nSec;

		p_file.getField(s,255); //adpi name or adp set name
		if(_stricmp(s, "ADPISETNAME") == 0) {
			nCurSet++;
			//adp set name
			p_file.getField(s,255);
			//s is name of adpi set
			m_vSetNamesVec.push_back(s);
			m_vDefaultItemsVec.addItem(NULL);
			m_vOverlapItemsVec.addItem(NULL);
			ASSERT(nCurSet == m_vSetNamesVec.size()-1);
			ASSERT(nCurSet == m_vDefaultItemsVec.getCount()-1);
			ASSERT(nCurSet == m_vOverlapItemsVec.getCount()-1);
		}
		else {

			CAircraftDispPropItem* pItem = new CAircraftDispPropItem(s);
			if(pItem->GetFlightType()->GetName() == "DEFAULT" || pItem->GetFlightType()->GetName() == "OVERLAP") {
				p_file.skipField(17);
			}
			else {
				//p_file.getField(s,255);
				FlightConstraint flightCon;
				flightCon.SetAirportDB(m_pInTerm->m_pAirportDB);
				//paxCon.setConstraint(s);
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
			
		
			{
				p_file.getInteger(l);
				pItem->SetColor((COLORREF) l);

				p_file.getInteger(n);
				pItem->SetLogoOn((BOOL)n);

				p_file.getInteger(n);
				pItem->SetLineType((CAircraftDispPropItem::ENUM_LINE_TYPE) n);

				p_file.getInteger(n);
				pItem->SetVisible((BOOL) n);

				p_file.getFloat(f);
				pItem->SetDensity(f);

				p_file.getFloat(f);
				pItem->SetScaleSize(f);
			}


			if(pItem->GetFlightType()->GetName() == "DEFAULT") {
				m_vDefaultItemsVec[nCurSet] = pItem;
			}
			else if(pItem->GetFlightType()->GetName() == "OVERLAP") {
				m_vOverlapItemsVec[nCurSet] = pItem;
			}
			else {
				m_vItemsVec[nCurSet].addItem(pItem);
			}
		}

		p_file.getLine();
	}
}


void CAircraftDispProps::writeData (ArctermFile& p_file) const
{
	//write PDP set count
	int nSetCount = m_vSetNamesVec.size();
	p_file.writeInt(nSetCount);
	p_file.writeInt(m_nSetIdx);
	p_file.writeLine();

	CAircraftDispPropItem* pItem = NULL;

	for(int i=0; i<nSetCount; i++) {
		p_file.writeField("ADPISETNAME");
		p_file.writeField(m_vSetNamesVec[i]);
		p_file.writeLine();
		pItem = m_vDefaultItemsVec[i]; writeADPI( pItem, p_file );
		pItem = m_vOverlapItemsVec[i]; writeADPI( pItem, p_file );
		int nCount = m_vItemsVec[i].size();
	    for( int j = 0; j < nCount; j++ ) {
			pItem = m_vItemsVec[i][j];
			writeADPI( pItem, p_file );
	    }
	}
}

void CAircraftDispProps::writeADPI(CAircraftDispPropItem* pADPI, ArctermFile& p_file) const
{
	ASSERT(pADPI);
	//write aircraft type
	
	char s[2560];
	CFlightType* pFT = pADPI->GetFlightType();
	p_file.writeField(pFT->GetName());

	if(pFT->GetName() == "DEFAULT" || pFT->GetName() == "OVERLAP") {
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
		pFT->GetFlightConstraint().WriteSyntaxStringWithVersion(s);
		p_file.writeField(s);
		p_file.writeInt((int) pFT->GetIsFlightConstraintAdd());

/*		p_file.writeInt((int) pFT->GetAreaportalApplyType());
		if(pFT->GetAreaportalApplyType() != ENUM_APPLY_NONE) {
			if(pFT->GetArea().name.IsEmpty())
				p_file.writeBlankField();
			else
				p_file.writeField(pFT->GetArea().name);
			if(pFT->GetPortal().name.IsEmpty())
				p_file.writeBlankField();
			else
				p_file.writeField(pFT->GetPortal().name);
			p_file.writeInt((int)pFT->GetIsTimeApplied()); //area/portal time
			if(pFT->GetIsTimeApplied()) {
				CString sTime = pFT->GetStartTime().Format("%H:%M:%S");
				p_file.writeField(sTime);
				sTime = pFT->GetEndTime().Format("%H:%M:%S");
				p_file.writeField(sTime);
			}
			else {
				p_file.writeBlankField();
				p_file.writeBlankField(); 
			}
			p_file.writeInt((int) pFT->GetIsAreaPortalAdd());
		}
		else {
			p_file.writeBlankField();
			p_file.writeBlankField();
			p_file.writeBlankField();
			p_file.writeBlankField();
			p_file.writeBlankField();
		}
*/
		p_file.writeInt((int) pFT->GetIsBirthTimeApplied());
		if(pFT->GetIsBirthTimeApplied()) {
			CString sTime = pFT->GetMinBirthTime().Format("%H:%M:%S");
			p_file.writeField(sTime);
			sTime = pFT->GetMaxBirthTime().Format("%H:%M:%S");
			p_file.writeField(sTime);
			p_file.writeInt((int) pFT->GetIsBirthTimeAdd());
		}
		else {
			p_file.writeBlankField();
			p_file.writeBlankField();
			p_file.writeBlankField();
		}
		
		p_file.writeInt((int) pFT->GetIsDeathTimeApplied());
		if(pFT->GetIsDeathTimeApplied()) {
			CString sTime = pFT->GetMinDeathTime().Format("%H:%M:%S");
			p_file.writeField(sTime);
			sTime = pFT->GetMaxDeathTime().Format("%H:%M:%S");
			p_file.writeField(sTime);
			p_file.writeInt((int) pFT->GetIsDeathTimeAdd());
		}
		else {
			p_file.writeBlankField();
			p_file.writeBlankField(); 
			p_file.writeBlankField(); 
		}
	}

	//write shape
	//p_file.writeInt((int) pADPI->GetShape().first);
	//p_file.writeInt((int) pADPI->GetShape().second);
	
	//
	//write color
	p_file.writeInt((long) pADPI->GetColor());
	//write logo on
	p_file.writeInt((int) pADPI->IsLogoOn());
	//write line type
	p_file.writeInt((int) pADPI->GetLineType());	
	//write isLeaveTrail
	//p_file.writeInt((int) pADPI->IsLeaveTrail());
	//write isVisible
	p_file.writeInt((int) pADPI->IsVisible());
	//write density
	p_file.writeFloat(static_cast<float>(pADPI->GetDensity()));
	//write scale size
	p_file.writeFloat(pADPI->GetScaleSize());

	p_file.writeLine();
}


int CAircraftDispProps::AddSet(const CString& _s, CAircraftDispPropItem* _pDefaultItem, CAircraftDispPropItem* _pOverlapItem)
{
	m_vSetNamesVec.push_back(_s);
	m_vDefaultItemsVec.addItem(_pDefaultItem);
	m_vOverlapItemsVec.addItem(_pOverlapItem);

	ASSERT(m_vSetNamesVec.size() == m_vDefaultItemsVec.getCount());
	ASSERT(m_vSetNamesVec.size() == m_vOverlapItemsVec.getCount());

	m_vItemsVec.resize(m_vItemsVec.size()+1);

	ASSERT(m_vSetNamesVec.size() == m_vItemsVec.size());

	return m_vSetNamesVec.size()-1;
}

void CAircraftDispProps::DeletSet(int _idx)
{
	m_vSetNamesVec.erase(m_vSetNamesVec.begin()+_idx);
	delete m_vDefaultItemsVec[_idx];
	m_vDefaultItemsVec.removeItem(_idx);
	delete m_vOverlapItemsVec[_idx];
    m_vOverlapItemsVec.removeItem(_idx);
	int nCount = m_vItemsVec[_idx].getCount();
	for(int i=0; i<nCount; i++) {
		delete m_vItemsVec[_idx][i];
	}
	m_vItemsVec[_idx].clear();
	m_vItemsVec.erase(m_vItemsVec.begin()+_idx);

	ASSERT(m_vSetNamesVec.size() == m_vDefaultItemsVec.getCount());
	ASSERT(m_vSetNamesVec.size() == m_vOverlapItemsVec.getCount());
	ASSERT(m_vSetNamesVec.size() == m_vItemsVec.size());

	if(m_nSetIdx >= static_cast<int>(m_vSetNamesVec.size()))
		m_nSetIdx--;
	ASSERT(m_nSetIdx >= 0);
}

const CString& CAircraftDispProps::GetSetName(int _idx) const
{
	ASSERT(_idx >= 0);
	ASSERT(_idx < static_cast<int>(m_vSetNamesVec.size()));
	return m_vSetNamesVec[_idx];
}

void CAircraftDispProps::RenameCurrentSet(const CString& _s)
{
	m_vSetNamesVec[m_nSetIdx] = _s;
}

void CAircraftDispProps::SetCurrentSet(int _idx)
{
	ASSERT(_idx >= 0);
	ASSERT(_idx < static_cast<int>(m_vSetNamesVec.size()));
	ASSERT(_idx < static_cast<int>(m_vItemsVec.size()));
	ASSERT(_idx < m_vDefaultItemsVec.getCount());
	ASSERT(_idx < m_vOverlapItemsVec.getCount());

	m_nSetIdx = _idx;
}

// find Best Match Type from the sortedContainer
int CAircraftDispProps::FindBestMatch(const AirsideFlightDescStruct& fds, bool bARR,Terminal* pTerminal)
{
	for(int j = 0; j < GetCount(); j++) 
	{
		CAircraftDispPropItem* pADPI = GetDispPropItem(j);

		//TODO: see if this flight constraint matches the AirsideFlightDescStruct
		if(pADPI->GetFlightType()->Matches(fds,bARR, pTerminal)) 
			return j;
	}
	
	return -1;
}