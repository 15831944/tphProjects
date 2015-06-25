#pragma once


class  CLaneRange
{
public:
	CLaneRange();
	CLaneRange(int nStart, int nEnd);
	~CLaneRange(void);

public:
	int GetStartLane()const{	return m_nStartLane; }
	int GetEndLane()const{	return m_nEndLane; }
	void SetStartLane(int nStart){	m_nStartLane = nStart; }
	void SetEndLane(int nEnd){	m_nEndLane = nEnd; }
	CString MakeDatabaseString()const;
	void ParseDatabaseString(const CString& strDatabaseString);
	CString GetLaneRange()const;

private:

	int m_nStartLane;
	int m_nEndLane;
};
