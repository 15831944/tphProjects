// Movies.h: interface for the CMovies class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVIES_H__F304643A_4FC7_4FAF_810F_A552DAA12B5B__INCLUDED_)
#define AFX_MOVIES_H__F304643A_4FC7_4FAF_810F_A552DAA12B5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\DataSet.h"
#include <vector>

class CMovie;

class CMovies : public DataSet
{
public:
	CMovies();
	virtual ~CMovies();

	virtual void clear();
	virtual void initDefaultValues();

	virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData ( ArctermFile& p_file );
	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Movies Data"; }
    virtual const char *getHeaders (void) const{  return ".,..,..,,,"; }

	int AddMovie(const CString& _s);
	void DeleteMovie(int _idx);
	//const CString& GetMovieName(int _idx) const { return m_vNames[_idx]; }
	CMovie* GetMovie(int _idx) { return m_vMovies[_idx]; }
	//void SetMovieName(int _idx, const CString& _s) { m_vNames[_idx] = _s; }
	int GetMovieCount() const { return m_vMovies.size(); }

	//void SetCurrentMovie(int _idx);
	//int GetCurrentMovieIdx() const { return m_nCurrentMovie; }
	//CMovie* GetCurrentMovie();


protected:
	std::vector<CMovie*> m_vMovies;
	//std::vector<CString> m_vNames;
	//int m_nCurrentMovie;

};

#endif // !defined(AFX_MOVIES_H__F304643A_4FC7_4FAF_810F_A552DAA12B5B__INCLUDED_)
