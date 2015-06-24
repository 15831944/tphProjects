// MobileElemTypeStrDB.h: interface for the CMobileElemTypeStrDB class.
#pragma once
#include "common\dataset.h"

#define NOPAX_COUNT                           22
#define ALLPAX_COUNT                          (NOPAX_COUNT + 1)    // include the header pax type PASSENGER
#define NOPAX_COUNT_EXCLUDE_USER_DEFINE       (NOPAX_COUNT - 3) 



class CMobileElemTypeStrDB : public DataSet  
{
protected:
	std::vector<CString> m_csList;

public:
	CMobileElemTypeStrDB();
	virtual ~CMobileElemTypeStrDB();

	CString GetString( int _nIdx );

	// return -1 for no entry
	int GetIndex( CString _csStr );

	void RemoveAt( int _nIdx );
	void AddElem( CString& _csStr);
	void ModifyElem( int _indx, const CString& _csStr );
	int GetCount()const { return (int)m_csList.size();	};
	// clear data structure before load data from the file.
	virtual void clear();
	virtual void readData( ArctermFile& p_file );
	virtual void readObsoleteData ( ArctermFile& p_file );
	void read2_6Data(ArctermFile& p_file );
	void read2_5Data(ArctermFile& p_file );
	void read2_4Data(ArctermFile& p_file );
	void read2_3Data(ArctermFile& p_file );
	void read2_2Data(ArctermFile& p_file );
	virtual void writeData( ArctermFile& p_file ) const;
    virtual void initDefaultValues();

	virtual const char *getTitle () const;
	virtual const char *getHeaders () const;


};
