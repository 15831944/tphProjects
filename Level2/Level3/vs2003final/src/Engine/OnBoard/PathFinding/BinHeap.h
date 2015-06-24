#pragma once

#include <vector>

// Change this to change the Score type or the Key type.
// I suggest using integers for the key for several reasons,
// one is for speed, the other is for accuracy.  Float values
// are really accurate for small values, but as values get larger
// they are less accurate.
typedef std::pair<int,int> HeapElement;
//		std::pair<Score, Index>


// Only create an instance of this class if you have defined your own 
// CompareObject.  For examples, scroll to bottom of this file.
template <class CompareObject>
class BinHeap
{
protected:
	/************************************************************************/
	/* Internal Data Representation                                         */
	/************************************************************************/
	std::vector<HeapElement>			mHeap; 
	int									mSize;
	CompareObject						mComp;

public:
	BinHeap(void)
	{
		// Why 64?  Because the heap grows at 64*2^n where n = number of times
		// resized. {64,128,256,512,1024}
		mHeap.resize(64);  
		mSize = 0;
	}
	virtual ~BinHeap(void){};

	// ADT Methods
public:
	//************************************
	// Method:    Clear
	// FullName:  BinHeap::Clear
	// Access:    public 
	// Returns:   void
	// Description: Clears the heap for a new data set, but don't shrink the memory 
	//				used  This makes the heap perform very well over time, as it 
	//				doesn't resize often ( a costly operation) and it will 
	//				statistically grow to the optimum size
	//************************************
	virtual void Clear(){mSize = 0;}

	//************************************
	// Method:    Insert
	// FullName:  BinHeap::Insert
	// Access:    public 
	// Returns:   void
	// Description: Inserts and sorts the pair by theScore using the Compare method
	// Parameter: int theScore : value by which the heap is sorted
	// Parameter: int theKey : the value associated with the score.  Typically an 
	//				index into a separate array
	//************************************
	virtual void Insert(int theScore, int theKey)
	{
		// Grow the heap
		mSize++;
		if(mSize >= (int)mHeap.size())
			mHeap.resize(mHeap.size()*2);

		// insert at the end
		mHeap[mSize] = HeapElement(theScore,theKey);

		// move up the tree
		int thePosition = mSize;
		int theParent = thePosition/2;

		while (theParent > 0 && Compare(mHeap[thePosition] , mHeap[theParent]))
		{
			HeapElement anElement = mHeap[theParent];
			mHeap[theParent] = mHeap[thePosition];
			mHeap[thePosition] = anElement;

			thePosition = theParent;
			theParent = thePosition/2;
		}

		// Finished!

	}

	//************************************
	// Method:    Remove
	// FullName:  BinHeap::Remove
	// Access:    public 
	// Returns:   int : Index (pair.second) of lowest Score (pair.first)
	// Description: returns the key value associated with the lowest score,
	//				then resorts the heap so the ?smallest? score is on top
	//************************************
	virtual int	Remove()
	{
		if (mSize == 0) return -1;

		int theReturnValue = mHeap[1].second;

		// Swap End to Top
		mHeap[1] = mHeap[mSize];

		// shrink
		mSize--;
		if(mSize <= 1) return theReturnValue;

		// Re sort the top element to bottom
		int thePosition = 1;
		int theFirstChild = 2*thePosition;
		int theSecondChild = 2*thePosition + 1;

		while (theFirstChild <= mSize)
		{
			if (theSecondChild <= mSize)
			{
				if (Compare(mHeap[theFirstChild], mHeap[theSecondChild]))
				{
					if(Compare(mHeap[theFirstChild], mHeap[thePosition]))
					{
						HeapElement anElement = mHeap[thePosition];
						mHeap[thePosition] = mHeap[theFirstChild];
						mHeap[theFirstChild] = anElement;

						thePosition = theFirstChild;
						theFirstChild = 2*thePosition;
						theSecondChild = 2*thePosition + 1;

						continue;
					}
				}
				else
				{
					if(Compare(mHeap[theSecondChild], mHeap[thePosition]))
					{
						HeapElement anElement = mHeap[thePosition];
						mHeap[thePosition] = mHeap[theSecondChild];
						mHeap[theSecondChild] = anElement;

						thePosition = theSecondChild;
						theFirstChild = 2*thePosition;
						theSecondChild = 2*thePosition + 1;

						continue;
					}
				}
			}
			else
			{
				if(Compare(mHeap[theFirstChild], mHeap[thePosition]))
				{
					HeapElement anElement = mHeap[thePosition];
					mHeap[thePosition] = mHeap[theFirstChild];
					mHeap[theFirstChild] = anElement;

					thePosition = theFirstChild;
					theFirstChild = 2*thePosition;
					theSecondChild = 2*thePosition + 1;

					continue;
				}
			}
			break;
		}

		return theReturnValue;
	}

	// Compare Method
protected:
	//************************************
	// Method:    Compare
	// FullName:  BinHeap::Compare
	// Access:    virtual protected 
	// Returns:   bool
	// Description: Compares the score values (HeapElement.first)
	//				For a MinHeap, use less-than, fora MaxHeap, use
	//				Greater-than.  To make this generic, use a functor!
	// Parameter: HeapElement theLeft
	// Parameter: HeapElement theRight
	//************************************
	virtual bool Compare(HeapElement theLeft, HeapElement theRight)
	{
		return mComp(theLeft , theRight);
	}
};

//////////////////////////////////////////////////////////////////////////
// Specific Versions of the BinaryHeap
//////////////////////////////////////////////////////////////////////////

// Sorts such that the element removed is always the one paired with the
// lowest score.
class _minHeap_Comp
{
public:
	bool operator()(HeapElement _left, HeapElement _right)
	{
		return _left.first < _right.first;
	};
};

// Sorts such that the element removed is always the one paired with the
// highest score.
class _maxHeap_Comp
{
public:
	bool operator()(HeapElement _left, HeapElement _right)
	{
		return _left.first > _right.first;
	};
};

// Typedefs avoid template parameters in code!
typedef BinHeap<_minHeap_Comp> MinHeap;
typedef BinHeap<_maxHeap_Comp> MaxHeap;
