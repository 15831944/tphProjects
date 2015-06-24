#ifndef CONVERT_ACGATETOSTAND_HELPER_H
#define CONVERT_ACGATETOSTAND_HELPER_H

#include "proclist.h"
#include "gate.h"
#include "Inputs\In_term.h"
#include "Inputs\AirsideInput.h"
#pragma once
//
#include "dep_srce.h"
#include "dep_sink.h"
#include "lineproc.h"
#include "baggage.h"
#include "hold.h"
#include "barrier.h"
#include "FLOORCHG.h"
#include "ApronProc.h"
#include "conveyor.h"
#include "DeiceBayProc.h"
#include "ElevatorProc.h"
#include "Escalator.h"
#include "FixProc.h"
#include "MovingSideWalk.h"
#include "RunwayProc.h"
#include "Stair.h"
#include "TaxiwayProc.h"
#include "StandProc.h"
#include "arp.h"
#include "IntegratedStation.h"
#include "NodeProc.h"
#include "contourproc.h"

class DependentSource;
class DependentSink;
class LineProcessor;
class BaggageProcessor;
class HoldingArea;
class Barrier;
class FloorChangeProcessor;


class IntegratedStation;
class MovingSideWalk;
class ElevatorProc;
class Conveyor;
class Stair;
class Escalator;
class ArpProc;
class RunwayProc;
class TaxiwayProc;
class StandProc;
class NodeProc;
class DeiceBayProc;
class FixProc;
class ContourProc;
class ApronProc;

//
class ProcessorListEx;
class GateProcessorEx;
class CPlacementEx;
class FakeInputTerminal;
class CAirsideInput;


class CConvertACGateToStandHelper
{
protected:
	CString m_strPrjPath;
	FakeInputTerminal * m_pFakeInputTerminal;

public:
//	CConvertACGateToStandHelper(void);
	~CConvertACGateToStandHelper(void);

	CConvertACGateToStandHelper(CString& strPath);

	void Convert();
	void WriteFiles();
	StandProc* ConvertACGateToStand(GateProcessorEx* acGate);
};

//

class ProcessorEx : public Processor
{
	friend ProcessorListEx;
};

class ProcessorListEx : public ProcessorList
{
public:
	ProcessorListEx( StringDictionary* _pStrDict );
	ProcessorListEx( StringDictionary* _pStrDict,InputFiles fileType);

protected:
	std::vector<GateProcessorEx*> m_vACGate;

public:
	std::vector<GateProcessorEx*>& GetACGates(){	return m_vACGate;	}
	Processor *readOldProcessorVer22 (ArctermFile& procFile, bool _bFirst = false );
	Processor *readProcessor (ArctermFile& procFile, bool _bFirst = false );
	virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData ( ArctermFile& p_file);

};

//
class GateProcessorEx : public GateProcessor
{
protected:
	BOOL fCanBeACStandGate;

public:
	GateProcessorEx(void);
	~GateProcessorEx(void);

	virtual int readSpecialField(ArctermFile & file);
	BOOL getACStandGateFlag(){	return fCanBeACStandGate; };
};

class CPlacementEx : public CPlacement
{
protected:
	//std::pair<Processor*, CProcessor2>
	std::vector<CProcessor2*> m_vACGateProc2;
	std::vector<GateProcessorEx*> m_vACGate;
public:
	virtual void readData (ArctermFile& p_file);
	void SetACGates(std::vector<GateProcessorEx*> & vACGate) {	m_vACGate = vACGate;	}
	
	//Just used In Ternimal's Placement
	std::vector<CProcessor2*>& GetACGatesPlacement() {	return m_vACGateProc2;	}
	
	//Just used In Airside's Placement
	void AppendACGatesPlacement(std::vector<CProcessor2*>&);

};


class FakeInputTerminal : public InputTerminal
{
public:

protected:
	// Miscellaneous Paramters
	
	ProcessorListEx * m_pProcList;
	CPlacementEx    * m_pPlacement;

public:
	FakeInputTerminal();
	~FakeInputTerminal();

	void loadInputs ( const CString& _pProjPath );

	ProcessorListEx* GetProcListEx(){	return m_pProcList;	}
	CPlacementEx*    GetPlacement() {	return m_pPlacement;	}

};





#endif//