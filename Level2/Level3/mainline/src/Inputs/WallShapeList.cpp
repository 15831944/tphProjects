#include "StdAfx.h"
#include ".\wallshapelist.h"
#include "../common/exeption.h"
#include "in_term.h"
#include <Common/strdict.h>

#define isNumeric(x)  ((x>=48&&x<=57)||x==43||x==45||x==46)
WallShapeList::WallShapeList() : DataSet(TerminalWallShapeFile, (float)2.4)
{
}


WallShapeList::WallShapeList(int ftype)
: DataSet(ftype, (float)2.4)
{
}

void WallShapeList::clear (void)
{
	for(int i=0;i<(int)m_shapelist.size();++i)
	{
		delete m_shapelist[i];
	}
	m_shapelist.clear();
}
void WallShapeList::readObsoleteData( ArctermFile& p_file )
{
	if(p_file.getVersion() <= 2.21)
	{
		p_file.getLine();

		ProcessorID strucID;

		assert( m_pInTerm );
		strucID.SetStrDict( m_pInTerm->inStrDict );

		while (!p_file.isBlank ())
		{
			WallShape* wallsh = new WallShape;

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

			wallsh->init(strucID);
//			get height
			double heigth_;
			p_file.getFloat(heigth_);
			wallsh->SetHeight(heigth_);
			//get width
			double width_;
			p_file.getFloat(width_);
			wallsh->SetWidth(width_);
			//get FloornUM
			int floor_;
			p_file.getInteger(floor_);
			wallsh->SetFloorIndex(floor_);
			//get path
			//
			int ptcount;
			p_file.getInteger(ptcount);
			wallsh->clearPath();
			for(int i=0;i<ptcount;++i){
				Point pt;
				p_file.getPoint(pt);
				wallsh->AddPoint(pt);
			}
			//
			m_shapelist.addItem(wallsh);

			p_file.getLine();
		}

	}
	else if ( p_file.getVersion() <= 2.30 && p_file.getVersion() > 2.21 )
	{
		p_file.getLine();

		ProcessorID strucID;

		assert( m_pInTerm );
		strucID.SetStrDict( m_pInTerm->inStrDict );

		while (!p_file.isBlank ())
		{
			WallShape* wallsh = new WallShape;
			//get name 
			if (!strucID.readProcessorID (p_file))
				throw new StringError ("Invalid processor IDs, ProcessorList::getProcessor");
			wallsh->init(strucID);

			//		char name[256] = {0};
			//		p_file.getField(name, 255);
			//		if(0 == name[0])
			//			continue;
			////		wallsh.SetNameStr(name);
			//		wallsh->setID(name);
			//get height
			double heigth_;
			p_file.getFloat(heigth_);
			wallsh->SetHeight(heigth_);
			//get width
			double width_;
			p_file.getFloat(width_);
			wallsh->SetWidth(width_);
			//get FloornUM
			int floor_;
			p_file.getInteger(floor_);
			wallsh->SetFloorIndex(floor_);
			//get path
			//
			int ptcount;
			p_file.getInteger(ptcount);
			wallsh->clearPath();
			for(int i=0;i<ptcount;++i){
				Point pt;
				p_file.getPoint(pt);
				wallsh->AddPoint(pt);
			}

			m_shapelist.addItem(wallsh);

			p_file.getLine();
		}
	}
}
void WallShapeList::readData (ArctermFile& p_file)
{
	p_file.getLine();

	ProcessorID strucID;

	assert( m_pInTerm );
	strucID.SetStrDict( m_pInTerm->inStrDict );
	std::vector<WallShape*> invalidShape;

	while (!p_file.isBlank ())
	{
		WallShape* wallsh = new WallShape;
		//get name 
		if (!strucID.readProcessorID (p_file))
			throw new StringError ("Invalid processor IDs, ProcessorList::getProcessor");
		wallsh->init(strucID);

//		char name[256] = {0};
//		p_file.getField(name, 255);
//		if(0 == name[0])
//			continue;
////		wallsh.SetNameStr(name);
//		wallsh->setID(name);
		//get height
		double heigth_;
		p_file.getFloat(heigth_);
		wallsh->SetHeight(heigth_);
		//get width
		double width_;
		p_file.getFloat(width_);
		wallsh->SetWidth(width_);
		//get FloornUM
		int floor_;
		p_file.getInteger(floor_);
		wallsh->SetFloorIndex(floor_);
		//get path
		//
		int ptcount;
		p_file.getInteger(ptcount);
		wallsh->clearPath();
		for(int i=0;i<ptcount;++i){
			Point pt;
			p_file.getPoint(pt);
			wallsh->AddPoint(pt);
		}
		//get Display Properties
		WallShape::CWallShapeDispProperties prop;
		int nProp=0;
		while (nProp<WSDP_ARRAY_SIZE)
		{
			int disp;
			p_file.getInteger(disp);
			prop.bDisplay[nProp] = (BOOL)disp;
			long ulClr;
			p_file.getInteger(ulClr);
			prop.color[nProp] = ulClr;
			nProp++;
		}
		wallsh->SetWallShapeDispProperties(prop);
	
		if( IsNameValid(wallsh->getID().GetIDString()) )
		{
			m_shapelist.addItem(wallsh);
		}
		else{
			invalidShape.push_back(wallsh);
		}

		p_file.getLine();
	}


	for(int i=0;i<(int)invalidShape.size();++i)
	{
		WallShape* pShape = invalidShape[i];
		CStructureID nextID = GetNextWallID(pShape->getID());
		pShape->setID(nextID.GetIDString());
		m_shapelist.addItem(pShape);
	}
}

