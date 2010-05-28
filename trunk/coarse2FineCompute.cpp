#include "coarse2FineCompute.h"
coarse2FineCompute::coarse2FineCompute(int imageDepth)
{
	_imageDepth=imageDepth;
}

coarse2FineCompute::~coarse2FineCompute(void)
{
}


// basic functions
	template <class T>
	T EnforceRange(const T& x,const int& MaxValue) {return __min(__max(x,0),MaxValue-1);};

//--------------------------------------------------------------------------------------------------
// function to interplate multi-channel image plane for (x,y)
// --------------------------------------------------------------------------------------------------
//template <class T1,class T2>
inline void BilinearInterpolate(const IplImage* pImage,int width,int height,int nChannels,double x,double y,IplImage* result,int i,int j)
{
	int xx,yy,m,n,u,v,k,offset;
	xx=x;
	yy=y;
	double dx,dy,s;
	dx=__max(__min(x-xx,1),0);
	dy=__max(__min(y-yy,1),0);

	//memset(result,0,sizeof(IplImage*)*nChannels);

	for(m=0;m<=1;m++)
		for(n=0;n<=1;n++)
		{
			u=EnforceRange(xx+m,width);
			v=EnforceRange(yy+n,height);
			offset=(v*width+u)*nChannels;
			s=fabs(1-m-dx)*fabs(1-n-dy);
			for(k=0;k<nChannels;k++)
				k++;
				//result[k]+=pImage[offset+k]*s;
				(result->imageData + i*result->widthStep)[j+k]=(pImage->imageData + m*pImage->widthStep)[n+k]*s;
		}
}
//------------------------------------------------------------------------------------------------------------
// function to warp an image with respect to flow field
// pWarpIm2 has to be allocated before hands
//------------------------------------------------------------------------------------------------------------
//template <class T1,class T2>
int warpImage(IplImage* pWarpIm2, const IplImage* pIm1, const IplImage* pIm2, const IplImage* pVx, const IplImage* pVy)
{
	int ans;
	int width,height,nChannels;
	width=pIm2->width;
	height=pIm2->height;
	nChannels=pIm2->nChannels;
	for(int i=0;i<height;i++)
		for(int j=0;j<width;j++)
		{
			int offset=i*width+j;
			double x,y;
			if(pVy->nChannels!=1){//1 channel support for now
				return -1;
			}
		//	y=i+pVy[offset];		
			y=((uchar *)(pVy->imageData + i*pVy->widthStep))[j]+i;			
		//	x=j+pVx[offset];
			x=((uchar *)(pVx->imageData + i*pVx->widthStep))[j]+j;
			//offset*=nChannels;
			if(x<0 || x>width-1 || y<0 || y>height-1)
			{
				for(int k=0;k<nChannels;k++)
					//pWarpIm2[offset+k]=pIm1[offset+k];
					(pWarpIm2->imageData + i*pWarpIm2->widthStep)[j+k]=(pIm1->imageData + i*pIm1->widthStep)[j+k];
				continue;
			}
			//BilinearInterpolate(pIm2,width,height,nChannels,x,y,pWarpIm2+offset);
			BilinearInterpolate(pIm2,width,height,nChannels,x,y,pWarpIm2,i,j);
			
		}

		return 0;
}



//need to recieve cvSobel with function pointer;
int getDXs(const IplImage* src,IplImage* dest_dx,IplImage* dest_dy){	
	//IplImage* orginalImage = cvLoadImage(fileAddress,0);
	
	/*create temp images*/
	IplImage* df_dx = cvCreateImage(cvGetSize(src),IPL_DEPTH_16S,1);
	IplImage* df_dy = cvCreateImage(cvGetSize(src),IPL_DEPTH_16S,1);

	//dest_dx=cvCreateImage(cvGetSize(src),IPL_DEPTH_8U,1); 
	//dest_dy=cvCreateImage(cvGetSize(src),IPL_DEPTH_8U,1); 
	/* use sobel to find derivatives */
	cvSobel( src, df_dx, 1, 0, 3);
	cvSobel( src, df_dy, 0, 1, 3);

	/* Convert signed to unsigned 8*/
	cvConvertScaleAbs( df_dx , dest_dx, 1, 0);
	cvConvertScaleAbs( df_dy , dest_dy, 1, 0); 

	//toolsKit::cvShowManyImages("dx dy",2,dest_dx,dest_dy);


	return 0;
}






