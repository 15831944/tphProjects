#pragma once
//#include "InputAirsideDll.h"
#include "ALTObject.h"
#include "Common\Path2008.h"
#include "AirsidePaxBusParkSpot.h"
#include "Database\DBElementCollection.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API AirsideBagCartParkSpot : public AirsideParkSpot
{
public:
	typedef ref_ptr<AirsideBagCartParkSpot> RefPtr;
	virtual CString GetTypeName()const;
	virtual ALTOBJECT_TYPE GetType()const;



	virtual CString getPropTable()const;
	virtual CString getViewTable()const;

protected:
	ALTObject * NewCopy();

};


class INPUTAIRSIDE_API AirsideBagCartParkSpotList//: public DBElementCollection<AirsideBagCartParkSpot>
{
public:
	AirsideBagCartParkSpotList();
	virtual ~AirsideBagCartParkSpotList();
protected:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;


public:
	void ReadData(int nAirportID);
	void CleanData();
	size_t GetItemCount();
	AirsideBagCartParkSpot *GetItemByIndex(size_t nIndex);

	std::vector<AirsideBagCartParkSpot *> m_vParkingSpot;

};