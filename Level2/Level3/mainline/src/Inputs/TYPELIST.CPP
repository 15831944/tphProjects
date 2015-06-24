#include "stdafx.h"

#ifndef REPORT
#include "inputs\in_term.h"
#endif

#include "inputs\typelist.h"
#include "common\termfile.h"
#include "common\strdict.h"
#include "common\exeption.h"
#include "../Common/ARCTracker.h"


// give default value
void PassengerTypeList::initDefaultValues (void)
{
	clear();
	m_nLevelCount = 1;
	m_nBranchCount[0] = 2;
    m_szTypeNames[0][0] = m_pStrDict->addString ("BUS");
    m_szTypeNames[1][0] = m_pStrDict->addString ("PLE");
	initLevelName() ;
}
void PassengerTypeList::initLevelName() 
{
	CString name ;
	for (int i = 0 ; i < 16 ;i++)
	{
         name.Format(_T("Level %d"),i+1) ;
		 LevelName[i] = name ;
	}
}
// clears StringDictionary, all Indexes, and resets size
void PassengerTypeList::clear (void)
{
    int i;
    for (i = 0; i < BRANCHES; i++)
        for (int j = 0; j < LEVELS; j++)
            m_szTypeNames[i][j] = 0;

    for (i = 0; i < LEVELS; i++)
        m_nBranchCount[i] = 0;

    m_nLevelCount = 0;

//    if ( !m_pStrDict )
//        strDict = new StringDictionary;

	assert( m_pStrDict );

    // Add mobile element type names to pax type name dictionary
    m_pStrDict->addString ("ARRIVAL");
    m_pStrDict->addString ("DEPARTURE");
    m_pStrDict->addString ("TRANSIT");
    m_pStrDict->addString ("TRANSFER");
//    m_pStrDict->addString ("SENDER");
//    m_pStrDict->addString ("GREETER");
}

void PassengerTypeList::readData (ArctermFile& p_file)
{
    char szPaxType[64];
    m_nLevelCount = 0;
	if(p_file.getVersion() > (float)2.2)
	{
		p_file.getLine() ;
		CString name ;
		int level = 0 ;
		while (!p_file.isBlankField())
		{
			p_file.getField(szPaxType,64) ;
			LevelName[level] = szPaxType ;
			level++ ;
		}
	}
    for (int branch = 0; p_file.getLine(); branch++)
    {
        for (int level = 0; level < LEVELS; level++)
        {
            if (!p_file.isBlankField())
            {
                p_file.getField (szPaxType, 64);
                m_szTypeNames[branch][level] = m_pStrDict->addString (szPaxType);
                m_nBranchCount[level]++;
                if (level >= m_nLevelCount)
                    m_nLevelCount = level + 1;
            }
            else
                p_file.skipField(1);
        }
    }

	if (!m_nLevelCount)
		initDefaultValues();
}

void PassengerTypeList::writeData (ArctermFile& p_file) const
{
    if (!m_nLevelCount)
        return;
   
	CString levelName ;
	for (int i = 0 ; i < 16 ; i++)
	{
		levelName.Format("%s",LevelName[i]) ;
		p_file.writeField(levelName) ;
	}
	p_file.writeLine();
    // construct whole string here
    //char str[1024];  //changed by matt
	char* str =new char[20480];
    for (int branch = 0; branch < BRANCHES; branch++)
    {
        str[0] = '\0';
        for (int level = 0; level < m_nLevelCount; level++)
        {
            if (branch < m_nBranchCount[level])
                strcat (str, getTypeName (branch, level));
            strcat (str, ",");
        }
        p_file.writeField (str);
        p_file.writeLine();
    }
	delete []str;

}

// adds 'strings' to the string dictionary and sets the Indexes in
// typeNames in the correct location
// strCount must not exceed the maximum number of BRANCHES
// the levels must be initialized from the root downward (0 - 4)
int PassengerTypeList::addLevel (int _nLevel, int _nStrIdx )
{
    if( _nLevel != m_nLevelCount)
        return FALSE;

    m_nBranchCount[_nLevel] = 1;
    m_szTypeNames[0][_nLevel] = _nStrIdx;
	m_nLevelCount++;
	return TRUE;
}