IplImage* coarse2FineCompute::LaplaceCompute(IplImage* input,IplImage* input2){
	IplImage* output= cvCreateImage(cvSize(input->width, input->height), input->depth, input->nChannels);;
	cvLaplace( input, input2, 3 );

	return output;
}


IplImage* coarse2FineCompute::createWarp(IplImage* WarpImage2, IplImage* img1,IplImage* img2,IplImage* vx,IplImage* vy){

	//IplImage* WarpImage2 = cvCreateImage(cvSize(img2->width,img2->height ),img2->depth,img2->nChannels );
	int status=warpImage(WarpImage2,img1, img2,vx,vy);
	if (status==-1)
		cout<<"warp corrently support only 1 channel pics"<<endl;
	return WarpImage2;
}

void coarse2FineCompute::Coarse2FineFlow(IplImage* vx, 
										 IplImage* vy, 
										 IplImage &warpI2,
										 const IplImage &Im1, 
										 const IplImage &Im2, 
										 double alpha, 
										 double gamma,
										 double ratio, 
										 int minWidth,
										 int nOuterFPIterations, 
										 int nInnerFPIterations, 
										 int nCGIterations)
{
	// first build the pyramid of the two images
	GaussPyramid Pyramid1;
	GaussPyramid Pyramid2;		
	cout<<"Constructing pyramid..."<<endl;
	Pyramid1.ConstructPyramid(Im1,ratio,minWidth);


	Pyramid2.ConstructPyramid(Im2,ratio,minWidth);
	cout<<"done!"<<endl;

	// now iterate from the top level to the bottom
	IplImage* WarpImage2=NULL;

	for(int k=Pyramid1.nlevels()-1;k>=0;k--)
	{		
		cout<<"Pyramid level "<<k<<"-";

		int width=Pyramid1.getImageFromPyramid(k)->width;
		int height=Pyramid1.getImageFromPyramid(k)->height;
		int depth=Pyramid1.getImageFromPyramid(k)->depth;
		int nChannels=Pyramid1.getImageFromPyramid(k)->nChannels;
		cout<<"width:"<<width<<"  height:"<<height<<endl;

		//on top level(first iteration)
		if(k==Pyramid1.nlevels()-1)
		{
			cout<<"first iteration:"<<k<<endl;
			vx=cvCreateImage(cvSize(width,height),depth,nChannels);
			vy=cvCreateImage(cvSize(width,height),depth,nChannels);
			cvZero(vx);
			cvZero(vy);
			//clone image2 to warpImage2 (in first iter)
			WarpImage2 = cvCreateImage(cvSize(Pyramid2.getImageFromPyramid(k)->width,Pyramid2.getImageFromPyramid(k)->height ),Pyramid2.getImageFromPyramid(k)->depth, Pyramid2.getImageFromPyramid(k)->nChannels );
			WarpImage2=  cvCloneImage(Pyramid2.getImageFromPyramid(k));			
		}
		else
		{
			IplImage *tempVx = cvCreateImage(cvSize(width, height), vx->depth, vx->nChannels);
			cvResize(vx, tempVx); 
			vx=tempVx;			
			IplImage *tempVy = cvCreateImage(cvSize(width, height), vy->depth, vy->nChannels);
			cvResize(vy, tempVy); 
			vy=tempVy;			
			//create the warp image
			WarpImage2 = cvCreateImage(cvSize(Pyramid2.getImageFromPyramid(k)->width,Pyramid2.getImageFromPyramid(k)->height ),Pyramid2.getImageFromPyramid(k)->depth, Pyramid2.getImageFromPyramid(k)->nChannels );
			cvZero(WarpImage2);
			WarpImage2=createWarp(WarpImage2,Pyramid1.getImageFromPyramid(k),Pyramid2.getImageFromPyramid(k),vx,vy);
			toolsKit::cvShowManyImages("warpImage2",1, WarpImage2);
			
		    //toolsKit::cvShowManyImages("pyramid iteration",4, Pyramid1.getImageFromPyramid(k),Pyramid2.getImageFromPyramid(k),vx,vy);
			//IplImage* out=LaplaceCompute(Pyramid1.getImageFromPyramid(k),Pyramid2.getImageFromPyramid(k));			
		}						
		SmoothFlowPDE2( Pyramid1.getImageFromPyramid(k),Pyramid2.getImageFromPyramid(k),WarpImage2,vx,vy,alpha,gamma,nOuterFPIterations,nInnerFPIterations,nCGIterations);	
	}
	//warpFL(WarpImage2,Pyramid1.getImageFromPyramid(k),Pyramid2.getImageFromPyramid(k),vx,vy);
}

