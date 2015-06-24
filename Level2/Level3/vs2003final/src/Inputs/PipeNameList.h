// PipeNameList.h: interface for the CPipeNameList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIPENAMELIST_H__EC4DDDA3_4F17_43F3_8928_69C0C2B74D59__INCLUDED_)
#define AFX_PIPENAMELIST_H__EC4DDDA3_4F17_43F3_8928_69C0C2B74D59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
class ArctermFile;

class CPipeNameList  
{
protected:
	std::vector<int> m_vPipeList;
public:
	CPipeNameList();
	virtual ~CPipeNameList();

	int GetSize() const;
	int GetPipeIdx( int _nIdx ) const;
	std::vector<int>& GetPipeVector() { return m_vPipeList;	};
	void AddPipeIdx( int _iPipeIdx );
	void WriteData( ArctermFile& p_file)const;
	void ReadData( ArctermFile& p_file);
};

#endif // !defined(AFX_PIPENAMELIST_H__EC4DDDA3_4F17_43F3_8928_69C0C2B74D59__INCLUDED_)
