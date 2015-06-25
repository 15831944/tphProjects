#pragma once


#ifdef _DEBUG // used only in debug mode

#include <vector>
#include <string>
#include <ostream>

class ARCProbPlacer
{
public:
	ARCProbPlacer(const char* pFileName);
	~ARCProbPlacer(void);

	bool Init(const char* pFileName);
	bool Check(const char* p) const;
	void Dump(std::ostream& os) const;

private:
	bool m_bToFetch;
	std::vector<std::string> m_vecItems;

	class DumpString
	{
	public:
		DumpString(std::ostream& os)
			: _os(os)
		{

		}
		void operator ()(const std::string& str)
		{
			_os << str << std::endl;
		}
	private:
		std::ostream& _os;
	};

};

#endif // _DEBUG