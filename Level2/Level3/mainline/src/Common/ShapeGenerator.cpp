#include "StdAfx.h"
#include ".\shapegenerator.h"

#include "ARCVector.h"
#include "line.h"
#include "BizierCurve.h"


bool ShapeGenerator::GenWidthPath(const std::vector<Point>& _vInPathPt,double dwidth,std::vector<Point>& _outPts)
{
	
	_outPts.clear();
	if(_vInPathPt.size()<2)return false;

	ARCVector2 k,k1,k2;
	Point  p_pre,p_this,p_next;
		
	p_pre=_vInPathPt[0];p_this=_vInPathPt[0];p_next=_vInPathPt[1];
	k.reset(p_this,p_next);k.Rotate(90);
	k.Normalize();
	k.SetLength(dwidth*0.5);
	Point p1,p2;
	p1.setPoint( p_this.getX()+k[VX],p_this.getY()+k[VY],p_this.getZ() );
	p2.setPoint( p_this.getX()-k[VX],p_this.getY()-k[VY],p_this.getZ() );
	_outPts.push_back(p1); _outPts.push_back(p2);
	int nptCount =(int)_vInPathPt.size();
	for(int i=1;i<nptCount-1;i++)
	{		
		p_this=_vInPathPt[i];
		p_next=_vInPathPt[i+1];
		k1.reset(p_pre,p_this);
		k2.reset(p_this,p_next);
		k1.Normalize();
		k2.Normalize();
		k=k1+k2;
		k.Rotate(90);
		k.Normalize();
		double dCosVal = k1.GetCosOfTwoVector( k );
		double dAngle = acos( dCosVal );		
		DistanceUnit dAdjustedWidth=1.0/sin(dAngle);
		k.SetLength(0.5*dwidth*dAdjustedWidth);
		p1.setPoint(p_this.getX()+k[VX],p_this.getY()+k[VY],p_this.getZ());
		p2.setPoint(p_this.getX()-k[VX],p_this.getY()-k[VY],p_this.getZ());
		_outPts.push_back(p1);_outPts.push_back(p2);
		p_pre=p_this;
		p_this=p_next;
	}
	p_this=_vInPathPt[nptCount-2];
	p_next=_vInPathPt[nptCount-1];
	k.reset(p_this,p_next);
	k.Rotate(90);
	k.Normalize();
	k.SetLength(0.5*dwidth);
	p1.setPoint(p_next.getX()+k[VX],p_next.getY()+k[VY],p_next.getZ());
	p2.setPoint(p_next.getX()-k[VX],p_next.getY()-k[VY],p_next.getZ());
	_outPts.push_back(p1);_outPts.push_back(p2);
	
	return true;
}