// add a cell to the level 
void PassengerTypeList::addCell (int _nLevel, const char *string)
{
	int nStrIdx = m_pStrDict->addString (string);
	addCell( _nLevel, nStrIdx );
}

void PassengerTypeList::addCell (int _nLevel, int _nStrIdx )
{
    if( _nLevel > m_nLevelCount )
        throw new OutOfRangeError ("Passenger Type List");

	if( _nLevel == m_nLevelCount )
	{
		addLevel( _nLevel, _nStrIdx );
	}
	else
	{
		int nBCount = m_nBranchCount[_nLevel];
		if( nBCount >= BRANCHES )
			throw new OutOfRangeError ("Passenger Type List");

		m_nBranchCount[_nLevel]++;
		m_szTypeNames[nBCount][_nLevel] =_nStrIdx;
	}
}


int PassengerTypeList::find (int p_val, const int *p_list, int p_count)
{
    for (int i = 0; i < p_count; i++)
        if (p_list[i] == p_val)
            return i;
    return -1;
}

#ifndef REPORT
void PassengerTypeList::UpdateLevel (int p_level, int p_branches,
    char p_strings[BRANCHES][LEVELS])
{
	assert( p_level < LEVELS && p_branches < BRANCHES );
	
	m_nBranchCount[p_level] = p_branches;
	for( int i=0; i<BRANCHES; i++ )
	{
		if( i < p_branches )
			m_szTypeNames[i][p_level] = m_pStrDict->findString( p_strings[i] );
		else
			m_szTypeNames[i][p_level] = 0;
	}
}

#endif

void PassengerTypeList::replaceString (int p_level, int p_branch,
    const char *p_str)
{
	// 1. can not replace string directly, it may affect other item
//    if (m_szTypeNames[p_branch][p_level] != m_pStrDict->findString (p_str))
//        strDict->replaceString (typeNames[p_branch][p_level], p_str);
	// 2. maybe it can not be found
// 	m_szTypeNames[p_branch][p_level] = m_pStrDict->findString( p_str );
	// 3. use addString
	// NOTE: it may reserve some garbage string in the memory,
	//       but luckily they will not be saved into file
	m_szTypeNames[p_branch][p_level] = m_pStrDict->addString(p_str);
}

// returns level where ndx can be found
int PassengerTypeList::getLevel (int ndx) const
{
    for (int level = 0; level < m_nLevelCount; level++)
        for (int branch = 0; branch < m_nBranchCount[level]; branch++)
            if (m_szTypeNames[branch][level] == (int)ndx)
                return level + 1;
    return 0;
}

// returns total number of leaves below the passed level
int PassengerTypeList::getLeafCount (int startLevel) const
{
    if (startLevel >= m_nLevelCount)
        return 1;

    int total = m_nBranchCount[startLevel];
    for (int level = startLevel+1; level < m_nLevelCount; level++)
		  total *= m_nBranchCount[level];

    return total;
}

/*
// sets start and end lead indexes of path into list
void PassengerTypeList::getLeafRange (const int *path, int level,
    int& start, int& end) const
{
    start = 0;
    for (int i = 0; i <= level; i++)
        start += path[i] * getLeafCount (i+1);

    end = start;
    if (level < levelCount - 1)
        end += getLeafCount (level+1) - 1;
}

 */



// assuming leafIndex is an index into a one dimensional array of
// leaves, set branches to the leaf's path through the tree
void PassengerTypeList::getBranchList (int leafIndex, int *branches) const
{
	PLACE_METHOD_TRACK_STRING();
    for (int i = 0; i < MAX_PAX_TYPES; i++)
        branches[i] = 0;

    int branch;
	int level = 0;
    for (; level < m_nLevelCount - 1; level++)
    {
        branch = (int) (leafIndex / getLeafCount (level+1));
        branches[level] = m_szTypeNames[branch][level];
        leafIndex = leafIndex % getLeafCount (level+1);
    }
    branches[level] = m_szTypeNames[leafIndex][level];
}

