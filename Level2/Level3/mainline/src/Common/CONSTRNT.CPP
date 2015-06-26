#include "stdafx.h"
#include ".\constrnt.h"
#include "termfile.h"

// complex logic to force to correct sorted order in CONSTRDB
// the intent of the test is to return the tighter constraint, based
// first on the number of criteria, and second on the precedence of
// the criteria
bool Constraint::operator < (const Constraint& p_const) const
{
    int thisCriteria = getCriteriaCount();
	int otherCriteria = p_const.getCriteriaCount();

    if (thisCriteria == otherCriteria)
        return getTotalPrecedence() > p_const.getTotalPrecedence();
    else
        return thisCriteria > otherCriteria;
}

void Constraint::readConstraint (ArctermFile& p_file,SAVE_TYPE _type )
{
	//char str[256] = "";
	char str[1024] = "";  //changed by matt in 5,12,2004

	p_file.getSubField (str, ';');
	 nConstraintVersion = GetAndRemoveConstraintVersion( str,_type );
	setConstraint( str, nConstraintVersion );
}

void Constraint::readConstraintWithVersion(const char* _csStr,SAVE_TYPE _type )
{
	char szConstraint[1024] = "";
	strcpy(szConstraint, _csStr);
	 nConstraintVersion = GetAndRemoveConstraintVersion(szConstraint,_type);
	setConstraint(szConstraint, nConstraintVersion);
}




// read the constraint version info
// old version will have no number return 0
// new version start with 100
// assume first 3 chars in the string are the version info, if version >= 100
// get the version and remove the version info
int Constraint::GetAndRemoveConstraintVersion( char* _csStr,SAVE_TYPE _type)
{
	if( !_csStr )
		return 0;
    
	if( isdigit(_csStr[0]) && isdigit(_csStr[1]) && isdigit(_csStr[2]) )
	{
		int nConstraintVersion =  100 * (_csStr[0]-'0') + 10 * (_csStr[1]-'0') + (_csStr[2]-'0');
		// remove the version info
		char tempStr[1024];
		strcpy( tempStr, &(_csStr[3]) );
		strcpy( _csStr, tempStr );
		return nConstraintVersion;
	}
	if(_csStr[0] == ' ' || _csStr[1] == ' '|| _csStr[2] == ' ')
	{
		TCHAR val[1024] = {0} ;
		_tcsncpy(val,_csStr,3) ;
         int nConstraintVersion = atoi(val);
		 char tempStr[1024];
		 strcpy( tempStr, &(_csStr[3]) );
		 strcpy( _csStr, tempStr );
		 return nConstraintVersion;
	}
	if(_type == FILE_TYPE)
	   return 0;
	else
	  return 102 ;
}

void Constraint::writeConstraint (ArctermFile& p_file) const
{
   // char str[256];
	char str[2560] = "";  //changed by matt in 5,12,2004
    WriteSyntaxStringWithVersion (str);
    p_file.writeField (str);
}

void Constraint::setConstraintWithVersion(const CString& p_str)
{
	TCHAR pchar[1024] = {0} ;
	_tcscpy(pchar,p_str) ;
	nConstraintVersion = GetAndRemoveConstraintVersion(pchar) ;
	setConstraint(pchar,nConstraintVersion) ;
}
// need to get version info first and then call set
void Constraint::setConstraintWithVersion ( char *p_str )
{
	nConstraintVersion = GetAndRemoveConstraintVersion( p_str);
	setConstraint( p_str, nConstraintVersion );
}

	// will add VERSION_CONSTRAINT_CURRENT
void Constraint::WriteSyntaxStringWithVersion (char *p_str) const
{
	sprintf( p_str, "%3d", VERSION_CONSTRAINT_CURRENT );
	char tempStr[1024];
	WriteSyntaxString( tempStr );
	strcat( p_str, tempStr );
}

void Constraint::getSerialize( CString& p_str )const
{
	char str[2560] = "";  //changed by matt in 5,12,2004
	WriteSyntaxStringWithVersion (str);
	p_str = str;
}
