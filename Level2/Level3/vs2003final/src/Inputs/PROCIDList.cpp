#include "StdAfx.h"
#include ".\procidlist.h"

#include "IN_TERM.H"
#include <Common/termfile.h>

/*****
*
*   Processor ID List
*
*****/

ProcessorIDList::ProcessorIDList () : UnsortedContainer<ProcessorID> (8, 0, 8)
{
}

void ProcessorIDList::initDefault (InputTerminal* _pInTerm)
{
	clear();
	ProcessorID* pId = new ProcessorID;
	pId->SetStrDict( _pInTerm->inStrDict);
	addItem (pId);
}

int ProcessorIDList::operator == (const ProcessorIDList& p_id) const
{
	if (getCount() != p_id.getCount())
		return 0;

	for (int i = 0; i < getCount(); i++)
		if (!(*(getItem (i)) == *(p_id.getItem (i))))
			return 0;
	return 0;
}

void ProcessorIDList::setID (int p_ndx, const ProcessorID& p_id)
{
	*(getItem (p_ndx)) = p_id;
}

int ProcessorIDList::areAllBlank (void) const
{
	for (int i = 0; i < getCount(); i++)
		if (!(getItem (i)->isBlank()))
			return 0;
	return 1;
}

// returns 1 if ID list is blanked by removal
int ProcessorIDList::remove (const ProcessorID& p_id,InputTerminal*_pTerm)
{
	for (int i = getCount() - 1; i >= 0; i--)
	{
		if (p_id.idFits (*getItem (i)))
			deleteItem (i);
		else if(getItem(i)->idFits(p_id)&&!getItem(i)->isBlank())
		{
			if(!_pTerm->HasBrother(p_id,*getItem(i)))
				deleteItem(i);
		}
	}
	return !getCount();
}


void ProcessorIDList::replace (const ProcessorID& p_old, const ProcessorID& p_new,InputTerminal*_pTerm)
{
	for (int i = getCount() - 1; i >= 0; i--)
	{
		if (p_old.idFits (*getItem (i)))
			getItem (i)->copyID (p_new);
		else if(getItem(i)->idFits(p_old)&&!getItem(i)->isBlank())
		{
			if(!getItem(i)->idFits(p_new))
			{
				if(!_pTerm->HasBrother(p_old,*getItem(i)))
					getItem (i)->copyID (p_new);
			}
		}
	}
}

bool ProcessorIDList::IfUsedThisProcssor(const ProcessorID& _procID ,InputTerminal*_pTerm,std::vector<CString>& _stringVec) const
{
	int iIDCount = this->getCount();
	for( int i=0; i<iIDCount; ++i )
	{
		ProcessorID* pID =getItem( i );
		if( *pID == _procID )
		{
			return true;
		}
		else if( pID->idFits( _procID ) && !pID->isBlank() )
		{
			if(!_pTerm->HasBrother(_procID,*pID))
				return true;
		}
		else if( _procID.idFits( *pID ) )
		{
			return true;
		}
	}

	return false;
}
/*
bool ProcessorIDList::IfUsedThisProcssor( const ProcessorID& _procID )const
{
int iIDCount = this->getCount();
for( int i=0; i<iIDCount; ++i )
{
ProcessorID* pID =getItem( i );
if( *pID == _procID )
{
return true;
}
else if( pID->idFits( _procID ) )
{
return true;
}
else if( _procID.idFits( *pID ) )
{
return true;
}
}

return false;
}*/

// Arcterm File input routine
void ProcessorIDList::readIDList (ArctermFile& p_file, InputTerminal* _pInTerm,bool _bWithOneToOne )
{
	clear();
	char *str1, *str2, str[1024];
	p_file.getField (str, 1024);
	if (!str[0])
		return;

	str2 = str - 1;
	ProcessorID *id;
	while (str2)
	{
		str1 = str2 + 1;
		str2 = strstr (str1, ";");
		if (str2)
			str2[0] = '\0';

		id = new ProcessorID;
		id->SetStrDict( _pInTerm->inStrDict );
		id->setID (str1);
		addItem (id);
	}
}

void ProcessorIDList::writeIDList (ArctermFile& p_file) const
{
	char strID[1024] = "";
	CString str=_T("");
	if (getCount())
		(getItem (0))->printID (strID);
	str+=strID;
	for (int i = 1; i < getCount(); i++)
	{
		//strcat (str, ";");
		(getItem (i))->printID (strID);
		str+=_T(";");
		str+=strID;
	}
	p_file.writeField (str.GetBuffer());
}

// stream based output method
stdostream& operator << (stdostream& s, const ProcessorIDList& p_list)
{
	if (!p_list.getCount())
		return s;

	s << *(p_list.getID(0));
	for (int i = 1; i < p_list.getCount(); i++)
		s << ';' << *(p_list.getID(i));

	return s;
}

