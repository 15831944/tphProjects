// PaxType.h: interface for the CPaxType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAXTYPE_H__0224EC6B_B685_43B6_A6C8_54ACF296FFDC__INCLUDED_)
#define AFX_PAXTYPE_H__0224EC6B_B685_43B6_A6C8_54ACF296FFDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\inputs\MobileElemConstraint.h"
#include "..\inputs\AreasPortals.h"

class Terminal;

enum AREA_PORTAL_APPLY_TYPE { ENUM_APPLY_NONE, ENUM_APPLY_AREA, ENUM_APPLY_PORTAL };

class CPaxType  
{
public:
	void SetArea( const CArea& _area );
	void SetPortal( const CPortal& _portal);
	void SetIsPaxConAdd(BOOL _bAdd );
    void SetAreaportalApplyType( AREA_PORTAL_APPLY_TYPE _enumAreaPortalApply );
	void SetIsAreaPortalAdd(BOOL _bAdd );
	void SetIsTimeApplied(BOOL _bAdd );
	void SetIsBirthTimeApplied(BOOL _bAdd );
	void SetIsBirthTimeAdd(BOOL _bAdd );
	void SetIsDeathTimeApplied(BOOL _bAdd );
	void SetIsDeathTimeAdd(BOOL _bAdd );
	void SetMaxDeathTime( COleDateTime _dt);
	void SetMinDeathTime( COleDateTime _dt);
	void SetMaxBirthTime( COleDateTime _dt);
	void SetMinBirthTime( COleDateTime _dt);
	void SetEndTime( COleDateTime _dt);
	void SetStartTime( COleDateTime _dt);
	
	CArea& GetArea( ){ return m_area;	}
	CPortal& GetPortal( ){ return m_portal; }
	BOOL GetIsPaxConAdd(  ){ return m_isPaxConAdd; } const
    AREA_PORTAL_APPLY_TYPE GetAreaportalApplyType( ){ return m_enumAreaPortalApply; } const
	BOOL GetIsAreaPortalAdd( ){ return m_isAreaPortalAdd;	} const
	BOOL GetIsTimeApplied( ){ return m_isTimeApplied;	} const
	BOOL GetIsBirthTimeApplied( ){ return m_isBirthTimeApplied;	} const
	BOOL GetIsBirthTimeAdd( ){ return m_isBirthTimeAdd;	} const
	BOOL GetIsDeathTimeApplied(){ return m_isDeathTimeApplied;	} const
	BOOL GetIsDeathTimeAdd( ){ return m_isDeathTimeAdd;	} const
	COleDateTime GetMaxDeathTime(  ){ return m_oMaxDeathTime;	} const
	COleDateTime GetMinDeathTime( ){ return m_oMinDeathTime;	} const
	COleDateTime GetMaxBirthTime(  ){ return m_oMaxBirthTime;	} const
	COleDateTime GetMinBirthTime(  ){ return m_oMinBirthTime;	} const
	COleDateTime GetEndTime(  ){ return m_oEndTime; } const
	COleDateTime GetStartTime(  ){ return m_oStartTime; } const


	CPaxType();
	CPaxType(const CPaxType& _rhs);
	CPaxType& operator	=(const CPaxType& _rhs);
	virtual ~CPaxType();

	
	CString ScreenPrint( BOOL _HasReturn=FALSE )const;
	const CString& GetName(){ return m_csName;}  const
	void SetName( CString _csName ){ m_csName = _csName; }
	CMobileElemConstraint& GetPaxCon(){ return m_paxCon; }
	void SetPaxCon( const CMobileElemConstraint& _paxCon ){ m_paxCon = _paxCon; }

	//added by Nic April23, 2002
	BOOL Matches(const MobDescStruct& pds, Terminal* pTerminal) const;
	bool fit( CPaxType& _rhs);

protected:
	CString m_csName;

	CMobileElemConstraint m_paxCon;
	BOOL m_isPaxConAdd;

	enum AREA_PORTAL_APPLY_TYPE m_enumAreaPortalApply;
	CArea m_area;
	CPortal m_portal;
	BOOL m_isAreaPortalAdd;
	BOOL m_isTimeApplied;
	COleDateTime m_oStartTime;
	COleDateTime m_oEndTime;
	
	BOOL m_isBirthTimeApplied;
	COleDateTime m_oMinBirthTime;
	COleDateTime m_oMaxBirthTime;
	BOOL m_isBirthTimeAdd;

	BOOL m_isDeathTimeApplied;
	COleDateTime m_oMinDeathTime;
	COleDateTime m_oMaxDeathTime;
	BOOL m_isDeathTimeAdd;
};

#endif // !defined(AFX_PAXTYPE_H__0224EC6B_B685_43B6_A6C8_54ACF296FFDC__INCLUDED_)