void ShapeGenerator::GenWidthPath(const Path& centerPath,double width,widthPath& output)
{
	output.leftpts.clear();
	output.rightpts.clear();
	
	if(centerPath.getCount()<2)return ;

	ARCVector2 k,k1,k2;
	Point  p_pre,p_this,p_next;

	p_pre=centerPath.getPoint(0);p_this=p_pre;p_next=centerPath.getPoint(1);
	k.reset(p_this,p_next);k.Rotate(90);
	k.Normalize();
	k.SetLength(width*0.5);
	Point p1,p2;
	p1.setPoint( p_this.getX()+k[VX],p_this.getY()+k[VY],p_this.getZ() );
	p2.setPoint( p_this.getX()-k[VX],p_this.getY()-k[VY],p_this.getZ() );
	output.leftpts.push_back(p1); output.rightpts.push_back(p2);
	int nptCount =centerPath.getCount();

	bool bleftInAdjust = false;               //record the begin adjust;
	bool brightInAdjust = false;
	for(int i=1;i<nptCount-1;i++)
	{		
		p_this=centerPath.getPoint(i);
		p_next=centerPath.getPoint(i+1);		

		k1.reset(p_pre,p_this);     
		k2.reset(p_this,p_next);
		k1.Normalize();
		k2.Normalize();
		k=k1+k2;
		k.Rotate(90);
		k.Normalize();
		double dCosVal = k1.GetCosOfTwoVector( k );
		double dAngle = acos( dCosVal );		
		DistanceUnit dAdjustedWidth=1.0/sin(dAngle);
		
		k.SetLength(0.5*width*dAdjustedWidth);
		p1.setPoint(p_this.getX()+k[VX],p_this.getY()+k[VY],p_this.getZ());
		p2.setPoint(p_this.getX()-k[VX],p_this.getY()-k[VY],p_this.getZ());		

		output.leftpts.push_back(p1);
		output.rightpts.push_back(p2);
		
		p_pre=p_this;
		p_this=p_next;
	}
	p_this=centerPath.getPoint(nptCount-2);
	p_next=centerPath.getPoint(nptCount-1);
	k.reset(p_this,p_next);
	k.Rotate(90);
	k.Normalize();
	k.SetLength(0.5*width);
	p1.setPoint(p_next.getX()+k[VX],p_next.getY()+k[VY],p_next.getZ());
	p2.setPoint(p_next.getX()-k[VX],p_next.getY()-k[VY],p_next.getZ());
	output.leftpts.push_back(p1);output.rightpts.push_back(p2);

	return;
	//adjust the points
	ARCPath3::iterator itr;
	ARCPath3::iterator start,end;
	bool bInAdjust = false;
	
	int ibegin,iend;

	//ARCVector3 startV,endV;
	//itr = output.leftpts.begin();
	for(int i=0;i<nptCount-1;i++)
	{		
		ARCVector3 vcenter,vside; 
		vcenter = ARCVector3( centerPath.getPoint(i),centerPath.getPoint(i+1) ) ;
		vside = output.leftpts[i+1] - output.leftpts[i];
		if( vcenter.GetCosOfTwoVector(vside)<0 )
		{
			if(!bInAdjust) { start = itr; ibegin =i; }
			
			bInAdjust = true;
		}
		else
		{
			if(bInAdjust)
			{
				end = itr; iend = i;
				//startV = *start;endV = *end;
				//adjust the points
				for(int j = ibegin;j<iend+1;j++ )
				{

					ARCVector3 v1 = output.leftpts[ibegin] - output.leftpts[ibegin-1];
					ARCVector3 v2 = output.leftpts[iend+1] - output.leftpts[iend];
					ARCVector3 vp = output.leftpts[ibegin] - output.leftpts[iend];
					double ratio =  (-vp[VX] * v2[VY] + v2[VX] * vp[VY] ) / ( v2[VX]*v1[VY] - v1[VX] * v2[VY] );
					//ARCVector3 vres1 = output.leftpts[ibegin] - ratio * v1 ;
					//ratio = ( v1[VX]* vp[VY] - v1[VY]*vp[VX]);
					

					output.leftpts[j] = output.leftpts[ibegin] - ratio * v1 ;
				}
				//
				start = end;
			}			
			bInAdjust = false;
		}
		//itr++;
	}

	//itr = output.rightpts.begin();	
	for(int i=0;i<nptCount-1;i++)
	{		
		ARCVector3 vcenter,vside; 
		vcenter = ARCVector3( centerPath.getPoint(i),centerPath.getPoint(i+1) ) ;
		vside = output.rightpts[i+1] - output.rightpts[i];
		if( vcenter.GetCosOfTwoVector(vside)<0 )
		{
			if(!bInAdjust) { start = itr; ibegin = i; }
			bInAdjust = true;
		}
		else
		{
			if(bInAdjust)
			{
				end = itr;   iend= i;
				//startV = *start;endV = *end;
				//adjust the points
				for(int j = ibegin;j<iend+1;j++ )
				{				
					ARCVector3 v1 = output.rightpts[ibegin] - output.rightpts[ibegin-1];
					ARCVector3 v2 = output.rightpts[iend+1] - output.rightpts[iend];
					ARCVector3 vp = output.rightpts[ibegin] - output.rightpts[iend];
					double ratio =  (-vp[VX] * v2[VY] + v2[VX] * vp[VY] ) / ( v2[VX]*v1[VY] - v1[VX] * v2[VY] );
					
					output.rightpts[j] = output.rightpts[ibegin] - ratio * v1 ; 
				}
				//
				start = end;
			}			
			bInAdjust = false;
		}
		//itr++;
	}

}

