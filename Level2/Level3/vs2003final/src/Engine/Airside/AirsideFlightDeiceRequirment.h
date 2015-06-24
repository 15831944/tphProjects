#pragma once


//flight deice requirement at the time
class FlightDeiceRequirment
{
public:
	FlightDeiceRequirment(){ 
		m_antiicefluidReq = 0.0;
		m_deicefluidReq =0.0;
		m_deiceVehicleNum =1;
		m_antiiceVehicleNum = 1;
		m_deiceFlowrate = 20;
		m_antiiceFlowrate = 20;
	}

	bool bNeedDeice()const{ return m_deicefluidReq + m_antiicefluidReq > 0; }

	int m_deiceVehicleNum;  //numbers vehicle service at one time
	double m_deiceFlowrate; // (USG/min)

	int m_antiiceVehicleNum; 
	double m_antiiceFlowrate; // (USG/min)

	double minFluidReq()const{ return max( minDeiceFluidReq(), minAntiiceFluidReq()); }

	//ElapsedTime servTimePerVehicle()const{ return ElapsedTime(fluidPerVehicle()*60.0/m_deiceFlowrate); }
	//int nVehicleRequir()const{ return m_deiceVehicleNum + m_antiiceVehicleNum; }
public:
	ElapsedTime m_holdoverTime;
	ElapsedTime m_inspectionTime;
	double m_deicefluidReq;
	double m_antiicefluidReq;

	double minDeiceFluidReq()const{ return m_deicefluidReq/max(1,m_deiceVehicleNum); }
	double minAntiiceFluidReq()const{ return m_antiicefluidReq/max(1,m_antiiceVehicleNum); }
};
