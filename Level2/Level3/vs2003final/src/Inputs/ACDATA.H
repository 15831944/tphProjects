#ifndef AIRCRAFT_DATABASE_H
#define AIRCRAFT_DATABASE_H

#include <string.h>
#include "common\containr.h"
#include "common\dataset.h"

class Aircraft
{
public:
	enum SortBy{CODE, NAME, DEFAULT_CAPACITY};

private:                              
static SortBy sortBy;

protected:
	char code[AC_TYPE_LEN];
	char name[AC_NAME_LEN];
	float height;
	float length;
	float span;
	float MZFW;
	float OEW;
	float MTOW;
	float MLW;
	int defaultCapacity;
public:
	Aircraft ();
	virtual ~Aircraft () {};

	int operator == (const Aircraft& p_ac) const
		{ return strcmpi (code, p_ac.code) == 0; }
	int operator < (const Aircraft& p_ac) const;
	int isInformationSame(const Aircraft& p_ac) const;
	int isCodeSame(const Aircraft& p_ac) const
		{return strcmpi(code, p_ac.code) == 0; }

	// accessors
	void getCode (char *p_str) const { strcpy (p_str, code); }
	void getName (char *p_str) const { strcpy (p_str, name); }
	const char* getCode () const {return code; }
	const char* getName () const {return  name; }
	float getHeight (void) const { return height; }
	float getLength (void) const { return length; }
	float getSpan (void) const { return span; }
	float getMZFW (void) const { return MZFW; }
	float getOEW (void) const { return OEW; }
	float getMTOW (void) const { return MTOW; }
	float getMLW (void) const { return MLW; }
	int getDefaultCapacity (void) const { return defaultCapacity; }

	// accessors
	void setCode (const char *p_str) { strcpy (code, p_str); }
	void setName (const char *p_str) { strcpy (name, p_str); }
	void setHeight (float p_flt) { height = p_flt; }
	void setLength (float p_flt) { length = p_flt; }
	void setSpan (float p_flt) { span = p_flt; }
	void setMZFW (float p_flt) { MZFW = p_flt; }
	void setOEW (float p_flt) { OEW = p_flt; }
	void setMTOW (float p_flt) { MTOW = p_flt; }
	void setMLW (float p_flt) { MLW = p_flt; }
	void setDefaultCapacity (int p_cap) { defaultCapacity = p_cap; }

	void readAircraft (ArctermFile &p_file);
	void writeAircraft (ArctermFile &p_file) const;

	static void sortByWhat(SortBy p_criteria){ sortBy = p_criteria;}
};

class AircraftDatabase : public UnsortedContainer<Aircraft>, public DataSet
{
public:
    AircraftDatabase() : UnsortedContainer<Aircraft> (4, 0, 4),
        DataSet (AircraftDatabaseFile) { ownsElements(1); }
    virtual ~AircraftDatabase() {};

    virtual void clear (void) { UnsortedContainer<Aircraft>::clear(); }

    virtual void readData (ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;

    virtual const char *getTitle (void) const
        { return "Aircraft Database"; }
    virtual const char *getHeaders (void) const
        { return "Code,Description,Height,Length,Span,MZFW,OEW,MTOW,MLW,Capacity"; }

    void sort (void);
};

#endif
