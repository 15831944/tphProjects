// StatisticalTools.h: interface for the CStatisticalTools class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATISTICALTOOLS_H__85EAE664_A839_4EA1_BA3E_33F2E1827DDB__INCLUDED_)
#define AFX_STATISTICALTOOLS_H__85EAE664_A839_4EA1_BA3E_33F2E1827DDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include<vector>
#include <algorithm>
template <class DataType>
class CStatisticalTools  
{
	std::vector<DataType>m_vDatas;
public:
	CStatisticalTools(){};
	virtual ~CStatisticalTools(){};
public:
	void AddNewData(  const DataType& _newData , bool _bAutoSort = false )
	{
		m_vDatas.push_back( _newData );
		if( _bAutoSort )
		{
			SortData();
		}
	};
	DataType GetPercentile( int _iPercent )const
	{
		ASSERT( _iPercent>0 && _iPercent <=100 );
		int iSize = (int)m_vDatas.size();		
		if( iSize <= 0 )
		{
			return DataType(0.0);
		}
		int iIdx = (int)((iSize*_iPercent / 100.0) + 0.5);
		if (iIdx != 0)
			iIdx--;
		
		ASSERT( iIdx >=0 );
		if( iIdx < iSize )
		{
			return m_vDatas[ iIdx ];
		}
		else
		{
			return m_vDatas[ iSize -1 ];
		}
		
	};

	// get percentile using Microsoft Excel Algorithm
	DataType GetMSExcelPercentile(int _iPercent)const
	{
		ASSERT(_iPercent>0 && _iPercent <=100);
		int iSize = (int)m_vDatas.size();
		if(iSize <= 0)
		{
			return DataType(0.0);
		}
		else if(iSize == 1 && _iPercent < 100)
		{
			return DataType(0.0);
		}
		else if(_iPercent == 100)
		{
			return m_vDatas[iSize - 1];
		}
		else
		{
			double k_d = (double)(iSize - 1) * (double)_iPercent / 100.0f;
			int k = (int)k_d;
			double d = k_d - k;
			DataType pPercentile = m_vDatas[k] + d * (m_vDatas[k+1] - m_vDatas[k]);
			return pPercentile;
		}
	};

	double GetSigma()const
	{
		int iSize = (int)m_vDatas.size();
		if( iSize <= 1 )
		{
			return 0.0;
		}
		double dQ = 0.0;
		double dValue1 = 0.0;
		double dValue2 = 0.0;
		for( int i=0; i<iSize; ++i )
		{
			dValue1 += m_vDatas[ i ] * m_vDatas[ i ]; 
			//// TRACE ("\n%d\n",(long)(m_vDatas[i]) );
			dValue2 += (double)(m_vDatas[ i ]) ;
		}

		dQ = dValue1 - dValue2* dValue2 / iSize;
		if (dQ < 0.0)
		{
			return 0.0;
		}
		return sqrt( dQ / ( iSize -1 ) );
	};

	void SortData(){ std::sort(m_vDatas.begin(),m_vDatas.end() );};
	void CopyToolData(CStatisticalTools& tool)
	{
		m_vDatas.insert(m_vDatas.end(), tool.m_vDatas.begin(),tool.m_vDatas.end());
		SortData();
	};

	double GetMin()const
	{
		int iSize = (int)m_vDatas.size();
		if( iSize <= 0 )
			return 0.0;
	
		double dMin = m_vDatas.at(0);
		for( int i=1; i< iSize; ++i )
		{
			if (dMin > m_vDatas.at(i))
				dMin = m_vDatas.at(i);
		}

		return dMin;
	}

	double GetMax()const
	{
		int iSize = (int)m_vDatas.size();
		if( iSize <= 0 )
			return 0.0;

		double dMax = m_vDatas.at(0);
		for( int i=1; i< iSize; ++i )
		{
			if (dMax < m_vDatas.at(i))
				dMax = m_vDatas.at(i);
		}

		return dMax;
	}

	double GetAvarage()const
	{
		double dValue1 = 0.0;
		double dSum = 0.0;
		int iSize = (int)m_vDatas.size();
		if( iSize <= 0 )
		{
			return 0.0;
		}
		
		for( int i=0; i<iSize; ++i )
		{
			dSum += m_vDatas[i];
		}

		dValue1 = dSum/iSize;
		return dValue1;
	}

    DataType GetSum()const
    {
        DataType sum = 0;
        for(size_t i=0; i<m_vDatas.size(); ++i )
        {
            sum += m_vDatas[i];
        }
        return sum;
    }

	void Clear()
	{
		m_vDatas.clear();
	}
};

#endif // !defined(AFX_STATISTICALTOOLS_H__85EAE664_A839_4EA1_BA3E_33F2E1827DDB__INCLUDED_)