void ShapeGenerator::GenWidthPath(const CPath2008& centerPath,double width,widthPath& output)
{
	output.leftpts.clear();
	output.rightpts.clear();

	if(centerPath.getCount()<2)return ;

	ARCVector2 k,k1,k2;
	CPoint2008  p_pre,p_this,p_next;

	p_pre=centerPath.getPoint(0);p_this=p_pre;p_next=centerPath.getPoint(1);
	k.reset(p_this,p_next);k.Rotate(90);
	k.Normalize();
	k.SetLength(width*0.5);
	CPoint2008 p1,p2;
	p1.setPoint( p_this.getX()+k[VX],p_this.getY()+k[VY],p_this.getZ() );
	p2.setPoint( p_this.getX()-k[VX],p_this.getY()-k[VY],p_this.getZ() );
	output.leftpts.push_back(p1); output.rightpts.push_back(p2);
	int nptCount =centerPath.getCount();

	bool bleftInAdjust = false;               //record the begin adjust;
	bool brightInAdjust = false;
	for(int i=1;i<nptCount-1;i++)
	{		
		p_this=centerPath.getPoint(i);
		p_next=centerPath.getPoint(i+1);		

		k1.reset(p_pre,p_this);     
		k2.reset(p_this,p_next);
		k1.Normalize();
		k2.Normalize();
		k=k1+k2;
		k.Rotate(90);
		k.Normalize();
		double dCosVal = k1.GetCosOfTwoVector( k );
		double dAngle = acos( dCosVal );		
		DistanceUnit dAdjustedWidth=1.0/sin(dAngle);

		k.SetLength(0.5*width*dAdjustedWidth);
		p1.setPoint(p_this.getX()+k[VX],p_this.getY()+k[VY],p_this.getZ());
		p2.setPoint(p_this.getX()-k[VX],p_this.getY()-k[VY],p_this.getZ());		

		output.leftpts.push_back(p1);
		output.rightpts.push_back(p2);

		p_pre=p_this;
		p_this=p_next;
	}
	p_this=centerPath.getPoint(nptCount-2);
	p_next=centerPath.getPoint(nptCount-1);
	k.reset(p_this,p_next);
	k.Rotate(90);
	k.Normalize();
	k.SetLength(0.5*width);
	p1.setPoint(p_next.getX()+k[VX],p_next.getY()+k[VY],p_next.getZ());
	p2.setPoint(p_next.getX()-k[VX],p_next.getY()-k[VY],p_next.getZ());
	output.leftpts.push_back(p1);output.rightpts.push_back(p2);

	return;
	//adjust the points
	ARCPath3::iterator itr;
	ARCPath3::iterator start,end;
	bool bInAdjust = false;

	int ibegin,iend;

	//ARCVector3 startV,endV;
	//itr = output.leftpts.begin();
	for(int i=0;i<nptCount-1;i++)
	{		
		ARCVector3 vcenter,vside; 
		vcenter = ARCVector3( centerPath.getPoint(i),centerPath.getPoint(i+1) ) ;
		vside = output.leftpts[i+1] - output.leftpts[i];
		if( vcenter.GetCosOfTwoVector(vside)<0 )
		{
			if(!bInAdjust) { start = itr; ibegin =i; }

			bInAdjust = true;
		}
		else
		{
			if(bInAdjust)
			{
				end = itr; iend = i;
				//startV = *start;endV = *end;
				//adjust the points
				for(int j = ibegin;j<iend+1;j++ )
				{

					ARCVector3 v1 = output.leftpts[ibegin] - output.leftpts[ibegin-1];
					ARCVector3 v2 = output.leftpts[iend+1] - output.leftpts[iend];
					ARCVector3 vp = output.leftpts[ibegin] - output.leftpts[iend];
					double ratio =  (-vp[VX] * v2[VY] + v2[VX] * vp[VY] ) / ( v2[VX]*v1[VY] - v1[VX] * v2[VY] );
					//ARCVector3 vres1 = output.leftpts[ibegin] - ratio * v1 ;
					//ratio = ( v1[VX]* vp[VY] - v1[VY]*vp[VX]);


					output.leftpts[j] = output.leftpts[ibegin] - ratio * v1 ;
				}
				//
				start = end;
			}			
			bInAdjust = false;
		}
		//itr++;
	}

	//itr = output.rightpts.begin();	
	for(int i=0;i<nptCount-1;i++)
	{		
		ARCVector3 vcenter,vside; 
		vcenter = ARCVector3( centerPath.getPoint(i),centerPath.getPoint(i+1) ) ;
		vside = output.rightpts[i+1] - output.rightpts[i];
		if( vcenter.GetCosOfTwoVector(vside)<0 )
		{
			if(!bInAdjust) { start = itr; ibegin = i; }
			bInAdjust = true;
		}
		else
		{
			if(bInAdjust)
			{
				end = itr;   iend= i;
				//startV = *start;endV = *end;
				//adjust the points
				for(int j = ibegin;j<iend+1;j++ )
				{				
					ARCVector3 v1 = output.rightpts[ibegin] - output.rightpts[ibegin-1];
					ARCVector3 v2 = output.rightpts[iend+1] - output.rightpts[iend];
					ARCVector3 vp = output.rightpts[ibegin] - output.rightpts[iend];
					double ratio =  (-vp[VX] * v2[VY] + v2[VX] * vp[VY] ) / ( v2[VX]*v1[VY] - v1[VX] * v2[VY] );

					output.rightpts[j] = output.rightpts[ibegin] - ratio * v1 ; 
				}
				//
				start = end;
			}			
			bInAdjust = false;
		}
		//itr++;
	}
}

