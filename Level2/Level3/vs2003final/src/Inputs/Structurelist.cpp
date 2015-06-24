#include "stdafx.h"
#include "structurelist.h"
#include "../common/exeption.h"
#include "in_term.h"
#include <Common/strdict.h>
#include "CommonData\Textures.h"
CStructureList::CStructureList() : DataSet(AirsideStructureFile, (float)2.4)
{
}

CStructureList::CStructureList(int ftype)
: DataSet(ftype, (float)2.4)
{
}

void CStructureList::clear (void)
{
	for (int i =0; i <(int)m_structurelist.size(); ++i)
	{
		delete m_structurelist[i];
	}
	m_structurelist.clear();
}

void CStructureList::readObsoleteData( ArctermFile& p_file )
{
	if(p_file.getVersion() <= 2.21)
	{
		p_file.getLine();

		ProcessorID strucID;

		assert( m_pInTerm );
		strucID.SetStrDict( m_pInTerm->inStrDict );

		while (!p_file.isBlank ())
		{
			CStructure *strct = new CStructure;

			char name[256] = {0};
			p_file.getField(name, 255);
			if(0 == name[0])
				continue;
			int ii = 0;
			CString strName(name);
			strName.TrimRight();
			strName.TrimRight('-');
			while(IsNameValid(strName)==false)
			{
				strName.Format("%s%d", strName,ii);
				ii++;
			}
			strucID.setID(strName);
			strct->init(strucID);

			long tid;
			p_file.getInteger(tid);
			CString texture;		
			const char* filename = Textures::GetTextureFileName((Textures::TextureEnum)tid);
			texture = filename;
			texture = texture.Left(texture.GetLength()-4);
				
			if (texture.CompareNoCase("lawn1") == 0)
			{
				texture = _T("lawn-1");
			}
			if (texture.CompareNoCase("lawn2") == 0)
			{
				texture = _T("lawn-2");
			}
			if (texture.CompareNoCase("lawn3") == 0)
			{
				texture = _T("lawn-3");
			}

			strct->setTexture(texture);
			long ptcount;
			p_file.getInteger(ptcount);
			long nfloor;
			p_file.getInteger(nfloor);
			for (int i = 0; i < ptcount; i++)
			{
				double ptx, pty, ptz;
				p_file.getFloat(ptx);
				p_file.getFloat(pty);
				p_file.getFloat(ptz);
				Point pt(ptx, pty, ptz);

				strct->addPoint(pt);
			}
			strct->setFloorNum(nfloor);
			m_structurelist.addItem (strct);

			p_file.getLine();
		}
	}
	if(p_file.getVersion() < 2.41)
	{
		p_file.getLine();

		ProcessorID strucID;

		assert( m_pInTerm );
		strucID.SetStrDict( m_pInTerm->inStrDict );

		while (!p_file.isBlank ())
		{
			CStructure *strct = new CStructure;
			//get name 
			if (!strucID.readProcessorID (p_file))
				throw new StringError ("Invalid processor IDs, ProcessorList::getProcessor");
			strct->init(strucID);

			//char name[256] = {0};
			//p_file.getField(name, 255);
			//if(0 == name[0])
			//	continue;

			//strct.SetNameStr(name);

			long tid;
			p_file.getInteger(tid);
			CString texture;			
			const char* filename = Textures::GetTextureFileName((Textures::TextureEnum)tid);
			texture = filename;
			texture = texture.Left(texture.GetLength()-4);

			if (texture.CompareNoCase("lawn1") == 0)
			{
				texture = _T("lawn-1");
			}
			if (texture.CompareNoCase("lawn2") == 0)
			{
				texture = _T("lawn-2");
			}
			if (texture.CompareNoCase("lawn3") == 0)
			{
				texture = _T("lawn-3");
			}
			strct->setTexture(texture);
			long ptcount;
			p_file.getInteger(ptcount);
			long nfloor;
			p_file.getInteger(nfloor);
			for (int i = 0; i < ptcount; i++)
			{
				double ptx, pty, ptz;
				p_file.getFloat(ptx);
				p_file.getFloat(pty);
				p_file.getFloat(ptz);
				Point pt(ptx, pty, ptz);

				strct->addPoint(pt);
			}
			strct->setFloorNum(nfloor);
			m_structurelist.addItem (strct);

			p_file.getLine();
		}
	}


}