void WallShapeList::writeData (ArctermFile& p_file) const
{
	int nSCount=m_shapelist.size();
	for(int i = 0; i < nSCount; i++)
	{
		//Write name
//		p_file.writeField( (const char*)m_shapelist[i].GetNameStr() );
//		p_file.writeField()
		char szID[256];
		m_shapelist.getItem(i)->getID().printID (szID);
		p_file.writeField (szID);
		//write height
		p_file.writeDouble(m_shapelist.getItem(i)->GetHeight());
		//write width
		p_file.writeDouble(m_shapelist.getItem(i)->GetWidth());
		//write floorNum
		p_file.writeInt(m_shapelist.getItem(i)->GetFloorIndex());
		//write path
		//write point count		
		int nPtCount=m_shapelist.getItem(i)->GetPointCount();
		p_file.writeInt(nPtCount);
		//write
		for(int k=0;k<nPtCount;k++){
			Point pt=m_shapelist.getItem(i)->GetPointAt(k);
			p_file.writePoint(pt);
		}
		//write display properties
		WallShape::CWallShapeDispProperties prop;
		m_shapelist.getItem(i)->GetWallShapeDispProperties(prop);

		int nProp=0;
		while (nProp<WSDP_ARRAY_SIZE)
		{
			p_file.writeInt((int)prop.bDisplay[nProp]);
			p_file.writeInt((long)prop.color[nProp]);
			nProp++;
		}
		

		p_file.writeLine();
	}
}

