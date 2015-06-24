// MathResultDataSet.h: interface for the CMathResultDataSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATHRESULTDATASET_H__41B9A4DA_8096_4D1D_802B_27F1B3705E2D__INCLUDED_)
#define AFX_MATHRESULTDATASET_H__41B9A4DA_8096_4D1D_802B_27F1B3705E2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Common/dataset.h"
#include <vector>

struct MathResultRecord;

class CMathResultDataSet : public DataSet  
{
public:
	CMathResultDataSet(int p_file);
	virtual ~CMathResultDataSet();

	virtual void clear();
	virtual void readData(ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;
	virtual const char* getTitle() const{return "Math Process Result Data";}
	virtual const char* getHeaders() const{return "Time, Pro-1, Pro-2,,,";}

	void SetProcessNames(std::vector<std::string>* pvstrPros) { m_pvstrPros = pvstrPros; }
	void SetMathResultRecord(std::vector<MathResultRecord>* pvResult) {m_pvResult = pvResult; }

protected:
	std::vector<std::string>* m_pvstrPros;
	std::vector<MathResultRecord>* m_pvResult;
};

#endif // !defined(AFX_MATHRESULTDATASET_H__41B9A4DA_8096_4D1D_802B_27F1B3705E2D__INCLUDED_)
