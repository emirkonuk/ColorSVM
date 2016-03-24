#include <stdio.h>
#include <conio.h>
#include <windows.h>

#define LOW_RES_IMAGE_FOLDER_NAME "C:\\Users\\Emir\\Google Drive\\Mikroskop-Tüm Sistem\\Trials\\"
#define HIGH_RES_IMAGE_FOLDER_NAME "C:\\Users\\Viavis02\\Desktop\\SVM training thrash\\"
#define LOW_RES_IMAGE_NAME "Low_Res_"
#define HIGH_RES_IMAGE_NAME "High_Res_Focused_"
#define BINARY_IMAGE_NAME "High_Res_Focused_"
#define LOW_RES 1
#define HIGH_RES 0
#define BINARY -1
#define CONTOUR -2

extern "C"{
	int fileCountInFolder( char *FolderName);
	void fileNameCreate( char *ImageFileName, int Resolution);
}