/************************************************************************
FUNCTION:build a tree structure
IN		:NULL
OUT		:NULL                                                                      
/************************************************************************/
void WallShapeList::BuildProcGroupIdxTree()
{
	m_groupIdxTree.ClearTree();
	for( int i=0; i<static_cast<int>(m_shapelist.size()); i++ )
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
WallShape* WallShapeList::GetStructure(int nIndex) const
{
	ASSERT(nIndex >=0 && nIndex < m_shapelist.getCount());
	return m_shapelist.getItem(nIndex);
}
/************************************************************************
FUNCTION:get all the structure according with the pID from structure list
IN		:pID,the filtrate condition
OUT		:the id group that stand for the structure                                                                      
/************************************************************************/
GroupIndex WallShapeList::getGroupIndexOriginal (const CStructureID& pID) const
{
	GroupIndex groupIndex = {-1, -1};

	// test for ALL structures
	if (pID.isBlank())
	{
		groupIndex.start = 0;
		groupIndex.end = static_cast<int>(m_shapelist.size()) - 1;
		return groupIndex;
	}

	int nFirstIndex = -1;
	int nLastIndex = -1;
	int nCount = static_cast<int>(m_shapelist.size());
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
void WallShapeList::getAllGroupNames (StringList& strDict) const
{
	GroupIndex group;
	group.start = 0;
	group.end = static_cast<int>(m_shapelist.size())-1;

	getGroupNames (strDict, group, 0);
}
/************************************************************************
FUNCTION:get all the string that at the specified level of the id from the specified structure GROUP
IN		:group,the structure group,such as, group {0,100}
level,the level of the structure id,it cann't larger than 4
OUT		:strDict,all the string at the level	                                                                      
/************************************************************************/
void WallShapeList::getGroupNames (StringList& strDict,
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
FUNCTION:get all the string at the first level of id
IN		:
OUT		:strDict,a list that stored the string that tally with                                                                 
/************************************************************************/
bool WallShapeList::IsNameValid(const char* chName)
{
	CStructureID stID;
	stID.SetStrDict(m_pInTerm->inStrDict);
	stID.setID(chName);
	for (int i = 0; i< static_cast<int>(m_shapelist.size()) ;i++)
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
void WallShapeList::getMemberNames (const  CStructureID& pID, StringList& strDict) const
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

CStructureID WallShapeList::GetNextWallID(const CStructureID& m_nNewID)
{
	//char buf[128];
	//CStructureID pNewID = m_nNewID;
	//int lastlevel = pNewID.idLength()-1;
	//pNewID.getNameAtLevel(buf,lastlevel);
	//char c = toupper(buf[0]);
	//int len = strlen(buf);
	//if(isAllNumeric(buf))
	//{
	//	int i = atoi(buf);
	//	do {
	//		i++;
	//		itoa(i,buf,10);
	//		pNewID.setNameAtLevel(buf,lastlevel);
	//	} while(!this->IsNameValid(buf));
	//}
	//else if(len <= 2 && c >= 'A' && c <= 'Z') {
	//	do {
	//		NextAlphaNumeric(buf);
	//		pNewID.setNameAtLevel(buf,lastlevel);
	//	} while(!this->IsNameValid(buf));
	//}
	//else {
	//	//other
	//	do {
	//		strcat(buf, "_COPY");
	//		pNewID.setNameAtLevel(buf,lastlevel);
	//	} while(!this->IsNameValid(buf));
	//}

	//return pNewID;

	char buf[128];
	CStructureID pNewID = m_nNewID;
	int lastLevel = pNewID.idLength()-1;
	pNewID.getNameAtLevel(buf, lastLevel);

	//check if last level of ID is numeric or alphanumeric
	char c = toupper(buf[0]);
	int len = strlen(buf);
	if(isAllNumeric(buf)) {
		//numeric
		int i = atoi(buf);
		do {
			i++;
			_itoa(i,buf,10);
			pNewID.setNameAtLevel(buf,lastLevel);
		} while(!this->IsNameValid(pNewID.GetIDString()));
	}
	else if(len <= 2 && c >= 'A' && c <= 'Z') {
		char d = toupper(buf[len - 1]);
		if( d >= 'A' && d <= 'Z')
		{
			do {
				NextAlphaNumeric(buf);
				pNewID.setNameAtLevel(buf,lastLevel);
			} while(!this->IsNameValid(pNewID.GetIDString()));
		}
		else
		{
			do {
				strcat(buf, "_COPY");
				pNewID.setNameAtLevel(buf,lastLevel);
			} while(!this->IsNameValid(pNewID.GetIDString()));
		}
	}
	else {
		//other
		do {
			strcat(buf, "_COPY");
			pNewID.setNameAtLevel(buf,lastLevel);
		} while(!this->IsNameValid(pNewID.GetIDString()));
	}

	return pNewID;
}

WallShapeList::~WallShapeList()
{
	clear();
}
