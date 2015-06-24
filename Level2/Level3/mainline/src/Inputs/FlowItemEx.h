// FlowItemEx.h: interface for the CFlowItemEx class.
#pragma once
#include "../common/termfile.h"
#include <CommonData/procid.h>
class CRule;




class CFlowItemEx  
{
protected:
	/*************** Flow condition *************/	
	long m_lMaxQueueLength;
	long m_lMaxWaitMins;
	long m_lMaxSkipTime;
	/********************************************/
	

	/***************** Channel condition **********/
	// 1= yes
	// 0 = no
	int m_iIsOneToOne;
	/********************************************/	

	/***************** Route condition **********/
	
	//  0 use nothing
	//  1 use pipe system
	//  2 use user selected pipes
	int m_iTypeOfUsingPipe;
	std::vector<int> m_vPipeList;

	int m_iAvoidFixQueue;
	int m_iAvoidOverFlowQueue;
	int m_iAvoidDensity;
	double m_dDensityOfArea;/* xx sq m / pax */
	std::vector<CString>m_vAreas;
	/********************************************/	
	
	int m_nOneXOneState; // 0 : NonChannel
						 // 1 : ChannelStart
						 //	2 : ChannelEnd
    /*********************************************/
	bool m_bIsFollowInFlow;    //0 if lead in flow 
							//1 if follow with others in flow
	
	std::vector<CRule*> m_vRules;
	void ClearRules();

public:

	bool GetFollowState() const;
	void SetFollowState(bool _state);
	int GetChannelState() const;
	void SetChannelState(int _state);
	CFlowItemEx();
	CFlowItemEx(const CFlowItemEx& _other);
	virtual ~CFlowItemEx();

	void SetTypeOfUsingPipe( int _iType ) { m_iTypeOfUsingPipe = _iType;	};
	int GetTypeOfUsingPipe() const { return m_iTypeOfUsingPipe;	};
	
	void SetOneToOneFlag( int _iFlag ){ m_iIsOneToOne = _iFlag;	};
	int GetOneToOneFlag() const { return m_iIsOneToOne;	};
	int GetSize() const;
	int GetPipeIdx( int _nIdx ) const;
	std::vector<int>& GetPipeVector() { return m_vPipeList;	};
	void AddPipeIdx( int _iPipeIdx );
	void WriteData( ArctermFile& p_file)const;
	void ReadData( ArctermFile& p_file);

	bool IfUseThisPipe( int _iPipeIdx )const;
	void DeletePipeOrAdjust( int _iPipeIdx );

	/*************** Flow condition *************/	
	void SetMaxQueueLength( long _lQueueLength ){ m_lMaxQueueLength = _lQueueLength;}
	long GetMaxQueueLength()const { return m_lMaxQueueLength ;}

	void SetMaxWaitMins( long _lMins ){ m_lMaxWaitMins = _lMins;}
	long GetMaxWaitMins()const { return m_lMaxWaitMins ;}
	 
	void SetMaxSkipTime(long _lSkipTime){m_lMaxSkipTime=_lSkipTime;}
	long GetMaxSkipTime()const{return m_lMaxSkipTime;}
	/********************************************/


	/***************** Route condition **********/
	void SetAvoidFixQueue( int _iAvoidFixQueue ) { m_iAvoidFixQueue = _iAvoidFixQueue;	}
	int GetAvoidFixQueue()const { return m_iAvoidFixQueue;	}

	void SetAvoidOverFlowQueue( int _iAvoidOverFlowQueue ) { m_iAvoidOverFlowQueue = _iAvoidOverFlowQueue;	}
	int GetAvoidOverFlowQueue()const { return m_iAvoidOverFlowQueue;	}

	void SetAvoidDensity( int _iAvoidDensity ) { m_iAvoidDensity = _iAvoidDensity;	}
	int GetAvoidDensity()const { return m_iAvoidDensity;	}

	void SetDensityOfArea( double _dDensity ) { m_dDensityOfArea = _dDensity;	}
	double GetDensityOfArea()const { return m_dDensityOfArea;	}
	
	std::vector<CString>& GetDensityArea() { return m_vAreas;}
	void SetDesityArea(const std::vector<CString>& vArea){m_vAreas = vArea;}
	/********************************************/
	void GetRules(std::vector<const CRule*>& _vRules) const;
	void InitRules();
	const CFlowItemEx& operator=(const CFlowItemEx& _other);
};


