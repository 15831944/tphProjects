#pragma once

class OnboardFlightInSim;
class OnboardDeckGroundInSim;
class CDeckManager;
class CDeck;
class GroundInSim;
class OnboardAircraftElementInSim;
class OnboardCellInSim;
class OnboardCellPath;
class ARCVector3;


class OnboardCellGraph
{
public:
	OnboardCellGraph(OnboardFlightInSim *pFlight);
	virtual ~OnboardCellGraph(void);

public:
	void AddDeckGroundInSim(OnboardDeckGroundInSim*) ;


	void InitGraph(CDeckManager* pDeckManger);

	OnboardDeckGroundInSim* GetDeckInSim(CDeck* pDeck);
	OnboardDeckGroundInSim* GetDeckInSim(int nDeck);
	GroundInSim* GetGroundInSim(CDeck* pDeck);
	void setOnboardCellBarrier();

	//delete all the information
	void Reset();
	
	bool FindPath( const OnboardAircraftElementInSim* pEntryElementInSim, const OnboardAircraftElementInSim* pExitElementInSim, OnboardCellPath& _movePath );
	//find path


	//object
	bool FindPath(const ARCVector3& sourcePoint, const ARCVector3& destPoint, 
		OnboardCellPath& _movePath);

	bool FindPath(const OnboardAircraftElementInSim* pEntryElementInSim, const ARCVector3& destPoint, 
		OnboardCellPath& _movePath);

	bool FindPath(const ARCVector3& sourcePoint, const OnboardAircraftElementInSim* pExitElementInSim, 
		OnboardCellPath& _movePath);

	bool FindPath(OnboardCellInSim *pOriginCell, OnboardCellInSim *pDestCell,
		OnboardCellPath& _movePath);


	void loadFile();

protected:
    OnboardCellInSim* GetElementCellInDeck( const OnboardAircraftElementInSim* pElementInSim );

protected:
	std::vector<OnboardDeckGroundInSim* > m_vGroundInSimList;

	OnboardFlightInSim *m_pFlightInSim;

public:
	std::map< std::pair<OnboardCellInSim*, OnboardCellInSim *>, OnboardCellPath > m_mapCellPath;

	bool m_bLoadFile;

};
