#if !defined(AFX_MSCHART_H__453EB8EF_BB07_4C9F_A1B7_CDF602E1AAB5__INCLUDED_)
#define AFX_MSCHART_H__453EB8EF_BB07_4C9F_A1B7_CDF602E1AAB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "../ChartDataDefine.h"
#include <vector>
#include <math.h>
using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CDMSChart
class C2DChartData;
// Dispatch interfaces referenced by this interface
class CVcTitle;
class CVcFootnote;
class CVcBackdrop;
class CVcLegend;
class CVcDataGrid;
class CVcPlot;

/////////////////////////////////////////////////////////////////////////////
// CMSChart wrapper class

class AFX_CLASS_EXPORT CMSChart : public CWnd
{
protected:
	DECLARE_DYNCREATE(CMSChart)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x3a2b370c, 0xba0a, 0x11d1, { 0xb1, 0x37, 0x0, 0x0, 0xf8, 0x75, 0x3f, 0x5d } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); }

// Attributes
public:

// Operations
public:
	BOOL GetAutoIncrement();
	void SetAutoIncrement(BOOL bNewValue);
	BOOL GetRandomFill();
	void SetRandomFill(BOOL bNewValue);
	long GetChartType();
	void SetChartType(long nNewValue);
	short GetColumn();
	void SetColumn(short nNewValue);
	short GetColumnCount();
	void SetColumnCount(short nNewValue);
	CString GetColumnLabel();
	void SetColumnLabel(LPCTSTR lpszNewValue);
	short GetColumnLabelCount();
	void SetColumnLabelCount(short nNewValue);
	short GetColumnLabelIndex();
	void SetColumnLabelIndex(short nNewValue);
	CString GetData();
	void SetData(LPCTSTR lpszNewValue);
	CString GetFootnoteText();
	void SetFootnoteText(LPCTSTR lpszNewValue);
	BOOL GetRepaint();
	void SetRepaint(BOOL bNewValue);
	short GetRow();
	void SetRow(short nNewValue);
	short GetRowCount();
	void SetRowCount(short nNewValue);
	CString GetRowLabel();
	void SetRowLabel(LPCTSTR lpszNewValue);
	short GetRowLabelCount();
	void SetRowLabelCount(short nNewValue);
	short GetRowLabelIndex();
	void SetRowLabelIndex(short nNewValue);
	short GetSeriesColumn();
	void SetSeriesColumn(short nNewValue);
	long GetSeriesType();
	void SetSeriesType(long nNewValue);
	BOOL GetShowLegend();
	void SetShowLegend(BOOL bNewValue);
	long GetDrawMode();
	void SetDrawMode(long nNewValue);
	long GetBorderStyle();
	void SetBorderStyle(long nNewValue);
	BOOL GetEnabled();
	void SetEnabled(BOOL bNewValue);
	long GetHWnd();
	CVcTitle GetTitle();
	CVcFootnote GetFootnote();
	CString GetTitleText();
	void SetTitleText(LPCTSTR lpszNewValue);
	BOOL GetStacking();
	void SetStacking(BOOL bNewValue);
	long GetTextLengthType();
	void SetTextLengthType(long nNewValue);
	BOOL GetAllowSelections();
	void SetAllowSelections(BOOL bNewValue);
	BOOL GetAllowSeriesSelection();
	void SetAllowSeriesSelection(BOOL bNewValue);
	BOOL GetAllowDynamicRotation();
	void SetAllowDynamicRotation(BOOL bNewValue);
	short GetActiveSeriesCount();
	CVcBackdrop GetBackdrop();
	CVcLegend GetLegend();
	CVcDataGrid GetDataGrid();
	CVcPlot GetPlot();
	BOOL GetAllowDithering();
	void SetAllowDithering(BOOL bNewValue);
	BOOL GetDoSetCursor();
	void SetDoSetCursor(BOOL bNewValue);
	BOOL GetChart3d();
	VARIANT GetChartData();
	void SetChartData(const VARIANT& newValue);
	long GetMousePointer();
	void SetMousePointer(long nNewValue);
	void Refresh();
	void EditPaste();
	void EditCopy();
	void Layout();
	void ToDefaults();
	void SelectPart(short part, short index1, short index2, short index3, short index4);
	void GetSelectedPart(short* part, short* index1, short* index2, short* index3, short* index4);
	void TwipsToChartPart(long xVal, long yVal, short* part, short* index1, short* index2, short* index3, short* index4);
	long GetOLEDragMode();
	void SetOLEDragMode(long nNewValue);
	long GetOLEDropMode();
	void SetOLEDropMode(long nNewValue);
	void OLEDrag();
	LPUNKNOWN GetDataSource();
	void SetRefDataSource(LPUNKNOWN newValue);
	CString GetDataMember();
	void SetDataMember(LPCTSTR lpszNewValue);

