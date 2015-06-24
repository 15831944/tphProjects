#pragma once
//airside scale data
class CFloorAdjustmentData
{
public:
	static double ReadDataFromDataBase() ;
	static void WriteDataToDataBase(double _val) ;
};
