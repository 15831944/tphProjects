/*****
*
*   Class       FixedQueue
*   Author      Lincoln Fellingham, Aviation Research Corporation
*   Date        July 14, 1993
*
*   Purpose     Member of Processor that handles fixed queue functions
*
*****/

#ifndef FIXED_QUEUE_H
#define FIXED_QUEUE_H

#include "engine\sequencq.h"
#include "common\path.h"
#include "common\point.h"
#include "common\elaptime.h"
#include "person.h"
//It handles fixed queue functions.
class FixedQueue : public SequentialQueue
{

protected:
    //It determines if the Person is on the same segment of the fixed queue as the actual waiting line.
    int onTailSegment (const Person *aPerson) const;

    //It resets destination of all approaching Persons that are destined for the same path segment as the tail of the queue.
    //It is called when tail advances or a Person arrives at tail.
    void updateApproaching (ElapsedTime updateTime);

    //It is called after a person arrives at the corner of the queue.
    //It is used to determine if the Person is now approaching the actual queue.
	virtual void approachQueue (Person *aPerson, ElapsedTime approachTime);
    

//int getNumOftwoPathSegment(int index,Person *aPerson) ;
//	int   countIndex ;
//	int num_person ;
//	DOUBLE GetDistanceBetweenPersonToHead(Person* person);
//	DOUBLE GetDistanceFormCurrentSegmentToHead(int cur_index) ;

	int getSegment (Person *aPerson, DistanceUnit inStep) const ;
	virtual Point getLocation (Person *aPerson, DistanceUnit inStep) const ;
	void getQueuePosition (Person *aPerson) const ;


	virtual void setPersonCurrentPosition(Person *aPerson,const ElapsedTime& arriveTime);

public:
    FixedQueue (const Path& path) : SequentialQueue (path) {

	};
    virtual ~FixedQueue () {};

    //It adds a person to the list of approaching Persons and calculates the first waiting position (tail of the queue or last path corner).
   virtual void addToQueue (Person *aPerson);

   virtual void arriveAtQueue (Person *aPerson, ElapsedTime arriveTime);

    //it gets the current location of the tail
    void getTailPoint (Person *aPerson) const;

	// get the most appropriate corner's index.
	int getFixQEntryPointIndex();

    //It returns queue type.
    virtual char isFixed (void) const { return 'Y'; };

	//////////////////////////////////////////////////////////////////////////
	// for entry points  handle 
	//
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//add by cj 5/30/2008
	//for hanlde entry point , entry point will be used only in the fixqueue
	// the realize of these virtual function is in the FixQueue Class ,
	// another class which is the son of this class do not do anything in these function.
	//////////////////////////////////////////////////////////////////////////
public:	
	class CEntryPoint
	{
	private:
		int courrentIndex ;
		std::vector<int> EntryPoints ;
	public:
		CEntryPoint():courrentIndex(-1){};
		int GetCurrentIndex() { 
			if((int)EntryPoints.size() == 0 || courrentIndex == -1)
			{
                courrentIndex = -1 ;
				return -1 ;
			}
			return EntryPoints[ courrentIndex] ;
		} ;
		int NextEntryPoint() 
		{
			if(courrentIndex == -1 || (int)EntryPoints.size() == 0)
			{
				courrentIndex = -1 ;
				return -1 ;
			}
			int val = EntryPoints[courrentIndex] ;
			int index = courrentIndex ;
			int max_index = 0x00FF ;
			for(int i = 0 ;i < (int)EntryPoints.size() ;i++)
			{
				if( EntryPoints[i] > val )
				{
					if(EntryPoints[i] < max_index)
					{
						max_index = EntryPoints[i] ;
						index = i ;
					}
				}
			}
			if(index == courrentIndex)
				courrentIndex = -1 ;
			else
				courrentIndex = index ;
			return courrentIndex ;
		};
		int DelEntryPoint(int index) 
		{
			std::vector<int>::iterator iter = std::find(EntryPoints.begin() ,EntryPoints.end() , index) ;
			if(iter != EntryPoints.end() )
				EntryPoints.erase(iter);
			return index ;
		} ;
		void DeleteAllEntryPoint()
		{
			EntryPoints.clear() ;
			courrentIndex = -1 ;
		};
		int AddEntryPoint(int index) { 
			if(std::find(EntryPoints.begin() ,EntryPoints.end() , index) == EntryPoints.end() )
				EntryPoints.push_back(index);
			if(courrentIndex == -1)
                ResetEntryState();
			return index ;
		};

		std::vector<int>* GetEntryPoints()
		{
			return &EntryPoints ;
		}
		void ResetEntryState()
		{
			if(EntryPoints.size() == 0)
				courrentIndex = -1 ;
			else
			{
				int val = EntryPoints[0] ;
				int index = 0 ;
				for(int i = 0 ; i < (int) EntryPoints.size() ;i ++)
				{
					if(EntryPoints[i] < val)
					{
						val = EntryPoints[i] ;
						index = i ;
					}
				}
				courrentIndex = index ;
			}
		}
	};
	//return index 
protected:

	CEntryPoint  EntryPoints ;
public :

	//return index 
	int  AddEntryPoint(int index) 
	{
		if(index == 0)
			index = LAST_SEGMENT ;
		return  EntryPoints.AddEntryPoint(index) ;
	}
	void  DeleteEntryPoint(int index) 
	{
		EntryPoints.DelEntryPoint(index) ;
	}
	//delete all entry point 
	void  DeleteAllEntryPoint() 
	{
		EntryPoints.DeleteAllEntryPoint() ;
	}
	//change the point which the index 
	std::vector<int>* GetEntryPoints() 
	{
		return EntryPoints.GetEntryPoints() ;
	}
	void ResetEntryState() 
	{
		EntryPoints.ResetEntryState() ;
	}

};


#endif
