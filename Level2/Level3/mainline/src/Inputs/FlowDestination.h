// FlowDestination.h: interface for the CFlowDestination class.
#pragma once
#include <CommonData/procid.h>
#include "AreasPortals.h"

typedef std::vector<int>PIPES;

class CFlowDestination  
{
private:	
	int m_iTypeOfOwnerShip;	// 0 mine; 1 inherited; 2 combined
	bool m_bHasVisited;
	ProcessorID m_procID;

	/*************** Flow condition *************/
	int m_iProbality;
	long m_lMaxQueueLength;
	long m_lMaxWaitMins;
	long m_lMaxSkipTime;
	/********************************************/
	
	/***************** Channel condition **********/
	int m_iIsOneToOne;
	int m_nOneXOneState;    // 0 :nonChannel
							// 1 :channelStart
							// 2 :channelEnd
	/********************************************/	

	/***************** Route condition **********/
	int m_iTypeOfUsingPipe;	// m_iTypeOfUsingPipe = 0 use nothing
							// m_iTypeOfUsingPipe = 1 use pipe system
							// m_iTypeOfUsingPipe = 2 use user selected pipes
	PIPES m_vUsedPipes;
	int m_iAvoidFixQueue;
	int m_iAvoidOverFlowQueue;
	int m_iAvoidDensity;
	double m_dDensityOfArea; /* xx sq m / pax */

	// areas name
	std::vector<CString>m_vAreas;
	/********************************************/	

   /*********************************************/
	bool m_bIsFollowInFlow; //0 if lead in flow; 1 if follow with others in flow
		
public:
	CFlowDestination();
	virtual ~CFlowDestination();

	bool GetFollowState() const;
	void SetFollowState(bool _state);
	void SetOneXOneState(int _State);
	int GetOneXOneState() const;
	void SetTypeOfOwnership( int _iType ){ m_iTypeOfOwnerShip = _iType;	};
	int GetTypeOfOwnership() const { return m_iTypeOfOwnerShip;	};

	void SetVisitFlag( bool _bFlag ){ m_bHasVisited = _bFlag;	};
	bool GetVisitFlag() const { return m_bHasVisited;	};

	void SetProcID( const ProcessorID& _procID ){ m_procID = _procID;	};
	const ProcessorID&  GetProcID() const {  return m_procID;	};
	
	/*************** Flow condition *************/
	void SetProbality( int _iProbality ){ m_iProbality = _iProbality;};
	int GetProbality() const { return m_iProbality ;};

	void SetMaxQueueLength( long _lQueueLength ){ m_lMaxQueueLength = _lQueueLength;}
	long GetMaxQueueLength()const { return m_lMaxQueueLength ;}

	void SetMaxWaitMins( long _lMins ){ m_lMaxWaitMins = _lMins;}
	long GetMaxWaitMins()const { return m_lMaxWaitMins ;}

	void SetMaxSkipTime(long _lSkipTime){m_lMaxSkipTime=_lSkipTime;}
	long GetMaxSkipTime()const{return m_lMaxSkipTime;}
	/********************************************/

	/***************** Channel condition **********/
	void SetOneToOneFlag( int _iFlag ){ m_iIsOneToOne = _iFlag;	};
	int GetOneToOneFlag() const { return m_iIsOneToOne;	};
	/********************************************/

	/***************** Route condition **********/
	void SetTypeOfUsingPipe( int _iType ) { m_iTypeOfUsingPipe = _iType;	};
	int GetTypeOfUsingPipe() const { return m_iTypeOfUsingPipe;	};

	void AddUsedPipe(int _iIdxOfPipe ){ m_vUsedPipes .push_back( _iIdxOfPipe );};
	int GetUsedPipeCount() const { return m_vUsedPipes.size();	};
	 PIPES& GetPipeVector() { return m_vUsedPipes;	};
	int GetPipeAt( int _iIdx ) const ;
	void ClearAllPipe(){ m_vUsedPipes.clear();	};
	bool IfUseThisPipe(int nCurPipeIndex)const;

	void SetAvoidFixQueue( int _iAvoidFixQueue ) { m_iAvoidFixQueue = _iAvoidFixQueue;	}
	int GetAvoidFixQueue()const { return m_iAvoidFixQueue;	}

	void SetAvoidOverFlowQueue( int _iAvoidOverFlowQueue ) { m_iAvoidOverFlowQueue = _iAvoidOverFlowQueue;	}
	int GetAvoidOverFlowQueue()const { return m_iAvoidOverFlowQueue;	}

	void SetAvoidDensity( int _iAvoidDensity ) { m_iAvoidDensity = _iAvoidDensity;	}
	int GetAvoidDensity()const { return m_iAvoidDensity;	}

	void SetDensityOfArea( double _dDensity ) { m_dDensityOfArea = _dDensity;	}
	double GetDensityOfArea()const { return m_dDensityOfArea;	}
	
	const std::vector<CString>& GetDensityArea() const{ return m_vAreas;}
	std::vector<CString>& GetDensityArea() { return m_vAreas;}
	int operator == ( const CFlowDestination& _anotherInstance ) const { return m_procID == _anotherInstance.m_procID ;};
	bool operator != (const CFlowDestination& _anotherInstance ) const;
};