void ShapeGenerator::GenSmoothPath(const ARCPath3& _in,ARCPath3& _out,double smoothLen)
{
	_out.clear();
	if(_in.size()<2)return;
	
	ARCVector3 ctrPts[3];
	
	
	_out.push_back(_in[0]);

	for(int i=1;i<(int)_in.size()-1;i++)
	{
		ctrPts[0] = _in[i-1];
		ctrPts[1] = _in[i];
		ctrPts[2] = _in[i+1];
		
		//get the two side point
		ARCVector3 v1(ctrPts[1],ctrPts[0]);
		ARCVector3 v2(ctrPts[1],ctrPts[2]);
		double minLen1,minLen2;
		minLen1 = 0.45 * (ctrPts[1]-ctrPts[0]).Magnitude();
		minLen2	= 0.45 * (ctrPts[2]-ctrPts[1]).Magnitude();
		if(v1.Magnitude() <smoothLen) 
			minLen1= v1.Magnitude() * 0.9;
		if(v2.Magnitude()<smoothLen) 
			minLen2 = v2.Magnitude() * 0.9;
		v1.Normalize();v2.Normalize();
		ctrPts[0] = ctrPts[1] + v1 * minLen1; ctrPts[2] = ctrPts[1] + v2*minLen2;
		std::vector<ARCVector3> ctrlPoints(ctrPts,ctrPts+3);
		std::vector<ARCVector3> output;
		int outPtsCnt = 8;
		GenBezierPath(ctrlPoints,output,outPtsCnt);
		for(int i = 0;i<outPtsCnt;i++)
		{
			_out.push_back(output[i]);
		}
	}

	_out.push_back( _in[_in.size()-1 ] );

}
void ShapeGenerator::GenBezierPath(const std::vector<ARCVector3>& _input, std::vector<ARCVector3>& _output, int nPtCount )
{	
	_output.clear();_output.reserve(nPtCount);
	BizierCurve::GenCurvePoints(_input,_output,nPtCount);
}

