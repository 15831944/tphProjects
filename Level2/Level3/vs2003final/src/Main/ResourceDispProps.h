// ResourceDispProps.h: interface for the CResourceDispProps class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCEDISPPROPS_H__5CA51DED_15C8_46BA_AF34_984FEA2C6F0F__INCLUDED_)
#define AFX_RESOURCEDISPPROPS_H__5CA51DED_15C8_46BA_AF34_984FEA2C6F0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\dataset.h"
#include "ResourceDispPropItem.h"
#include <vector>

typedef std::vector<CResourceDispPropItem*> CResourceDispPropItemPtrVector;
class CResourcePoolDataSet;

class CResourceDispProps : public DataSet
{
public:
	CResourceDispProps();
	virtual ~CResourceDispProps();
	virtual void clear();
	virtual void initDefaultValues();

	virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Resource Display Data"; }
    virtual const char *getHeaders (void) const{  return ".,..,..,,,"; }

	void loadInputs( const CString& _csProjPath, InputTerminal* _pInTerm );
	void saveInputs( const CString& _csProjPath, bool _bUndo );

	int GetCount();
	CResourceDispPropItem* GetDispPropItem( int _nIdx );

	void AddItemToAllSets( int _nResPoolIdx ); //adds a CResourceDispPropItem* contructed from _nResPoolIdx to each RDP set
	void DeleteItemFromAllSets( int _nIdx ); 

	int AddRDPSet(const CString& _s);
	void DeletRDPSet(int _idx);
	const CString& GetRDPSetName(int _idx) const;
	void RenameCurrentRDPSet(const CString& _s);
	int GetCurrentRDPSetIdx() const { return m_nRDPSetIdx; }
	void SetCurrentRDPSet(int _idx);
	int GetRDPSetCount() const { return (int)m_vRDPSetNamesVec.size(); }

protected:
	void AddDispPropItem( CResourceDispPropItem* _pItem ){ m_vRDPItemsVec[m_nRDPSetIdx].push_back( _pItem ); }
	void DeleteItem( int _nIdx );
	void writeRDPI(CResourceDispPropItem* pRDPI, ArctermFile& p_file ) const;
	std::vector<CResourceDispPropItemPtrVector> m_vRDPItemsVec;
	std::vector<CString> m_vRDPSetNamesVec;
	int m_nRDPSetIdx;

	const CResourcePoolDataSet* m_pResPoolDB;

};

#endif // !defined(AFX_RESOURCEDISPPROPS_H__5CA51DED_15C8_46BA_AF34_984FEA2C6F0F__INCLUDED_)
