#pragma once
#include <Common/dataset.h>
//#include <ogre/OgreQuaternion.h>
#include <common\Point2008.h>
#include <common/ARCVector4.h>
#include "Common/point.h"
class CNonPaxRelativePos
{
public:
	CNonPaxRelativePos()
 		:m_nTypeIdx(-1)
 		,m_relatePos(0.0,0.0,0.0)
 		,m_rotate(0.0,0.0,0.0,1.0)
	{

	}
	int m_nTypeIdx;
	CPoint2008 m_relatePos;
	ARCVector4 m_rotate;
};


class CNonPaxRelativePosSpec : public DataSet
{
public:
	CNonPaxRelativePosSpec(void);
	~CNonPaxRelativePosSpec(void);

	int GetCount()const{  return (int)m_DataList.size(); }
	CNonPaxRelativePos* GetItem(int idx)const{ return m_DataList.at(idx); }
	typedef std::vector<CNonPaxRelativePos*> NonPaxRelativePosList;

	bool GetNonPaxRelatePosition(CPoint2008& ptRelatePos, ARCVector4& rotation, int nNonPaxTypeID, int nNonPaxIndex = 0);
	//return defined count of type
	int GetTypeItemCount(int nTypeIdx)const;
	bool GetTypeItemVisible(int nTypeIdx);
	bool SetTypeItemVisible(int nTypeIdx,bool bVisible);
	void InitDefault();

	//
	void AddData(CNonPaxRelativePos* pPos){ m_DataList.push_back(pPos); }
	void RemoveData(CNonPaxRelativePos* pPos);

//implement of dataset
	virtual void clear (void);

	virtual void readData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;
	virtual void readObsoleteData(ArctermFile& p_file );
	void read2_3Data(ArctermFile& p_file);

	virtual const char *getTitle (void) const ;
	virtual const char *getHeaders (void) const ;

protected:
	NonPaxRelativePosList m_DataList;
	std::map<int,bool> m_vNonPaxTypeVisible;
};
