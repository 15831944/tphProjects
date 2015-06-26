// SimMobileElemList.h: interface for the CSimMobileElemList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMMOBILEELEMLIST_H__0FD92117_DE85_43D5_A8E9_0C00015FBB3F__INCLUDED_)
#define AFX_SIMMOBILEELEMLIST_H__0FD92117_DE85_43D5_A8E9_0C00015FBB3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MobileElement;

class CSimMobileElemList  
{
protected:
	std::vector<MobileElement*> m_vMobileElementList;
	int m_nSize;
public:
	CSimMobileElemList();
	virtual ~CSimMobileElemList();


	// allocated space.
	void Initialize( int _nSize );

	// register MobileElement base on its id as index.
	void Register( MobileElement* _pMob );

	//when the passenger flush log, delete the passenger, and set NULL
	void Unregister(MobileElement *_pMob);
	
	//check the passenger exists or not
	bool IsAlive(MobileElement *_pMob) const;

	// delete all mobileelement, and delete the list
	void Clear();

	long m_lliveCount;

	MobileElement* GetAvailableElement(int nMobileID);
};

#endif // !defined(AFX_SIMMOBILEELEMLIST_H__0FD92117_DE85_43D5_A8E9_0C00015FBB3F__INCLUDED_)
