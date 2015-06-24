// PaxDispProps.h: interface for the CPaxDispProps class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAXDISPPROPS_H__5C4B9A5C_2AC0_4636_AD9F_D9C5C88D510C__INCLUDED_)
#define AFX_PAXDISPPROPS_H__5C4B9A5C_2AC0_4636_AD9F_D9C5C88D510C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\dataset.h"
#include "common\containr.h"
#include "PaxDispPropItem.h"
#include <vector>


//typedef std::vector<CPaxDispPropItem*> CPaxDispPropItemPtrVector;
typedef SortedContainer<CPaxDispPropItem> CPaxDispPropItemPtrVector;
class CPaxDispProps	: public DataSet
{
public:
	int FindBestMatch(const MobDescStruct& pds, Terminal* pTerminal);
	void DeleteItem( int _nIdx );	
	void DeleteAndReplaceItem( int _iIdx, CPaxDispPropItem* _pPaxDisp);
	int GetCount();
	CPaxDispPropItem* GetDispPropItem( int _nIdx );
	void AddDispPropItem( CPaxDispPropItem* _pItem ){ m_vPDPItemsVec[m_nPDPSetIdx].addItem( _pItem ); }
	CPaxDispPropItem* GetOverLapDispProp();
	CPaxDispPropItem* GetDefaultDispProp();
	bool Overlap(std::vector<int> paxItemVector,CPaxDispPropItem* pPDPI);


	int AddPDPSet(const CString& _s, CPaxDispPropItem* _pDefaultItem, CPaxDispPropItem* _pOverlapItem);
	void DeletPDPSet(int _idx);
	const CString& GetPDPSetName(int _idx) const;

	void RenameCurrentPDPSet(const CString& _s);
	int GetCurrentPDPSetIdx() const { return m_nPDPSetIdx; }
	void SetCurrentPDPSet(int _idx);
	int GetPDPSetCount() const { return (int)m_vPDPSetNamesVec.size(); }

	CPaxDispProps();
	virtual ~CPaxDispProps();
	virtual void clear();
	virtual void initDefaultValues();

	virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Mobile Elements Display Data"; }
    virtual const char *getHeaders (void) const{  return ".,..,..,,,"; }

	void loadInputs( const CString& _csProjPath, InputTerminal* _pInTerm );
	void saveInputs( const CString& _csProjPath, bool _bUndo );

	void readObsoletePre23(ArctermFile& p_file);
	void readObsolete23(ArctermFile& p_file);
	void readObsolete24(ArctermFile& p_file);
		

protected:
	void writePDPI(CPaxDispPropItem* pPDPI, ArctermFile& p_file ) const;
	CPaxDispPropItemPtrVector m_vOverlapItemsVec;
	CPaxDispPropItemPtrVector m_vDefaultItemsVec;
	std::vector<CPaxDispPropItemPtrVector> m_vPDPItemsVec;
	std::vector<CString> m_vPDPSetNamesVec;

	int m_nPDPSetIdx;
};

#endif // !defined(AFX_PAXDISPPROPS_H__5C4B9A5C_2AC0_4636_AD9F_D9C5C88D510C__INCLUDED_)
