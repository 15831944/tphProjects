// DisplayPropOverrides.h: interface for the CDisplayPropOverrides class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISPLAYPROPOVERRIDES_H__5FE0D33B_DD9F_47EB_8636_98073715651E__INCLUDED_)
#define AFX_DISPLAYPROPOVERRIDES_H__5FE0D33B_DD9F_47EB_8636_98073715651E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\dataset.h"

class CDisplayPropOverrides : public DataSet  
{
public:
	CDisplayPropOverrides();
	virtual ~CDisplayPropOverrides();

	virtual void readData(ArctermFile& p_file);
	//virtual void readObsoleteData (ArctermFile& p_file);
    virtual void writeData(ArctermFile& p_file) const;
	virtual void readObsoleteData ( ArctermFile& p_file );
    virtual const char *getTitle(void) const { return "Display Property Overrides Data"; }
    virtual const char *getHeaders (void) const{  return "Name,Data,,,"; }

	virtual void clear();

	BOOL m_pbProcDisplayOverrides[7];
};

#endif // !defined(AFX_DISPLAYPROPOVERRIDES_H__5FE0D33B_DD9F_47EB_8636_98073715651E__INCLUDED_)
