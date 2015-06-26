#pragma once

enum RunwayAssignmentStrategyType
{
	FlightPlannedRunway = 0,
	ClosestRunway,
	ManagedAssignment,



	RunwayAssignmentStrategyType_Count
};

enum RunwayExitStrategyType
{
	MaxBrakingFirstOnEitherSide = 0,
	MaxBrakingFirstOnSameSideAsArrivalGate,
	NormalBrakingExitOnEitherSide,
	NormalBrakingExitOnSameSide,
	ManagedExit,



	LandingRunwayExitStrategyType_Count

};
