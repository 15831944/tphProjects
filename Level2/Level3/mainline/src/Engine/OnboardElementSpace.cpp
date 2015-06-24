#include "stdafx.h"
#include "OnboardElementSpace.h"
#include "GroundInSim.h"
#include "OnboardAircraftElementInSim.h"
#include "Common/Line2008.h"
OnboardElementSpace::OnboardElementSpace(GroundInSim* pGroundInSim)
:m_pGroundInSim(pGroundInSim)
,m_pCellInSim(NULL)
{
	mSpaceCellInSim.clear();
}

OnboardElementSpace::~OnboardElementSpace()
{
	mSpaceCellInSim.clear();	
}

bool OnboardElementSpace::getOccupySpace(OnboardSpaceCellInSim& spaceCellInSim)const
{
	if (mSpaceCellInSim.IsEmpty())
		return false;
	
	spaceCellInSim = mSpaceCellInSim;
	return true;
}

void OnboardElementSpace::CalculateSpace()
{
	if (m_pGroundInSim == NULL)
		return;
	
	//start cell
	CPoint2008 ptLeftTop = getLeftTop();
	OnboardCellInSim* pStartCell = GetGround()->getCell(ptLeftTop);
	if (pStartCell == NULL)
		return;
	int iStartCell = pStartCell->getTileIndex();

	//right top cell
	CPoint2008 ptRightTop = getRightTop();
	OnboardCellInSim* pRightTopCell = GetGround()->getCell(ptRightTop);
	if(pRightTopCell == NULL)
		return;
	int iRightTopCell = pRightTopCell->getTileIndex();

	//left bottom cell
	CPoint2008 ptLeftBottom = getLeftBottom();
	OnboardCellInSim* pLeftBottomCell = GetGround()->getCell(ptLeftBottom);
	if(pLeftBottomCell == NULL)
		return;
	int iLeftBottomCell = pLeftBottomCell->getTileIndex();

	//end cell
	CPoint2008 ptRightBottom = getRightBottom();
	OnboardCellInSim* pEndCell = GetGround()->getCell(ptRightBottom);
	if (pEndCell == NULL)
		return;
	int iEndCell = pEndCell->getTileIndex();

	//search seat occupied cell
	SearchSpace(iStartCell,iRightTopCell,iLeftBottomCell,iEndCell);
}

CPoint2008 OnboardElementSpace::getLeftTop()
{
	CPoint2008 ptLeftBottom, ptTopRight;
	m_pollygon.GetBoundsPoint(ptLeftBottom,ptTopRight);

	CPoint2008 ptLeftTop;
	ptLeftTop.setX(ptLeftBottom.getX());
	ptLeftTop.setY(ptTopRight.getY());
	ptLeftTop.setZ(0);

	return ptLeftTop;
}

CPoint2008 OnboardElementSpace::getRightTop()
{
	CPoint2008 ptLeftBottom, ptTopRight;
	m_pollygon.GetBoundsPoint(ptLeftBottom,ptTopRight);

	return ptTopRight;
}

CPoint2008 OnboardElementSpace::getLeftBottom()
{
	CPoint2008 ptLeftBottom, ptTopRight;
	m_pollygon.GetBoundsPoint(ptLeftBottom,ptTopRight);

	return ptLeftBottom;
}

CPoint2008 OnboardElementSpace::getRightBottom()
{
	CPoint2008 ptLeftBottom, ptTopRight;
	m_pollygon.GetBoundsPoint(ptLeftBottom,ptTopRight);

	CPoint2008 ptRightBottom;
	ptRightBottom.setX(ptTopRight.getX());
	ptRightBottom.setY(ptLeftBottom.getY());
	ptRightBottom.setZ(0);

	return ptRightBottom;
}

GroundInSim* OnboardElementSpace::GetGround()
{
	return m_pGroundInSim;
}

void OnboardElementSpace::SearchSpace( int iStartCell,int iRightTopCell,int iLeftBottomCell,int iEndCell )
{
	//calculate min/max col and row
	RectMap& _Gndmap = GetGround()->getLogicMap();
	int nCount = _Gndmap.mCols;
	int iStart = min(iStartCell,iLeftBottomCell);

	int nRowCount = ABS(iStartCell-iLeftBottomCell)/nCount + 1;
	int nColCount = ABS(iRightTopCell - iStartCell) + 1;

	for (int nRow = 0; nRow < nRowCount; nRow++)
	{
		for (int nCol = 0; nCol < nColCount; nCol++)
		{
			int titleIndex = nCol + iStart + nRow*nCount;
			OnboardCellInSim* pCellInSim = GetGround()->getCell(titleIndex);
			SetCellState(pCellInSim);
			if (pCellInSim->GetState() != OnboardCellInSim::Idle)
			{
				mSpaceCellInSim.addItem(pCellInSim);
			}
		}
	}
}

bool OnboardElementSpace::GetCenterPoint(CPoint2008& location)
{
	CPoint2008 ptBottomLeft, ptTopRight;
	m_pollygon.GetBoundsPoint(ptBottomLeft,ptTopRight);

	CLine2008 line(ptBottomLeft,ptTopRight);
	line.GetCenterPoint(location);
	return true;
}

void OnboardElementSpace::CreatePollygon(OnboardAircraftElementInSim* pOnboardElement)
{
	pOnboardElement->CreatePollygon();
	CPoint2008 location;
	if (pOnboardElement->GetLocation(location))
	{
		m_pCellInSim = m_pGroundInSim->getCell(location);
	}
}

bool OnboardElementSpace::contain( OnboardCellInSim* pCellInSim )
{
	CPath2008 path = pCellInSim->GetPath();
	CPollygon2008 cellPollygon;
	cellPollygon.init(path.getCount(),path.getPointList());

	return m_pollygon.contains(cellPollygon);
}

bool OnboardElementSpace::intersect( OnboardCellInSim* pCellInSim )
{
	CPath2008 path = pCellInSim->GetPath();
	CPollygon2008 cellPollygon;
	cellPollygon.init(path.getCount(),path.getPointList());

	return m_pollygon.IfOverlapWithOtherPolygon(cellPollygon);
}

void OnboardElementSpace::SetCellState( OnboardCellInSim* pCellInSim )
{
	if (contain(pCellInSim))
	{
		pCellInSim->SetState(OnboardCellInSim::FullOccupied);
	}
	else if (intersect(pCellInSim))
	{
		pCellInSim->SetState(OnboardCellInSim::HalfOccupied);
	}
}