void ShapeGenerator::GenWidthPipePath(const ARCPath3& centerPath,const double& width, WidthPipePath& output){
	if(centerPath.size()<2)return ;

	ARCVector2 k,k1,k2;
	ARCVector3  p_pre,p_this,p_next;

	p_pre=centerPath[0];p_this=p_pre;p_next=centerPath[1];
	ARCVector3 k_3 = p_next -p_this;
	k .reset( k_3[VX],k_3[VY]) ;k.Rotate(-90);
	k.Normalize();
	k.SetLength(width*0.5);
	Point p1,p2;
	p1.setPoint( p_this[VX]+k[VX],p_this[VY]+k[VY],p_this[VZ] );
	p2.setPoint( p_this[VX]-k[VX],p_this[VY]-k[VY],p_this[VZ] );
	output.leftpath.push_back(p1); output.rightpath.push_back(p2);
	int nptCount =centerPath.size();

	bool bleftInAdjust = false;               //record the begin adjust;
	bool brightInAdjust = false;
	for(int i=1;i<nptCount-1;i++)
	{		
		p_this=centerPath[i];
		p_next=centerPath[i+1];		

		k_3 = p_this - p_pre;	k1.reset(k_3[VX],k_3[VY]);     
		k_3 = p_next - p_this;  k2.reset(k_3[VX],k_3[VY]);
		k1.Normalize();
		k2.Normalize();
		k=k1+k2;
		k.Rotate(-90);
		k.Normalize();
		double dCosVal = k1.GetCosOfTwoVector( k );
		double dAngle = acos( dCosVal );		
		DistanceUnit dAdjustedWidth=1.0/sin(dAngle);

		k.SetLength(0.5*width*dAdjustedWidth);
		p1.setPoint(p_this[VX]+k[VX],p_this[VY]+k[VY],p_this[VZ]);
		p2.setPoint(p_this[VX]-k[VX],p_this[VY]-k[VY],p_this[VZ]);		

		output.leftpath.push_back(p1);
		output.rightpath.push_back(p2);

		p_pre=p_this;
		p_this=p_next;
	}
	p_this=centerPath[nptCount-2];
	p_next=centerPath[nptCount-1];
	k_3 = p_next - p_this; k.reset(k_3[VX],k_3[VY]);
	k.Rotate(-90);
	k.Normalize();
	k.SetLength(0.5*width);
	p1.setPoint(p_next[VX]+k[VX],p_next[VY]+k[VY],p_next[VZ]);
	p2.setPoint(p_next[VX]-k[VX],p_next[VY]-k[VY],p_next[VZ]);
	output.leftpath.push_back(p1);output.rightpath.push_back(p2);
}

void ShapeGenerator::GenWidthPipePath(const ARCPath3& centerPath,const double& width1,const double& width2, WidthPipePath& output)
{
	if(centerPath.size()<2)return ;

	ARCVector2 k,k1,k2;
	ARCVector3  p_pre,p_this,p_next;

	p_pre=centerPath[0];p_this=p_pre;p_next=centerPath[1];
	ARCVector3 k_3 = p_next -p_this;
	k .reset( k_3[VX],k_3[VY]) ;k.Rotate(-90);
	k.Normalize();
	k.SetLength(width1*0.5);
	Point p1,p2;
	p1.setPoint( p_this[VX]+k[VX],p_this[VY]+k[VY],p_this[VZ] );
	p2.setPoint( p_this[VX]-k[VX],p_this[VY]-k[VY],p_this[VZ] );
	output.leftpath.push_back(p1); output.rightpath.push_back(p2);
	int nptCount =centerPath.size();
	double Theta = fabs((width1-width2)/nptCount);
	bool bleftInAdjust = false;               //record the begin adjust;
	bool brightInAdjust = false;
	double width =0.0;
	for(int i=1;i<nptCount-1;i++)
	{
		if (width1>=width2)
		{
			width = width1-Theta*i;
		}
		else
		{
			width = width1+Theta*i;
		}
		p_this=centerPath[i];
		p_next=centerPath[i+1];		

		k_3 = p_this - p_pre;	k1.reset(k_3[VX],k_3[VY]);     
		k_3 = p_next - p_this;  k2.reset(k_3[VX],k_3[VY]);
		k1.Normalize();
		k2.Normalize();
		k=k1+k2;
		k.Rotate(-90);
		k.Normalize();
		double dCosVal = k1.GetCosOfTwoVector( k );
		double dAngle = acos( dCosVal );		
		DistanceUnit dAdjustedWidth=1.0/sin(dAngle);

		k.SetLength(0.5*width*dAdjustedWidth);
		p1.setPoint(p_this[VX]+k[VX],p_this[VY]+k[VY],p_this[VZ]);
		p2.setPoint(p_this[VX]-k[VX],p_this[VY]-k[VY],p_this[VZ]);		

		output.leftpath.push_back(p1);
		output.rightpath.push_back(p2);

		p_pre=p_this;
		p_this=p_next;
	}
	p_this=centerPath[nptCount-2];
	p_next=centerPath[nptCount-1];
	k_3 = p_next - p_this; k.reset(k_3[VX],k_3[VY]);
	k.Rotate(-90);
	k.Normalize();
	k.SetLength(0.5*width2);
	p1.setPoint(p_next[VX]+k[VX],p_next[VY]+k[VY],p_next[VZ]);
	p2.setPoint(p_next[VX]-k[VX],p_next[VY]-k[VY],p_next[VZ]);
	output.leftpath.push_back(p1);output.rightpath.push_back(p2);
}