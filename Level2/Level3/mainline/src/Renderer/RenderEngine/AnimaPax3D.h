#pragma once
// #include "3DNodeWithTag.h"
#include "3DNodeObject.h"
#include "AnimaMob3DList.h"

//#define ALLOC_PAX3D_USE_POOL

#ifdef ALLOC_PAX3D_USE_POOL
#include <boost/pool/object_pool.hpp>
#endif // ALLOC_PAX3D_USE_POOL


enum EAnimaPaxState
{
	PaxStateInvalid = -1,
	PaxStateStart = 0,

	PaxStanding = PaxStateStart,
	PaxWalking,
	PaxPuttingLuggage,
	PaxSeated,
	PaxSidleLeft,
	PaxSidleRight,
	PaxBackaway,
	PaxPickup,
	PaxPushDown,
	PaxPushOn,
	PaxStateCount,
};


struct RENDERENGINE_API PaxAnimationData
{
	EAnimaPaxState eAnimaState; // pax current major animation state
// 	int nSubAnimaState; // pax current minor state, usually different animation clip under same major state
						// now not used, so ignored

	long nTimeStart; // current animation clip start time, in 10ms
	long nTimeLength; // current animation clip time length, in 10ms, ignored if negative
	long nTimePosition; // current animation clip time position, in 10ms

	bool bLooped; // whether the animation clip looped


	PaxAnimationData(EAnimaPaxState _eAnimaState = PaxStateInvalid/*, int _nSubAnimaState = 0*/)
		: eAnimaState(_eAnimaState)
// 		, int nSubAnimaState(_nSubAnimaState)
		, nTimeStart(0)
		, nTimeLength(-1)
		, nTimePosition(0)
		, bLooped(true)
	{

	}

	void StartOrContinueAnimation(EAnimaPaxState _eAnimaState,
		long _nTimeStart,
		long _nCurTime,
		long _nTimeLength = -1/*ignored if negative*/);

	LPCTSTR GetAnimationStateString() const;
	static LPCTSTR GetAnimationStateString(EAnimaPaxState _eAnimaState/*, int _nSubAnimaState*/);

private:
	static const TCHAR* const m_sAnimationStateStrings[];
};


class RENDERENGINE_API CAnimaPax3D : public C3DNodeObject
{
public:
	CAnimaPax3D();
	CAnimaPax3D(int nPaxid, Ogre::SceneNode* pNode);

	int GetPaxID()const{ return m_nPaxID; }


	void SetShape( const std::pair<int, int>& shapePair );
	void SetAnimationShape( const std::pair<int, int>& shapePair );
	void SetShape( const CString& strShape );
	void SetColor( COLORREF color );

	void SetAnimationData( const PaxAnimationData& animaData );
	const PaxAnimationData& GetAnimationData() const { return m_animaData; } // direct modification disallowed

	void DestroyEntity();
	void OnTimeUpdate(double dTime);

	CString GetShapeName() const;

protected:
	int m_nPaxID;

	// entity of 
	Ogre::Entity* m_pPaxEnt;
	CString m_strShape;

	PaxAnimationData m_animaData;
	Ogre::AnimationState* m_pAnimationState;

 	double m_dLastStateTime;	
};

class RENDERENGINE_API CAnimaPax3DList: public  CAnimaMob3DList<CAnimaPax3D>
{

};


//////////////////////////////////////////////////////////////////////////
class ARCColor3;
class RENDERENGINE_API CAnimaElevator3D : public C3DNodeObject
{
public:
	CAnimaElevator3D(){  m_Id =-1; }
	CAnimaElevator3D(int nID, Ogre::SceneNode* pNode):C3DNodeObject(pNode)
	{
		m_Id = nID;		
	}

	void BuildShape(double dlen, double dwidth, double doritation,const ARCColor3& color);

protected:
	int m_Id;
};

class RENDERENGINE_API CAnimaElevator3DList: public  CAnimaMob3DList<CAnimaElevator3D>
{

};



//////////////////////////////////////////////////////////////////////////
class RENDERENGINE_API CAnimaTrain3D : public C3DNodeObject
{
public:
public:
	CAnimaTrain3D(){  m_Id =-1; }
	CAnimaTrain3D(int nID, Ogre::SceneNode* pNode):C3DNodeObject(pNode)
	{
		m_Id = nID;		
	}

	void BuildShape(double dlen, double dwidth, int carNum,const ARCColor3& color);

protected:
	int m_Id;
};

class RENDERENGINE_API CAnimaTrain3DList : public CAnimaMob3DList<CAnimaTrain3D>
{
public:

};

//////////////////////////////////////////////////////////////////////////
class RENDERENGINE_API CAnimaResourceElm3D :public C3DNodeObject
{
public:
public:
	CAnimaResourceElm3D(){  m_Id =-1; }
	CAnimaResourceElm3D(int nID, Ogre::SceneNode* pNode):C3DNodeObject(pNode)
	{
		m_Id = nID;		
	}

	void BuildShape(double dlen, double dwidth, double doritation,ARCColor3& color);

protected:
	int m_Id;
};

class RENDERENGINE_API CAnimaResourceElm3DList : public CAnimaMob3DList<CAnimaResourceElm3D>
{
public:

};
