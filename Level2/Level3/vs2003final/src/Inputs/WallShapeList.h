#ifndef __SHAPELIST_H_
#define __SHAPELIST_H_

#include "WallShape.h"
#include "common\dataset.h"
#include "../Engine/ProcGroupIdxTree.h"


class WallShapeList:public DataSet
{
public:
	WallShapeList();
	WallShapeList(int/*InputFiles*/ ftype);

	~WallShapeList();

protected:
	//	std::vector<WallShape > m_shapelist;
	WallShapArray	m_shapelist;
	CProcGroupIdxTree m_groupIdxTree;


private:
	/************************************************************************
	FUNCTION:build a tree structure
	IN		:NULL
	OUT		:NULL                                                                      
	/************************************************************************/
	void BuildProcGroupIdxTree();
	/************************************************************************
	FUNCTION:Get a structure at specified position
	IN		:nIndex,position
	OUT		:a pointer of the structure                                                            
	/************************************************************************/
	WallShape* GetStructure(int nIndex) const;
	/************************************************************************
	FUNCTION:get all the structure according with the pID from structure list
	IN		:pID,the filtrate condition
	OUT		:the id group that stand for the structure                                                                      
	/************************************************************************/
	GroupIndex getGroupIndexOriginal (const CStructureID& pID) const;
public:
	/************************************************************************
	FUNCTION:make sure the structure id is valid
	IN		:chName,a string of structure id
	OUT		:true,valid
	false,invalid                                                                      
	/************************************************************************/
	bool IsNameValid(const char* chName);
	/************************************************************************
	FUNCTION:get all the string at the first level of id
	IN		:
	OUT		:strDict,a list that stored the string that tally with                                                                 
	/************************************************************************/
	void getAllGroupNames (StringList& strDict) const;
	/************************************************************************
	FUNCTION:get all the string that at the specified level of the id from the specified structure GROUP
	IN		:group,the structure group,such as, group {0,100}
	level,the level of the structure id,it cann't larger than 4
	OUT		:strDict,all the string at the level	                                                                      
	/************************************************************************/
	void getGroupNames (StringList& strDict,const GroupIndex& group, int level) const;
	/************************************************************************
	FUNCTION:Get all the structure after the structure pID
	IN		:pID,the structure id
	OUT		:strDict,the string list stored the id string at levelpID.idLength()
	/************************************************************************/
	void getMemberNames (const  CStructureID& pID, StringList& strDict) const;

	const WallShapArray& GetShapelist() const { return m_shapelist; }

	size_t getShapeNum()
	{
		return m_shapelist.getCount();
	}
	WallShape* getShapeAt(int idx)
	{
		int count=m_shapelist.getCount();
		ASSERT(idx >=0 && idx< count);
		return m_shapelist.getItem(idx);
	}
	WallShape *getWallShape(const  CStructureID& id)
	{
		GroupIndex group = getGroupIndexOriginal (id);
		if (group.start != group.end)
			return NULL;

		if (group.start != -1)
			return m_shapelist.getItem (group.start);
		else
			return NULL;
	}
	void addShape(WallShape* pws)
	{
		m_shapelist.addItem(pws);
		BuildProcGroupIdxTree();
	}
	void removeShape (const CStructureID& id)
	{
		GroupIndex deleteItem = getGroupIndexOriginal (id);
		if (deleteItem.start == deleteItem.end)
		{
			m_shapelist.deleteItem (deleteItem.start);
			BuildProcGroupIdxTree();
		}
	}
	void removeShape(int idx)
	{
		removeShape(getShapeAt(idx)->getID());
		//int nCount=m_shapelist.getCount();
		//if(! (idx< nCount) )return;
		//std::vector<WallShape> ::iterator itr=m_shapelist.begin();
		//for(int i=0;i<idx;i++)itr++;
		//m_shapelist.erase(itr);
	}
	//void removeShapeByName(CString & str){
	//	std::vector<WallShape> ::iterator itr=m_shapelist.begin();
	//	for(size_t i=0;i< m_shapelist.size();i++){
	//		if(m_shapelist[i].GetNameStr()==str)break;
	//		itr++;
	//	}
	//	m_shapelist.erase(itr);
	//}
	

	CStructureID GetNextWallID(const CStructureID& m_nNewID); 

public:
	// Removes all processors from the list
	virtual void clear (void);

	virtual void readData (ArctermFile& p_file);

	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Wall Shape Layout File"; }

	virtual const char *getHeaders (void) const { return "GLuint, Wall shape point list."; }
	virtual void readObsoleteData ( ArctermFile& p_file );
};

#endif