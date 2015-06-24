// MultiMobConstraint.cpp: implementation of the CMultiMobConstraint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MultiMobConstraint.h"
#include "common\exeption.h"
#include "common\termfile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiMobConstraint::CMultiMobConstraint()
{
	//m_nCount = 0;
	//m_pConstraints = NULL; 
}


//CMultiMobConstraint::CMultiMobConstraint( const CMultiMobConstraint& _const )
//{
//	initConst (_const.getConstraint(0), _const.getCount()); 
//}
//
//
//const CMultiMobConstraint & CMultiMobConstraint::operator = (const CMultiMobConstraint &p_const)
//{ 		
//	if( this != &p_const )
//	{
//		initConst (p_const.getConstraint(0), p_const.getCount()); 
//	}	
//	return *this; 
//}
CMultiMobConstraint::~CMultiMobConstraint()
{
	freeConstraints();
}


// creates MultiConst from passed values
void CMultiMobConstraint::initConst (const CMobileElemConstraint *constList, int num)
{
    //if (m_nCount)
        freeConstraints();

    initConstCount (num);
    //memcpy (m_pConstraints, constList, sizeof (CMobileElemConstraint) * m_nCount);
	for (int  i = 0; i< num; ++i)
	{
		m_pConstraints[i] = constList[i];
	}
}

// allocates pointers for num Constraints
void CMultiMobConstraint::initConstCount (int num)
{
    /*if (m_nCount)
        freeConstraints();

    m_nCount = num;
    m_pConstraints = new CMobileElemConstraint[m_nCount];*/
	m_pConstraints.resize(num);
}

// creates a list of a single default constraint
void CMultiMobConstraint::initDefault (InputTerminal* _pInTerm)
{
    CMobileElemConstraint defaultConst(_pInTerm);

	//defaultConst.SetInputTerminal( _pInTerm );

    initConstCount (1);
    m_pConstraints[0] = defaultConst;
}

// set PassengerConstraint pointer at index to aConst
void CMultiMobConstraint::setConstraint ( const CMobileElemConstraint aConst, int index)
{
    if (index < 0 || index >= getCount())
		throw new OutOfRangeError ("MultiPaxConstraint::setConstraint");

    m_pConstraints[index] = aConst;
}

// returns the PassengerConstraint at index num
const CMobileElemConstraint *CMultiMobConstraint::getConstraint (int num) const
{
    if (num < 0 || num >= getCount())
		throw new OutOfRangeError ("MultiPaxConstraint::getConstraint");

    return &m_pConstraints[num];
}

CMobileElemConstraint * CMultiMobConstraint::getConstraint( int num )
{
	if (num < 0 || num >= getCount())
		throw new OutOfRangeError ("MultiPaxConstraint::getConstraint");

	return &m_pConstraints[num];
}
// reallocates pointer list to include passed constraint
void CMultiMobConstraint::addConstraint (CMobileElemConstraint aConst)
{
    if (getCount() == 255)
        return;
	m_pConstraints.push_back(aConst);

 //   CMobileElemConstraint constList[256];//???????????????
	//if( m_nCount > 0 )
	//{
	//	//memcpy (constList, m_pConstraints, sizeof (CMobileElemConstraint) * m_nCount);
	//	for (int i =0; i < m_nCount; ++ i)
	//	{
	//		constList[i] = m_pConstraints[i];
	//	}
	//}
 //   constList[m_nCount] = aConst;
 //   initConst (constList, m_nCount+1);
}

// removes 1 constraint from list
void CMultiMobConstraint::deleteConst (int p_ndx)
{
    /*if (p_ndx < 0 || p_ndx >= m_nCount)
        return;

    if (m_nCount == 1)
    {
        freeConstraints();
        return;
    }

    for (int i = p_ndx; i < m_nCount - 1; i++)
        m_pConstraints[i] = m_pConstraints[i+1];
    m_nCount--;*/
	m_pConstraints.erase(m_pConstraints.begin()+p_ndx);
}

// test each PassengerConstraint to see if they apply to paxType
int CMultiMobConstraint::isMatched (const CMobileElemConstraint& p_const, bool _bIgnoreStar/* = false*/ ) const
{
    if (isNone())
        return FALSE;

    for (int i = 0; i < getCount(); i++)
        if (m_pConstraints[i].fitex (p_const, _bIgnoreStar ))
            return TRUE;

    return FALSE;
}

// returns non-zero if the constraint contains p_index at p_level
int CMultiMobConstraint::contains (int p_level, int p_index) const
{
    for (int i = 0; i < getCount(); i++)
        if (m_pConstraints[i].contains (p_level, p_index))
            return 1;
    return 0;
}

// reads a number of Constraints from a field of the file
void CMultiMobConstraint::readConstraints (ArctermFile& file, InputTerminal* _pInTerm )
{
    CMobileElemConstraint aConst(_pInTerm);

	//aConst.SetInputTerminal( _pInTerm );

    if (file.isBlankField())
    {
        // blank constraint field
        initConstCount(1);
        setConstraint (aConst, 0);
        file.skipField(1);
    }
    else
    {
        initConstCount (file.countSubFields (';'));
        for (int i = 0; i < getCount(); i++)
        {
            aConst.readConstraint (file);
            setConstraint (aConst, i);
        }
    }
}

void CMultiMobConstraint::writeConstraints (ArctermFile& file, int p_display) const
{
    if (!getCount())
    {
        file.writeField ("DEFAULT");
        return;
    }

  
   char string[2560];
	
    const CMobileElemConstraint *aConst = getConstraint (0);
    if (p_display)
		aConst->screenPrint( string, 0, MAX_PAX_TYPE_LEN );
    else
        aConst->WriteSyntaxStringWithVersion (string);

    for (int i = 1; i < getCount(); i++)
    {
        strcat (string, ";");
        aConst = getConstraint (i);
        if (p_display)
            //aConst->screenPrint (string + strlen (string), MAX_PAX_TYPE_LEN);
			aConst->screenPrint (string + strlen (string), 0, MAX_PAX_TYPE_LEN);
        else
            aConst->WriteSyntaxStringWithVersion (string + strlen (string));
    }
    file.writeField (string);
}

// returns TRUE if MultiConst applies to NO passenger type
int CMultiMobConstraint::isNone() const 
{
	return (!getCount() || (getCount() == 1 &&m_pConstraints[0].isNone())); 
}
bool CMultiMobConstraint::operator == ( const CMultiMobConstraint &p_const )const
{
	if( getCount() != p_const.getCount() )
	{
		return false;
	}
	
	for (int i = 0; i < getCount(); i++)
    {        
        const CMobileElemConstraint * pConst = getConstraint (i);
		bool bAtLeastOneEqual = false;
		for( int j=0; j<p_const.getCount(); ++j )
		{
			const CMobileElemConstraint * pOtherConst = p_const.getConstraint (j);
			if( *pConst == * pOtherConst  )
			{
				bAtLeastOneEqual = true;
				break;
			}
		}

		if( !bAtLeastOneEqual )
		{
			return false;
		}
	}

	return true;
}

void CMultiMobConstraint::freeConstraints( void )
{
	/*if( m_nCount==0 )
		return;
	if( m_pConstraints ) 
		delete [] m_pConstraints;
	m_nCount = 0; 
	m_pConstraints = NULL;*/
	m_pConstraints.clear();
}