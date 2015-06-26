#ifndef _ARCUNITMANAGEROWNERENUM_HEADER_
#define _ARCUNITMANAGEROWNERENUM_HEADER_

//warning : delete the item may cause a read record from DB with wrong data.
typedef enum AFX_DATA_EXPORT ARCUnitManagerOwnerID //umid
{
	UM_ID_GLOBAL_DEFAULT = 0,//global default Unit
	UM_ID_GLOBAL_1 = 0, //Terminal global Unit
	UM_ID_GLOBAL_2,//Airside global Unit
	UM_ID_GLOBAL_3,//Landside global Unit
	UM_ID_1,//Airside Runway Dialog
	UM_ID_2,//Airside Taxiway Dialog
	UM_ID_3,//Airside Gate Dialog
	UM_ID_4,//Airside DeicePad Dialog
	UM_ID_5,//Airside Stretch Dialog
	UM_ID_6,//Airside Roundabout Dialog
	UM_ID_7,//Airside Vehicle Pool Parking Dialog
	UM_ID_8,//Airside TrafficLight Dialog
	UM_ID_9,//Airside TollGate Dialog
	UM_ID_10,//Airside StopSign Dialog
	UM_ID_11,//Airside YieldSign Dialog
	UM_ID_12,//Airside Way Point Dialog
	UM_ID_13,//Airside Hold Dialog
	UM_ID_14,//Airside Sector Dialog
	UM_ID_15,//Airside Surface Dialog
	UM_ID_16,//Airside Contour Dialog
	UM_ID_17,//Airside Route Dialog
	UM_ID_18,//Airside Wall Shape Dialog
	UM_ID_19,//Wake Vortex Weight Categories Dialog
	UM_ID_20,// Approach Speed Categories Dialog
	UM_ID_21,//Surface Bearing Categories Dialog
	UM_ID_22,//Wingspan Categories Dialog
	UM_ID_23,//Engine start and parameter
	UM_ID_24,//Airport reference point
	UM_ID_25,//Airside Adjacency Constraints Specifcation
}ARCUnitManagerOwnerID;

#endif