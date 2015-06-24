// Walkthrough.h: interface for the CWalkthrough class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WALKTHROUGH_H__1F7FFB56_AD72_40EF_819E_5BF89745315C__INCLUDED_)
#define AFX_WALKTHROUGH_H__1F7FFB56_AD72_40EF_819E_5BF89745315C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IN_TERM.H"

class ArctermFile;
class ARCVector3;

class CWalkthrough  
{
public:
	enum VIEWPOINT {
		FIRSTPERSON=0,
		THIRDPERSON
	};

	enum WalkthroughMobType
	{
		Mob_None = -1,

		Mob_Pax = 0,
		Mob_Flight,
		Mob_Vehicle,
	};


	CWalkthrough(EnvironmentMode envmode);
	virtual ~CWalkthrough();

	UINT GetPaxID() const;
	VIEWPOINT GetViewpoint() const { return m_viewpoint; }
	const ARCVector3& GetViewpointOffset() const;
	const ARCVector3& GetViewpointDirection() const;
	long GetStartTime() const { return m_nStartTime; }
	long GetEndTime() const { return m_nEndTime; }
	const CString& GetPaxIDDesc() const 
	{ 
		return m_sPaxIDDesc; 
	}
	EnvironmentMode GetEnvMode() const 
	{ 
		return m_eEnvMode; 
	}
	
	void SetPaxID( UINT nPaxID ) { m_nPaxID = nPaxID; }
	void SetViewpoint( VIEWPOINT v ) { m_viewpoint = v; }
	void SetStartTime( long nTime ) { m_nStartTime = nTime; }
	void SetEndTime( long nTime ) { m_nEndTime = nTime; }
	void SetPaxIDDesc( const CString& s ) { m_sPaxIDDesc = s; }
	void SetEnvMode( EnvironmentMode envmode) { m_eEnvMode = envmode; }

	static CWalkthrough* CreateFromFile(ArctermFile& _file);
	static CWalkthrough* CreateFromObsoleteFile(ArctermFile& _file);
	static void WriteToFile(const CWalkthrough& _wt, ArctermFile& _file);

protected:
	CWalkthrough();

private:
	UINT m_nPaxID;
	VIEWPOINT m_viewpoint;
	long m_nStartTime;
	long m_nEndTime;
	CString m_sPaxIDDesc;
	EnvironmentMode m_eEnvMode;

};

#endif // !defined(AFX_WALKTHROUGH_H__1F7FFB56_AD72_40EF_819E_5BF89745315C__INCLUDED_)
