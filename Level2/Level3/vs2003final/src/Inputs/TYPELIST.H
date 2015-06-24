#ifndef PASSENGER_TYPE_LIST_H
#define PASSENGER_TYPE_LIST_H

#include "common\template.h"
#include "common\dataset.h"
#include "common\strdict.h"
#include "inputsdll.h"


class INPUTS_TRANSFER PassengerTypeList : public DataSet
{
protected:
    StringDictionary* m_pStrDict;

    int m_szTypeNames[BRANCHES][LEVELS];
    int m_nBranchCount[LEVELS];
    int m_nLevelCount;
protected:
	CString LevelName[16]  ;
	void initLevelName() ;
public:
	CString GetLevelName(int _level) { if(_level < 0 || _level >15) return CString(); return LevelName[_level] ;};
	void SetLevelName(const CString& _Val , int _level) { if(_level < 0 || _level >15) return ;  LevelName[_level] = _Val ;};
public:
    PassengerTypeList( StringDictionary* _pStrDict) : DataSet (PassengerDescriptionFile,(float)2.3)
	{ m_pStrDict = _pStrDict; clear(); initLevelName() ;}
    virtual ~PassengerTypeList () {};

	// give default value
    virtual void initDefaultValues();

	// pass the global pointer
    virtual void initStrDict (StringDictionary *_pDict) { m_pStrDict = _pDict; }

    // clears both arrays, levelCount, and the StringDictionary
    void clear (void);

    virtual void readData (ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;

    virtual const char *getTitle (void) const
		{ return "Passenger Type Names"; }
    virtual const char *getHeaders (void) const
        { return "Level 1,Level 2,Level 3,Level 4,Level 5,Level 6,Level 7,Level 8,Level 9,Level 10,Level 11,Level 12,Level 13,Level 14,Level 15,Level 16"; }

    // adds 'strings' to the string dictionary and sets the Indexes in
    // typeNames in the correct location
    // strCount must not exceed the maximum number of BRANCHES
    // the levels must be initialized from the root downward (0 - 4)
    int addLevel (int level, int nStrIdx );

	// add a cell to the level 
    void addCell (int level, const char *string);
    void addCell (int p_level, int p_index);
    void replaceString (int p_level, int p_branch, const char *p_str);
    int find (int p_val, const int *p_list, int p_count);
    void UpdateLevel (int p_level, int p_branches, char p_strings[BRANCHES][LEVELS]);


    /*****
    *
    *   Query functions used to describe type structure
    *
    *****/

    // returns count of levels
    int getLevels (void) const { return m_nLevelCount; }

    // returns count of branches at passed level
    int getBranchesAtLevel (int level) const
        { return m_nBranchCount[level]; }

    // returns count of all level
    const int *getBranches (void) const { return m_nBranchCount; }

    // returns total number of nodes at & below below the passed level
    int getLeafCount (int startLevel = 0) const;
//    int getNodeCount (int startLevel) const;

    // sets start and end lead indexes of path into list
    void getLeafRange (const int *path, int level, int& start, int& end) const;

    // assuming leafIndex is an index into a one dimensional array of
    // leaves, set branches to the leaf's path through the tree
    void getBranchList (int leafIndex, int *branches) const;

    // returns level where ndx can be found
    int getLevel (int ndx) const;

    // returns the Index at the passed branch and level
    int getTypeIndex (int branch, int level) const
          { return (int)m_szTypeNames[branch][level]; }

    // returns the Index of the passed string
    int getTypeIndex (const char *str)
        { return m_pStrDict->findString (str); }

    // returns the string at the passed branch and level
    const char *getTypeName (int branch, int level) const
        { return m_pStrDict->getString (m_szTypeNames[branch][level]); }

    // returns the string at ndx in the string dictionary
    const char *getTypeName (int ndx) const
        { return m_pStrDict->getString (ndx); }

    // writes type names to file in a comma separated grid
    void echoPaxTypeNames (const char *filename) const;
};


#endif
