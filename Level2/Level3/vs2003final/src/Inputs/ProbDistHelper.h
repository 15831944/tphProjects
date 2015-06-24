// ProbDistHelper.h: interface for the ProbDistHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROBDISTHELPER_H__346D1DC8_1C76_459E_B7EF_DFD24310A5F8__INCLUDED_)
#define AFX_PROBDISTHELPER_H__346D1DC8_1C76_459E_B7EF_DFD24310A5F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ProbabilityDistribution;

enum {
	PD_GRAPHTYPE_PDF=0,
	PD_GRAPHTYPE_CDF
};

class ProbDistHelper  
{
private:
	ProbDistHelper();
	virtual ~ProbDistHelper();

public:
	static int GetParamCount(const ProbabilityDistribution* _pd);
	static const char* GetParamName(const ProbabilityDistribution* _pd, int idx);
	static double GetParamValue(const ProbabilityDistribution* _pd, int idx);
	static BOOL SetParamValue(const ProbabilityDistribution* _pd, int idx, double val, int* pError=NULL); //returns FALSE if value is not valid for this pd
	static bool HasParams(const ProbabilityDistribution* _pd);
	static double GetSampleValue(const ProbabilityDistribution* _pd, int idx);
	static double GetSamplePct(const ProbabilityDistribution* _pd, int idx);
	static double GetTotalSamplePct(const ProbabilityDistribution* _pd);
	static ProbabilityDistribution* CreateProbabilityDistribution(int nType);
	static bool Is100Pct(const ProbabilityDistribution* _pd);
	static bool IsValidHistogram(const ProbabilityDistribution* _pd);
	static const char* GetErrorMsg(int nErrorID);

	static int GetPDFGraphPointCount(const ProbabilityDistribution* _pd);
	static bool GetPDFGraphPoints(const ProbabilityDistribution* _pd, double* x, double* y);
	static int GetCDFGraphPointCount(const ProbabilityDistribution* _pd);
	static bool GetCDFGraphPoints(const ProbabilityDistribution* _pd, double* x, double* y);

};

#endif // !defined(AFX_PROBDISTHELPER_H__346D1DC8_1C76_459E_B7EF_DFD24310A5F8__INCLUDED_)
