// AreaDensityOfPaxtype.h: interface for the CAreaDensityOfPaxtype class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AREADENSITYOFPAXTYPE_H__F050B77F_AFF0_46BD_95AC_A382EB847765__INCLUDED_)
#define AFX_AREADENSITYOFPAXTYPE_H__F050B77F_AFF0_46BD_95AC_A382EB847765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// include 
#include "MobileElemConstraint.h"
#include <CommonData/procid.h>
#include <map>

class CAreaDensityKey
{
protected:
	CMobileElemConstraint m_paxType;
	ProcessorID			  m_destProcID;

public:
	CAreaDensityKey( const CMobileElemConstraint& _paxType, const ProcessorID& _destID )
				   : m_paxType( _paxType ), m_destProcID( _destID )
	{		}

	// get and set
	void setPaxType( const CMobileElemConstraint& _paxType ) { m_paxType = _paxType; }
	void setDestID( const ProcessorID& _destID ) { m_destProcID = _destID;	}

	const CMobileElemConstraint& getPaxType( void ) const { return m_paxType;	}
	const ProcessorID& getDestID( void ) const { return m_destProcID;}

	// fits
	bool fits( const CMobileElemConstraint& _mobCon, const ProcessorID& _destID ) const
	{
		return m_paxType.fits( _mobCon ) && m_destProcID.idFits( _destID );
	}
	
	// operator < for sort
	bool operator<( const CAreaDensityKey& _key ) const
	{
		return m_paxType < _key.m_paxType || 
			   (!( _key.m_paxType < m_paxType) &&  m_destProcID<_key.m_destProcID);
	}
	
};

typedef std::map< CAreaDensityKey, int > CMaxCountOfPaxType;

#endif // !defined(AFX_AREADENSITYOFPAXTYPE_H__F050B77F_AFF0_46BD_95AC_A382EB847765__INCLUDED_)
