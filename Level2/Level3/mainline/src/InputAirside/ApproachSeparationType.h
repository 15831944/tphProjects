#pragma once

enum FlightClassificationBasisType
{
//	CategoryType_unknown = 0,
	WingspanBased = 0,
	SurfaceBearingWeightBased,
	WakeVortexWightBased,
	ApproachSpeedBased,
	NoneBased,

	CategoryType_Count,
};
const static CString FlightClassificationBasisType_Nname[] = 
{
	_T("Wing Span"),
	_T("Surface Breaking Weight"),
	_T("Wake Vortex Wight"),
	_T("Approach Speed"),
	_T("None"),



	_T("Count")
};
enum ApproachSeparationType
{
	LandingBehindLandingOnSameRunway = 0,
	LandingBehindLandingOnIntersectingRunway_LAHSO,
	LandingBehindLandingOnIntersectingRunway_NON_LAHSO,
	LandingBehindTakeoffOnSameRunway,
	LandingBehindTakeoffOnIntersectingRunway,

	ApproachSeparationType_Count,
};

enum FlightTypeStyle
{
	Heavy = 0,
	Medium,
	Light,

	FlightTypeStyle_Count,
};