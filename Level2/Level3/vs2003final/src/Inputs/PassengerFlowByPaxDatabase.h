// PassengerFlowByPaxDatabase.h: interface for the CPassengerFlowByPaxDatabase class.
//
// Created by Lijun, Nov 06, 2002
// Passenger Flow is a tree start from START and terminated at the END.
// This class is for the purpose of travel through this tree easily, 
// instead of use the PassengerFlowDatabase which is the base on one processor 
// and list number of passenger types and its destinations.  
//
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PASSENGERFLOWBYPAXDATABASE_H__524280FD_845E_443D_B2CB_1D97CB2AD789__INCLUDED_)
#define AFX_PASSENGERFLOWBYPAXDATABASE_H__524280FD_845E_443D_B2CB_1D97CB2AD789__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPassengerFlowByPaxDatabase  
{
public:
	CPassengerFlowByPaxDatabase();
	virtual ~CPassengerFlowByPaxDatabase();

};

#endif // !defined(AFX_PASSENGERFLOWBYPAXDATABASE_H__524280FD_845E_443D_B2CB_1D97CB2AD789__INCLUDED_)
