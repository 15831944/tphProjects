// SideMoveWalkProcData.h: interface for the CSideMoveWalkProcData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIDEMOVEWALKPROCDATA_H__5E232EE1_0AFB_43E3_8901_991FE13FB856__INCLUDED_)
#define AFX_SIDEMOVEWALKPROCDATA_H__5E232EE1_0AFB_43E3_8901_991FE13FB856__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PROCDB.H"

#include "common\termfile.h"
#include "common\point.h"

class CSideMoveWalkProcData : public ProcessorDataElement  
{
private:
	DistanceUnit m_dMoveSpeed;
	DistanceUnit m_dWidth;
	int m_iCapacity;
	InputTerminal* m_pInputTerm;
public:
	CSideMoveWalkProcData(const ProcessorID& id );
	CSideMoveWalkProcData(){	m_dMoveSpeed = 100.0;	m_dWidth = 100.0;	m_iCapacity = 0;};
	virtual ~CSideMoveWalkProcData();
public:
	void SetMoveSpeed( DistanceUnit _dSpeed ){ m_dMoveSpeed = _dSpeed; };
	DistanceUnit GetMoveSpeed() const { return m_dMoveSpeed;};

	void SetCapacity( int  _iCapacity ){ m_iCapacity = _iCapacity; };
	int GetCapacity() const { return m_iCapacity;};

	void SetWidth( DistanceUnit _dWidth){ m_dWidth = _dWidth; };
	DistanceUnit GetWidth() const { return m_dWidth;};
	void SetInputTerm( InputTerminal* _pTerm ){ m_pInputTerm = _pTerm;	};
	virtual void readData (ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;
};

#endif // !defined(AFX_SIDEMOVEWALKPROCDATA_H__5E232EE1_0AFB_43E3_8901_991FE13FB856__INCLUDED_)
