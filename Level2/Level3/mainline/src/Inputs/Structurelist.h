#ifndef __STRUCTURE_LIST_
#define __STRUCTURE_LIST_

#include "Structure.h"
#include "common\dataset.h"
#include "../Engine/ProcGroupIdxTree.h"
#define isNumeric(x)  ((x>=48&&x<=57)||x==43||x==45||x==46)
class CStructureList : public DataSet
{
public:
	CStructureList();
	CStructureList(int/*InputFiles*/ ftype);

protected:
	StructureArray m_structurelist;
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
	CStructure* GetStructure(int nIndex) const;
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
	void getMemberNames (const ProcessorID& pID, StringList& strDict) const;

	const StructureArray& GetStructureList() const { return m_structurelist; }
	size_t getStructureNum()
	{
		return m_structurelist.getCount();
	}
	CStructure* getStructureAt(int idx)
	{
		int count=m_structurelist.getCount();
		ASSERT(idx >=0 && idx< count);
		return m_structurelist.getItem(idx);
	}
	CStructure *getStructure(const ProcessorID& id)
	{
		GroupIndex group = getGroupIndexOriginal (id);
		if (group.start != group.end)
			return NULL;

		if (group.start != -1)
			return m_structurelist.getItem (group.start);
		else
			return NULL;
	}
	void addStructure(CStructure* pws)
	{
		m_structurelist.addItem(pws);
		BuildProcGroupIdxTree();
	}
	void removeStructure (const CStructureID& id)
	{
		GroupIndex deleteItem = getGroupIndexOriginal (id);
		if (deleteItem.start == deleteItem.end)
		{
			m_structurelist.deleteItem (deleteItem.start);
			BuildProcGroupIdxTree();
		}
	}
	void removeStructure(int idx)
	{
		removeStructure(getStructureAt(idx)->getID());
		//int nCount=m_shapelist.getCount();
		//if(! (idx< nCount) )return;
		//std::vector<WallShape> ::iterator itr=m_shapelist.begin();
		//for(int i=0;i<idx;i++)itr++;
		//m_shapelist.erase(itr);
	}
	~CStructureList(){
		
	}

    CStructureID GetNextStructureID(const CStructureID& m_nNewID);
public:
	// Removes all processors from the list
	virtual void clear (void);

	virtual void readData (ArctermFile& p_file);

	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Structure Layout File"; }

	virtual const char *getHeaders (void) const { return "GLuint, Structure point list."; }

	virtual void readObsoleteData ( ArctermFile& p_file );
};

#endif