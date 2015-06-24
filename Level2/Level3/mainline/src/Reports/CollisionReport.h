// CollisionReport.h: interface for the CCollisionReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLISIONREPORT_H__E8112CDF_5F1E_4AF6_9E92_9CD12B385CF9__INCLUDED_)
#define AFX_COLLISIONREPORT_H__E8112CDF_5F1E_4AF6_9E92_9CD12B385CF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"
#include "Reports.h"
#include "CollisionUitl.h"
#include "common\pollygon.h"
class CCollisionReport : public CBaseReport  
{
public:
	CCollisionReport(Terminal* _pTerm, const CString& _csProjPath,int _iFloorCount);

	void ReadSpecifications(const CString& _csProjPath);
	virtual ~CCollisionReport();
    virtual int GetReportFileType (void) const { return CollisionReportFile; };
	virtual void InitParameter( const CReportParameter* _pPara );

protected:
	CCollisionUitl m_collisionUitl;
	ElapsedTime m_curTime;
	int m_nIntervalCount;
	long m_nPaxCount;
	double m_dPortal;
	float m_fThreshold;
	int m_bThreshold;
	int m_iFloorCount;
	Pollygon m_region;
	std::vector<type_rect> vect_rect;
    virtual int GetSpecFileType (void) const { return CollisionSpecFile; };
    virtual const char *GetTitle (void) const { return "Collision_Report"; };
	void calculateCollisionReport (const CMobileElemConstraint& paxType, ArctermFile& p_file);
	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );
//	bool Exist(ComparisonID *comparison, long id1, long id2, long count);

	


};

#endif // !defined(AFX_COLLISIONREPORT_H__E8112CDF_5F1E_4AF6_9E92_9CD12B385CF9__INCLUDED_)
