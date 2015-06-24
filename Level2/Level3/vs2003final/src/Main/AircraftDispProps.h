// PaxDispProps.h: interface for the CPaxDispProps class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AIRCRAFTDISPPROPS_H__
#define __AIRCRAFTDISPPROPS_H__

#pragma once


#include "AircraftDispPropItem.h"

#include "common\AIRSIDE_BIN.h"
#include "common\containr.h"
#include "common\dataset.h"

#include <vector>
//switches of individual aircraft display
struct AircraftDisplaySwitch
{
	bool m_bUseGrpSet;
	bool m_bTagOn;
	bool m_bLogoOn;
	bool m_bTrackOn;
	AircraftDisplaySwitch():m_bUseGrpSet(true),m_bTagOn(false),m_bLogoOn(false),m_bTrackOn(false){}
};

typedef SortedContainer<CAircraftDispPropItem> CAircraftDispPropItemVector;

class CAircraftDispProps	: public DataSet
{
public:
	//bARR = arr, !bARR = dep
	int FindBestMatch(const AirsideFlightDescStruct& fds,bool bARR, Terminal* pTerminal);
	void DeleteItem( int _nIdx );	
	void DeleteAndReplaceItem( int _iIdx, CAircraftDispPropItem* _pPaxDisp);
	int GetCount();
	CAircraftDispPropItem* GetDispPropItem( int _nIdx );
	void AddDispPropItem( CAircraftDispPropItem* _pItem ){ m_vItemsVec[m_nSetIdx].addItem( _pItem ); }
	CAircraftDispPropItem* GetOverLapDispProp();
	CAircraftDispPropItem* GetDefaultDispProp();


	int AddSet(const CString& _s, CAircraftDispPropItem* _pDefaultItem, CAircraftDispPropItem* _pOverlapItem);
	void DeletSet(int _idx);
	const CString& GetSetName(int _idx) const;

	void RenameCurrentSet(const CString& _s);
	int GetCurrentSetIdx() const { return m_nSetIdx; }
	void SetCurrentSet(int _idx);
	int GetSetCount() const { return m_vSetNamesVec.size(); }

	CAircraftDispProps();
	virtual ~CAircraftDispProps();
	virtual void clear();
	virtual void initDefaultValues();

	virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Aircraft Display Data"; }
    virtual const char *getHeaders (void) const{  return ".,..,..,,,"; }

	void loadInputs( const CString& _csProjPath, InputTerminal* _pInTerm );
	void saveInputs( const CString& _csProjPath, bool _bUndo );		

protected:
	void writeADPI(CAircraftDispPropItem* pADPI, ArctermFile& p_file ) const;
	CAircraftDispPropItemVector m_vOverlapItemsVec;
	CAircraftDispPropItemVector m_vDefaultItemsVec;
	std::vector<CAircraftDispPropItemVector> m_vItemsVec;
	std::vector<CString> m_vSetNamesVec;

	int m_nSetIdx;
};

#endif // __AIRCRAFTDISPPROPS_H__
