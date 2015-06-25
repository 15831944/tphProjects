#ifndef _ARCUNIT_COMMON__H
#define _ARCUNIT_COMMON__H


class ARCUnit{
	
public:
	enum LengthUnit{MM=0,CM,M,KM,MILE,NM,INCH,FEET,LEVEL,LUnitCount };
	enum VelocityUnit{MpS=0,KNOT,CMpS,FEETpMIN, KMpHR,VUnitCount };
	enum TimeUnit{SECOND=0,MINUS,HOUR,DAY, TUnitCount };
	enum WeightUnit{KG=0,G,POUND,WUnitCount };

	const static CString LengthUnitStr[];
	const static CString VeclocityUnitStr[];
	const static CString TimeUnitStr[];
	const static CString WeightUnitStr[];

	const static CString LengthUnitLongStr[];
	const static CString VeclocityUnitLongStr[];
	const static CString TimeUnitLongStr[];
	const static CString WeightUnitLongStr[];

	const static double LengthCon[][2]; //conversion to m;
	const static double VelocityCon[][2]; //conversions to m/s;
	const static double TimeCon[][2];    // conversions to s;
	const static double WeightCon[][2];

	static double ConvertLength(double val , LengthUnit fromUnit, LengthUnit toUnit = M);
	static double ConvertVelocity(double val, VelocityUnit fromUnit, VelocityUnit toUnit = MpS);
	static double ConvertTime(double val ,TimeUnit fromUnit, TimeUnit toUnit = SECOND);
	static double ConvertWeight(double val, WeightUnit fromUnit, WeightUnit toUnit = KG);

	static double LBSToKG(double d);
	static double KgsToLBS(double dIn);
};






#endif