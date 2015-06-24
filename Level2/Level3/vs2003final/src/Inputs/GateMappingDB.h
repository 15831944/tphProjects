// GateMappingDB.h: interface for the CGateMappingDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATEMAPPINGDB_H__6A5736E1_8BAE_4EE3_BC08_1B4B5516BE12__INCLUDED_)
#define AFX_GATEMAPPINGDB_H__6A5736E1_8BAE_4EE3_BC08_1B4B5516BE12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\dataset.h"
#include <CommonData/procid.h>
#include "in_term.h"

class CGateMapping
{
private:
	InputTerminal* pTerm;
	CString		strMapName;
	ProcessorID standGate;
	ProcessorID arrivalGate;
	ProcessorID departureGate;
	
public:
	CGateMapping(InputTerminal* pTerm)
	{
		ASSERT( pTerm );
		strMapName = "DEFAULT NAME";
		standGate.SetStrDict( pTerm->inStrDict);
		arrivalGate.SetStrDict( pTerm->inStrDict);
		departureGate.SetStrDict( pTerm->inStrDict);
	};
	CGateMapping() 
	{
		strMapName = "DEFAULT NAME";
	};
	virtual ~CGateMapping(){};
	// get and set
	const CString& getMapName( void ) const {return strMapName;}
	void setMapName( const CString& _strName ) { strMapName = _strName; }
	
	const ProcessorID& getStandGate( void ) const { return standGate; }
	void setStandGate( const ProcessorID& entry ) { standGate = entry;}
	
	const ProcessorID& getArrGate( void ) const { return arrivalGate;	}
	void setArrGate( const ProcessorID& entry ) { arrivalGate = entry;	}
	
	const ProcessorID& getDepGate( void ) const { return departureGate; }
	void setDepGate( const ProcessorID& entry ) { departureGate = entry;	}
	
	// read and write
	void readMapData( ArctermFile& p_file,StringDictionary* _pStrDict );
	void writeMapData( ArctermFile& p_file ) const ;
};


class CGateMappingDB : public DataSet  
{
private:
	std::vector<CGateMapping> m_vectMapDB;
public:
	CGateMappingDB();
	virtual ~CGateMappingDB();
	
	int addItem( CGateMapping& entry ) { m_vectMapDB.push_back(entry); return m_vectMapDB.size()-1;}
	void removeItem( int _index );
	const CGateMapping& getItem( int _index ) const;
	CGateMapping* GetItem( int _index );
	int getCount( void ) const { return m_vectMapDB.size(); }
	int getIndex( const CString& strMapName ) const;
	
	bool ifExistInDB( const CString& _strName, int _iNoCheckIndex = -1);
	// clear data structure before load data from the file.
	virtual void clear();
	virtual void readData( ArctermFile& p_file );
	virtual void writeData( ArctermFile& p_file ) const;
    virtual void initDefaultValues();
	
	virtual const char *getTitle () const;
	virtual const char *getHeaders () const;
	
};

#endif // !defined(AFX_GATEMAPPINGDB_H__6A5736E1_8BAE_4EE3_BC08_1B4B5516BE12__INCLUDED_)
