#pragma once
/**
*	Diagnosis class to provide ALTO mobile elements diagnosis functionality.
	Mobile element's in different Mode(A.L.T.O)'s movement log saved in separate log file, 	
	evermore, do not need build project while checking interested mobile element's movement sequence.
**/
#include "ServiceObject.h"
#include "Common/elaptime.h"

class Person;
class CMobElementsDiagnosis : public ServiceObject < CMobElementsDiagnosis >
{
public:
	CMobElementsDiagnosis();
	~CMobElementsDiagnosis(void);

	void Initialize(const CString& strProjPath);

	void Diagnose(Person* pMobElement, const ElapsedTime& p_time);

	bool isInList(int nPaxID)const;
private:

	void cleanUp();

	void DiagnoseAirsideMobElement(Person* pMobElement, const ElapsedTime& p_time);
	void DiagnoseLandsideMobElement(Person* pMobElement, const ElapsedTime& p_time);
	void DiagnoseTerminalMobElement(Person* pMobElement, const ElapsedTime& p_time);
	void DiagnoseOnBoardMobElement(Person* pMobElement, const ElapsedTime& p_time);

	bool mEnableDiagnosis;
	std::string mFilePath;
	std::vector< long > mMobElementList; 
};


CMobElementsDiagnosis* MobElementsDiagnosis();
AFX_INLINE CMobElementsDiagnosis* MobElementsDiagnosis()
{
	return CMobElementsDiagnosis::GetInstance();

}
