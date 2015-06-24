#include "StdAfx.h"
#include ".\nonpaxrelativeposspec.h"
#include <algorithm> 
CNonPaxRelativePosSpec::CNonPaxRelativePosSpec(void): DataSet(NonPaxRelativePosFile,2.4f)
{
}

CNonPaxRelativePosSpec::~CNonPaxRelativePosSpec(void)
{
}

void CNonPaxRelativePosSpec::clear( void )
{
	for(int i=0;i<(int)m_DataList.size();i++)
		delete m_DataList.at(i);
	m_DataList.clear();
	m_vNonPaxTypeVisible.clear();
}

const char * CNonPaxRelativePosSpec::getTitle( void ) const
{
	return _T("Non Pax Relative Position Specification");
}

const char * CNonPaxRelativePosSpec::getHeaders( void ) const
{
	return _T("TypeName, Position, Rotation");
}

void CNonPaxRelativePosSpec::readObsoleteData(ArctermFile& p_file )
{
	read2_3Data(p_file);
	InitDefault();
}

void CNonPaxRelativePosSpec::InitDefault()
{
	if (m_vNonPaxTypeVisible.size() != 12)
	{
		m_vNonPaxTypeVisible.clear();
		m_vNonPaxTypeVisible.insert(std::make_pair(1,true));
		m_vNonPaxTypeVisible.insert(std::make_pair(2,true));
		m_vNonPaxTypeVisible.insert(std::make_pair(3,true));
		m_vNonPaxTypeVisible.insert(std::make_pair(8,true));
		m_vNonPaxTypeVisible.insert(std::make_pair(9,true));
		m_vNonPaxTypeVisible.insert(std::make_pair(10,true));
		m_vNonPaxTypeVisible.insert(std::make_pair(11,true));
		m_vNonPaxTypeVisible.insert(std::make_pair(12,true));
		m_vNonPaxTypeVisible.insert(std::make_pair(13,true));
		m_vNonPaxTypeVisible.insert(std::make_pair(14,true));
		m_vNonPaxTypeVisible.insert(std::make_pair(15,true));
		m_vNonPaxTypeVisible.insert(std::make_pair(16,true));
	}
}

void CNonPaxRelativePosSpec::read2_3Data(ArctermFile& p_file)
{
	clear();

	if(p_file.getVersion() < 2.3f )
		return ;

	while( p_file.getLine() )
	{	
		CNonPaxRelativePos* newData = new CNonPaxRelativePos();

		p_file.getInteger(newData->m_nTypeIdx);

		CPoint2008 pt;
		p_file.getPoint2008(pt);
		newData->m_relatePos = pt;

		double x,y,z,w;
		p_file.getFloat(x);
		p_file.getFloat(y);
		p_file.getFloat(z);
		p_file.getFloat(w);

		newData->m_rotate = ARCVector4(x,y,z,w);
		m_DataList.push_back(newData);
	}	
}

void CNonPaxRelativePosSpec::readData( ArctermFile& p_file )
{	
	clear();
	m_vNonPaxTypeVisible.clear();

	if(p_file.getVersion() < 2.4f )
		return ;

	p_file.getLine();
	int nNonPaxTypeCount;
	p_file.getInteger(nNonPaxTypeCount);
	p_file.getLine();
	for (int i = 0; i <nNonPaxTypeCount; i++)
	{
		int nNonPaxTypeIndex;
		p_file.getInteger(nNonPaxTypeIndex);
		int nVisible;
		bool bVisible;
		p_file.getInteger(nVisible);
		bVisible = nVisible?true:false;
		m_vNonPaxTypeVisible.insert(std::make_pair(nNonPaxTypeIndex,bVisible));
		p_file.getLine();
	}

	int nCount;
	p_file.getInteger(nCount);
	p_file.getLine();
	for (int i = 0; i < nCount; i++)
	{	
		CNonPaxRelativePos* newData = new CNonPaxRelativePos();
	
		p_file.getInteger(newData->m_nTypeIdx);

		CPoint2008 pt;
		p_file.getPoint2008(pt);
		newData->m_relatePos = pt;

		double x,y,z,w;
		p_file.getFloat(x);
		p_file.getFloat(y);
		p_file.getFloat(z);
		p_file.getFloat(w);

		newData->m_rotate = ARCVector4(x,y,z,w);
		m_DataList.push_back(newData);
		p_file.getLine();
	}	
	InitDefault();
}

