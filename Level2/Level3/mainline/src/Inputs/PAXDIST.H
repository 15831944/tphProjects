/*****
*
*   Class       PaxTypeDistribution
*   Author      Lincoln Fellingham
*   Date        Dec 92
*   Purpose     Defines a distribution of how passengers on any given
*               flight break down into the various user defined
*               categories
*
*               A single distribution will be associated with the type
*               of flight that this distribution describes
*
*   Revisions   Mar 96 - LF - Revised storage structure from storing
*               leaf probabilities to storing all branches of the tree
*               with their associated breakdowns. Levels, branches, and
*               totalBranches describe the structure of the tree, and
*               values stores all the probabilities in the format:
*
*               Values[0]   Values[1]   etc.
*               ---------   ---------
*               A           AA
*               B           AB
*               C           BA
*                           BB
*                           CA
*                           CB
*
*****/

#ifndef PASSENGER_TYPE_DISTRIBUTION_H
#define PASSENGER_TYPE_DISTRIBUTION_H

#include "../Common/ProbabilityDistribution.h"

class ArctermFile;

class PaxTypeDistribution : public ProbabilityDistribution
{
protected:

    // need to store full tree parameters and every branch percentage
    int levels;
    int branches[LEVELS];
    int totalBranches[LEVELS];

    float *values[LEVELS];

public:
    PaxTypeDistribution (const int *p_branches);
    PaxTypeDistribution (const PaxTypeDistribution& p_dist);
    virtual ~PaxTypeDistribution ();

    void initBranches (float *p_branches, int p_count, int p_series);

    const PaxTypeDistribution& operator = (const
        PaxTypeDistribution& p_dist);

    // set the value stored at a specific location in the tree in percent
    void setValue (const int *p_path, float p_value);

    // return the value stored at a specific location in the tree in percent
    float getValue (const int *p_path) const;

    // construct a path through the tree that evaluates to passed leaf index
    void makePathToLeaf (int *p_path, int p_leaf) const;

    // returns total probability (0.0 - 1.0) of a random draw resulting in
    // the passed path. Path must have values right to the leaf.
    float getProbability (const int *p_path) const;

    // checks to see if all sub-branches of the passed path evaluate to 100%
    int checkPathTotal (const int *p_path, int p_level) const;
    // checks all paths to ensure that every branch evalutes to 100%
    int checkTotals (void) const;

    // get a random leaf value
    virtual double getRandomValue (void) const;

    // get the average leaf value
    virtual double getMean (void) const;

    int getNodeCount (void) const;

    virtual int getProbabilityType (void) const { return PASSENGER_TYPE; }
    virtual const char *getProbabilityName (void) const { return "PaxTypeDistribution"; }

    virtual void readDistribution (ArctermFile& p_file);
	void readOldDistribution(ArctermFile& p_file);
    virtual void writeDistribution (ArctermFile& p_file) const;
    virtual bool isEqual(const ProbabilityDistribution* pOther);

public:
	void ClearEngineInitializeFlag() const;
	void EngineInitialize() const;
	double getEngineRandomValue (void) const;
	mutable float *percentages;
};

#endif
