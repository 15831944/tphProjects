// ActivityDensityParams.h: interface for the CActivityDensityParams class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIVITYDENSITYPARAMS_H__1B6B900A_C011_48D9_8A6A_6927EEF89D6E__INCLUDED_)
#define AFX_ACTIVITYDENSITYPARAMS_H__1B6B900A_C011_48D9_8A6A_6927EEF89D6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\ARCVector.h"
#include "common\dataset.h"
#include <vector>

class CActivityDensityParams : public DataSet  
{
public:
	CActivityDensityParams();
	virtual ~CActivityDensityParams();


	virtual void clear();
	virtual void initDefaultValues();

	virtual void readData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Activity Density Parameters"; }
    virtual const char *getHeaders (void) const{  return ".,..,..,,,"; }

	// Description: Load from default file
	// Exception:	FileVersionTooNewError
	void loadInputs( const CString& _csProjPath, InputTerminal* _pInTerm );

	void saveInputs( const CString& _csProjPath, bool _bUndo );

	const ARCVector2& GetAOISize() const { return m_AOISize; }
	const ARCVector2& GetAOISubdivs() const { return m_AOISubdivs; }
	const ARCVector2& GetAOICenter() const { return m_AOICenter; }
	int GetAOIFloor() const { return m_nAOIFloor; }
	double GetLOSMinimum() const { return m_dLOSMin; }
	double GetLOSInterval() const { return m_dLOSInterval; }
	int GetLOSLevels() const { return m_nLOSLevels; }
	COLORREF GetLOSColor(int level) const;
	COLORREF GetLOSColor(double dLOS) const;
	std::vector<COLORREF>* GetLOSColors() { return &m_vLOSColors; }
	const CString GetLOSLevelDescription(int nLevel) const;

	void SetAOISize(double x, double y);
	void SetAOISubdivs(double x, double y);
	void SetAOICenter(double x, double y);
	void SetAOIFloor(int nFloor);
	void SetLOSMinimum(double min);
	void SetLOSInterval(double interval);
	void SetLOSLevels(int levels);
	void SetLOSColor(int level, COLORREF col);

protected:
	ARCVector2 m_AOISize;
	ARCVector2 m_AOISubdivs;
	ARCVector2 m_AOICenter;
	int m_nAOIFloor;
	double m_dLOSMin;
	double m_dLOSInterval;
	int m_nLOSLevels;
	std::vector<COLORREF> m_vLOSColors;
};

#endif // !defined(AFX_ACTIVITYDENSITYPARAMS_H__1B6B900A_C011_48D9_8A6A_6927EEF89D6E__INCLUDED_)
