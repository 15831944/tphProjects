#pragma once

const static int TypeBaseValue = 100;
//enum enumVehicleLandSideType
//{
//	VehiclType_Private=100,
//	VehicleType_Public,
//	VehicleType_Taxi,
//	VehicleType_Iterate,
//	VehicleType_LandSideCount
//};

enum enumVehicleBaseType
{
	//VehicleType_unknown = 0,
	//VehicleType_FuelTruck,
	//VehicleType_FuelBrowser,
	//VehicleType_CateringTruck,
	//VehicleType_BaggageTug,
	//VehicleType_BaggageCart,
	//VehicleType_MaintenanceTruck,
	//VehicleType_Lift,
	//VehicleType_CargoTug,
	//VehicleType_CargoULDCart,
	//VehicleType_Conveyor,
	//VehicleType_Stairs,
	//VehicleType_StaffCar,
	//VehicleType_CrewBus,
	VehicleType_General = 100,
	VehicleType_TowTruck,
	VehicleType_DeicingTruck,
	VehicleType_PaxTruck,
	VehicleType_InspectionTruck,
	VehicleType_FollowMeCar,
	VehicleType_BaggageTug,
	VehicleType_JointBagTug,
	VehicleType_BaggageCart,
	//add new type before this line
	//if add new type, the following variable should be modify
	//const static CString VehicleTypeName[]
	//const static CString VehicleTypeCondition[]
	VehicleType_Count
};

const static CString VehicleTypeName[] = 
{
		_T("General"),    //0 
		_T("Tow truck"),              //1
		_T("Deicing truck"),          //2
		_T("Pax bus"),        //3
		_T("Inspection Car"),
		_T("Follow Me Car"),
		_T("Baggage Tug"),
		_T("Joint Bag Tug"),
		_T("Baggage Cart"),
		//add new before this line
		_T("Invalid Type")
};
//const static CString LandSideVehicleTypeName[]=
//
//{
//
//   _T("Private Vehicle"),
//   _T("Public Vehicle"),
//   _T("Random Taxi"),
//   _T("Iterate vehicle")
//	   
//};

enum enumVehicleUnit
{
	PerVehicle = 0,
	Liters,
	Pax,
	Bag,
	Kg,
	Carts,
	VehicleUnit_Count	
};

const static CString VehicleUnitName[] = 
{
		_T("Per vehicle"),			  //0 
		_T("liters"),				  //1
		_T("pax"),					  //2
		_T("bags"),					  //3
		_T("kg"),					  //4
		_T("carts"),
		//add new before this line
		_T("Invalid Unit")
};


//const static CString VehicleDefaultName[] = 
//{
//	_T("Unknown"),
//		_T("Fuel truck"),    //0 
//		_T("Fuel Browser"),			  //1
//		_T("Catering truck"),   //2
//		_T("Baggage tug"),            //3
//		_T("Baggage cart"),    //4
//		_T("Maintenance truck"),      //5
//		_T("Lift"),                   //6
//		_T("Cargo tug"),              //7
//		_T("Cargo ULD cart"),    //8
//		_T("Conveyor"),               //9
//		_T("Stairs"),                 //10
//		_T("Staff car"),              //11
//		_T("Crew bus"),               //12   
//		_T("Tow truck"),              //13
//		_T("Deicing truck"),          //14
//		_T("Pax bus"),        //15
//
//
//		//add new before this line
//		_T("Invalid Vehicle")
//};


enum enumVehicleTypeCondition 
{
	enumVehicleTypeCondition_Per100Liters = 0,
	enumVehicleTypeCondition_Per10Pax,
	enumVehicleTypeCondition_DurationOfCarts,
	enumVehicleTypeCondition_Per10Bags,
	enumVehicleTypeCondition_PerVehicle,
	enumVehicleTypeCondition_PriorPushToPush,




	//add new before this line, and need to modify const CString VehicleConditionName[]
	enumVehicleTypeCondition_Count
};
const CString VehicleConditionName[] =
{	
	    _T("/100 liters (min)"),
		_T("/pax (sec)"),
		_T("duration of carts"),
		_T("/10 bags (min)"),
		_T("per vehicle (min)"),
		_T("prior push to push"),



		//add new before this line, and need to modify enum enumVehicleTypeCondition 
		_T("Invalid Condition")
};

enum TOWOPERATIONTYPE
{
	TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY = 0,
	TOWOPERATIONTYPE_PUSHANDTOW,
	TOWOPERATIONTYPE_PULLANDTOW,
	TOWOPERATIONTYPE_PUSHBACKTORELEASEPOINT,

	//NOTE: add new type before this line
	TOWOPERATIONTYPE_COUNT// total count
};
