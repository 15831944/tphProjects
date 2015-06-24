// PaxTags.h: interface for the CPaxTags class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAXTAGS_H__1E0D2424_0C99_4209_8E8E_DD94BFBB64E9__INCLUDED_)
#define AFX_PAXTAGS_H__1E0D2424_0C99_4209_8E8E_DD94BFBB64E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\dataset.h"
#include "common\containr.h"
#include <vector>
#include "PaxType.h"

#define PAX_NONE		0x00000000
#define PAX_ID			0x00000001
#define PAX_FLIGHTID	0x00000002
#define PAX_PAXTYPE		0x00000004
#define PAX_GATE		0x00000008
#define PAX_GATETIME	0x00000010
#define PAX_BAGCOUNT	0x00000020
#define PAX_CARTCOUNT	0x00000040
#define PAX_STATE		0x00000080
#define PAX_PROCESSOR	0x00000100
#define PAX_WALKINGSPEED	0x00000200

class CPaxTagItem
{
public:
	CPaxTagItem(CString _csName);
	virtual ~CPaxTagItem();

	CPaxType* GetPaxType(){ return &m_paxType; }
	BOOL IsShowTags(){ return m_bShowTags; }
	DWORD GetTagInfo() { return m_dwTagInfo; }

	void SetPaxType( CPaxType _paxType ) { m_paxType = _paxType; }
	void SetShowTags( BOOL _bShowTags ) { m_bShowTags = _bShowTags; }
	void SetTagInfo( DWORD _dwTagInfo ) { m_dwTagInfo = _dwTagInfo; }
    int operator < (CPaxTagItem& Item)  {  if(this!=NULL) return m_paxType.GetPaxCon() < Item.GetPaxType()->GetPaxCon(); else return false; }
protected:
	CPaxType m_paxType;
	BOOL m_bShowTags;
	DWORD m_dwTagInfo; //bit mask to determine what info to show
};


//typedef std::vector<CPaxTagItem*> CPaxTagItemPtrVector;
typedef SortedContainer<CPaxTagItem> CPaxTagItemPtrVector;
class CPaxTags : public DataSet  
{
public:
	CPaxTags();
	virtual ~CPaxTags();
    
	void DeleteItem( int _nIdx );	
	int GetCount();
	CPaxTagItem* GetPaxTagItem( int _nIdx );
	void AddPaxTagItem( CPaxTagItem* _pItem ){ m_vPaxTagItemsVec[m_nPTSetIdx].addItem( _pItem ); }
	CPaxTagItem* GetOverLapPaxTags() { return m_vOverlapItemsVec[m_nPTSetIdx]; }
	CPaxTagItem* GetDefaultPaxTags() { return m_vDefaultItemsVec[m_nPTSetIdx]; }

	int AddPTSet(const CString& _s, CPaxTagItem* _pDefaultItem, CPaxTagItem* _pOverlapItem);
	void DeletPTSet(int _idx);
	const CString& GetPTSetName(int _idx) const;

	void RenameCurrentPTSet(const CString& _s);
	int GetCurrentPTSetIdx() const { return m_nPTSetIdx; }
	void SetCurrentPTSet(int _idx);
	int GetPTSetCount() const { return m_vPTSetNamesVec.size(); }
    int FindBestMatch(const MobDescStruct& pds, Terminal* pTerminal);

	virtual void clear();
	virtual void initDefaultValues();

	virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Mobile Element Tags Data"; }
    virtual const char *getHeaders (void) const{  return ".,..,..,,,"; }

	// Description: Read Data From Default File.
	// Exception:	FileVersionTooNewError
	void loadInputs( const CString& _csProjPath, InputTerminal* _pInTerm );

	void saveInputs( const CString& _csProjPath, bool _bUndo );

protected:
	void writePTI(CPaxTagItem* pPTI, ArctermFile& p_file ) const;
	CPaxTagItemPtrVector m_vOverlapItemsVec;
	CPaxTagItemPtrVector m_vDefaultItemsVec;
	std::vector<CPaxTagItemPtrVector> m_vPaxTagItemsVec;
	std::vector<CString> m_vPTSetNamesVec;

	int m_nPTSetIdx;

};

#endif // !defined(AFX_PAXTAGS_H__1E0D2424_0C99_4209_8E8E_DD94BFBB64E9__INCLUDED_)
