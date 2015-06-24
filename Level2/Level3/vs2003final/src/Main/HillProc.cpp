#include "StdAfx.h"
#include "TessellationManager.h"
#include "HillProc.h"
/*
GLenum  _lType;
std::list<Point> _lVids;
int   _lFcnt;
struct _tTriangle{
	Point pts[3];
	_tTriangle(const Point&p1,const Point&p2,const Point&p3){
		pts[0]=p1;pts[1]=p2;pts[2]=p3;
	}
	_tTriangle(){};
};
std::vector<_tTriangle> _lTriangles;

void  __stdcall   _tBegin  (GLenum type)  { _lType = type; _lFcnt = 0; }; 
void  __stdcall   _tVertex (void * pdata)   
{ 
	double *ppoint=(double *)pdata;
	_lVids.push_back( Point(ppoint[0],ppoint[1],ppoint[2]) );
}

void  __stdcall   _tEnd    (void){
	unsigned int   fst, sec;
	size_t    i, n;
	
	_tTriangle tri;
	switch (_lType) {
		case GL_TRIANGLES:                  // single triangle
			while(!_lVids.empty()){		
				Point p1= *_lVids.rbegin();_lVids.pop_back();
				Point p2=*_lVids.rbegin();_lVids.pop_back();
				Point p3=*_lVids.rbegin();_lVids.pop_back();
				_lTriangles.push_back(_tTriangle(p1,p2,p3));

				_lFcnt++;
			}
			 break;

		 case GL_TRIANGLE_STRIP:             // strip of triangles
			 tri.pts [0] =  *_lVids.rbegin();_lVids.pop_back();
			 tri.pts[1] =  *_lVids.rbegin();_lVids.pop_back();

			 n = _lVids.size ();

			 for (i = 0, fst = 1, sec = 2; i < n; i++) {
				 tri.pts [sec] =  *_lVids.rbegin();_lVids.pop_back();

				 _lTriangles.push_back(tri);

				 tri.pts [0] = tri.pts [fst];

				 if ( i & 1 ) {
					 fst = 2; sec = 1;
				 }
				 else {
					 fst = 1; sec = 2;
				 }

				 _lFcnt++;
			 }
			 break;

		 case GL_TRIANGLE_FAN:        // triangles fanning from single vertex
			 tri.pts [0] =  *_lVids.rbegin();_lVids.pop_back();
			 tri.pts [1] =  *_lVids.rbegin();_lVids.pop_back();
			 n = _lVids.size();
			 for (i = 0; i < n; i++) {
				 tri.pts [2] = *_lVids.rbegin();_lVids.pop_back();
				 _lTriangles.push_back(tri);
				 tri.pts [1] = tri.pts [2];
				 _lFcnt++;
			 }
			 break;
		}                                  // switch
	_lType = -1;
	_lVids.clear();

}; 

*/
//unsigned int IDcounter;
//unsigned int layPointCount;
//get Trianles form tesslation 
void  HillProc::GenAllTriBlocks(CContourNode * father){
/*	//tranverse the tree and get the father node ;
	if(father==NULL)return ;
	CContourNode * son=father->m_pFirstChild ;
	while(son!=NULL){
		GenAllTriBlocks(son);//
		//get the data from the contour processor;
		son=son->m_pNext;
	}
	
	//GenTriBlocks(*fatherdata,fatherhight,*sondata,sonhight);
	//if(son->m_pFirstChild==NULL)pLeafnode.push_back(son);
	double buf[512][3];
	
	//get Triangle data from tesslation obj
	GLUtesselator *m_pTObj;	
	m_pTObj = gluNewTess();
	gluTessCallback(m_pTObj, GLU_TESS_BEGIN, (_GLUfuncptr) &_tBegin);    
	gluTessCallback(m_pTObj, GLU_TESS_END, (_GLUfuncptr) &_tEnd);
	gluTessCallback(m_pTObj, GLU_TESS_VERTEX, (_GLUfuncptr) &_tVertex);
	gluTessCallback(m_pTObj, GLU_TESS_EDGE_FLAG, (_GLUfuncptr) &glEdgeFlag);
	gluTessCallback(m_pTObj, GLU_TESS_COMBINE, (_GLUfuncptr) &combineCallback);
	gluTessCallback(m_pTObj, GLU_TESS_ERROR, (_GLUfuncptr) &errorCallback);
	gluTessProperty(m_pTObj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
	CString fatherid=father->m_KeyVal;
	gluTessNormal(m_pTObj,0.0,0.0,1.0);

	DATAIN *fatherdata=m_pDoc->GetProcessor2FromName(fatherid,EnvMode_AirSide)->GetProcessor()->serviceLocationPath();
	double fatherhight=((ContourProc *)(m_pDoc->GetProcessor2FromName(fatherid,EnvMode_AirSide)->GetProcessor()))->getHeight();
	
	gluTessBeginPolygon(m_pTObj, NULL);
gluTessBeginContour(m_pTObj);
	//out constraint 
	int noffset=0;
	for( int i=0;i<fatherdata->getCount();i++){
		Point & _p=fatherdata->getPoint(i);
		_p.setZ(fatherhight);
		buf[i][0]=_p.getX();
		buf[i][1]=_p.getY();
		buf[i][2]=fatherhight;		
		
		gluTessVertex(m_pTObj,buf[i],buf[i]);
	}
	layPointCount+=fatherdata->getCount();

	gluTessEndContour(m_pTObj);
	noffset+=fatherdata->getCount();
	son=father->m_pFirstChild ;
	//In constraint
	while(son!=NULL){
		gluTessBeginContour(m_pTObj);
		CString sonid=son->m_KeyVal;
		DATAIN *sondata=m_pDoc->GetProcessor2FromName(sonid,EnvMode_AirSide)->GetProcessor()->serviceLocationPath();
		double sonhight=((ContourProc *)(m_pDoc->GetProcessor2FromName(sonid,EnvMode_AirSide)->GetProcessor()))->getHeight();
		for( int i=0;i<sondata->getCount();i++){
			Point & _p=sondata->getPoint(i);
			_p.setZ(sonhight);
			buf[i+noffset][0]=_p.getX();
			buf[i+noffset][1]=_p.getY();
			buf[i+noffset][2]=sonhight;
			//m_obj.addPoints(_p);	
			
			gluTessVertex(m_pTObj,buf[i+noffset],buf[i+noffset]);
			//IDcounter[0]++;
		}
		noffset+=sondata->getCount();
		son=son->m_pNext;
		gluTessEndContour(m_pTObj);
	}
	gluTessEndPolygon(m_pTObj);	

	gluDeleteTess(m_pTObj);
	*/
}
void HillProc ::DrawProc(){
/*	 double scoord[]={0.0001,0.0,0.0};
	 double tcoord[]={0.0,0.0001,0.0};
	IDcounter=0;
	_lVids.clear();
	_lTriangles.clear();
	_lType=-1;
	GenAllTriBlocks(m_first);
	size_t nTriCount=_lTriangles.size();*/
	/*glDeleteLists(m_displaylist,1);
	m_displaylist=glGenLists(1);
	glNewList(m_displaylist,GL_COMPILE);*/
/*	glEnable( GL_LIGHTING);
	glEnable(GL_LIGHT1);
	//glColor3d(1.0,1.0,1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,textureid);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	
	for(size_t i=0;i<nTriCount;i++){
		//get the normal of triangle
		
		Point p1,p2,p3;
		p1=_lTriangles[i].pts[0];p2=_lTriangles[i].pts[1];p3=_lTriangles[i].pts[2];
		double t_normalx=p1.getY()*(p2.getZ()-p3.getZ()) + p2.getY()*(p3.getZ()-p1.getZ()) + p3.getY()*(p1.getZ()-p2.getZ());
		double t_normaly=p1.getZ()*(p2.getX()-p3.getX()) + p2.getZ()*(p3.getX()-p1.getX()) + p3.getZ()*(p1.getX()-p2.getX());
		double t_normalz=p1.getX()*(p2.getY()-p3.getY()) + p2.getX()*(p3.getY()-p1.getY()) + p3.getX()*(p1.getY()-p2.getY());
		Point t_normal;
		t_normal.setPoint(-t_normalx,-t_normaly,-t_normalz);
		t_normal.Normalize();

		glBegin(GL_TRIANGLES);
			double s,t;
			glNormal3d(t_normal.getX(),t_normal.getY(),t_normal.getZ());
			s=p1.getX()*scoord[0]+p1.getY()*scoord[1]+p1.getZ()*scoord[2];
			t=p1.getX()*tcoord[0]+p1.getY()*tcoord[1]+p1.getZ()*tcoord[2];
			glTexCoord2d(s,t);
			glVertex3d(p1.getX(),p1.getY(),p1.getZ());
			s=p2.getX()*scoord[0]+p2.getY()*scoord[1]+p2.getZ()*scoord[2];
			t=p2.getX()*tcoord[0]+p2.getY()*tcoord[1]+p2.getZ()*tcoord[2];
			glTexCoord2d(s,t);
			glVertex3d(p2.getX(),p2.getY(),p2.getZ());
			s=p3.getX()*scoord[0]+p3.getY()*scoord[1]+p3.getZ()*scoord[2];
			t=p3.getX()*tcoord[0]+p3.getY()*tcoord[1]+p3.getZ()*tcoord[2];
			glTexCoord2d(s,t);
			glVertex3d(p3.getX(),p3.getY(),p3.getZ());
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);*/
	/*glEndList();*/
/*	m_bDirtflag=false;
	*/
}


bool HillProc::CalBlocks(){
	m_bDirtflag=true;
	return true;
}
