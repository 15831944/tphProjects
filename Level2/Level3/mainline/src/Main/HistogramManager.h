// HistogramManager.h: interface for the CHistogramManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HISTOGRAMMANAGER_H__1842103E_8319_4EA9_94EE_40753180E74A__INCLUDED_)
#define AFX_HISTOGRAMMANAGER_H__1842103E_8319_4EA9_94EE_40753180E74A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\common\DataSet.h"
#include <vector>

class CHistogramManager : public DataSet
{
public:
	CHistogramManager();
	virtual ~CHistogramManager();

	virtual void clear();
	virtual void readData(ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;
	virtual const char* getTitle() const{return "Histogram Data";}
	virtual const char* getHeaders() const{return "Value";}

	std::vector<double>& GetValueVector() { return m_vdbValue; }
	void SetValueVector(const std::vector<double>& vdbValue) { m_vdbValue = vdbValue; }
	void AddElement(const double& dbValue);
	double DeleteElement(int iIndex);
	double ModifyElement(int iIndex, const double& dbValue);
protected:
	std::vector<double> m_vdbValue;	
};

#endif // !defined(AFX_HISTOGRAMMANAGER_H__1842103E_8319_4EA9_94EE_40753180E74A__INCLUDED_)
