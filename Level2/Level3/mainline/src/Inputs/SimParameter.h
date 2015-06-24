// SimParameter.h: interface for the CSimParameter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPARAMETER_H__D1579635_1AA3_4841_99AD_4BFF7C6FD3A9__INCLUDED_)
#define AFX_SIMPARAMETER_H__D1579635_1AA3_4841_99AD_4BFF7C6FD3A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\common\template.h"
#include "inputsdll.h"
#include "..\common\dataset.h"
#include<vector>
#include"reports\simgeneralpara.h"
#include "../common/StartDate.h"
class INPUTS_TRANSFER CSimParameter : public DataSet
{
	CSimGeneralPara* m_pSimPara;
	CString m_sSimName;
	CStartDate m_startDate;		// temporary used.

	// all pax in flight between start time and end time will be generate
	/*CStartDate	m_dateStart;
	ElapsedTime m_timeStart;
	CStartDate	m_dateEnd;
	ElapsedTime m_timeEnd;

	ElapsedTime	m_initPeriod;
	ElapsedTime m_followupPeriod;*/
	//std::vector<CSimGeneralPara*>m_vAllSimPara;
public:
	void SetFollowupPeriod(const ElapsedTime& _t);
	void SetInitPeriod(const ElapsedTime& _t);
	const ElapsedTime& GetFollowupPeriod() const;
	const ElapsedTime& GetInitPeriod() const;
	void SetStartDate(const CStartDate& _date){ m_startDate = _date; }
	const CStartDate& GetStartDate() const;
	CSimParameter();
	virtual ~CSimParameter();


    virtual void clear ();

	CString GetSimName() const { return m_sSimName;	};
	void SetSimName( CString _sName ){ m_sSimName = _sName;	}
	virtual void readData(ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);
    virtual void writeData(ArctermFile& p_file) const;

    virtual const char *getTitle(void) const { return "Miscellaneous Simulation Paramters"; }
    virtual const char *getHeaders (void) const{  return "NUMBER OF RUN,SEEDRANDOM,MAX_PAX_COUNT,NOPAX,BARRIERS,HUBBING"; }

	int GetRandomFlag();
	
	unsigned int GetSeed(){ return m_pSimPara->GetRamdomSpeed(); }
	void SetSeed( unsigned int _uSeed ){ m_pSimPara->SetRamdomSpeed( _uSeed ); }

	void SetPaxCount( long _lCount ){ m_pSimPara->SetMaxCount( _lCount ); }
	long GetPaxCount(){ return m_pSimPara->GetMaxCount(); }

	int GetVisitorsFlag(){ return m_pSimPara->IfNoPaxEnable(); }
	void SetVisitorsFlag( int _nVisitorsFlag ){ m_pSimPara->EnableNoPax( _nVisitorsFlag ); }

	int GetBaggageFlag(){ return 0; }//no use
	void SetBaggageFlag( int _nBaggageFlag ){ }

	
	int GetBarrierFlag(){ return m_pSimPara->IfBarriersEnable(); }
	int GetHubbingFlag(){ return m_pSimPara->IfHubbingEnable(); }
	int GetCongestionImpactFlag(){ return m_pSimPara->IfCongestionImpactEnable(); }
	int GetCollisionFlag(){return m_pSimPara->IfCollisionEnable();}
	int GetAreaFlag(){return m_pSimPara->IfAreaEnable();}
	int GetPipFlage(){return m_pSimPara->IfPipeEnable();}
	int GetAreaItemFlag(const CString& sAreaName){return m_pSimPara->IfAreaItemEnable(sAreaName);}
	int GetPipeItemFlag(const CString& sPipeName){return m_pSimPara->IfPipeItemEnable(sPipeName);}
	void SetRandomFlag( int _nRandomFlag );

	

	
	
	void SetBarrierFlag( int _nBarrierFlag ){ m_pSimPara->EnableBarriers( _nBarrierFlag );}
	void SetHubbingFlag( int _nHubbingFlag ){ m_pSimPara->EnableHubbing( _nHubbingFlag );}
	void SetCongestionFlag( int _bEnable){ m_pSimPara->EnableCongestionImpact(_bEnable); }
	void SetCollisionFlag(int _bEnable){m_pSimPara->EnableCollision(_bEnable);}
	void SetAreaFlag(int _bEnable){m_pSimPara->EnableArea(_bEnable);}
	void SetPipeFlag(int _bEnable){m_pSimPara->EnablePipe(_bEnable);}
	void SetPipeItemFlag(const CString& sPipeName,BOOL bEnable){m_pSimPara->EnablePipeItem(sPipeName,bEnable);}
	void SetAreaItemFlag(const CString& sAreaName,BOOL bEnable){m_pSimPara->EnableAreaItem(sAreaName,bEnable);}

	void SetSimPara( CSimGeneralPara* _pSimPara) { m_pSimPara = _pSimPara ;}
	CSimGeneralPara* GetSimPara(){ return m_pSimPara;	}

	//get, set flags which take effect on whether or not have such kind of no pax
	std::vector<BOOL>& GetNoPaxDetailFlag(){ return m_pSimPara->GetNoPaxDetailFlag();	};
	void SetNoPaxDetailFlag( const std::vector<BOOL>& _vFlags ){ m_pSimPara->SetNoPaxDetailFlag( _vFlags );	}

	void SetStartTime( const ElapsedTime& _timeStart ) { m_pSimPara->SetStartTime(_timeStart);	}
	void SetEndTime( const ElapsedTime& _timeEnd ) { m_pSimPara->SetEndTime(_timeEnd);	}

	const ElapsedTime& GetStartTime()const { return m_pSimPara->GetStartTime();	}
	const ElapsedTime& GetEndTime()const { return m_pSimPara->GetEndTime();	}
};

#endif // !defined(AFX_SIMPARAMETER_H__D1579635_1AA3_4841_99AD_4BFF7C6FD3A9__INCLUDED_)
