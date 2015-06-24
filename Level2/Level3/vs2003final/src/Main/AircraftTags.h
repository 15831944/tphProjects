// PaxTags.h: interface for the CAircraftTags class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AIRCRAFTTAGS_H__
#define __AIRCRAFTTAGS_H__

#pragma once

#include "common\dataset.h"
#include "common\containr.h"
#include <vector>
#include "FlightType.h"

//#define ACTAG_NONE			0x00000000
//#define ACTAG_FLIGHTID		0x00000001	// flight id
//#define ACTAG_TYPE			0x00000002	// ac type
//#define ACTAG_ORIGDEST		0x00000004  // origin or destination
//#define ACTAG_TIME			0x00000008  // scheduled arrival or departure time
//#define ACTAG_GATE			0x00000010	// arr or dep gate
//#define ACTAG_RUNWAY		0x00000020	// arr or dep runway
//#define ACTAG_SIDSTAR		0x00000040	// star or sid
//#define ACTAG_SPEED			0x00000080	// speed
//#define ACTAG_HEADING		0x00000100  // heading
//#define ACTAG_ALTITUDE		0x00000200  // altitude
//#define ACTAG_DELAY			0x00000400  // landing or takeoff delay

#define ACTAG_NONE			0x00000000
#define ACTAG_FLIGHTID		0x00000001	// flight id
#define ACTAG_OPERATEMODE   0x00000002  // operational mode
#define ACTAG_SPEED         0x00000004  // speed
#define ACTAG_GATE          0x00000008  // gate
#define ACTAG_RUNWAY        0x00000010  // runway
#define ACTAG_SIDSTAR       0x00000020  // sid/star
#define ACTAG_ORIGDEST      0x00000040  // origin/dest
#define ACTAG_ACTYPE        0x00000080  // AC type
#define ACTAG_ALTITUDEC     0x00000100  // altitude
#define ACTAG_STATE         0x00000200  // state

class CAircraftTagItem
{
public:
	CAircraftTagItem(const CString& _csName);
	virtual ~CAircraftTagItem();

	CFlightType* GetFlightType(){ return &m_flightType; }
	BOOL IsShowTags() const { return m_bShowTags; }
	DWORD GetTagInfo() const { return m_dwTagInfo; }

	void SetFlightType( const CFlightType& _flightType ) { m_flightType = _flightType; }
	void SetShowTags( BOOL _bShowTags ) { m_bShowTags = _bShowTags; }
	void SetTagInfo( DWORD _dwTagInfo ) { m_dwTagInfo = _dwTagInfo; }
    int operator < (const CAircraftTagItem& Item) const {  if(this!=NULL) return m_flightType.GetFlightConstraint() < Item.m_flightType.GetFlightConstraint(); else return false; }
protected:

	CFlightType m_flightType;
	BOOL m_bShowTags;
	DWORD m_dwTagInfo; //bit mask to determine what info to show
};


typedef SortedContainer<CAircraftTagItem> CAircraftTagItemVector;

class CAircraftTags : public DataSet  
{
public:
	CAircraftTags();
	virtual ~CAircraftTags();
    
	void DeleteItem( int _nIdx );	
	int GetCount();
	CAircraftTagItem* GetTagItem( int _nIdx );
	void AddTagItem( CAircraftTagItem* _pItem ){ m_vACTagItemsVec[m_nSetIdx].addItem( _pItem ); }
	CAircraftTagItem* GetOverLapTags() { return m_vOverlapItemsVec[m_nSetIdx]; }
	CAircraftTagItem* GetDefaultTags() { return m_vDefaultItemsVec[m_nSetIdx]; }

	int AddSet(const CString& _s, CAircraftTagItem* _pDefaultItem, CAircraftTagItem* _pOverlapItem);
	void DeletSet(int _idx);
	const CString& GetSetName(int _idx) const;

	void RenameCurrentSet(const CString& _s);
	int GetCurrentSetIdx() const { return m_nSetIdx; }
	void SetCurrentSet(int _idx);
	int GetSetCount() const { return m_vSetNamesVec.size(); }
    int FindBestMatch(const AirsideFlightDescStruct& fds, bool bARR, Terminal* pTerminal);

	virtual void clear();
	virtual void initDefaultValues();

	virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Aircraft Tags Data"; }
    virtual const char *getHeaders (void) const{  return ".,..,..,,,"; }

	// Description: Read Data From Default File.
	// Exception:	FileVersionTooNewError
	void loadInputs( const CString& _csProjPath, InputTerminal* _pInTerm );

	void saveInputs( const CString& _csProjPath, bool _bUndo );

protected:
	void writeATI(CAircraftTagItem* pATI, ArctermFile& p_file ) const;
	CAircraftTagItemVector m_vOverlapItemsVec;
	CAircraftTagItemVector m_vDefaultItemsVec;
	std::vector<CAircraftTagItemVector> m_vACTagItemsVec;
	std::vector<CString> m_vSetNamesVec;

	int m_nSetIdx;

};

#endif // __AIRCRAFTTAGS_H__
