#include "StdAfx.h"
#include ".\shadowobj.h"
#define INFNITY 100000000
void ShadowObj::SetConnectivity(){
	unsigned int p1i, p2i, p1j, p2j;
	unsigned int P1i, P2i, P1j, P2j;
	unsigned int i,j,ki,kj;
	size_t nPlanes=m_faces.size();
	for(i=0;i<nPlanes-1;i++){
		for(j=i+1;j<nPlanes;j++){
			for(ki=0;ki<3;ki++){
				if(! m_faces[i].neigh[ki]){
					for(kj=0;kj<3;kj++){
						p1i=ki;
						p1j=kj;
						p2i=(ki+1)%3;
						p2j=(kj+1)%3;

						p1i=m_faces[i].p[p1i];
						p2i=m_faces[i].p[p2i];
						p1j=m_faces[j].p[p1j];
						p2j=m_faces[j].p[p2j];

						P1i=( (p1i+p2i)-abs( int(p1i-p2i) ) )/2;
						P2i=( (p1i+p2i)+abs( int(p1i-p2i) ) )/2;
						P1j=( (p1j+p2j)-abs( int(p1j-p2j) ) )/2;
						P2j=( (p1j+p2j)+abs( int(p1j-p2j) ) )/2;

						if((P1i==P1j) && (P2i==P2j)){  //they are neighbours
							m_faces[i].neigh[ki] = j+1;	  
							m_faces[j].neigh[kj] = i+1;	  
						}
					}
				}
			}
		}
	}
}