void IPLsqrt_mul2(IplImage* img){
	//BwImage imgA(img);
	//for (int i=0; i<img->width;i++)
	//	for(int j=0;j<img->height;j++)
	//		imgA[i][j] = 1/(2*sqrt((double)imgA[i][j]));
	IplImageIterator<unsigned char> it(img);
	while (!it) {      
	  *it= 1/(2*sqrt((double)*it)); 
	  ++it;
	}
}

void cvMulScalar(IplImage* img,double scalar){
	IplImageIterator<unsigned char> it(img);
	while (!it) {      
	  *it= scalar*(double)*it; 
	  ++it;
	}
}

double psiDerivative(double x,double epsilon){	
	double y=1 / (2 * sqrt( x + epsilon ) ) ;
	return y;
}

IplImage* psiDerivative(IplImage* x,double epsilon){	
	//double y=1 / (2 * sqrt( x + epsilon ) ) ;
	//cvShowImage("before",x);
	cvAddS(x,cvRealScalar(epsilon),x);
	
	IPLsqrt_mul2(x);
	//cvShowImage("after",x);
//	x->imageData
	return x;
}



IplImage* computePsidash(IplImage* Ikt_Org,IplImage* Ikx,IplImage* Iky,IplImage* IXt_axis, IplImage* Ixx, IplImage* Ixy,
						 IplImage* IYt_ayis	,IplImage* Iyy ,IplImage* du ,IplImage* dv ,double gamma){
	
	IplImage* ans1=cvCreateImage(cvSize( Ikt_Org->width, Ikt_Org->height ),IPL_DEPTH_8U,Ikt_Org->nChannels);
	IplImage* ans2=cvCreateImage(cvSize( Ikt_Org->width, Ikt_Org->height ),IPL_DEPTH_8U,Ikt_Org->nChannels);
	IplImage* ans3=cvCreateImage(cvSize( Ikt_Org->width, Ikt_Org->height ),IPL_DEPTH_8U,Ikt_Org->nChannels);
	IplImage* ans4=cvCreateImage(cvSize( Ikt_Org->width, Ikt_Org->height ),IPL_DEPTH_8U,Ikt_Org->nChannels);

	/* ( Ikz + Ikx*du + Iky*dv )^ 2 +
	   gamma * ( ( Ixz + Ixx*du + Ixy*dv )^ 2 +
			   ( Iyz + Ixy*du + Iyy*dv )^ 2 )
	*/
	
	//( Ikz + Ikx*du + Iky*dv )^ 2==>ans1
	cvMul(Ikx,du,ans1);
	cvMul(Iky,dv,ans2);
	cvAdd(Ikt_Org,ans1,ans1);
	cvAdd(ans1,ans2,ans2);
	cvPow(ans2,ans1,2);
	//( Ixz + Ixx*du + Ixy*dv )^ 2==>ans2
	cvMul(Ixx,du,ans2);
	cvMul(Ixy,dv,ans3);
	cvAdd(IXt_axis,ans2,ans2);
	cvAdd(ans2,ans3,ans3);
	cvPow(ans3,ans2,2);
	//( Iyz + Ixy*du + Iyy*dv )^ 2==>ans3
	cvMul(Ixy,du,ans3);
	cvMul(Iyy,dv,ans4);
	cvAdd(IYt_ayis,ans3,ans3);
	cvAdd(ans3,ans4,ans4);
	cvPow(ans4,ans3,2);

	//========gamma*ans3
	cvAdd(ans3,ans4,ans3);
	cvMulScalar(ans3,gamma);
	cvAdd(ans1,ans3,ans1);
	toolsKit::cvShowManyImages("pesdia:ans1,ans2,ans3,ans4",4,ans1,ans2,ans3,ans4);
	//clean
	cvReleaseImage( &ans2 ); 
	cvReleaseImage( &ans3 ); 
	cvReleaseImage( &ans4 ); 

	return ans1;
}

