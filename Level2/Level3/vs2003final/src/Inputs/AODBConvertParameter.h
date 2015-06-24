#pragma once
#include "Common/ALTObjectID.h"
#include <afxdisp.h>

class InputTerminal;

namespace AODB
{
	class FieldMapFile;

	class ConvertParameter
	{
	public:
		ConvertParameter();
		~ConvertParameter();

	public:
		COleDateTime oleStartDate;

		InputTerminal *pTerminal;

		ALTObjectIDList m_vStandList;

		AODB::FieldMapFile*	pMapFile;

	protected:
	private:
	};
}