void ShadowObj::CalcPlane(){
	size_t nFace=m_faces.size();
	for(size_t i=0;i<nFace;i++){		
		sPlane & plane=m_faces[i];
		Point& v1=m_points[plane.p[0]];
		Point& v2=m_points[plane.p[1]];
		Point& v3=m_points[plane.p[2]];	
	
		plane.PlaneEq.a = v1.getY()*(v2.getZ()-v3.getZ()) + v2.getY()*(v3.getZ()-v1.getZ()) + v3.getY()*(v1.getZ()-v2.getZ());
		plane.PlaneEq.b = v1.getZ()*(v2.getX()-v3.getX()) + v2.getZ()*(v3.getX()-v1.getX()) + v3.getZ()*(v1.getX()-v2.getX());
		plane.PlaneEq.c = v1.getX()*(v2.getY()-v3.getY()) + v2.getX()*(v3.getY()-v1.getY()) + v3.getX()*(v1.getY()-v2.getY());
		plane.PlaneEq.d =-( v1.getX()*(v2.getY()*v3.getZ() - v3.getY()*v2.getZ()) +
			v2.getX()*(v3.getY()*v1.getZ() - v1.getY()*v3.getZ()) +
			v3.getX()*(v1.getY()*v2.getZ() - v2.getY()*v1.getZ()) );
		plane.normal.setPoint(plane.PlaneEq.a,plane.PlaneEq.b,plane.PlaneEq.c);
		plane.normal.Normalize();
        	
	}
}
void ShadowObj::DrawGLObject(){
	unsigned int i, j;

	glBegin(GL_TRIANGLES);
	size_t nPlanes=m_faces.size();

	for (i=0; i<nPlanes; i++){
		sPlane & _plane=m_faces[i];
		glNormal3d(- _plane.normal.getX(), -_plane.normal.getY(), -_plane.normal.getZ());
		for (j=0; j<3; j++){			
			glVertex3d(m_points[_plane.p[j]].getX(),
				m_points[_plane.p[j]].getY(),
				m_points[_plane.p[j]].getZ());
		}
	}
	glEnd();
}
void ShadowObj::CastShadow(double *lp){
	unsigned int	i, j, k, jj;
	unsigned int	p1, p2;
	Point			v1, v2;
	DistanceUnit	side;

	//set visual parameter
	size_t nPlanes=m_faces.size();
	for (i=0;i<nPlanes;i++){
		// chech to see if light is in front or behind the plane (face plane)
		sPlaneEq & _plane=m_faces[i].PlaneEq;
		side =	_plane.a*lp[0]+
			_plane.b*lp[1]+
			_plane.c*lp[2]+
			_plane.d*lp[3];
		if (side >0) m_faces[i].visible = FALSE;
		else m_faces[i].visible = TRUE;
	}
	
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_STENCIL_TEST);
	glColorMask(0, 0, 0, 0);
	glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

	// first pass, stencil operation decreases stencil value
	glFrontFace(GL_CCW);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	for (i=0; i<nPlanes;i++){
		if (m_faces[i].visible)
			for (j=0;j<3;j++){
				k = m_faces[i].neigh[j];
				if ((!k) || (!m_faces[k-1].visible)){
					// here we have an edge, we must draw a polygon
					p1 = m_faces[i].p[j];
					jj = (j+1)%3;
					p2 = m_faces[i].p[jj];

					//calculate the length of the vector
					v1.setPoint( (m_points[p1].getX() - lp[0])*INFNITY,
								 (m_points[p1].getY() - lp[1])*INFNITY,
								 (m_points[p1].getZ() - lp[2])*INFNITY );

					v2.setPoint( (m_points[p2].getX() - lp[0])*INFNITY,
								 (m_points[p2].getY() - lp[1])*INFNITY,
								 (m_points[p2].getZ() - lp[2])*INFNITY );
					

					//draw the polygon
					glBegin(GL_TRIANGLE_STRIP);
					glVertex3d(m_points[p1].getX(),
						m_points[p1].getY(),
						m_points[p1].getZ());
					glVertex3d(m_points[p1].getX() + v1.getX(),
						m_points[p1].getY() + v1.getY(),
						m_points[p1].getZ() + v1.getZ());

					glVertex3d(m_points[p2].getX(),
						m_points[p2].getY(),
						m_points[p2].getZ());
					glVertex3d(m_points[p2].getX() + v2.getX(),
						m_points[p2].getY() + v2.getY(),
						m_points[p2].getZ() + v2.getZ());
					glEnd();
				}
			}
	}

	// second pass, stencil operation increases stencil value
	glFrontFace(GL_CW);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	for (i=0; i<nPlanes;i++){
		if (m_faces[i].visible)
			for (j=0;j<3;j++){
				k = m_faces[i].neigh[j];
				if ((!k) || (!m_faces[k-1].visible)){
					// here we have an edge, we must draw a polygon
					p1 = m_faces[i].p[j];
					jj = (j+1)%3;
					p2 = m_faces[i].p[jj];

					//calculate the length of the vector
					v1.setPoint( (m_points[p1].getX() - lp[0])*INFNITY,
								 (m_points[p1].getY() - lp[1])*INFNITY,
								 (m_points[p1].getZ() - lp[2])*INFNITY );

					v2.setPoint( (m_points[p2].getX() - lp[0])*INFNITY,
								 (m_points[p2].getY() - lp[1])*INFNITY,
								 (m_points[p2].getZ() - lp[2])*INFNITY );

					//draw the polygon
					glBegin(GL_TRIANGLE_STRIP);
					glVertex3d(m_points[p1].getX(),
						m_points[p1].getY(),
						m_points[p1].getZ());
					glVertex3d(m_points[p1].getX() + v1.getX(),
						m_points[p1].getY() + v1.getY(),
						m_points[p1].getZ() + v1.getZ());

					glVertex3d(m_points[p2].getX(),
						m_points[p2].getY(),
						m_points[p2].getZ());
					glVertex3d(m_points[p2].getX() + v2.getX(),
						m_points[p2].getY() + v2.getY(),
						m_points[p2].getZ() + v2.getZ());
					glEnd();
				}
			}
	}

	glFrontFace(GL_CCW);
	glColorMask(1, 1, 1, 1);

	//draw a shadowing rectangle covering the entire screen
	glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glStencilFunc(GL_NOTEQUAL, 0, 0xffffffff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(-0.1f, 0.1f,-0.10f);
	glVertex3f(-0.1f,-0.1f,-0.10f);
	glVertex3f( 0.1f, 0.1f,-0.10f);
	glVertex3f( 0.1f,-0.1f,-0.10f);
	glEnd();
	glPopMatrix();
	//glDisable(GL_BLEND);

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_LIGHTING);
	glDisable(GL_STENCIL_TEST);
	glShadeModel(GL_SMOOTH);
	
}