void coarse2FineCompute::computePsidashFS_brox(IplImage* iterU,IplImage* iterV,int width,int height,int channels,flowUV* UV){	
	//init masks
	double a[] = {1,1};
	double b[] = {1,-1};
	double c[]={0.5,0.5};
	CvMat* matOnes = &cvMat( 1, 2, CV_64FC1, a ); // 64FC1 for double
	CvMat* matOnesT=&cvMat( 2, 1, CV_64FC1, a );
	cvTranspose(matOnes,matOnesT);

	CvMat* matHalf = &cvMat( 1, 2, CV_64FC1, c ); // 64FC1 for double
	CvMat* matHalfT=&cvMat( 2, 1, CV_64FC1, c );
	cvTranspose(matHalf,matHalfT);
	
	
	CvMat* matOneNegOne = &cvMat( 1, 2, CV_64FC1, b ); // 64FC1 for double
	CvMat* matOneNegOneT=&cvMat( 2, 1, CV_64FC1, b );;
	cvTranspose(matOneNegOne,matOneNegOneT);
	//init temp params
	IplImage* ux=cvCreateImage(cvSize( width, height ),_imageDepth,channels);
	IplImage* uy=cvCreateImage(cvSize( width, height ),_imageDepth,channels);
	IplImage* vx=cvCreateImage(cvSize( width, height ),_imageDepth,channels);
	IplImage* vy=cvCreateImage(cvSize( width, height ),_imageDepth,channels);
	IplImage* uxd=cvCreateImage(cvSize( width, height ),_imageDepth,channels);
	IplImage* vxd=cvCreateImage(cvSize( width, height ),_imageDepth,channels);
	IplImage* uyd=cvCreateImage(cvSize( width, height ),_imageDepth,channels);
	IplImage* vyd=cvCreateImage(cvSize( width, height ),_imageDepth,channels);
	IplImage* t=cvCreateImage(cvSize( width, height ),_imageDepth,channels);
	IplImage* uxpd=cvCreateImage(cvSize( width, height ),_imageDepth,channels);
	IplImage* uypd=cvCreateImage(cvSize( width, height ),_imageDepth,channels);
	IplImage* vxpd=cvCreateImage(cvSize( width, height ),_imageDepth,channels);
	IplImage* vypd=cvCreateImage(cvSize( width, height ),_imageDepth,channels);

	//compute psidashFS
	cvFilter2D(iterU,ux,matOneNegOne);// x and y derivatives of u by 2d convolution
	cvFilter2D(iterU,uy,matOneNegOneT);// x and y derivatives of u by 2d convolution
	cvFilter2D(iterV,vx,matOneNegOne);// x and y derivatives of v by 2d convolution
	cvFilter2D(iterV,vy,matOneNegOneT);	// x and y derivatives of v by 2d convolution

	//toolsKit::cvShowManyImages("Image22",6,iterU,iterV,ux,uy,vx,vy);		

	cvFilter2D(ux,uxd,matHalf);//averaging as per the numerics section of the second chapter.for x
	cvFilter2D(vx,vxd,matHalf);

	cvFilter2D(uy,uyd,matHalfT);//averaging as per the numerics section of the second chapter.for y
	cvFilter2D(vy,vyd,matHalfT);	
	
	cvFilter2D(uyd,t,matHalf);// Computes the delta u(i+1/2, j) and delta u(i-1/2, j).
	cvPow(ux,ux,2);//ux^2
	cvPow(t,t,2);//t^2
	cvAdd(ux,t,uxpd);//uxpd = ux^2 + t^2 

	
	cvFilter2D(uxd,t,matHalfT);//Computes the delta u(i, j+1/2) and delta u(i, j-1/2).
	cvPow(uy,uy,2);//uy^2
	cvPow(t,t,2);//t^2
	cvAdd(uy,t,uypd);//uypd = uy^2 + t^2	
	
	cvFilter2D(vyd,t,matHalf);// Computes the delta v(i+1/2, j) and delta v(i-1/2, j).
	cvPow(vx,vx,2);//vx^2
	cvPow(t,t,2);//t^2
	cvAdd(vx,t,vxpd);//vxpd = vx^2 + t^2

	cvFilter2D(vxd,t,matHalfT);// Computes the delta v(i+1/2, j) and delta v(i-1/2, j).
	cvPow(vy,vy,2);//vx^2
	cvPow(t,t,2);//t^2
	cvAdd(vy,t,vypd);//vypd=vy^2 + t^2
	
	//toolsKit::cvShowManyImages("before:uypd,vypd,ans1",3,uypd,vypd,UV->getPsidashFSAns1());			
	//toolsKit::cvShowManyImages("before:vxpd,vxpd,ans2",3,vxpd,vxpd,UV->getPsidashFSAns2());	
	cvAdd(uypd,vypd,UV->getPsidashFSAns1());
	cvAdd(uxpd,vxpd,UV->getPsidashFSAns2());
	//toolsKit::cvShowManyImages("after:uypd,vypd,ans1",3,uypd,vypd,UV->getPsidashFSAns1());			
	//toolsKit::cvShowManyImages("after:vxpd,vxpd,ans2",3,vxpd,vypd,UV->getPsidashFSAns2());			
	psiDerivative(UV->getPsidashFSAns1(),0.001);
	psiDerivative(UV->getPsidashFSAns2(),0.001);
	
	cvReleaseImage( &ux ); 
	cvReleaseImage( &uy ); 
	cvReleaseImage( &vx ); 
	cvReleaseImage( &vy ); 
	cvReleaseImage( &uxd ); 
	cvReleaseImage( &vxd ); 
	cvReleaseImage( &uyd ); 
	cvReleaseImage( &vyd ); 
	cvReleaseImage( &t ); 
	cvReleaseImage( &uxpd ); 
	cvReleaseImage( &uypd ); 
	cvReleaseImage( &vxpd ); 
	cvReleaseImage( &vypd ); 

//	return ans;
}

