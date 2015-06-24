// Movie.h: interface for the CMovie class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVIE_H__FAAC5869_77DB_477E_AD03_8CB05E0E62C5__INCLUDED_)
#define AFX_MOVIE_H__FAAC5869_77DB_477E_AD03_8CB05E0E62C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class ArctermFile;
class C3DCamera;
class CKeyFrame;
class CCameraData;

class CMovie  
{
public:
	int FindRelationKeyFrame(void* pVoid);
	void SetRelationKeyFrame(int i,void* pVoid);
	CString GetKFRunTimeFormated(int _idxEnd,int _idxBegin=0);
	CMovie();
	CMovie(const CString& _sName);
	virtual ~CMovie();

	int GetKFCount() const;
	const C3DCamera& GetKFCamera(int _idx) const;
	long GetKFTime(int _idx) const;
	long GetKFRunTime(int _idxEnd,int _idxBegin=0) const; //in 1/100 seconds
	int GetAnimSpeed() const { return m_nAnimSpeed; }
	const CString& GetName() const { return m_sName; }
	void Interpolate(long _time, C3DCamera& _cam) const; //sets _cam to interpolation at time _time
	int InsertKF(const CCameraData& _cam, long _time);
	int DeleteKF(long _time);
	int MoveKF(long _fromtime, long _totime);
	void SetAnimSpeed(int _speed) { m_nAnimSpeed = _speed; }
	void SetName(const CString& _sName);

	void ReadFromFile(ArctermFile& _file);
	void ReadObsoleteFromFile(ArctermFile& _file);
	void WriteToFile(ArctermFile& _file) const;

protected:
	void Recalc();

protected:
	BOOL m_bNeedsRecalc;
	std::vector<CKeyFrame*> m_vKFVec;
	int m_nAnimSpeed; //speed x 10 --> m_nAnimSpeed = 50 => 5x
	CString m_sName;
};

#endif // !defined(AFX_MOVIE_H__FAAC5869_77DB_477E_AD03_8CB05E0E62C5__INCLUDED_)
