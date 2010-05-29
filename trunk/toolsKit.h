#pragma once

#include <cv.h>
#include <highgui.h>

#include <stdio.h>
#include <stdarg.h>
#include "SparseToolKit.h"
#include "IplImageIterator.h"

	class toolsKit
	{
	public:
		toolsKit();
		static void cvShowManyImages(char* title, int nArgs, ...);
		template <class PEL>
		static void IPLsqrt_mul2(IplImageIterator<PEL> it){
			while (!it) {      
				*it= 1/(2*sqrt((double)*it)); 
				++it;
			}
		}
		template <class PEL>
		static void IPL_mul_inverse_loop(IplImageIterator<PEL> it){
			double one=1;
			while (!it) {      
				if(0!=((double)*it))
					*it= one/((double)*it); 
				++it;
			}
		}
		static void IPL_mul_inverse(IplImage* img,int opType);
		static void IPL_add(IplImage* img,IplImage* img2,IplImage* dest);	
		static void IPL_print(IplImage *image);
		static void cvMulScalar(IplImage* img,double scalar);
		static void opt_flow_lk();
		virtual ~toolsKit(void);


	};
