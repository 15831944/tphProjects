// SimGeneralPara.h: interface for the CSimGeneralPara class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMGENERALPARA_H__494564D8_4E7A_4C98_8A12_93B61EF36A1F__INCLUDED_)
#define AFX_SIMGENERALPARA_H__494564D8_4E7A_4C98_8A12_93B61EF36A1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include "../Common/StartDate.h"

class ArctermFile;
class InputTerminal;
class CSimGeneralPara  
{
	//name
	//CString m_strParaName;
	//running times 
	int m_iNumberOfRun;
	// m_iRandomSpeed == -1 : true ramdom 
	// m_iRandomSpeed == other positive number :ramdom speed
	int m_iRandomSpeed;

	CStartDate	m_dateStart;
	ElapsedTime m_timeStart;
	ElapsedTime m_timeEnd;

	ElapsedTime	m_initPeriod;
	ElapsedTime m_followupPeriod;

	// m_lMaxCount == -1 : disable
	// m_lMaxCount == other positive number :max pax count
	int m_lMaxCount;

	BOOL m_bHaveNoPax;
	BOOL m_bEnableBarriers;
	BOOL m_bEnableHubbing;
	BOOL m_bEnableCongestionImpact;
	BOOL m_bEnableCollision;
	BOOL m_bEnableArea;
	BOOL m_bEnablePipe;
	std::vector<BOOL>m_vNonPaxDetailFlag;
	std::map<CString,int>m_vPipe;
	std::map<CString,int>m_vArea;

public:

	CSimGeneralPara( CStartDate _startDate );
	virtual ~CSimGeneralPara();
public:
	void SetFollowupPeriod(const ElapsedTime& _t);
	void SetInitPeriod(const ElapsedTime& _t);
	void SetEndTime(const ElapsedTime& _t);
	void SetStartTime(const ElapsedTime& _t);
	const ElapsedTime& GetFollowupPeriod() const;
	const ElapsedTime& GetInitPeriod() const;
	const ElapsedTime& GetEndTime() const;
	const ElapsedTime& GetStartTime() const;
	const CStartDate& GetStartDate() const;
	void SetRamdomSpeed( int _iSpeed ){ m_iRandomSpeed = _iSpeed;	};
	int GetRamdomSpeed()const { return m_iRandomSpeed;	};

	void SetMaxCount( long _lMaxCount ){ m_lMaxCount = _lMaxCount;	};
	long GetMaxCount()const { return m_lMaxCount;	};

	void EnableNoPax( BOOL _bEnable ){ m_bHaveNoPax = _bEnable;	};
	void EnableBarriers( BOOL _bEnable ){ m_bEnableBarriers = _bEnable;	};
	void EnableHubbing( BOOL _bEnable ){ m_bEnableHubbing = _bEnable;	};
	void EnableCongestionImpact(BOOL _bEnable){ m_bEnableCongestionImpact = _bEnable; }
	void EnableArea(BOOL _bEnable){m_bEnableArea = _bEnable;}
	void EnablePipe(BOOL _bEnable){m_bEnablePipe = _bEnable;}
	void EnableCollision(BOOL _bEnable){m_bEnableCollision = _bEnable;}
	BOOL IfNoPaxEnable()const{ return m_bHaveNoPax;	};
	BOOL IfBarriersEnable()const{ return m_bEnableBarriers;	};
	BOOL IfHubbingEnable()const{ return m_bEnableHubbing;	};
	BOOL IfCongestionImpactEnable()const { return m_bEnableCongestionImpact; }

	BOOL IfAreaEnable()const;
	BOOL IfPipeEnable()const;
	BOOL IfCollisionEnable()const;

	BOOL IfAreaItemEnable(const CString& sAreaName)const;
	BOOL IfPipeItemEnable(const CString& sPipeName)const;

	void EnableAreaItem(const CString& sAreaName,int bEnable);
	void EnablePipeItem(const CString& sPipeName,int bEnable);

	std::vector<BOOL>& GetNoPaxDetailFlag(){ return m_vNonPaxDetailFlag;	}
	void SetNoPaxDetailFlag( const std::vector<BOOL>& _vFlags ){ m_vNonPaxDetailFlag = _vFlags;	};

	std::map<CString,int>& GetAreaFlag(){return m_vArea;}
	void SetAreaFlag(const std::map<CString,int>& _vArea){m_vArea = _vArea;}

	std::map<CString,int>& GetPipeFlag(){return m_vPipe;}
	void SetPipeFlag(const std::map<CString,int>& _vPipe){m_vPipe = _vPipe;}

	//void SetParaName( CString _strName ){ m_strParaName = _strName;	}
	//CString GetParaName()const{ return m_strParaName;	}

	void SetNumberOfRun( int _iNumber ){ m_iNumberOfRun = _iNumber;	};
	int GetNumberOfRun()const { return m_iNumberOfRun;	}

	//virtual CStirng GetClassType
	virtual void WriteData( ArctermFile& _file );
	virtual void ReadData( ArctermFile& _file,InputTerminal* _interm );
	virtual void readObsoleteData( ArctermFile& _file,InputTerminal* _interm );
	void read2_6Data(ArctermFile& _file,InputTerminal* _interm );
	void read2_7Data(ArctermFile& _file,InputTerminal* _interm );
	virtual CString GetClassType()const { return "GENERAL";	};
	

};

#endif // !defined(AFX_SIMGENERALPARA_H__494564D8_4E7A_4C98_8A12_93B61EF36A1F__INCLUDED_)
