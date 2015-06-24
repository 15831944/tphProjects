// Walkthroughs.cpp: implementation of the CWalkthroughs class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Walkthroughs.h"

#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWalkthroughs::CWalkthroughs(InputFiles fileSort) : DataSet(fileSort, 2.3f)
{

}

CWalkthroughs::~CWalkthroughs()
{
	clear();
}

void CWalkthroughs::clear()
{
	m_vNames.clear();
	int nCount = m_vWalkthroughs.size();
	for(int i=0; i<nCount; i++) {
		delete m_vWalkthroughs[i];
	}
	m_vWalkthroughs.clear();

}

void CWalkthroughs::readData (ArctermFile& p_file)
{
	char s[256];
	p_file.getLine();
	int nMovieCount;
	p_file.getInteger(nMovieCount);
	m_vWalkthroughs.reserve(nMovieCount);
	m_vNames.reserve(nMovieCount);

	for(int i=0; i<nMovieCount; i++) {
		p_file.getLine();
		p_file.getField(s, 255);
		m_vNames.push_back(s);
		CWalkthrough* pWT = CWalkthrough::CreateFromFile(p_file);
		m_vWalkthroughs.push_back(pWT);
	}
}

void CWalkthroughs::readObsoleteData (ArctermFile& p_file)
{
	char s[256];
	p_file.getLine();
	int nMovieCount;
	p_file.getInteger(nMovieCount);
	m_vWalkthroughs.reserve(nMovieCount);
	m_vNames.reserve(nMovieCount);

	for(int i=0; i<nMovieCount; i++) {
		p_file.getLine();
		p_file.getField(s, 255);
		m_vNames.push_back(s);
		CWalkthrough* pWT = CWalkthrough::CreateFromObsoleteFile(p_file);
		m_vWalkthroughs.push_back(pWT);
	}
}

void CWalkthroughs::writeData (ArctermFile& p_file) const
{
	int nMovieCount = m_vWalkthroughs.size();
	p_file.writeInt(nMovieCount);
	p_file.writeLine();

	for(int i=0; i<nMovieCount; i++) {
		p_file.writeField(m_vNames[i]);
		p_file.writeLine();
		CWalkthrough::WriteToFile(*m_vWalkthroughs[i], p_file);
	}
}

int CWalkthroughs::AddWalkthrough(EnvironmentMode _envmode, const CString& _s)
{
	m_vNames.push_back(_s);
	m_vWalkthroughs.push_back(new CWalkthrough(_envmode));
	return m_vWalkthroughs.size();
}

void CWalkthroughs::DeleteWalkthrough(int _idx)
{
	m_vNames.erase(m_vNames.begin()+_idx);
	delete m_vWalkthroughs[_idx];
	m_vWalkthroughs.erase(m_vWalkthroughs.begin()+_idx);
}

void CWalkthroughs::DeleteWalkthrough( CWalkthrough* pWalkthrough )
{
	std::vector< CWalkthrough* >::iterator ite = std::find(m_vWalkthroughs.begin(), m_vWalkthroughs.end(), pWalkthrough);
	if (ite != m_vWalkthroughs.end())
	{
		std::vector< CString >::iterator iteName = m_vNames.begin() + (ite - m_vWalkthroughs.begin());
		m_vNames.erase(iteName);
		m_vWalkthroughs.erase(ite);
		delete pWalkthrough;
		return;
	}
	ASSERT(FALSE);
}
void CWalkthroughs::MoveWalthroughToOther( CWalkthroughs* otherWalkthroughs, CWalkthrough* pWalkthrough )
{
	std::vector< CWalkthrough* >::iterator ite = std::find(m_vWalkthroughs.begin(), m_vWalkthroughs.end(), pWalkthrough);
	if (ite != m_vWalkthroughs.end())
	{
		std::vector< CString >::iterator iteName = m_vNames.begin() + (ite - m_vWalkthroughs.begin());
		otherWalkthroughs->m_vWalkthroughs.push_back(pWalkthrough);
		otherWalkthroughs->m_vNames.push_back(*iteName);

		m_vNames.erase(iteName);
		m_vWalkthroughs.erase(ite);
		return;
	}
	ASSERT(FALSE);

}

void CWalkthroughs::SetWalkthroughName( CWalkthrough* pWalkthrough, const CString& _s )
{
	std::vector< CWalkthrough* >::iterator ite = std::find(m_vWalkthroughs.begin(), m_vWalkthroughs.end(), pWalkthrough);
	if (ite != m_vWalkthroughs.end())
	{
		std::vector< CString >::iterator iteName = m_vNames.begin() + (ite - m_vWalkthroughs.begin());
		*iteName = _s;
		return;
	}
	ASSERT(FALSE);
}