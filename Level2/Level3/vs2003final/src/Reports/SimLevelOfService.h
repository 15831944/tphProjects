// SimLevelOfService.h: interface for the CSimLevelOfService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMLEVELOFSERVICE_H__3DFC27EF_669C_4D64_8830_57A8150E6DCE__INCLUDED_)
#define AFX_SIMLEVELOFSERVICE_H__3DFC27EF_669C_4D64_8830_57A8150E6DCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include<map>
#include "SimGeneralPara.h"
#include "inputs\MobileElemConstraint.h"

typedef struct
{
	// m_iUpperQueueLength == -1 || m_iLowerQueueLength == -1 : no this criterion	
	int m_iUpperQueueLength;
	int m_iLowerQueueLength;

	CString m_strAreaName;
	// m_iUpperAreaDensity == -1 || m_iLowerAreaDensity == -1 : no this criterion
	int m_iUpperAreaDensity;
	int m_iLowerAreaDensity;

	// m_lUpperQueueWaitingSeconds == -1 || m_lLowerQueueWaitingSeconds == -1 : no this criterion
	long m_lUpperQueueWaitingSeconds;
	long m_lLowerQueueWaitingSeconds;
}SERVICE_ITEMS;

typedef std::pair<CMobileElemConstraint, SERVICE_ITEMS> Mob_ServicTime;

class ProcessorID;


class CSimLevelOfServiceItem
{
private:
	CString m_strProcessorsName;
	std::vector< Mob_ServicTime > m_vPaxTypeServices;
	
public:
	CSimLevelOfServiceItem();
	virtual ~CSimLevelOfServiceItem();
	
	void setProcessorName( const CString& _strName );
	const CString& GetProcessorName( void ) const;

	const std::vector<Mob_ServicTime>& getSpecificPaxTypeService() const;
	void setSpecificPaxTypeService( const std::vector<Mob_ServicTime>&  _vectorPaxTypeService );	

	void writeData( ArctermFile& _file );
	void readData( ArctermFile& _file,InputTerminal* _interm );

	const SERVICE_ITEMS* getTheNearestPax( const CMobileElemConstraint& _mob )const;

	const CMobileElemConstraint* getTheNearestPaxType( const CMobileElemConstraint& _mob )const;

private:
	void Sort();
};


class CSimLevelOfService : public CSimGeneralPara  
{
private:
	std::vector< CSimLevelOfServiceItem > m_vSimTLOSSetting;
	BOOL m_bDynamicCreateProc;
public:
	CSimLevelOfService( CStartDate _startDate );
	virtual ~CSimLevelOfService();
public:
	int getSettingCout( void ) const ;
	bool addSettingItem( const CSimLevelOfServiceItem& _item );
	CSimLevelOfServiceItem& getSetting( int _index );
	const CSimLevelOfServiceItem& GetSetting( int _index ) const;
	std::vector< CSimLevelOfServiceItem >& getSettingVector( void );
	BOOL getDynamicCreateProcFlag()const { return m_bDynamicCreateProc;	}
	void setDynamicCreateProcFlag( BOOL _bFlag ){ m_bDynamicCreateProc = _bFlag;}
	virtual void WriteData( ArctermFile& _file );
	virtual void ReadData( ArctermFile& _file ,InputTerminal* _interm );
	virtual CString GetClassType()const { return "LEVEOFSERVICE";	};

	const CSimLevelOfServiceItem* getTheNearestProc( const ProcessorID& _id );
private:
	bool ifExistInVector(  const CSimLevelOfServiceItem& _item );
};

bool SortByMobileElemConstraint( Mob_ServicTime _m1, Mob_ServicTime _m2 );
#endif // !defined(AFX_SIMLEVELOFSERVICE_H__3DFC27EF_669C_4D64_8830_57A8150E6DCE__INCLUDED_)
