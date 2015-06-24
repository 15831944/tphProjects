// DefaultDisplayProperties.h: interface for the CDefaultDisplayProperties class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFAULTDISPLAYPROPERTIES_H__486516E0_06EF_43B7_AA90_11FC21172626__INCLUDED_)
#define AFX_DEFAULTDISPLAYPROPERTIES_H__486516E0_06EF_43B7_AA90_11FC21172626__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\DataSet.h"
#include "common\ARCColor.h"


#define DEFAULT_COLORREF_COUNT 8
#define DEFAULT_BOOLVAL_COUNT 6
#define RESERVED_LINE 10
class CDefaultDisplayProperties : public DataSet 
{
public:
	enum ENUM_COLORREF {
		AreasColor,
		PortalsColor,
		ProcShapeColor,
		ProcServLocColor,
		ProcQueueColor,
		ProcInConstraintColor,
		ProcOutConstraintColor,
		ProcNameColor
	};

	enum ENUM_BOOLVAL {
		ProcShapeOn,
		ProcServLocOn,
		ProcQueueOn,
		ProcInConstraintOn,
		ProcOutConstraintOn,
		ProcNameOn
	};

	CDefaultDisplayProperties();
	virtual ~CDefaultDisplayProperties();

	BOOL GetBoolValue(enum ENUM_BOOLVAL _enumBoolVal) const;
	const ARCColor3& GetColorValue(ENUM_COLORREF _enumClrRef) const;
	void SetBoolValue(enum ENUM_BOOLVAL _enumBoolVal, BOOL _bVal );
	void SetColorValue(ENUM_COLORREF _enumClrRef, const ARCColor3& _clrRef);

	virtual void clear (){}

	virtual void readData(ArctermFile& p_file);
    virtual void writeData(ArctermFile& p_file) const;

    virtual const char *getTitle(void) const { return "Default Display Properties Data"; }
    virtual const char *getHeaders (void) const{  return "Label,Data"; }

protected:
	ARCColor3 m_colorRef[DEFAULT_COLORREF_COUNT];
	BOOL m_boolVal[DEFAULT_BOOLVAL_COUNT];

};

#endif // !defined(AFX_DEFAULTDISPLAYPROPERTIES_H__486516E0_06EF_43B7_AA90_11FC21172626__INCLUDED_)