void CStructureList::readData (ArctermFile& p_file)
{
	p_file.getLine();

	ProcessorID strucID;

	assert( m_pInTerm );
	strucID.SetStrDict( m_pInTerm->inStrDict );

	while (!p_file.isBlank ())
	{
		CStructure *strct = new CStructure;
		//get name 
		if (!strucID.readProcessorID (p_file))
			throw new StringError ("Invalid processor IDs, ProcessorList::getProcessor");
		strct->init(strucID);

		//char name[256] = {0};
		//p_file.getField(name, 255);
		//if(0 == name[0])
		//	continue;

		//strct.SetNameStr(name);

		char ti[256] = {0};
		p_file.getField(ti,255);
		strct->setTexture(ti);
		long ptcount;
		p_file.getInteger(ptcount);
		long nfloor;
		p_file.getInteger(nfloor);
		for (int i = 0; i < ptcount; i++)
		{
			double ptx, pty, ptz;
			p_file.getFloat(ptx);
			p_file.getFloat(pty);
			p_file.getFloat(ptz);
			Point pt(ptx, pty, ptz);

			strct->addPoint(pt);
		}
		strct->setFloorNum(nfloor);
		m_structurelist.addItem (strct);

		p_file.getLine();
	}

}

void CStructureList::writeData (ArctermFile& p_file) const
{
	for(int i = 0; i < (int)m_structurelist.size(); i++)
	{
		
		char szID[256];
		m_structurelist.getItem(i)->getID().printID (szID);
		p_file.writeField (szID);
        
		p_file.writeField((CString)m_structurelist.getItem(i)->getTexture());
			
		int ptcount = m_structurelist.getItem(i)->getPointNum();		
		p_file.writeInt(ptcount);
		p_file.writeInt(m_structurelist.getItem(i)->getFloorNum());
		for(int j = 0; j< ptcount; j ++)
		{
			Point pt = m_structurelist.getItem(i)->getPointAt(j);
			p_file.writeDouble( pt.getX() );
			p_file.writeDouble( pt.getY() );
			p_file.writeDouble( pt.getZ() );
		}

		p_file.writeLine();
		
	}
}
/************************************************************************
FUNCTION:build a tree structure
IN		:NULL
OUT		:NULL                                                                      
/************************************************************************/
void CStructureList::BuildProcGroupIdxTree()
{
	m_groupIdxTree.ClearTree();
	for( int i=0; i<static_cast<int>(m_structurelist.size()); i++ )
	{
		CStructureID tempID = GetStructure( i )->getID();		
		m_groupIdxTree.BuildTree( &tempID, getGroupIndexOriginal( tempID ) );
		while( tempID.getSuperGroup() )
		{
			m_groupIdxTree.BuildTree( &tempID, getGroupIndexOriginal( tempID ) );
		}		
	}
}
/************************************************************************
FUNCTION:Get a structure at specified position
IN		:nIndex,position
OUT		:a pointer of the structure                                                            
/************************************************************************/
CStructure* CStructureList::GetStructure(int nIndex) const
{
	ASSERT(nIndex >=0 && nIndex < m_structurelist.getCount());
	return m_structurelist.getItem(nIndex);
}
/************************************************************************
FUNCTION:get all the structure according with the pID from structure list
IN		:pID,the filtrate condition
OUT		:the id group that stand for the structure                                                                      
/************************************************************************/
GroupIndex CStructureList::getGroupIndexOriginal (const CStructureID& pID) const
{
	GroupIndex groupIndex = {-1, -1};

	// test for ALL structures
	if (pID.isBlank())
	{
		groupIndex.start = 0;
		groupIndex.end = static_cast<int>(m_structurelist.size()) - 1;
		return groupIndex;
	}

	int nFirstIndex = -1;
	int nLastIndex = -1;
	int nCount = static_cast<int>(m_structurelist.size());
	for( int i=0; i<nCount; i++ )
	{
		//		Processor *proc = procList.getItem ( i );
		//		if( prefixCompare ( *proc, (void*)&pID ) )
		if (pID.idFits(GetStructure(i)->getID()))
		{
			if( nFirstIndex == - 1 )
			{
				nFirstIndex = i;
			}
			nLastIndex = i;
		}
	}
	if ( nFirstIndex == -1 )
		return groupIndex;

	groupIndex.start = nFirstIndex;
	groupIndex.end = nLastIndex;    
	return groupIndex;
}
/************************************************************************
FUNCTION:get all the string at the first level of id
IN		:
OUT		:strDict,a list that stored the string that tally with                                                                 
/************************************************************************/
void CStructureList::getAllGroupNames (StringList& strDict) const
{
	GroupIndex group;
	group.start = 0;
	group.end = static_cast<int>(m_structurelist.size())-1;

	getGroupNames (strDict, group, 0);
}
/************************************************************************
FUNCTION:get all the string that at the specified level of the id from the specified structure GROUP
IN		:group,the structure group,such as, group {0,100}
		 level,the level of the structure id,it cann't larger than 4
OUT		:strDict,all the string at the level	                                                                      
/************************************************************************/
void CStructureList::getGroupNames (StringList& strDict,
								   const GroupIndex& group, int level) const
{
	char name[256];
	for (int i = group.start; i <= group.end; i++)
	{
		if (GetStructure(i)->getID().getNameAtLevel (name, level))
			strDict.addString (name);
	}
}
/************************************************************************
FUNCTION:make sure the structure id is valid
IN		:chName,a string of structure id
OUT		:true,valid
		 false,invalid                                                                      
/************************************************************************/
bool CStructureList::IsNameValid(const char* chName)
{
	CStructureID stID;
	stID.SetStrDict(m_pInTerm->inStrDict);
	stID.setID(chName);
	for (int i = 0; i< static_cast<int>(m_structurelist.size()) ;i++)
	{
		CStructureID tempid = GetStructure(i)->getID();
		if(tempid.idFits(stID)||stID.idFits(tempid))
			return false;
	}
	return true;
}
/************************************************************************
FUNCTION:Get all the structure after the structure pID
IN		:pID,the structure id
OUT		:strDict,the string list stored the id string at levelpID.idLength()
/************************************************************************/
void CStructureList::getMemberNames (const ProcessorID& pID, StringList& strDict) const
{
	GroupIndex group = getGroupIndexOriginal (pID);
	if (group.start != -1)
		getGroupNames (strDict, group, pID.idLength());
}

