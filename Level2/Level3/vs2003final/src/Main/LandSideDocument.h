#pragma once


//manager all landside stuff
class CTermPlanDoc;
class LandsideLayoutEditor;
class InputLandside;

class LandsideDocument
{
public:
	LandsideDocument(CTermPlanDoc * _pDoc);
	virtual ~LandsideDocument(void);

	
private:
	CTermPlanDoc * m_pParentDoc;
	//instance
	InputLandside* m_pInput;
	LandsideLayoutEditor* m_pLayoutEditor;
};
