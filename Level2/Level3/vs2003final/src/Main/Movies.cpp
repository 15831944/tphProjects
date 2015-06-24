// Movies.cpp: implementation of the CMovies class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "Movie.h"
#include "Movies.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMovies::CMovies() : DataSet( MoviesFile, 2.3f )
{
}

CMovies::~CMovies()
{
	clear();
}


void CMovies::clear()
{
	//m_vNames.clear();
	int nCount = m_vMovies.size();
	for(int i=0; i<nCount; i++) {
		delete m_vMovies[i];
	}
	m_vMovies.clear();
}

void CMovies::initDefaultValues()
{
}

void CMovies::readData (ArctermFile& p_file)
{
	p_file.getLine();
	int nMovieCount;
	p_file.getInteger(nMovieCount);
	m_vMovies.reserve(nMovieCount);
	//m_vNames.reserve(nMovieCount);

	p_file.getLine();
	for(int i=0; i<nMovieCount; i++) {
		CMovie* pMovie = new CMovie;
		pMovie->ReadFromFile(p_file);
		m_vMovies.push_back(pMovie);
	}
}

void CMovies::readObsoleteData (ArctermFile& p_file)
{
	char s[256];
	p_file.getLine();
	int nMovieCount;
	p_file.getInteger(nMovieCount);
	m_vMovies.reserve(nMovieCount);
	//m_vNames.reserve(nMovieCount);

	p_file.getLine();
	for(int i=0; i<nMovieCount; i++) {
		p_file.getField(s, 255);
		p_file.getLine();
		CMovie* pMovie = new CMovie;
		pMovie->ReadObsoleteFromFile(p_file);
		pMovie->SetName(s);
		m_vMovies.push_back(pMovie);
	}
}

void CMovies::writeData (ArctermFile& p_file) const
{
	int nMovieCount = m_vMovies.size();
	p_file.writeInt(nMovieCount);
	p_file.writeLine();

	for(int i=0; i<nMovieCount; i++) {
		//p_file.writeField(m_vNames[i]);
		//p_file.writeLine();
		m_vMovies[i]->WriteToFile(p_file);
	}
}

int CMovies::AddMovie(const CString& _s)
{
	//m_vNames.push_back(_s);
	m_vMovies.push_back(new CMovie(_s));
	return m_vMovies.size();
}

void CMovies::DeleteMovie(int _idx)
{
	//m_vNames.erase(m_vNames.begin()+_idx);
	delete m_vMovies[_idx];
	m_vMovies.erase(m_vMovies.begin()+_idx);
}