inline
static bool isAllNumeric(const char* buf)
{
	int len = strlen(buf);
	for(int i=0; i<len; i++) {
		if(!isNumeric(buf[i]))
			return false;
	}
	return true;
}


static void NextAlphaNumeric(char* buf)
{
	int len = strlen(buf);
	if(len==1) {
		if(buf[0]=='Z')
			strcpy(buf, "AA");
		else
			buf[0]++;
	}
	else if(len==2) {
		if(buf[1]=='Z') {
			if(buf[0]!='Z') {
				buf[0]++;
				buf[1]='A';
			}
			else
				strcat(buf, "_COPY");
		}
		else {
			buf[1]++; //increment last
		}
	}
	else {
		strcat(buf, "_COPY");
	}
}

CStructureID CStructureList::GetNextStructureID(const CStructureID& m_nNewID)
{
	char buf[128];
	CStructureID pNewID = m_nNewID;
	int lastlevel = pNewID.idLength()-1;
	pNewID.getNameAtLevel(buf,lastlevel);
	char c = toupper(buf[0]);
	int len = strlen(buf);
	if(isAllNumeric(buf))
	{
		int i = atoi(buf);
		do {
			i++;
			itoa(i,buf,10);
			pNewID.setNameAtLevel(buf,lastlevel);
		} while(!this->IsNameValid(pNewID.GetIDString()));
	}
	else if(len <= 2 && c >= 'A' && c <= 'Z') {
		do {
			NextAlphaNumeric(buf);
			pNewID.setNameAtLevel(buf,lastlevel);
			
		} while(!this->IsNameValid(pNewID.GetIDString()));
	}
	else {
		//other
		do {
			strcat(buf, "_COPY");
			pNewID.setNameAtLevel(buf,lastlevel);
		} while(!this->IsNameValid(pNewID.GetIDString()));
	}

	return pNewID;
}