void CNonPaxRelativePosSpec::writeData( ArctermFile& p_file ) const
{
	int nNonPaxTypeCount = (int)m_vNonPaxTypeVisible.size();
	p_file.writeInt(nNonPaxTypeCount);
	p_file.writeLine();
	std::map<int,bool>::const_iterator iter = m_vNonPaxTypeVisible.begin();
	for (; iter != m_vNonPaxTypeVisible.end(); ++iter)
	{
		p_file.writeInt((*iter).first);
		p_file.writeInt((*iter).second?1:0);
		p_file.writeLine();
	}
	int nCount = GetCount();
	p_file.writeInt(nCount);
	p_file.writeLine();
	for(int i=0;i<nCount;i++)
	{
		CNonPaxRelativePos* pPosData = GetItem(i);
		p_file.writeInt(pPosData->m_nTypeIdx);
		p_file.writePoint2008(pPosData->m_relatePos);
		p_file.writeDouble(pPosData->m_rotate.x);
		p_file.writeDouble(pPosData->m_rotate.y);
		p_file.writeDouble(pPosData->m_rotate.z);
		p_file.writeDouble(pPosData->m_rotate.w);
		p_file.writeLine();
	}
}

bool CNonPaxRelativePosSpec::GetNonPaxRelatePosition( CPoint2008& ptRelatePos, ARCVector4& rotation, 
													 int nNonPaxTypeID, int nNonPaxIndex /*= 0*/)
{
	int nTypeIndex = 0;
	for(int i = 0; i < GetCount(); i++ )
	{
		CNonPaxRelativePos* pItem = GetItem(i);
		if(pItem->m_nTypeIdx != nNonPaxTypeID)
			continue;
		if (GetTypeItemCount(nNonPaxTypeID) > nNonPaxIndex)
		{
			if (nTypeIndex != nNonPaxIndex)
			{
				nTypeIndex++;
				continue;
			}
		}

		ptRelatePos =  pItem->m_relatePos;
		rotation = pItem->m_rotate;
		return true;
	}
	ptRelatePos.setPoint(0.0, 0.0, 0.0);
	return false;
}

int CNonPaxRelativePosSpec::GetTypeItemCount( int nTypeIdx) const
{
	int nCount = 0;
	for(int i=0;i<GetCount();i++){
		CNonPaxRelativePos* pItem = GetItem(i);
		if(pItem->m_nTypeIdx == nTypeIdx){
			nCount++;
		}
	}
	return nCount;
}

bool CNonPaxRelativePosSpec::SetTypeItemVisible(int nTypeIdx,bool bVisible)
{
	std::map<int,bool>::iterator iter;
	iter = m_vNonPaxTypeVisible.find(nTypeIdx);
	if (iter != m_vNonPaxTypeVisible.end())
	{
		(*iter).second = bVisible;
		return true;
	}
	return false;
}

bool CNonPaxRelativePosSpec::GetTypeItemVisible(int nTypeIdx)
{
	std::map<int,bool>::iterator iter;
	iter = m_vNonPaxTypeVisible.find(nTypeIdx);
	if (iter != m_vNonPaxTypeVisible.end())
	{
		return (*iter).second;
		
	}
	return true;
}

void CNonPaxRelativePosSpec::RemoveData( CNonPaxRelativePos* pPos )
{
	std::vector<CNonPaxRelativePos*>::iterator itr = std::find(m_DataList.begin(),m_DataList.end(),pPos);
	if(itr!=m_DataList.end())
		m_DataList.erase(itr);
	delete pPos;
}

