#pragma once

#define  LAT_NORTH  "N"
#define  LAT_SOUTH	"S"
#define  LONG_EAST	"E"
#define  LONG_WEST  "W"


class ArctermFile;
class CLatitude;

class CLatLong
{
public:
	CLatLong(void);
	~CLatLong(void);

	//member
public:
	CString m_dir;
	unsigned int m_nDeg;
	unsigned int m_nMinutes;
	unsigned int m_nSeconds;
	unsigned int m_nDelta;
//operation
public :
	void SetValue(CString strValue);
	void SetValue(CString strValueLeft, CString strValueRight);

	virtual	void ReadData(ArctermFile &file);
	virtual	void WriteData(ArctermFile &file);

	int SplitString(LPCTSTR lpszString,int nStrLen,LPCTSTR lpszSep,CStringArray& strArray);
	int SplitString(LPCTSTR lpszString,int nStrLen,int chSep,CStringArray& strArray);
	void GetValue(CString &strValue,BOOL nFlags=TRUE);

protected:

private:

};

class CLatitude :public CLatLong
{

	public:
		CLatitude();
		CLatitude(long sec);
		~CLatitude();

	//member
public:

	//operation
public :

};

//////////////////////////////////////////////////////////////////////////
//class longitude
class CLongitude :public CLatLong
{

	public:
		CLongitude();
		CLongitude(long sec);
		~CLongitude();

	//member
public:

	//operation
public :

};