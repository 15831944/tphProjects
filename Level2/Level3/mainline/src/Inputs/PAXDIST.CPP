#include "stdafx.h"
#include "common\termfile.h"
#include "inputs\paxdist.h"
#include "common\exeption.h"



PaxTypeDistribution::PaxTypeDistribution (const int *p_branches)
{
    levels = 0;
    for (int i = 0; i < LEVELS; i++)
        if (p_branches[i])
            levels++;

    memcpy ((char *)branches, (char *)p_branches, LEVELS * sizeof (int));
    memset ((char *)totalBranches, '\0', LEVELS * sizeof (int));

    totalBranches[0] = branches[0];
    for (int ii = 1; ii < levels; ii++)
        totalBranches[ii] = branches[ii] * totalBranches[ii-1];

    for (int i3 = 0; i3 < levels; i3++)
    {
        values[i3] = new float[totalBranches[i3]];
        memset ((char *)(values[i3]), '\0', totalBranches[i3] * sizeof (float));
        initBranches (values[i3], branches[i3], totalBranches[i3] / branches[i3]);
    }
	percentages = NULL;
}

PaxTypeDistribution::PaxTypeDistribution (const PaxTypeDistribution& p_dist)
{
    levels = p_dist.levels;
    int size = LEVELS * sizeof (int);
    memcpy ((char *)branches, (char *)(p_dist.branches), size);
    memcpy ((char *)totalBranches, (char *)(p_dist.totalBranches), size);

    for (int i = 0; i < levels; i++)
        values[i] = new float[totalBranches[i]];
    *this = p_dist;
	percentages = NULL;
}

void PaxTypeDistribution::initBranches (float *p_branches, int p_count,
    int p_series)
{
    for (int i = 0; i < p_series; i++)
    {
		// avoid tolerance
		if( 100 % p_count != 0)
		{
			int _nMod = 100 % p_count;
			float _average = 1.0f*(100-_nMod)/p_count;

			int j = 0;
			for (; j < p_count; j++)
				p_branches[i*p_count+j] = _average;
			--j;// update last one.
			p_branches[i*p_count+j] = _average + 1.0f*_nMod;
		}
        else
            for (int j = 0; j < p_count; j++)
                p_branches[i*p_count+j] = (float)(100.0 / (float)p_count);
    }
}

PaxTypeDistribution::~PaxTypeDistribution ()
{
    for (int i = 0; i < levels; i++)
        delete [] (values[i]);

	if(percentages)
		delete []percentages;
}

const PaxTypeDistribution& PaxTypeDistribution::operator =
    (const PaxTypeDistribution& p_dist)
{
    int size = LEVELS * sizeof (int);
    if (memcmp ((char *)branches, (char *)(p_dist.branches), size) ||
            levels != p_dist.levels)
        throw new StringError ("Invalid copy, PaxTypeDistribution");

    for (int i = 0; i < levels; i++)
        memcpy ((char *)(values[i]), (char *)(p_dist.values[i]),
            totalBranches[i] * sizeof (float));

    return *this;
}

bool PaxTypeDistribution::isEqual(const ProbabilityDistribution* pOther)
{
    if(pOther->getProbabilityType() != getProbabilityType())
        return false;
    PaxTypeDistribution* pPaxProb = (PaxTypeDistribution*)pOther;
    int size = LEVELS * sizeof (int);
    if(memcmp((char *)branches, (char *)(pPaxProb->branches), size) != 0)
        return false;
    if(levels != pPaxProb->levels)
        return false;

    for (int i = 0; i < levels; i++)
    {
        if(memcmp((char *)(values[i]), (char *)(pPaxProb->values[i]), totalBranches[i]*sizeof (float)) != 0)
            return false;
    }
    return true;
}

// set the value stored at a specific location in the tree, as a percent
void PaxTypeDistribution::setValue (const int *p_path, float p_value)
{
    if (p_path[0] == -1)
        return;

    int level = levels, branch = 0, subBranches;

    // determine the level of interest
    for (int i = levels; i > 0; i--)
        if (p_path[i-1] == -1)
            level--;

    // calculate the index into the list of values at the specified level
    for (int ii = 0; ii < level; ii++)
    {
        subBranches = p_path[ii];
        for (int j = ii + 1; j < level; j++)
				subBranches *= branches[j];
        branch += subBranches;
    }
    values[level-1][branch] = p_value;
}

// return the value stored at a specific location in the tree, as a percent
float PaxTypeDistribution::getValue (const int *p_path) const
{
    if (p_path[0] == -1)
        return -1;

    int level = levels, branch = 0, subBranches;

    // determine the level of interest
    for (int i = levels; i > 0; i--)
        if (p_path[i-1] == -1)
            level--;

    // calculate the index into the list of values at the specified level
    for (int ii = 0; ii < level; ii++)
    {
        subBranches = p_path[ii];
        for (int j = ii + 1; j < level; j++)
				subBranches *= branches[j];
        branch += subBranches;
    }
    return values[level-1][branch];
}