flowUV* coarse2FineCompute::SmoothFlowPDE2( const IplImage* Im1, 
											const IplImage* Im2, 
											IplImage* warpIm2, 
											IplImage* uinit, 
											IplImage* vinit, 
											double alpha,
											double gamma,
											int nOuterFPIterations, 
											int nInnerFPIterations, 
											int nCGIterations){
		
		//dimentions
		int height=Im1->height;
		int width=Im1->width;
		int channels=Im1->nChannels;
		//this will hold the optical flow
	    //flowUV* UV=new flowUV(width,height,IPL_DEPTH_8U,channels);
		flowUV* UV=new flowUV(uinit,vinit);
		//init for the different DX,DY & DT		
		IplImage* Ikx=cvCreateImage(cvSize( width, height ),_imageDepth,channels); 
		IplImage* Iky=cvCreateImage(cvSize( width, height ),_imageDepth,channels);
		IplImage* Ikx2=cvCreateImage(cvSize( width, height ),_imageDepth,channels); 
		IplImage* Iky2=cvCreateImage(cvSize( width, height ),_imageDepth,channels); 
		IplImage* Ikt_Org=cvCreateImage(cvSize( width, height ),_imageDepth,channels); 
		IplImage* IXt_axis=cvCreateImage(cvSize( width, height ),_imageDepth,channels); 
		IplImage* IYt_ayis=cvCreateImage(cvSize( width, height ),_imageDepth,channels); 
		//the gradient of the gradient
		IplImage* Ixx=cvCreateImage(cvSize( width, height ),_imageDepth,channels); 
		IplImage* Ixy=cvCreateImage(cvSize( width, height ),_imageDepth,channels);
		IplImage* Iyx=cvCreateImage(cvSize( width, height ),_imageDepth,channels); 
		IplImage* Iyy=cvCreateImage(cvSize( width, height ),_imageDepth,channels);	
		//the addition in each iter to u&v
		IplImage* Du=cvCreateImage(cvSize( width, height ),_imageDepth,channels); 
		IplImage* Dv=cvCreateImage(cvSize( width, height ),_imageDepth,channels);

		//	IplImage* im1=cvCreateImage(cvSize( Im1->width, Im1->height ),IPL_DEPTH_8U,1); 
		//	IplImage* im2=cvCreateImage(cvSize( Im2->width, Im2->height ),IPL_DEPTH_8U,1);
		//IplImage *destination = cvCreateImage(cvSize( source->width, source->height ), IPL_DEPTH_8U, 1 );
			//convert to grayscale
		//	cvCvtColor(Im1,im1,CV_RGB2GRAY);
		//	cvCvtColor(Im2,im2,CV_RGB2GRAY);			
		
		//create the different DX of the pictures
		getDXs(Im1,Ikx,Iky);
		getDXs(Im2,Ikx2,Iky2);
		//by brox we need to take the gradient of the gradient:
		getDXs(Ikx,Ixx,Ixy);
		getDXs(Iky,Iyx,Iyy);

		//DXT of original images and their x&y gradiants
	 	cvAbsDiff(Im1,Im2,Ikt_Org);
		cvAbsDiff(Ikx,Ikx2,IXt_axis);
		cvAbsDiff(Iky,Iky2,IYt_ayis);
		//toolsKit::cvShowManyImages("Image22",6,Im1,Ikx,Iky,Im2,Ikx2,Iky2);
		//toolsKit::cvShowManyImages("Image22",9,Im1,Im2,Ikt_Org,Ikx,Ikx2,IXt_axis,Iky,Iky2,IYt_ayis);			
		
		//toolsKit::cvShowManyImages("Image22",3,Ikt_Org,IXt_axis,IYt_ayis);			
		
		//toolsKit::cvShowManyImages("Image23",3,Im2,Ikx2,Iky2);
		
	

		//init for SOR
		/*
		du = zeros( ht, wt ) ;
		dv = zeros( ht, wt ) ;
		tol = 1e-8 * ones( 2 * ht * wt, 1 ) ;
		duv = zeros( 2 * ht * wt, 1 ) ;
		*/
		
		//outer fixed pint iteration
		
		for(int iter=0;iter<nOuterFPIterations;iter++){
			// First compute the values of the data and smoothness terms
			IplImage* psidash=computePsidash(Ikt_Org,Ikx,Iky,IXt_axis,Ixx,Ixy,
											 IYt_ayis,Iyy,Du,Dv,gamma);

			
			// Compute new psidashFS
			IplImage* tempUadd=cvCreateImage(cvSize( width, height ),_imageDepth,channels); 
			IplImage* tempVadd=cvCreateImage(cvSize( width, height ),_imageDepth,channels); 
			cvAdd(UV->getU(),Du,tempUadd);
			cvAdd(UV->getV(),Dv,tempVadd);
			toolsKit::cvShowManyImages("uinit,vinit du dv",4,tempUadd,tempVadd,Du,Dv);			
			computePsidashFS_brox(tempUadd,tempVadd,width,height,channels,UV);
		}
	//clean temp vars
	cvReleaseImage( &Ikx ); 
	cvReleaseImage( &Iky ); 
	cvReleaseImage( &Ikx2 ); 
	cvReleaseImage( &Iky2 ); 
	cvReleaseImage( &Ikt_Org ); 
	cvReleaseImage( &IXt_axis ); 
	cvReleaseImage( &IYt_ayis ); 
	cvReleaseImage( &Ixx ); 
	cvReleaseImage( &Ixy ); 
	cvReleaseImage( &Iyx ); 
	cvReleaseImage( &Iyy ); 
	cvReleaseImage( &Du ); 
	cvReleaseImage( &Dv ); 
		
	return UV;

}

