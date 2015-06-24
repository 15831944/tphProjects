#pragma once
#include "./CommonInSim.h"
#include "AirsideResource.h"





class ENGINE_TRANSFER ClearanceItem
{
public:
	ClearanceItem& SetSpeed(double dspd){ m_vSpeed = dspd; return *this; }
	ClearanceItem& SetAltitude(double dalt){ m_dAlt = dalt; return *this; }
	ClearanceItem& SetTime(const ElapsedTime& t){ m_tTime=t; return *this; }
	ClearanceItem& SetDistInResource(DistanceUnit dDist){ m_dDist = dDist; return *this; }
	ClearanceItem& SetDelayTime(const ElapsedTime& dt){ m_tDelay = dt; return *this; }
	ClearanceItem& SetDelayReason(const FltDelayReason& fltdelayreason){ m_tDelayReseaon = fltdelayreason ; return *this; } 
	ClearanceItem& SetPosition(const CPoint2008& pos){ m_pos = pos; return *this; }
	ClearanceItem& SetAcceleration(double acc){ m_acc = acc; return *this; }
	ClearanceItem& SetMode( AirsideMobileElementMode mode ){ m_nMode = mode; return *this;}
	ClearanceItem& SetOffsetAngle(double dAngle){ m_offsetAngle = dAngle; return *this; }
	ClearanceItem& SetPushback(bool bPushback){ m_bPushback = bPushback; return *this; }
	ClearanceItem& SetResource(AirsideResource* pRes){ m_pResource=NULL; return *this; }

	double GetSpeed()const{ return m_vSpeed; }
	ElapsedTime GetTime()const{ return m_tTime; }
	AirsideResource * GetResource()const; 
	double GetDistInResource()const{ return m_dDist; }
	AirsideMobileElementMode GetMode()const{ return m_nMode; }
	DistanceUnit GetAltitude()const {  return m_dAlt; }
	FltDelayReason GetDelayReason()const{ return m_tDelayReseaon; }
	CPoint2008 GetPosition()const;
	double GetAcceleration()const{ return m_acc; }
	ElapsedTime GetDelayTime()const{ return m_tDelay; }
	double GetOffsetAngle()const{ return m_offsetAngle; }
 
	int GetDelayId()const{ return m_nDelayId; }
	void SetDelayId(int nId){ m_nDelayId = nId; }

	bool IsWirteLog()const{ return m_bWirteLog; }
	void WriteLog(bool b);

	bool IsPushback()const{return m_bPushback; }

public:
	ClearanceItem(AirsideResource * pRes, AirsideMobileElementMode fltmode , DistanceUnit dist=0 );
	DistanceUnit DistanceTo(const ClearanceItem& nextItem)const;
	
	friend std::ostream& operator << (std::ostream& s, const ClearanceItem& cItem);

protected:
	AirsideResource * m_pResource;  // Resource
	DistanceUnit m_dDist;           // the dist in resource	
	double m_dAlt;                  // expected altitude;

	ElapsedTime m_tTime;            // expected at time;	
	double m_vSpeed;                // expected speed
	AirsideMobileElementMode m_nMode;
	CPoint2008 m_pos;
	double m_acc;                   //expected acceleration
	
	double m_offsetAngle;          //offset angle with the direction , used in temporary parking 

	ElapsedTime m_tDelay;           // delay of the time;
	FltDelayReason m_tDelayReseaon; // delay reason
	int m_nDelayId;
	bool m_bWirteLog; 
	bool m_bPushback;
};

class ENGINE_TRANSFER Clearance
{

public:
	enum TYPE{};
public:
	int GetItemCount()const { return static_cast<int>(m_vItems.size()); }
	ClearanceItem GetItem(int idx)const{  return m_vItems[idx]; }
	ClearanceItem& ItemAt(int idx){ return m_vItems[idx]; }
	const ClearanceItem& ItemAt(int idx)const{ return m_vItems[idx]; }
	void AddItem(const ClearanceItem & newItem){ m_vItems.push_back(newItem); }
	void AddClearance(const Clearance& nextclearance);

	//get length of clearance [ibegin,iend);
	DistanceUnit GetLength(int ibegin, int iend )const;	
	
	//clear items from idx to end
	void ClearItems(int idx=0);

	void ClearAllItems();
	void RemoveItem(int idx);

	//
	ElapsedTime GetStartTime()const{ return m_startTime; }
	ElapsedTime SetStartTime(const ElapsedTime& t){ m_startTime = t; }

	
	friend std::ostream& operator << (std::ostream& s, const Clearance& cItem);
protected:
	std::vector<ClearanceItem> m_vItems;	
	TYPE m_tType;                   // Clearance Type
	ElapsedTime m_startTime;       // start Time;

};
