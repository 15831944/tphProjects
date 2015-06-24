
#pragma once

class BlockTag
{
public:
	BlockTag(){}
	BlockTag(const CString& s):m_sdec(s)
	{}
protected:
	CString m_sdec;
};