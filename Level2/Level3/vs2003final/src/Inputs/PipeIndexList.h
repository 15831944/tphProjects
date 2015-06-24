// PipeIndexList.h: interface for the CPipeIndexList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIPEINDEXLIST_H__8F563B03_0539_453F_9C50_106213ACE399__INCLUDED_)
#define AFX_PIPEINDEXLIST_H__8F563B03_0539_453F_9C50_106213ACE399__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "common\termfile.h"

class CPipeIndexList  
{
protected:

	// m_iTypeOfUsingPipe = 0 use nothing
	// m_iTypeOfUsingPipe = 1 use pipe system
	// m_iTypeOfUsingPipe = 2 use user selected pipes
	int m_iTypeOfUsingPipe;

	// 1= yes
	// 0 = no
	int m_iIsOneToOne;
	std::vector<int> m_vPipeList;
public:
	CPipeIndexList();
	virtual ~CPipeIndexList();
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
};

#endif // !defined(AFX_PIPEINDEXLIST_H__8F563B03_0539_453F_9C50_106213ACE399__INCLUDED_)
