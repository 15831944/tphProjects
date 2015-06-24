// StationLayout.h: interface for the CStationLayout class.
#pragma once
#include "..\engine\Car.h"
#include <CommonData\StationLayoutBase.h>

typedef std::vector<CCar*> CAR;

class CStationLayout : public CStationLayoutBase
{
private:
	CAR m_cars;
	HoldingArea* m_preBoardingArea;

protected:
	//project path
	CString m_strPath;

		
public:
	CStationLayout();
	CStationLayout(const CStationLayout& _anotherLayout);
	CStationLayout& operator = ( const CStationLayout& _anotherLayout );
	virtual ~CStationLayout();
public:

	void SetCarCapacity( int _nNumber );

	BOOL ComputeLayoutInfo(); 

	virtual void readData (ArctermFile& p_file );
	int GetCarCount() const { return m_cars.size();	};
	CCar* GetCar( int _nIdx ) { return m_cars[_nIdx];	}
	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Station Layout" ;};
	virtual const char *getHeaders (void) const {return "adjusted point,,,,cars,,,,,,,preBoarding area,,,,,, " ;};
	virtual void clear (void);

	
	HoldingArea* GetPreBoardingArea() {  return m_preBoardingArea;	};
    void SetPreBoardingArea(HoldingArea *_pArea){m_preBoardingArea = _pArea; }
	void DoOffSet( DistanceUnit _X, DistanceUnit _Y );

	void GetAllCars( CAR& _allCar) const;
	CAR& GetAllCars()  { return m_cars; };
	void AddNewCar( CCar* _pNewCar ){ m_cars.push_back( _pNewCar );	};
	void DeleteCar( int _nIdx );

	void setProjectPath(const CString& strPath){m_strPath = strPath;}
	CString getProjectPath() const{return m_strPath;}

};
