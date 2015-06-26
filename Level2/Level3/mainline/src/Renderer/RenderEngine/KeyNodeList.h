#pragma once

/**
the nodetype should have function getKey which will return the keytype
*/
template <class KeyType, class NodeType>
class Key3DNodeList
{
public:
	typedef boost::shared_ptr<NodeType> NodeTypePtr;
	typedef std::vector<NodeTypePtr> NodeList;

	int GetCount(){ return (int)mvNodeList.size(); }
	NodeTypePtr& GetItem(int idx){ return mvNodeList.at(idx); }
	NodeTypePtr& FindItem(KeyType pData)
	{
		for(int i=0;i<GetCount();i++)
		{
			NodeTypePtr& pNode = GetItem(i);
			if(pNode->getKey()==pData)
				return pNode;
		}
		static NodeTypePtr nul((NodeType*)NULL);
		return nul;
	}
	void RemoveItem(KeyType pData)
	{
		for(NodeList::iterator itr = mvNodeList.begin();itr!=mvNodeList.end();itr++)
		{
			NodeTypePtr& item = *itr;
			if(item->getKey() == pData)
			{
				mvNodeList.erase(itr);
				return;
			}
		}
	}
	void Clear(){ mvNodeList.clear(); }	
protected:
	NodeList mvNodeList;

};