//--------------------------------------------------------------------------------------------------------
// function to compute optical flow field using two fixed point iterations
// Input arguments:
// Im1, Im2:				    frame 1 and frame 2
// warpIm2:						the warped frame 2 according to the current flow field u and v
// u,v:							the current flow field, NOTICE that they are also output arguments
//	
//--------------------------------------------------------------------------------------------------------
void coarse2FineCompute::SmoothFlowPDE(const IplImage* Im1, 
									   const IplImage* Im2, 
									   IplImage* warpIm2, 
									   IplImage* u, 
									   IplImage* v, 
									   double alpha, 
									   int nOuterFPIterations, 
									   int nInnerFPIterations, 
									   int nCGIterations)
{
	IplImage* mask,imdx,imdy,imdt;
	int width,height,depth,nChannels,nPixels;
	width=Im1->width;
	height=Im1->height;
	depth=Im1->depth;
	nChannels=Im1->nChannels;
	nPixels=width*height;


	IplImage* du=cvCreateImage(cvSize(width,height),depth,NULL);
	IplImage* dv=cvCreateImage(cvSize(width,height),depth,NULL);
	IplImage* uu=cvCreateImage(cvSize(width,height),depth,NULL);
	IplImage* vv=cvCreateImage(cvSize(width,height),depth,NULL);
	IplImage* ux=cvCreateImage(cvSize(width,height),depth,NULL);
	IplImage* uy=cvCreateImage(cvSize(width,height),depth,NULL);
	IplImage* vx=cvCreateImage(cvSize(width,height),depth,NULL);
	IplImage* vy=cvCreateImage(cvSize(width,height),depth,NULL);
	IplImage* Phi_1st=cvCreateImage(cvSize(width,height),depth,NULL);
	IplImage* Psi_1st=cvCreateImage(cvSize(width,height),depth,nChannels);

	IplImage* imdxy,imdx2,imdy2,imdtdx,imdtdy;
	IplImage* ImDxy,ImDx2,ImDy2,ImDtDx,ImDtDy;
	IplImage* A11,A12,A22,b1,b2;
	IplImage* foo1,foo2;

	// variables for conjugate gradient
	//DImage r1,r2,p1,p2,q1,q2;
	//double* rou;
	//rou=new double[nCGIterations];

	//double varepsilon_phi=pow(0.001,2);
	//double varepsilon_psi=pow(0.001,2);

	//--------------------------------------------------------------------------
	// the outer fixed point iteration
	//--------------------------------------------------------------------------
	for(int count=0;count<nOuterFPIterations;count++)
	{
		/*
		// compute the gradient
		getDxs(imdx,imdy,imdt,Im1,warpIm2);

		// generate the mask to set the weight of the pix	els moving outside of the image boundary to be zero
		genInImageMask(mask,vx,vy);

		// set the derivative of the flow field to be zero
		du.reset();
		dv.reset();
		*/
		//--------------------------------------------------------------------------
		// the inner fixed point iteration
		//--------------------------------------------------------------------------
		for(int hh=0;hh<nInnerFPIterations;hh++)
		{
			/*
			// compute the derivatives of the current flow field
			if(hh==0)
			{
			uu.copyData(u);
			vv.copyData(v);
			}
			else
			{
			uu.Add(u,du);
			vv.Add(v,dv);
			}
			uu.dx(ux);
			uu.dy(uy);
			vv.dx(vx);
			vv.dy(vy);

			// compute the weight of phi
			Phi_1st.reset();
			double* phiData=Phi_1st.data();
			double temp;
			const double *uxData,*uyData,*vxData,*vyData;
			uxData=ux.data();
			uyData=uy.data();
			vxData=vx.data();
			vyData=vy.data();
			for(int i=0;i<nPixels;i++)
			{
			temp=uxData[i]*uxData[i]+uyData[i]*uyData[i]+vxData[i]*vxData[i]+vyData[i]*vyData[i];
			phiData[i]=1/(2*sqrt(temp+varepsilon_phi));
			}

			// compute the nonlinear term of psi
			Psi_1st.reset();
			double* psiData=Psi_1st.data();
			const double *imdxData,*imdyData,*imdtData;
			const double *duData,*dvData;
			imdxData=imdx.data();
			imdyData=imdy.data();
			imdtData=imdt.data();
			duData=du.data();
			dvData=dv.data();

			double _a  = 10000, _b = 0.1;
			if(nChannels==1)
			{
			for(int i=0;i<nPixels;i++)
			{
			temp=imdtData[i]+imdxData[i]*duData[i]+imdyData[i]*dvData[i];
			//if(temp*temp<0.04)
			psiData[i]=1/(2*sqrt(temp*temp+varepsilon_psi));
			//psiData[i] = _a*_b/(1+_a*temp*temp);
			}
			}
			else
			{
			for(int i=0;i<nPixels;i++)
			for(int k=0;k<nChannels;k++)
			{
			int offset=i*nChannels+k;
			temp=imdtData[offset]+imdxData[offset]*duData[i]+imdyData[offset]*dvData[i];
			//if(temp*temp<0.04)
			psiData[offset]=1/(2*sqrt(temp*temp+varepsilon_psi));
			//psiData[offset] =  _a*_b/(1+_a*temp*temp);
			}
			}

			// prepare the components of the large linear system
			ImDxy.Multiply(Psi_1st,imdx,imdy);
			ImDx2.Multiply(Psi_1st,imdx,imdx);
			ImDy2.Multiply(Psi_1st,imdy,imdy);
			ImDtDx.Multiply(Psi_1st,imdx,imdt);
			ImDtDy.Multiply(Psi_1st,imdy,imdt);

			if(nChannels>1)
			{
			ImDxy.collapse(imdxy);
			ImDx2.collapse(imdx2);
			ImDy2.collapse(imdy2);
			ImDtDx.collapse(imdtdx);
			ImDtDy.collapse(imdtdy);
			}
			else
			{
			imdxy.copyData(ImDxy);
			imdx2.copyData(ImDx2);
			imdy2.copyData(ImDy2);
			imdtdx.copyData(ImDtDx);
			imdtdy.copyData(ImDtDy);
			}

			// filtering
			imdx2.smoothing(A11,3);
			imdxy.smoothing(A12,3);
			imdy2.smoothing(A22,3);

			// add epsilon to A11 and A22
			A11.Add(alpha*0.1);
			A22.Add(alpha*0.1);

			// form b
			imdtdx.smoothing(b1,3);
			imdtdy.smoothing(b2,3);
			// laplacian filtering of the current flow field
			Laplacian(foo1,u,Phi_1st);
			Laplacian(foo2,v,Phi_1st);
			double *b1Data,*b2Data;
			const double *foo1Data,*foo2Data;
			b1Data=b1.data();
			b2Data=b2.data();
			foo1Data=foo1.data();
			foo2Data=foo2.data();

			for(int i=0;i<nPixels;i++)
			{
			b1Data[i]=-b1Data[i]-alpha*foo1Data[i];
			b2Data[i]=-b2Data[i]-alpha*foo2Data[i];
			}
			*/
			//-----------------------------------------------------------------------
			// conjugate gradient algorithm
			//-----------------------------------------------------------------------
			//r1.copyData(b1);
			//r2.copyData(b2);
			//du.reset();
			//dv.reset();
			//-----------------------------------------------------------------------
			// end of conjugate gradient algorithm
			//-----------------------------------------------------------------------
		}// end of inner fixed point iteration

		/*
		u.Add(du,1);
		v.Add(dv,1);
		warpFL(warpIm2,Im1,Im2,u,v);
		*/
	}// end of outer fixed point iteration




}








/////////////////////////////////////////////////tests///////////////////////////////////////
