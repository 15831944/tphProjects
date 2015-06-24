// DirectionUtil.h: interface for the CDirectionUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTIONUTIL_H__D52C0274_AF0F_4098_AAB9_DDAF972D48B9__INCLUDED_)
#define AFX_DIRECTIONUTIL_H__D52C0274_AF0F_4098_AAB9_DDAF972D48B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPaxFlowConvertor;
class CSinglePaxTypeFlow;
class CMobileElemConstraint;
class ProcessorID;
class LandsideTerminalLinkageInSimManager;
class CDirectionUtil  
{
public:
	CDirectionUtil();
	virtual ~CDirectionUtil();

public:
	void InitAllProcDirection( CPaxFlowConvertor& _convertor, LandsideTerminalLinkageInSimManager *pLinkage );

	virtual void ExpandAllProcessInFlow( CSinglePaxTypeFlow* _pFlow );
	virtual void BuildAllPossiblePath(  CSinglePaxTypeFlow* _pFlow , std::vector<CSinglePaxTypeFlow*>& _allPossibleFlow);


	virtual void BuildDirctionFromEverySinglePath( CSinglePaxTypeFlow* _pFlow , const CMobileElemConstraint& _paxType );

public:
	LandsideTerminalLinkageInSimManager *m_pLTLinkageManager; 
private:
	void TravelPath(  CSinglePaxTypeFlow* _pFlow ,const ProcessorID& _rootID , std::vector<ProcessorID>& _result );

};

#endif // !defined(AFX_DIRECTIONUTIL_H__D52C0274_AF0F_4098_AAB9_DDAF972D48B9__INCLUDED_)
