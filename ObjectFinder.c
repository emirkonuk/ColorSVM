#include "ObjectFinder.h"

void findObjects( IplImage *Image, FILE *text)
{
	double Sdv, Mean;
	CvScalar MeanScalar;
	CvScalar StandardDeviationScalar;
	IplImage *Lab, *LabOriginal = NULL, *ImageOriginal = NULL, *L = NULL, *a = NULL, *b = NULL, *Binary = NULL;
	char BinaryFileName[100] = {0};
	int i=0, Area=0;

	IplConvKernel *convKernel = cvCreateStructuringElementEx( 5, 5, 2, 2, CV_SHAPE_ELLIPSE, NULL);

	cvNamedWindow( "Image", 0);
	cvShowImage( "Image", Image);
	cvWaitKey(33);

	Lab = cvCreateImage(cvGetSize(Image), Image->depth, 3);
	L = cvCreateImage(cvGetSize(Image), Image->depth, 1);
	a = cvCreateImage(cvGetSize(Image), Image->depth, 1);
	b = cvCreateImage(cvGetSize(Image), Image->depth, 1);
	Binary = cvCreateImage(cvGetSize(Image), Image->depth, 1);

	// convert to CIE*Lab color space for better focusing
	cvCvtColor( Image, Lab, CV_BGR2Lab);
	LabOriginal = cvCloneImage( Lab);
	ImageOriginal = cvCloneImage( Image);
	cvSplit( Lab, L, a, b, NULL);
	cvSet( L, cvScalar(223,0,0,0),0);
	cvMerge( L, a, b, NULL, Lab);
	cvCvtColor( Lab, Image, CV_Lab2BGR);
	cvCvtColor( Image, Lab, CV_BGR2Lab);
	cvSplit( Lab, L, a, b, NULL);

	// Noise reduction
	cvMorphologyEx( a, a, NULL, convKernel, CV_MOP_OPEN, 1);
	
	// choose the 2nd channel as the red color is more prominent in this channel,
	cvAvgSdv( a, &MeanScalar, &StandardDeviationScalar,0) ;
	Sdv = StandardDeviationScalar.val[0];
	Mean = MeanScalar.val[0];

	// Threshold such that only a small number of outliers remain,
	cvThreshold( a, Binary, Mean + 6*Sdv, 255, CV_THRESH_BINARY);

	fileNameCreate( BinaryFileName, BINARY);
	cvSaveImage( BinaryFileName, Binary, 0);

	//cvNamedWindow("Binary", 0);
	//cvShowImage("Binary", Binary);
	//cvWaitKey(1);

	contourFinder( ImageOriginal, Binary, LabOriginal, text);

	cvReleaseImage( &Lab);
	cvReleaseImage( &LabOriginal);
	cvReleaseImage( &ImageOriginal);
	cvReleaseImage( &L);
	cvReleaseImage( &a);
	cvReleaseImage( &b);
	cvReleaseImage( &Binary);
	cvReleaseStructuringElement( &convKernel);

}
double colorDistance( CvScalar Color, double a, double b)
{
	return sqrt((a - Color.val[1]) * (a - Color.val[1]) + (b - Color.val[2]) * (b - Color.val[2]));
}
void contourFinder( IplImage *Image, IplImage* Binary, IplImage* LabOriginal, FILE *SampleColors)
{
	char ContourFileName[100] = {0};
	int i = 0;
	IplImage* AreaBased = cvCreateImage( cvGetSize(Binary), 8, 1 );
	IplImage* ColorBased = cvCreateImage( cvGetSize(Binary), 8, 3 );
	IplImage* ColorCompare = cvCreateImage( cvSize(256,256), 8, 1 );
	CvSeq* ColorContour = 0;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contour = 0, *smoothedcontour = 0;
	IplConvKernel *convKernel = cvCreateStructuringElementEx( 5, 5, 2, 2, CV_SHAPE_ELLIPSE, NULL);
		
	//cvMorphologyEx( Binary, Binary, NULL, convKernel, CV_MOP_OPEN, 1);
	cvDilate( Binary, Binary, convKernel, 1);
	//cvMorphologyEx( Binary, Binary, NULL, convKernel, CV_MOP_OPEN, 1);
	
	// Color comparing tools
	cvZero( ColorCompare);
	cvEllipse( ColorCompare, cvPoint(128, 121), cvSize(13, 6), -75, 0, 360, cvScalar(255, 255, 255,0), -1, 8, 0);
	cvEllipse( ColorCompare, cvPoint(133, 95), cvSize(15, 6), -75, 0, 360, cvScalar(255, 255, 255,0), -1, 8, 0);
	cvFindContours( ColorCompare, storage, &ColorContour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
	cvZero( ColorCompare);
	cvDrawContours( ColorCompare, ColorContour, cvScalar(255, 255, 255, 0), cvScalar(255, 255, 255, 0), -1, CV_FILLED, 8, cvPoint(0,0));
	cvNamedWindow("Color thingy", 0);
	cvShowImage("Color thingy", ColorCompare);
	cvWaitKey(0);

    cvFindContours( Binary, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
	
	cvZero( AreaBased);
	cvZero( ColorBased);

    for( ; contour != 0; contour = contour->h_next )
    {
		CvMoments Moments;
		double eccentricity = 0, Distance = 0;
		double mu20, mu02, mu11, lambda1, lambda2;
		CvScalar ColorMeans = {0};
		CvRect BoundingRect;
		IplImage* Highlighted=0;
		smoothedcontour = cvApproxPoly( contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 3, 1 );

		cvMoments( smoothedcontour, &Moments, 0);		
		mu20=Moments.mu20 / pow( Moments.m00, 2);
		mu02=Moments.mu02 / pow( Moments.m00, 2);
		mu11=Moments.mu11 / pow( Moments.m00, 2);
		lambda1=(0.5*(mu20 + mu02) + 0.5*sqrt(fabs((4*mu11*mu11)-(mu20 - mu02)*(mu20 - mu02))));
		lambda2=(0.5*(mu20 + mu02) - 0.5*sqrt(fabs((4*mu11*mu11)-(mu20 - mu02)*(mu20 - mu02))));
		eccentricity = sqrt(fabs(1- lambda2/lambda1));
		if(eccentricity >= 1) eccentricity = 0;
		// center of mass inside the object?

		//  Moments.m00 is the area
		if( Moments.m00 > 100)
		{   /* replace CV_FILLED with 1 to see the outlines */
			cvDrawContours( AreaBased, smoothedcontour, cvScalar(255, 255, 255, 0), cvScalar(255, 255, 255, 0), -1, CV_FILLED, 8, cvPoint(0,0));
			printf(" Area is %f\n", Moments.m00);
			printf(" position is x = %d and y = %d\n", (int)(Moments.m10 / Moments.m00), (int)(Moments.m01 / Moments.m00));
			printf(" Eccentricity is %f\n", eccentricity);

			ColorMeans = cvAvg( LabOriginal, AreaBased);
			printf("Mean values are %f %f %f\n", ColorMeans.val[0], ColorMeans.val[1], ColorMeans.val[2] );

			// if object's mean color is inside a predefined ColorContour
			if( !(cvPointPolygonTest( ColorContour, cvPoint2D32f( ColorMeans.val[1], ColorMeans.val[2]), 0) == -1))
				cvDrawContours( ColorBased, smoothedcontour, cvScalar(255, 255, 255, 0), cvScalar(255, 255, 255, 0), -1, CV_FILLED, 8, cvPoint(0,0));
			
			/*
			// burasý highlight iþi yapýyor

			// Moments.m10/Moments.m00 ve  Moments.m01/Moments.m00 objenin merkez noktalarý
			//bunlarý içine alan bir dörtgen belirliyorum, boyu 512*512
			BoundingRect = cvRect( Moments.m10/Moments.m00 - 256, Moments.m01/Moments.m00 - 256, 512, 512);// bu 512iþi sakat, dörtgen resmin dýþýna taþarsa falan hata verir, onun kontrolünü yok

			// resimlerin sadece bu dörtgen içindeki yerleri ile ilgilenicez, ROI: region of interest
			cvSetImageROI( Image, BoundingRect);
			cvSetImageROI( AreaBased, BoundingRect);

			// bir pencere aç, boyunu belirle
			cvNamedWindow("object", 0);
			cvResizeWindow("object", 512,512);

			// bir resim yarat
			Highlighted=cvCreateImage( cvSize( 512,512), 8, 3 );

			// bu resimdeki sadece objenin olduðu bölgeyi beyaz þle doldur
			cvAddS( Image, cvScalar(255,255,255,0), Highlighted, AreaBased);


			// 10 kere orjinali ve highlighted'ý sýrayla göster
			for(i=0; i < 10; i++)
			{
				cvShowImage("object", Highlighted);
				cvWaitKey(1);
				cvShowImage("object", Image);
				cvWaitKey(100);
			}	
			// temizlik
			cvResetImageROI( Image);
			cvResetImageROI( AreaBased);
			cvReleaseImage( &Highlighted);

			// bitti
			*/
			//fprintf( SampleColors, "{%08.4f, %08.4f}, %c\n", ColorMeans.val[1], ColorMeans.val[2], cvWaitKey(0));
			
		}
    }
	cvNamedWindow("Area", 0);
	cvShowImage("Area", AreaBased);
	cvWaitKey(1);
	cvNamedWindow("Color", 0);
	cvShowImage("Color", ColorBased);
	cvWaitKey(0);

	fclose( SampleColors);
	fileNameCreate( ContourFileName, CONTOUR);
	cvSaveImage( ContourFileName, ColorBased, 0);
 

	cvDestroyAllWindows();
	cvReleaseImage( &AreaBased);
	cvReleaseImage( &ColorBased);
	cvReleaseStructuringElement( &convKernel);
	cvClearMemStorage(storage);
	cvReleaseMemStorage( &storage);
}

