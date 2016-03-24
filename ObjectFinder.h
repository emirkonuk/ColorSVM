#include "FileHandling.h"
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>

extern "C"{
	void findObjects( IplImage *Image, FILE *SampleColors);
	void contourFinder( IplImage* Image, IplImage* Binary, IplImage* LabOriginal, FILE *SampleColors);
	double colorDistance( CvScalar Color, double a, double b);
}