// construct the path through the tree to evaluate to the passed leaf index
void PaxTypeDistribution::makePathToLeaf (int *p_path, int p_leaf) const
{
    // initialize path to -1
    for (int i = 0; i < LEVELS; i++)
        p_path[i] = -1;

    if (p_leaf >= totalBranches[levels-1])
        throw new StringError ("Bad call in make path to leaf");

    // calculate the number of sub-branches at each level
    int subBranchList[LEVELS];
    for (int ii = 0; ii < levels; ii++)
    {
        subBranchList[ii] = 1;
        for (int j = ii + 1; j < levels; j++)
            subBranchList[ii] *= branches[j];
    }

    // calculate the branch at each level using integer division and
    // remainder values
    for (int i3 = 0; i3 < levels; i3++)
    {
        p_path[i3] = (int)(p_leaf / subBranchList[i3]);
        p_leaf = p_leaf % subBranchList[i3];
    }
}

// returns total probability (0.0 - 1.0) of a random draw resulting in
// the passed path. Path must have values right to the leaf.
float PaxTypeDistribution::getProbability (const int *p_path) const
{
    int path[LEVELS];
    memcpy ((char *)path, (char *)p_path, sizeof (int) * LEVELS);

    float percent = 1.0;
    for (int i = levels - 1; i >= 0; i--)
    {
        percent *= (float)(getValue (path) / 100.0);
        path[i] = -1;
    }
    return percent;
}

// checks to see if all sub-branches of the passed path evaluate to 100%
int PaxTypeDistribution::checkPathTotal (const int *p_path,
    int p_level) const
{
    if (p_level == levels)
        return 1;

    float total = 0;
    int path[LEVELS];
    memcpy ((char *)path, (char *)p_path, sizeof (int) * LEVELS);
    for (int i = 0; i < branches[p_level]; i++)
    {
        path[p_level] = i;

        // as the path is constructed recurse through the sub-branches
        if (!checkPathTotal (path, p_level+1))
            return 0;
        total += getValue (path);
    }
    return (total == 100.0);
}

// checks all paths to ensure that every branch evalutes to 100%
int PaxTypeDistribution::checkTotals (void) const
{
    int path[LEVELS];
    for (int i = 0; i < LEVELS; i++)
        path[i] = -1;
    return checkPathTotal (path, 0);
}

// get a random leaf value
double PaxTypeDistribution::getRandomValue (void) const
{
	double dRes = 0.0;
    int path[LEVELS];
    int leafCount = totalBranches[levels-1];
    float *percentages = new float[leafCount];

    // get probability of evaluating each leaf in the tree
    for (int i = 0; i < leafCount; i++)
    {
        makePathToLeaf (path, i);
        percentages[i] = getProbability (path);
    }

    // make the probabilities cumulative
    for (int ii = 1; ii < leafCount; ii++)
        percentages[ii] += percentages[ii-1];

    percentages[leafCount - 1] = 1.0;

    // get a random number from 0.003 to 1.0
    // a minimum value of 0.0 would permit a hit on a 0% probability
    double randomVal = ((double)(1+random(RAND_MAX))) / ((double)RAND_MAX);
	
	int i3 = 0;
    for (; i3 < leafCount; i3++)
	{
        if (percentages[i3] - randomVal> 0.0)
		{
			dRes = i3;
			break;
		}
	}
	if( i3 == leafCount )
		dRes = leafCount - 1;

	delete [] percentages;
    return dRes;
}

// get the average leaf value
double PaxTypeDistribution::getMean (void) const
{
    int path[LEVELS];
    int leafCount = totalBranches[levels-1];
    float *percentages = new float[leafCount];

    // get probability of evaluating each leaf in the tree
    for (int i = 0; i < leafCount; i++)
    {
        makePathToLeaf (path, i);
        percentages[i] = getProbability (path);
    }

    // make the probabilities cumulative
    for (int ii = 1; ii < leafCount; ii++)
        percentages[ii] += percentages[ii-1];

    if (percentages[leafCount - 1] != 1.0)
        throw new StringError ("Invalid passenger type distribution");

    double randomVal = 0.5;
    for (int i3 = 0; i3 < leafCount; i3++)
        if (percentages[i3] >= randomVal)
            return i3;
    return leafCount - 1;
}

