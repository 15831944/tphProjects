// ACDetails.h: interface for the CACDetails class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACDETAILS_H__38665AAB_47B7_11D4_9312_0080C8F982B1__INCLUDED_)
#define AFX_ACDETAILS_H__38665AAB_47B7_11D4_9312_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CACDetails  
{
public:
	CACDetails();
	virtual ~CACDetails();

	CString Code() { return m_sCode; };
	CString Description() { return m_sDesc; };
	float Height() { return m_fHeight; };
	float Length() { return m_fLen; };
	float Span() { return m_fSpan; };
	float MZFW() { return m_fMZFW; };
	float OEW() { return m_fOEW; };
	float MTOW() { return  m_fMTOW; };
	float MLW() { return m_fMLW; };
	int Capacity() { return m_iCap; };

	void Code(CString code) { m_sCode = code; };
	void Description(CString desc) { m_sDesc = desc; };
	void Height(float height) { m_fHeight = height; };
	void Length(float length) { m_fLen = length; };
	void Span(float span) { m_fSpan = span; };
	void MZFW(float mzfw) { m_fMZFW = mzfw; };
	void OEW(float oew) { m_fOEW = oew; };
	void MTOW(float mtow) { m_fMTOW = mtow; };
	void MLW(float mlw) { m_fMLW = mlw; };
	void Capacity(int cap) { m_iCap = cap; };

protected:
	CString m_sCode;
	CString m_sDesc;
	float m_fHeight;
	float m_fLen;
	float m_fSpan;
	float m_fMZFW;
	float m_fOEW;
	float m_fMTOW;
	float m_fMLW;
	int m_iCap;
};

#endif // !defined(AFX_ACDETAILS_H__38665AAB_47B7_11D4_9312_0080C8F982B1__INCLUDED_)
