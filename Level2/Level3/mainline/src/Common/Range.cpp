#include "stdafx.h"
#include "range.h"


void CNumRange::MinorMerge( const CNumRange& other ) 
{
	m_minor = max(other.m_minor,m_minor);
	m_max = min(other.m_max,m_max);
}