#include "StdAfx.h"
#include ".\arcprobplacer.h"

#ifdef _DEBUG // used only in debug mode

#include <fstream>
#include <algorithm>

#define PROB_STRING_MAX_LEN             (1024)

ARCProbPlacer::ARCProbPlacer( const char* pFileName )
	: m_bToFetch(true)
{
	Init(pFileName);
}

ARCProbPlacer::~ARCProbPlacer(void)
{
}

bool ARCProbPlacer::Init( const char* pFileName )
{
	if (m_bToFetch)
	{
		m_vecItems.clear();
		std::ifstream is;
		is.open(pFileName, std::ios::in);
		if (is.is_open())
		{
			while (!is.eof())
			{
				char buf[PROB_STRING_MAX_LEN];
				is.getline(buf, PROB_STRING_MAX_LEN - 1);
				if (!Check(buf))
                    m_vecItems.push_back(buf);
			}
			return true;
		}
		m_bToFetch = false;
	}
	return false;
}

bool ARCProbPlacer::Check( const char* p ) const
{
	return m_vecItems.end() != std::find(m_vecItems.begin(), m_vecItems.end(), p);
}

void ARCProbPlacer::Dump( std::ostream& os ) const
{
	std::for_each(m_vecItems.begin(), m_vecItems.end(), DumpString(os));
}

#endif // _DEBUG