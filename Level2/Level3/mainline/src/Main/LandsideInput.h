#ifndef __LANDSIDE_INPUT_
#define __LANDSIDE_INPUT_

#pragma once

	class LandsideDocument;
	class LandsideInput 
	{
	public:

		LandsideInput(LandsideDocument * _pDoc);

		virtual ~LandsideInput(void);
	
		
	protected:
		LandsideDocument * m_pDoc;


	private:
		
	};


#endif