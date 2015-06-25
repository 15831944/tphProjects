#pragma once
#include "InboudRouteAssFlightTY.h"
class CRouteAssFlightTyContainer
{
public:
	virtual ~CRouteAssFlightTyContainer(void);
	CRouteAssFlightTyContainer(int Pro_ID,int classTY);
public:
	typedef std::vector<CRouteAssFlightTY*> FLIGHTTY_LIST ;
	typedef FLIGHTTY_LIST::iterator FLIGHTTY_LIST_ITER ;
	typedef FLIGHTTY_LIST::const_iterator FLIGHTTY_LIST_ITER_CONST ;
private:
	FLIGHTTY_LIST list_flightTY ;
	FLIGHTTY_LIST del_list_flightTY ;
	int proid ;
public:
	CRouteAssFlightTY* AddFlightTY(CString flightTY) ;
	void DelFlightTY(CRouteAssFlightTY* flightTY) ;
	void DelAllFlightTYFromDB() ;
	void OnSaveData() ;
	FLIGHTTY_LIST& GetFlightTys() { return list_flightTY ;} ;
public:
	void ReadFlightTYFromDB() ;
protected:
	int class_TY ;
};
class CInboundRouteAssFlightTyContainer : public CRouteAssFlightTyContainer
{
public:

	~CInboundRouteAssFlightTyContainer(void);
   CInboundRouteAssFlightTyContainer(int Pro_ID);

};
class COutboundRouteAssFlightTyContainer : public CRouteAssFlightTyContainer 
{
public:

	~COutboundRouteAssFlightTyContainer(void);
	COutboundRouteAssFlightTyContainer(int Pro_ID);
};