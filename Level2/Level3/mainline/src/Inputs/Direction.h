// Direction.h: interface for the CDirection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTION_H__5F922868_DAD5_4AF6_BAB7_2EA5A9C25EA9__INCLUDED_)
#define AFX_DIRECTION_H__5F922868_DAD5_4AF6_BAB7_2EA5A9C25EA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include<vector>
#include "PaxDirecton.h"
//class CPaxDirection;
//class CMobileElemConstraint;
//class ProcessorID;
class CDirection  
{
private:
	std::vector<CPaxDirection> m_paxDirctions;
public:
	CDirection();
	bool operator ==(const CDirection& direction);
	virtual ~CDirection();
public:
	bool IsEmpty() { return m_paxDirctions.empty(); }
	bool CanLeadTo( const CMobileElemConstraint& _paxType,  const ProcessorID& _destProcID,InputTerminal* _pTerm)const;
	void Insert( const CMobileElemConstraint& _paxType , const ProcessorID& _destProcID );
	void Insert(CDirection& Dirctions,const CMobileElemConstraint& _paxType);
	void Sort();
	std::vector<CPaxDirection>& GetPaxDirection() { return m_paxDirctions; }


	//for land side object
	bool CanLeadTo( const CMobileElemConstraint& _paxType,  const LandsideObjectLinkageItemInSim& _destProcID,InputTerminal* _pTerm)const;
	void Insert( const CMobileElemConstraint& _paxType , const LandsideObjectLinkageItemInSim& _destProcID );
	void Insert( const CMobileElemConstraint& _paxType , const std::vector<LandsideObjectLinkageItemInSim>& _vDestProcID );

	bool CanLeadTo( const CMobileElemConstraint& _paxType,  const std::vector<LandsideObjectLinkageItemInSim>& _vDestProcID,InputTerminal* _pTerm)const;



};

#endif // !defined(AFX_DIRECTION_H__5F922868_DAD5_4AF6_BAB7_2EA5A9C25EA9__INCLUDED_)