// read a distribution from a single line of an ArctermFile
void PaxTypeDistribution::readDistribution (ArctermFile& p_file)
{
    /*
    p_file.setToField (1);
        for (int i = 0; i < levels; i++)
            for (int j = 0; j < totalBranches[i]; j++)
                p_file.getFloat (values[i][j]);*/
	int iLineCount=0; //must to read line;

	int MaxCount = 1024;
    
    int iReadCount = 0; //readed!
	p_file.getLine();//
	
	p_file.getInteger(iLineCount);
	p_file.getLine();

	for (int i = 0; i < levels; i++)  //matt
		for (int j = 0; j < totalBranches[i]; j++)
		{
			if(!p_file.getFloat (values[i][j]))
			{
				if(iReadCount <= iLineCount) 
					p_file.getLine();
				else return;
				if(!p_file.getFloat (values[i][j])) //
					return;
				iReadCount++;
			};
		}

    
}

void PaxTypeDistribution::readOldDistribution(ArctermFile&p_file)
{
	p_file.setToField (1);
	for (int i = 0; i < levels; i++)
		for (int j = 0; j < totalBranches[i]; j++)
			p_file.getFloat (values[i][j]);
}
// write a distribution to a single line of an ArctermFile
void PaxTypeDistribution::writeDistribution (ArctermFile& p_file) const
{
    //for (int i = 0; i < levels; i++)
    //        for (int j = 0; j < totalBranches[i]; j++)
    //            p_file.writeFloat (values[i][j], 2);

	p_file.writeLine();

	int iLineCount =0;
	char sLineCount[8];
  
	int nodeCount = getNodeCount();
	
    iLineCount = (int)(7*nodeCount / 1024 ) + 1;
    
	int iEveryLineNode = (nodeCount / iLineCount) + 1;  // acount of one line 's nodecount

	_itoa(iLineCount,sLineCount,10);       //write line of count to file
	p_file.writeField (sLineCount);
    p_file.writeLine();

	
	int iReadEvLineCount = 0;                    //readed nodecount in one line
	int iReadAllCount = 0;
	
	for (int i = 0; i < levels; i++)
	{
		for (int j = 0; j < totalBranches[i]; j++)
		{
			iReadAllCount ++;
			iReadEvLineCount ++;
            p_file.writeFloat (values[i][j], 2);
            
			if(iReadEvLineCount >= iEveryLineNode)
			{
			   iReadEvLineCount =0;
			   if(iReadAllCount < nodeCount)  // donot to putline  of the last one node ,
		       p_file.writeLine();
			}
		}
	}

    
}

int PaxTypeDistribution::getNodeCount (void) const
{
    int nodeCount = 0;
    for (int i = 0; i < levels; i++)
        for (int j = 0; j < totalBranches[i]; j++)
            nodeCount++;
    return nodeCount;
}

void PaxTypeDistribution::EngineInitialize() const
{
	if(percentages)
		delete []percentages;
	percentages = NULL;

	//double dRes = 0.0;
	int path[LEVELS];
	int leafCount = totalBranches[levels-1];
	percentages = new float[leafCount];

	// get probability of evaluating each leaf in the tree
	for (int i = 0; i < leafCount; i++)
	{
		makePathToLeaf (path, i);
		percentages[i] = getProbability (path);
	}

	// make the probabilities cumulative
	for (int ii = 1; ii < leafCount; ii++)
		percentages[ii] += percentages[ii-1];

	//percentages[leafCount - 1] = 1.0;

}
// get a random leaf value
double PaxTypeDistribution::getEngineRandomValue (void) const
{
	if(percentages == NULL)
		EngineInitialize();

	if(percentages == NULL)
		return 0.0;

	//double dRes = 0.0;
	//int path[LEVELS];
	//int leafCount = totalBranches[levels-1];
	//float *percentages = new float[leafCount];

	//// get probability of evaluating each leaf in the tree
	//for (int i = 0; i < leafCount; i++)
	//{
	//	makePathToLeaf (path, i);
	//	percentages[i] = getProbability (path);
	//}

	//// make the probabilities cumulative
	//for (i = 1; i < leafCount; i++)
	//	percentages[i] += percentages[i-1];

	//percentages[leafCount - 1] = 1.0;

	// get a random number from 0.003 to 1.0
	// a minimum value of 0.0 would permit a hit on a 0% probability
		
	double dRes = 0.0;
	int leafCount = totalBranches[levels-1];

	int i = 0;
	double randomVal = ((double)(1+random(RAND_MAX))) / ((double)RAND_MAX);
	for (; i < leafCount; i++)
	{
		if (percentages[i] - randomVal> 0.0)
		{
			dRes = i;
			break;
		}
	}
	if( i == leafCount )
	{
		for (int j = leafCount - 2; j >= 0; j--)
		{
			if (percentages[j] != percentages[leafCount -1])
			{
				dRes = j+1;
				break;
			}
		}
	}		
	//delete [] percentages;
	return dRes;
}

void PaxTypeDistribution::ClearEngineInitializeFlag() const
{
	if(percentages)
		delete []percentages;
	percentages = NULL;
}




















