#pragma once

#include <vector>

#include ".\..\Common\ARCBoundingSphere.h"
#include "../InputAirside/BaggageParkingSpecData.h"
#include "..\InputAirside\PaxBusParking.h"
#include "../InputAirside/HoldShortLines.h"
#include "Taxiway3D.h"
class C3DView;
class CAirport3D;
class CAirspace3D;
class CTopograph3D;
class SelectableSettings;
class CAirsideGround;
class CFloor2;

class CAirside3D
{
public:
	typedef std::vector< CAirport3D* > CAirport3DList;

	CAirside3D(void);
	virtual ~CAirside3D(void);

	ALTObject3D * GetObject3D(int id);

	ALTObject3D * AddObject(ALTObject * pObj);
	//delete object with error id(=-1)
	
	void DeleteObject(int objID);	

	void UpdateObject(int objID);


	void UpdateAddorRemoveObjects();
	void UpdateAddorRemoveAirports();
	

	CAirspace3D * GetAirspace();
	CAirport3D * GetAirport(int nAirportID);
	CAirport3D * GetActiveAirport();

	CTopograph3D * GetTopograph();

	ARCBoundingSphere GetBoundingSphere()const;

	void DrawOGL(C3DView * pView, bool bWithLevels = true);
	void DrawSelectOGL(C3DView * pView, SelectableSettings& selSetting );
	
	void GetObject3DList(ALTObject3DList& objList)const;

	BOOL Init(int prjID);

	CAirport3DList & GetAirportList(){ return m_vAirports; }
	void UpdateAirportLevel(int nlevelid);
	CAirsideGround* GetAirportLevel(int nLevelid);

	std::vector<CFloor2* > GetAllAirportLevels();

	CAirsideGround* GetActiveLevel();

	void UpdateBagCartParkingInfo();
	void UpdatePaxBusParkingInfo();
	void RenderParkings(C3DView * pView);
	void SetTempBagCartParkings(BaggageCartParkingSpecData* bagCartSpecData);
	void SetTempPaxBusParkings( CPaxBusParkingList* paxBusParks );

	// reflect changes of the object, auto snap to  move, rotate,the object ...
	bool ReflectChangeOf(ALTObject3DList vObj3Ds);
	void ReflectRemoveOf(ALTObject3DList vObj3Ds);

	bool UpdateChangeOfObject(ALTObject* pObj); 

	void UpdateHoldShortLineInfo();
	void  RenderHoldShortLine(C3DView* pView);


	void RenderALTObjectText(C3DView * pView);

	std::vector<CTaxiInterruptLine3D* > GetHoldShortLine3DList()const{return pHoldShortLine3DList;}


	//Render the Obstruction Surface of Runway , taxiway
	void RenderObstructionSurface(C3DView* pView);

private:
		
	CAirspace3D* m_pAirspace;
	std::vector<CAirport3D*> m_vAirports;
	CTopograph3D* m_pTopograph;

	SelectableList m_vSelected;

	int m_nProjID;
	
protected:
	BaggageCartParkingSpecData m_Baggageparkings;
	CPaxBusParkingList m_paxbusParkings;

	BaggageCartParkingSpecData* m_pTempBaggageparkings;
	CPaxBusParkingList* m_pTempPaxbusParkings;

	std::vector<CTaxiInterruptLine3D* > pHoldShortLine3DList; 
	
};
