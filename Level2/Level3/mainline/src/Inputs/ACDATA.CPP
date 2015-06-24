#include "stdafx.h"
#include <math.h>
#include "inputs\acdata.h"


Aircraft::SortBy Aircraft::sortBy;

Aircraft::Aircraft ()
{
	code[0] = name[0] = '\0';
	height = length = span = MZFW = OEW = MTOW = MLW = 0.0;
	defaultCapacity = 0;
}

int Aircraft::operator < (const Aircraft& p_ac) const
{
	switch (sortBy)
	{
		case CODE:return _strcmpi (code, p_ac.code) < 0;
		case NAME:return strcmp(name, p_ac.name) < 0;
		default: return defaultCapacity<p_ac.defaultCapacity;
	}
}

int Aircraft::isInformationSame(const Aircraft& p_ac) const
{
	double verySmall = 0.0000000001;

	if (
		 _strcmpi(name, p_ac.name) == 0 &&
		 fabs(height - p_ac.height) < verySmall &&
		 fabs(length - p_ac.length) < verySmall &&
		 fabs(span - p_ac.span) < verySmall &&
		 fabs(MZFW - p_ac.MZFW) < verySmall  &&
		 fabs(OEW - p_ac.OEW ) < verySmall &&
		 fabs(MTOW - p_ac.MTOW ) < verySmall &&
		 fabs(MLW - p_ac.MLW) < verySmall &&
		 defaultCapacity == p_ac.defaultCapacity
	)
	{
		return 1;
	}
	else
	{
    	return 0;
	}

}

void Aircraft::readAircraft (ArctermFile &p_file)
{
    p_file.getField (code, AC_TYPE_LEN);
    p_file.getField (name, AC_NAME_LEN);
    p_file.getFloat (height);
    p_file.getFloat (length);
    p_file.getFloat (span);
    p_file.getFloat (MZFW);
    p_file.getFloat (OEW);
    p_file.getFloat (MTOW);
    p_file.getFloat (MLW);
    p_file.getInteger (defaultCapacity);
}

void Aircraft::writeAircraft (ArctermFile &p_file) const
{
    p_file.writeField (code);
    p_file.writeField (name);
    p_file.writeFloat (height, 2);
    p_file.writeFloat (length, 2);
    p_file.writeFloat (span, 2);
    p_file.writeFloat (MZFW, 2);
    p_file.writeFloat (OEW, 2);
    p_file.writeFloat (MTOW, 2);
    p_file.writeFloat (MLW, 2);
    p_file.writeInt (defaultCapacity);
}

void AircraftDatabase::readData (ArctermFile& p_file)
{
    p_file.setTrimSpaces (0);
    SortedContainer<Aircraft> sortedList;

    Aircraft *anAC;
    while (p_file.getLine())
    {
        anAC = new Aircraft;
        anAC->readAircraft (p_file);
        sortedList.addItem (anAC);
    }

    for (int i = 0; i < sortedList.getCount(); i++)
        addItem (sortedList.getItem (i));
}

void AircraftDatabase::writeData (ArctermFile& p_file) const
{
    Aircraft *anAC;
    int count = getCount();
    for (int i = 0; i < count; i++)
    {
        anAC = getItem (i);
        anAC->writeAircraft (p_file);
        p_file.writeLine();
    }
}

void AircraftDatabase::sort (void)
{
    SortedContainer<Aircraft> sortedList;

    for (int i = 0; i < getCount(); i++)
        sortedList.addItem (getItem (i));

    ownsElements (0);
    clear();
    ownsElements (1);
    for (int j = 0; j < sortedList.getCount(); j++)
        addItem (sortedList.getItem (j));
}

