#include "SVM.hpp"


CvSVM colorTrain()
{
	int width = 256, height = 256;
    Mat Image = Mat::zeros(height, width, CV_8UC3);

	double ColorLabels[10] = {1.0, -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, -1.0};
	Mat LabelsMat( 10, 1, CV_32FC1, ColorLabels);

	double TrainingColors[10][2] = {{127, 127}, {245, 10}, {127, 10}, {140, 127}, {10, 245},
									{117, 117}, {215, 10}, {117, 10}, {110, 127}, {10, 215}};
	Mat TrainingMat( 10, 2, CV_32FC1, TrainingColors);

	CvSVMParams params;
    params.svm_type    = CvSVM::C_SVC;
	params.kernel_type = CvSVM::RBF;
    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
	params.degree = 3;
	params.gamma = 0.0025;
	params.coef0 = 0;
	params.C = 10000;
	params.nu = 0.5;
	params.p = 0.1;
	params.class_weights = NULL;
	
	CvSVM SVM;
	SVM.train( TrainingMat, LabelsMat, Mat(), Mat(), params);

	Vec3b red (0,0,255), blue (255,0,0);
	for (int i = 0; i < Image.rows; ++i)
        for (int j = 0; j < Image.cols; ++j)
        {
            Mat sampleMat = (Mat_<float>(1,2) << i,j);
            float response = SVM.predict(sampleMat);

            if (response == RED)
                Image.at<Vec3b>(j, i)  = red;
            else if (response == BLUE) 
                 Image.at<Vec3b>(j, i)  = blue;
        }
		
	imwrite("result.png", Image);
	imshow("SVM Simple Example", Image);
	waitKey(0);

	return SVM;
}