public:
	//draw 2D 3DChart 
	virtual bool DrawChart(C2DChartData & c2dChartData);
	void SetAxiLabe(int nInterval);

	//change 3DChart type 
	bool SetMSChartType(Arc3DChartType actype3DChart = Arc3DChartType_2D_Bar);

	//print chart
	bool PrintMSChart(void);

	//export chart to bitmap file
	bool ExportToBMPFile(void);

	//property edit
	void PropertyEdit(void);
	// Attributes
public:
	enum
	{
		VtOk = 0,
		VtFail = 1000,
		VtErrorDeletingUsedObject = 1001,
		VtErrorDeletingDeletedObject = 1002,
		VtErrorCorruptData = 1003,
		VtErrorNotImplemented = 1004,
		VtErrorNoMemory = 1100,
		VtErrorInvalidArgument = 1101,
		VtErrorNotFound = 1102,
		VtErrorTooSmall = 1103,
		VtErrorInvalidRequest = 1104,
		VtErrorStreamIo = 1105,
		VtErrorUserIo = 1106,
		VtErrorCorruptArchive = 1107,
		VtErrorArchiveVersion = 1108,
		VtErrorArchiveTypeMismatch = 1109,
		VtErrorArchivePointerMismatch = 1110,
		VtErrorCannotOpenFile = 1111,
		VtErrorUnableToLoadString = 1112,
		VtErrorBufferTooSmall = 1113,
		VtErrorCopyingObject = 1114,
		VtErrorDuplicateObject = 1115,
		VtErrorActionCanceled = 1116,
		VtErrorInvalidIndex = 1117,
		VtErrorInvalidTypeConversion = 1118,
		VtErrorInvalidObject = 1119,
		VtErrorCreateWindow = 1120,
		VtErrorOSVersion = 1121,
		VtErrorLoadPicture = 1122,
		VtErrorInvalidSyntax = 1200,
		VtErrorIdentifierTooBig = 1201,
		VtErrorUnrecongizedFunction = 1202,
		VtErrorUnrecongizedSymbol = 1203,
		VtErrorUnexpectedEOS = 1204,
		VtErrorDuplicateSymbol = 1205,
		VtErrorDisplay = 1500,
		VtErrorInvalidFontName = 1501,
		VtErrorInvalidFont = 1502,
		VtErrorNoDisplayResources = 1503,
		VtErrorDefaultFontSubstituted = 1504,
		VtChError = 2000,
		VtChErrorInvalidHandle = 2001,
		VtChErrorNoData = 2002,
		VtChErrorInvalidSeriesNumber = 2003,
		VtChErrorInvalidAxis = 2004,
		VtChErrorRestrictedVersion = 2005,
		InvalidPropertyValue = 380,
		GetNotSupported = 394,
		SetNotSupported = 383,
		InvalidProcedureCall = 5,
		InvalidObjectUse = 425,
		WrongClipboardFormat = 461,
		DataObjectLocked = 672,
		ExpectedAnArgument = 673,
		RecursiveOleDrag = 674,
		FormatNotByteArray = 675,
		DataNotSetForFormat = 676
	}ErrorConstants;
	enum
	{
		chOLEDropNone = 0,
		chOLEDropManual = 1,
		chOLEDropAutomatic = 2
	}OLEDropConstants;
	enum
	{
		chOLEDragManual = 0,
		chOLEDragAutomatic = 1
	}OLEDragConstants;
	enum
	{
		VtHorizontalAlignmentLeft = 0,
		VtHorizontalAlignmentRight = 1,
		VtHorizontalAlignmentCenter = 2,
		VtHorizontalAlignmentFill = 3,
		VtHorizontalAlignmentFlush = 4
	}VtHorizontalAlignment;
	enum
	{
		VtVerticalAlignmentTop = 0,
		VtVerticalAlignmentBottom = 1,
		VtVerticalAlignmentCenter = 2
	}VtVerticalAlignment;
	enum
	{
		VtOrientationHorizontal = 0,
		VtOrientationVertical = 1,
		VtOrientationUp = 2,
		VtOrientationDown = 3
	}VtOrientation;
	enum
	{
		VtSortTypeNone = 0,
		VtSortTypeAscending = 1,
		VtSortTypeDescending = 2
	}VtSortType;
	enum
	{
		VtAngleUnitsDegrees = 0,
		VtAngleUnitsRadians = 1,
		VtAngleUnitsGrads = 2
	}VtAngleUnits;
	enum
	{
		VtPrintScaleTypeActual = 0,
		VtPrintScaleTypeFitted = 1,
		VtPrintScaleTypeStretched = 2
	}VtPrintScaleType;
	enum
	{
		VtPrintOrientationPortrait = 0,
		VtPrintOrientationLandscape = 1
	}VtPrintOrientation;
	enum
	{
		VtFontStyleBold = 1,
		VtFontStyleItalic = 2,
		VtFontStyleOutline = 4
	}VtFontStyle;
	enum
	{
		VtFontEffectStrikeThrough = 256,
		VtFontEffectUnderline = 512
	}VtFontEffect;
	enum
	{
		VtBrushStyleNull = 0,
		VtBrushStyleSolid = 1,
		VtBrushStylePattern = 2,
		VtBrushStyleHatched = 3
	}VtBrushStyle;
	enum
	{
		VtBrushPattern94Percent = 0,
		VtBrushPattern88Percent = 1,
		VtBrushPattern75Percent = 2,
		VtBrushPattern50Percent = 3,
		VtBrushPattern25Percent = 4,
		VtBrushPatternBoldHorizontal = 5,
		VtBrushPatternBoldVertical = 6,
		VtBrushPatternBoldDownDiagonal = 7,
		VtBrushPatternBoldUpDiagonal = 8,
		VtBrushPatternChecks = 9,
		VtBrushPatternWeave = 10,
		VtBrushPatternHorizontal = 11,
		VtBrushPatternVertical = 12,
		VtBrushPatternDownDiagonal = 13,
		VtBrushPatternUpDiagonal = 14,
		VtBrushPatternGrid = 15,
		VtBrushPatternTrellis = 16,
		VtBrushPatternInvertedTrellis = 17,
		VtBrushPatternCount = 18
	}VtBrushPatterns;
	enum
	{
		VtBrushHatchHorizontal = 0,
		VtBrushHatchVertical = 1,
		VtBrushHatchDownDiagonal = 2,
		VtBrushHatchUpDiagonal = 3,
		VtBrushHatchCross = 4,
		VtBrushHatchDiagonalCross = 5,
		VtBrushHatchCount = 6
	}VtBrushHatches;
	enum
	{
		VtShadowStyleNull = 0,
		VtShadowStyleDrop = 1
	}VtShadowStyle;
	enum
	{
		VtPenStyleNull = 0,
		VtPenStyleSolid = 1,
		VtPenStyleDashed = 2,
		VtPenStyleDotted = 3,
		VtPenStyleDashDot = 4,
		VtPenStyleDashDotDot = 5,
		VtPenStyleDitted = 6,
		VtPenStyleDashDit = 7,
		VtPenStyleDashDitDit = 8,
		VtPenStyleNative = 9
	}VtPenStyle;
	enum
	{
		VtPenJoinMiter = 0,
		VtPenJoinRound = 1,
		VtPenJoinBevel = 2
	}VtPenJoin;
	enum
	{
		VtPenCapButt = 0,
		VtPenCapRound = 1,
		VtPenCapSquare = 2
	}VtPenCap;
	enum
	{
		VtFrameStyleNull = 0,
		VtFrameStyleSingleLine = 1,
		VtFrameStyleDoubleLine = 2,
		VtFrameStyleThickInner = 3,
		VtFrameStyleThickOuter = 4
	}VtFrameStyle;
	enum
	{
		VtPictureTypeNull = 0,
		VtPictureTypeBitmap = 1,
		VtPictureTypeVector = 2,
		VtPictureTypeBMP = 3,
		VtPictureTypeWMF = 4
	}VtPictureType;
	enum
	{
		VtPictureOptionNoSizeHeader = 1,
		VtPictureOptionTextAsCurves = 2
	}VtPictureOption;
	enum
	{
		VtPictureMapTypeActual = 0,
		VtPictureMapTypeFitted = 1,
		VtPictureMapTypeStretched = 2,
		VtPictureMapTypeTiled = 3,
		VtPictureMapTypeCropFitted = 4,
		VtPictureMapTypeHorizontalTile = 5,
		VtPictureMapTypeVerticalTile = 6
	}VtPictureMapType;
	enum
	{
		VtGradientStyleHorizontal = 0,
		VtGradientStyleVertical = 1,
		VtGradientStyleRectangle = 2,
		VtGradientStyleOval = 3
	}VtGradientStyle;
	enum
	{
		VtFillStyleNull = 0,
		VtFillStyleBrush = 1,
		VtFillStyleGradient = 2
	}VtFillStyle;
	enum
	{
		VtMarkerStyleDash = 0,
		VtMarkerStylePlus = 1,
		VtMarkerStyleX = 2,
		VtMarkerStyleStar = 3,
		VtMarkerStyleCircle = 4,
		VtMarkerStyleSquare = 5,
		VtMarkerStyleDiamond = 6,
		VtMarkerStyleUpTriangle = 7,
		VtMarkerStyleDownTriangle = 8,
		VtMarkerStyleFilledCircle = 9,
		VtMarkerStyleFilledSquare = 10,
		VtMarkerStyleFilledDiamond = 11,
		VtMarkerStyleFilledUpTriangle = 12,
		VtMarkerStyleFilledDownTriangle = 13,
		VtMarkerStyle3dBall = 14,
		VtMarkerStyleNull = 15,
		VtMarkerStyleCount = 16
	}VtMarkerStyle;
	enum
	{
		VtProjectionTypePerspective = 0,
		VtProjectionTypeOblique = 1,
		VtProjectionTypeOrthogonal = 2,
		VtProjectionTypeFrontal = 3,
		VtProjectionTypeOverhead = 4
	}VtProjectionType;
	enum
	{
		VtSmoothingTypeNone = 0,
		VtSmoothingTypeQuadraticBSpline = 1,
		VtSmoothingTypeCubicBSpline = 2
	}VtSmoothingType;
	enum
	{
		VtDcTypeNull = 0,
		VtDcTypeDisplay = 1,
		VtDcTypePrinter = 2,
		VtDcTypeMetafile = 3
	}VtDcType;
	enum
	{
		VtTextOutputTypeHardware = 0,
		VtTextOutputTypePolygon = 1
	}VtTextOutputType;
	enum
	{
		VtTextLengthTypeVirtual = 0,
		VtTextLengthTypeDevice = 1
	}VtTextLengthType;
	enum
	{
		VtChChartType3dBar = 0,
		VtChChartType2dBar = 1,
		VtChChartType3dLine = 2,
		VtChChartType2dLine = 3,
		VtChChartType3dArea = 4,
		VtChChartType2dArea = 5,
		VtChChartType3dStep = 6,
		VtChChartType2dStep = 7,
		VtChChartType3dCombination = 8,
		VtChChartType2dCombination = 9,
		VtChChartType3dHorizontalBar = 10,
		VtChChartType2dHorizontalBar = 11,
		VtChChartType3dClusteredBar = 12,
		VtChChartType3dPie = 13,
		VtChChartType2dPie = 14,
		VtChChartType3dDoughnut = 15,
		VtChChartType2dXY = 16,
		VtChChartType2dPolar = 17,
		VtChChartType2dRadar = 18,
		VtChChartType2dBubble = 19,
		VtChChartType2dHiLo = 20,
		VtChChartType2dGantt = 21,
		VtChChartType3dGantt = 22,
		VtChChartType3dSurface = 23,
		VtChChartType2dContour = 24,
		VtChChartType3dScatter = 25,
		VtChChartType3dXYZ = 26,
		VtChChartTypeCount = 27
	}VtChChartType;
	enum
	{
		VtChSeriesTypeVaries = -2,
		VtChSeriesTypeDefault = -1,
		VtChSeriesType3dBar = 0,
		VtChSeriesType2dBar = 1,
		VtChSeriesType3dHorizontalBar = 2,
		VtChSeriesType2dHorizontalBar = 3,
		VtChSeriesType3dClusteredBar = 4,
		VtChSeriesType3dLine = 5,
		VtChSeriesType2dLine = 6,
		VtChSeriesType3dArea = 7,
		VtChSeriesType2dArea = 8,
		VtChSeriesType3dStep = 9,
		VtChSeriesType2dStep = 10,
		VtChSeriesType2dXY = 11,
		VtChSeriesType2dPolar = 12,
		VtChSeriesType2dRadarLine = 13,
		VtChSeriesType2dRadarArea = 14,
		VtChSeriesType2dBubble = 15,
		VtChSeriesType2dHiLo = 16,
		VtChSeriesType2dHLC = 17,
		VtChSeriesType2dHLCRight = 18,
		VtChSeriesType2dOHLC = 19,
		VtChSeriesType2dOHLCBar = 20,
		VtChSeriesType2dGantt = 21,
		VtChSeriesType3dGantt = 22,
		VtChSeriesType3dPie = 23,
		VtChSeriesType2dPie = 24,
		VtChSeriesType3dDoughnut = 25,
		VtChSeriesType2dDates = 26,
		VtChSeriesType3dBarHiLo = 27,
		VtChSeriesType2dBarHiLo = 28,
		VtChSeriesType3dHorizontalBarHiLo = 29,
		VtChSeriesType2dHorizontalBarHiLo = 30,
		VtChSeriesType3dClusteredBarHiLo = 31,
		VtChSeriesType3dSurface = 32,
		VtChSeriesType2dContour = 33,
		VtChSeriesType3dXYZ = 34,
		VtChSeriesTypeCount = 35
	}VtChSeriesType;
	enum
	{
		VtChPartTypeChart = 0,
		VtChPartTypeTitle = 1,
		VtChPartTypeFootnote = 2,
		VtChPartTypeLegend = 3,
		VtChPartTypePlot = 4,
		VtChPartTypeSeries = 5,
		VtChPartTypeSeriesLabel = 6,
		VtChPartTypePoint = 7,
		VtChPartTypePointLabel = 8,
		VtChPartTypeAxis = 9,
		VtChPartTypeAxisLabel = 10,
		VtChPartTypeAxisTitle = 11,
		VtChPartTypeSeriesName = 12,
		VtChPartTypePointName = 13,
		VtChPartTypeCount = 14
	}VtChPartType;
	enum
	{
		VtChLocationTypeTopLeft = 0,
		VtChLocationTypeTop = 1,
		VtChLocationTypeTopRight = 2,
		VtChLocationTypeLeft = 3,
		VtChLocationTypeRight = 4,
		VtChLocationTypeBottomLeft = 5,
		VtChLocationTypeBottom = 6,
		VtChLocationTypeBottomRight = 7,
		VtChLocationTypeCustom = 8
	}VtChLocationType;
	enum
	{
		VtChLabelComponentValue = 1,
		VtChLabelComponentPercent = 2,
		VtChLabelComponentSeriesName = 4,
		VtChLabelComponentPointName = 8
	}VtChLabelComponent;
	enum
	{
		VtChLabelLineStyleNone = 0,
		VtChLabelLineStyleStraight = 1,
		VtChLabelLineStyleBent = 2
	}VtChLabelLineStyle;
	enum
	{
		VtChLabelLocationTypeNone = 0,
		VtChLabelLocationTypeAbovePoint = 1,
		VtChLabelLocationTypeBelowPoint = 2,
		VtChLabelLocationTypeCenter = 3,
		VtChLabelLocationTypeBase = 4,
		VtChLabelLocationTypeInside = 5,
		VtChLabelLocationTypeOutside = 6,
		VtChLabelLocationTypeLeft = 7,
		VtChLabelLocationTypeRight = 8
	}VtChLabelLocationType;
	enum
	{
		VtChSubPlotLabelLocationTypeNone = 0,
		VtChSubPlotLabelLocationTypeAbove = 1,
		VtChSubPlotLabelLocationTypeBelow = 2,
		VtChSubPlotLabelLocationTypeCenter = 3
	}VtChSubPlotLabelLocationType;
	enum
	{
		VtChPieWeightBasisNone = 0,
		VtChPieWeightBasisTotal = 1,
		VtChPieWeightBasisSeries = 2
	}VtChPieWeightBasis;
	enum
	{
		VtChPieWeightStyleArea = 0,
		VtChPieWeightStyleDiameter = 1
	}VtChPieWeightStyle;
	enum
	{
		VtChAxisIdX = 0,
		VtChAxisIdY = 1,
		VtChAxisIdY2 = 2,
		VtChAxisIdZ = 3,
		VtChAxisIdNone = 4
	}VtChAxisId;
	enum
	{
		VtChDateIntervalTypeNone = 0,
		VtChDateIntervalTypeDaily = 1,
		VtChDateIntervalTypeWeekly = 2,
		VtChDateIntervalTypeSemimonthly = 3,
		VtChDateIntervalTypeMonthly = 4,
		VtChDateIntervalTypeYearly = 5
	}VtChDateIntervalType;
	enum
	{
		VtChScaleTypeLinear = 0,
		VtChScaleTypeLogarithmic = 1,
		VtChScaleTypePercent = 2
	}VtChScaleType;
	enum
	{
		VtChPercentAxisBasisMaxChart = 0,
		VtChPercentAxisBasisMaxRow = 1,
		VtChPercentAxisBasisMaxColumn = 2,
		VtChPercentAxisBasisSumChart = 3,
		VtChPercentAxisBasisSumRow = 4,
		VtChPercentAxisBasisSumColumn = 5
	}VtChPercentAxisBasis;
	enum
	{
		VtChAxisTickStyleNone = 0,
		VtChAxisTickStyleCenter = 1,
		VtChAxisTickStyleInside = 2,
		VtChAxisTickStyleOutside = 3
	}VtChAxisTickStyle;
	enum
	{
		VtChStatsMinimum = 1,
		VtChStatsMaximum = 2,
		VtChStatsMean = 4,
		VtChStatsStddev = 8,
		VtChStatsRegression = 16
	}VtChStats;
	enum
	{
		VtChNoDisplay = 0,
		VtChDisplayPlot = 1,
		VtChLayoutPlot = 2,
		VtChDisplayLegend = 4,
		VtChLayoutLegend = 8,
		VtChLayoutSeries = 16,
		VtChPositionSection = 32
	}VtChUpdateFlags;
	enum
	{
		VtChMouseFlagShiftKeyDown = 4,
		VtChMouseFlagControlKeyDown = 8
	}VtChMouseFlag;
	enum
	{
		VtChSsLinkModeOff = 0,
		VtChSsLinkModeOn = 1,
		VtChSsLinkModeAutoParse = 2
	}VtChSsLinkMode;
	enum
	{
		VtChDrawModeDraw = 0,
		VtChDrawModeBlit = 1
	}VtChDrawMode;
	enum
	{
		VtChContourColorTypeAutomatic = 0,
		VtChContourColorTypeGradient = 1,
		VtChContourColorTypeManual = 2
	}VtChContourColorType;
	enum
	{
		VtChContourDisplayTypeCBands = 0,
		VtChContourDisplayTypeCLines = 1
	}VtChContourDisplayType;
	enum
	{
		VtChSurfaceBaseTypePedestal = 0,
		VtChSurfaceBaseTypeStandard = 1,
		VtChSurfaceBaseTypeStandardWithCBands = 2,
		VtChSurfaceBaseTypeStandardWithCLines = 3
	}VtChSurfaceBaseType;
	enum
	{
		VtChSurfaceDisplayTypeNone = 0,
		VtChSurfaceDisplayTypeCBands = 1,
		VtChSurfaceDisplayTypeCLines = 2,
		VtChSurfaceDisplayTypeSolid = 3,
		VtChSurfaceDisplayTypeSolidWithCLines = 4
	}VtChSurfaceDisplayType;
	enum
	{
		VtChSurfaceProjectionTypeNone = 0,
		VtChSurfaceProjectionTypeCBands = 1,
		VtChSurfaceProjectionTypeCLines = 2
	}VtChSurfaceProjectionType;
	enum
	{
		VtChSurfaceWireframeTypeNone = 0,
		VtChSurfaceWireframeTypeMajor = 1,
		VtChSurfaceWireframeTypeMajorAndMinor = 2
	}VtChSurfaceWireframeType;
	enum
	{
		VtBorderStyleNone = 0,
		VtBorderStyleFixedSingle = 1
	}VtBorderStyle;
	enum
	{
		VtMousePointerDefault = 0,
		VtMousePointerArrow = 1,
		VtMousePointerCross = 2,
		VtMousePointerIbeam = 3,
		VtMousePointerIcon = 4,
		VtMousePointerSize = 5,
		VtMousePointerSizeNESW = 6,
		VtMousePointerSizeNS = 7,
		VtMousePointerSizeNWSE = 8,
		VtMousePointerSizeWE = 9,
		VtMousePointerUpArrow = 10,
		VtMousePointerHourGlass = 11,
		VtMousePointerNoDrop = 12,
		VtMousePointerArrowHourGlass = 13,
		VtMousePointerArrowQuestion = 14,
		VtMousePointerSizeAll = 15
	}VtMousePointer;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSCHART_H__453EB8EF_BB07_4C9F_A1B7_CDF602E1AAB5__INCLUDED_)
