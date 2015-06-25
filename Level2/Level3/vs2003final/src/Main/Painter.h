class Painter
{
public:
	Painter();
	Painter(const CString& mat);

	bool mbUseVertexColor;
	bool mbBackFace;
	bool mbAutoNorm;
	bool mbAutoTextCoord;
	ARCColor4 color;
	CString msMat;


	Painter& UseVertexColor(const ARCColor4& c){ mbUseVertexColor = true; color = c;  return *this; }

};

Painter::Painter()
{
	mbUseVertexColor  = false;
	mbBackFace = false;
	mbAutoNorm = false;
	mbAutoTextCoord = false;
}

Painter::Painter( const CString& mat )
{
	Painter();
	msMat = mat;
}