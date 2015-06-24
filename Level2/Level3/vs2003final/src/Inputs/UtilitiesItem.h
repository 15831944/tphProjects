// UtilitiesItem.h: interface for the CUtilitiesItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILITIESITEM_H__84591FAA_521D_46B1_9D1A_9B490BDF79A1__INCLUDED_)
#define AFX_UTILITIESITEM_H__84591FAA_521D_46B1_9D1A_9B490BDF79A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define UTILITIES_ITEM_ERROEDATA   -1

class CUtilitiesItem  
{
public:
	CUtilitiesItem();
	virtual ~CUtilitiesItem();

// Properties
public:
	void SetComponent(LPCTSTR szComponent) { m_strComponet = szComponent; }
	CString GetComponent() const		   { return m_strComponet;		  }
	void SetFixedCost(float fFixedCost)	   { m_fFixedCost = fFixedCost;	  }
	float GetFixedCost() const			   { return m_fFixedCost;		  }
	void SetLowCost(float fLowCost)		   { m_fLowCost = fLowCost;		  }
	float GetLowCost() const			   { return m_fLowCost;			  }
	void SetLowHrs(float fLowHrs)		   { m_fLowHrs = fLowHrs;		  }
	float GetLowHrs() const				   { return m_fLowHrs;			  }
	void SetMedCost(float fMedCost)		   { m_fMedCost = fMedCost;		  }
	float GetMedCost() const			   { return m_fMedCost;			  }
	void SetMedHrs(float fMedHrs)		   { m_fMedHrs = fMedHrs;		  }
	float GetMedHrs() const				   { return m_fMedHrs;			  }
	void SetHighCost(float fHighCost)	   { m_fHighCost = fHighCost;	  }
	float GetHighCost() const			   { return m_fHighCost;		  }
	void SetHighHrs(float fHighHrs)		   { m_fHighHrs = fHighHrs;		  }
	float GetHighHrs() const			   { return m_fHighHrs;			  }

// Data
protected:
	CString m_strComponet;
	float m_fFixedCost;
	float m_fLowCost;
	float m_fLowHrs;
	float m_fMedCost;
	float m_fMedHrs;
	float m_fHighCost;
	float m_fHighHrs;
};

#endif // !defined(AFX_UTILITIESITEM_H__84591FAA_521D_46B1_9D1A_9B490BDF79A1__INCLUDED_)
