// Walkthroughs.h: interface for the CWalkthroughs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WALKTHROUGHS_H__8B0F9516_B536_49FD_BC1B_0436CAE53738__INCLUDED_)
#define AFX_WALKTHROUGHS_H__8B0F9516_B536_49FD_BC1B_0436CAE53738__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\DataSet.h"
#include "Walkthrough.h"

#include <vector>

class CWalkthroughs : public DataSet
{
public:
	CWalkthroughs(InputFiles fileSort);
	virtual ~CWalkthroughs();

	virtual void clear();

	virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Walkthrough Data"; }
    virtual const char *getHeaders (void) const{  return ".,..,..,,,"; }

	int AddWalkthrough(EnvironmentMode _envmode, const CString& _s);
	void DeleteWalkthrough(int _idx);
	void DeleteWalkthrough(CWalkthrough* pWalkthrough);
	const CString& GetWalkthroughName(int _idx) const { return m_vNames[_idx]; }
	CWalkthrough* GetWalkthrough(int _idx) { return m_vWalkthroughs[_idx]; }
	void SetWalkthroughName(int _idx, const CString& _s) { m_vNames[_idx] = _s; }
	void SetWalkthroughName(CWalkthrough* pWalkthrough, const CString& _s);
	int GetWalkthroughCount() const { return m_vNames.size(); }

	void MoveWalthroughToOther( CWalkthroughs* otherWalkthroughs, CWalkthrough* pWalkthrough );

protected:
	std::vector< CWalkthrough* > m_vWalkthroughs;
	std::vector< CString > m_vNames;
};

#endif // !defined(AFX_WALKTHROUGHS_H__8B0F9516_B536_49FD_BC1B_0436CAE53738__INCLUDED_)
