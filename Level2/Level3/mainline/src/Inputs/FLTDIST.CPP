#include "stdafx.h"
#include "inputs\fltdist.h"
#include "common\flt_cnst.h"
#include "common\termfile.h"
#include "common\util.h"
#include "common\exeption.h"

#include "../Inputs/IN_TERM.H"

void FlightTypeDistribution::readDistribution (ArctermFile& p_file, InputTerminal* _pInTerm)
{
    // create ProbabilityDistribution
    char str[256];
    p_file.getField (str, 256);

    //FlightConstraint *aConst;
	CTransferFlightConstraint *aConst;
    int actualCount = p_file.countSubFields (';') / 2;
    double *indexes = new double[actualCount];
    double *probabilities = new double[actualCount];
	float fVersion = p_file.getVersion();

    // read processor ID / percentage pairs until invalid field found
    for (int i = 0; i < actualCount; i++)
    {
        indexes[i] = (double) i;
        aConst = new CTransferFlightConstraint;
		aConst->getFlightConstraint()->SetAirportDB(_pInTerm->m_pAirportDB);
        p_file.getSubField (str, ';');
		aConst->getFlightConstraint()->setConstraintWithVersion(str);
        destList.addItem (aConst);
        p_file.getSubField (str, ';');
        probabilities[i] = (int)(atof (str) * 100.0 + 0.5);
    }

    init (actualCount, indexes, probabilities);
    delete [] indexes;
    delete [] probabilities;
}

void FlightTypeDistribution::readDistribution( ArctermFile& p_file )
{
		ASSERT(false);
}
void FlightTypeDistribution::readVersion1 (ArctermFile& p_file, InputTerminal* _pInTerm)
{
    // create ProbabilityDistribution
    double aFloat;
    CTransferFlightConstraint *aConst;
    int maxCount = p_file.countColumns();
    double *indexes = new double[maxCount];
    double *probabilities = new double[maxCount];
    int actualCount = 0;

    // read processor ID / percentage pairs until invalid field found
    for (int i = 0; !p_file.eol() && !p_file.isBlankField(); i++)
    {
        actualCount++;
        indexes[i] = (double) i;
        aConst = new CTransferFlightConstraint;
		aConst->getFlightConstraint()->readConstraint ( p_file);
        destList.addItem (aConst);
        p_file.getFloat (aFloat);
        probabilities[i] = (int) (aFloat * 100.0 + 0.5);
    }

    init (actualCount, indexes, probabilities);
    delete [] indexes;
    delete [] probabilities;
}

void FlightTypeDistribution::writeDistribution (ArctermFile& p_file) const
{
    p_file.writeField (getProbabilityName());

    //char str[256] = ""; matt
	char str[2560] = "";
    //(destList.getItem(0))->WriteSyntaxStringWithVersion (str);
    (destList.getItem(0))->getFlightConstraint()->WriteSyntaxStringWithVersion(str);
	sprintf (str + strlen (str), ";%.2f", ((double)probs[0]) / 100.0);
	//p_file.writeField (str);

    for (int i = 1; i < count; i++)
    {
        strcat (str, ";");
        (destList.getItem(i))->getFlightConstraint()->WriteSyntaxStringWithVersion(str + strlen(str));
        sprintf (str + strlen (str), ";%.2f", ((double)probs[i]) / 100.0);
    }
    p_file.writeField (str);
}

void FlightTypeDistribution::addRow (CTransferFlightConstraint *p_group,
    unsigned char p_percent)
{
    destList.addItem (p_group);

	 if (count)
    {
        double *vals = new double[count+1];
        memcpy ((char *)vals, (char *)values, sizeof (double) * count);
        vals[count] = (double)count;
        delete [] values;
        values = vals;

        double *probabilities = new double[count+1];
        memcpy ((double *)probabilities, (double *)probs, sizeof(double)*count);
		  probabilities[count] = (int)p_percent;
		  //Jan98
		  if (count>0)
			probabilities[count] += probabilities[count-1];

        delete [] probs;
        probs = probabilities;
    }
    else
    {
	    values = new double[1];
        values[0] = (double)0;
        probs = new double[1];
        probs[0] = p_percent;
    }

    count++;
}

void FlightTypeDistribution::deleteRow (int p_ndx)
{
    destList.deleteItem (p_ndx);
    for (int i = p_ndx; i < count-1; i++)
        probs[i] = probs[i+1];
    count--;
}

// returns a destination group index
// if the flag RANDOM is passed the probability distribution is used
// if the flag SEQUENTIAL is used the groups are returned in order
// when using SEQUENTIAL selection, NULL is returned when all groups in
// the tranistion roq have been returned once
// other flags may be added to allow different logic to be used in
// selecting a destination group from the list
 CTransferFlightConstraint *FlightTypeDistribution::getDestinationGroup (int p_flag)
{
	 if (p_flag == RANDOM)
	 {
		  destIndex = 0;
		  return destList.getItem ((int)getRandomValue());
	 }
	 else if (p_flag == SEQUENTIAL && destIndex < count)
		  return destList.getItem (destIndex++);
	 return NULL;
}


//Jan98
int FlightTypeDistribution::getLastOfProbs()
{
	return static_cast<int>(probs[count-1]);
}


void FlightTypeDistribution::screenPrint (char *_str) const
{
    strcpy (_str, getProbabilityName());
	strcat (_str, ":");
	double nPrevVal = 0;
    for (int i = 0; i < count; i++)
	{
        //(destList.getItem(i))->screenPrint(_str + strlen (_str));
        (destList.getItem(i))->getFlightConstraint()->screenPrint(_str + strlen (_str));
        sprintf (_str + strlen (_str), ",%d%%", probs[i] - nPrevVal );
		nPrevVal = probs[i];
		if( i < count - 1 )
			strcat (_str, ";");
    }
}


