#include "ObjectFinder.h"
#include "SVM.hpp"

using namespace std;
using namespace cv;

void main()
{
	FILE* SampleColors = NULL;	
	char ImageFileName[100] = {0}, FrameNumber[10] = {0};
	IplImage* Image = 0;
	int FileCount = 0;


	while(true)
	{
		SampleColors = fopen("C:\\Users\\Viavis02\\Desktop\\burak denemeler\\SampleColors.txt", "a");
		strcpy( ImageFileName, "C:\\Users\\Viavis02\\Desktop\\burak denemeler\\111 - 6+\\");	
		sprintf( FrameNumber, "%05i", FileCount);
		strcat( ImageFileName, "High_Res_Focused_");
		strcat( ImageFileName, FrameNumber);
		strcat( ImageFileName, ".jpg");
		
		Image = cvLoadImage( ImageFileName, CV_LOAD_IMAGE_COLOR);
		
		strcpy(ImageFileName, "");

		if( !Image) break;
		fileNameCreate( ImageFileName, HIGH_RES);
		cvSaveImage( ImageFileName, Image, 0);

		findObjects( Image, SampleColors);
		cvReleaseImage( &Image);
		FileCount++;

	fclose( SampleColors);
	}
}