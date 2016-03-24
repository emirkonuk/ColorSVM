#include "FileHandling.h"

int fileCountInFolder( char *FolderName)
{
	int FileCount = 0;
	HANDLE hFind = NULL;
	WIN32_FIND_DATA FindFileData;
		
	if( (hFind = FindFirstFile( FolderName, &FindFileData)) != INVALID_HANDLE_VALUE)
	{
		FileCount++;
		while( FindNextFile(hFind, &FindFileData))
			FileCount++;
	}
	FindClose( hFind);

	return FileCount;
}


void fileNameCreate( char *ImageFileName, int Resolution)
{
	char ImageFolderName[100] = {0}, FrameNumber[10] = {0};
	int FileCount = 0; //number of image files in the save folder

	if( Resolution == HIGH_RES)
	{
		strcpy( ImageFolderName, HIGH_RES_IMAGE_FOLDER_NAME);
		strcat( ImageFolderName, "*.jpg");
		FileCount = fileCountInFolder( ImageFolderName);

		strcpy( ImageFileName, HIGH_RES_IMAGE_FOLDER_NAME);	
		sprintf( FrameNumber, "%05i", FileCount/3);
		strcat( ImageFileName, HIGH_RES_IMAGE_NAME);
		strcat( ImageFileName, FrameNumber);
		strcat( ImageFileName, ".jpg");
	}
	else if( Resolution == LOW_RES)
	{
		strcpy( ImageFolderName, LOW_RES_IMAGE_FOLDER_NAME);
		strcat( ImageFolderName, "*.jpg");
		FileCount = fileCountInFolder( ImageFolderName);

		strcpy( ImageFileName, LOW_RES_IMAGE_FOLDER_NAME);	
		sprintf( FrameNumber, "%05i", FileCount/3);
		strcat( ImageFileName, LOW_RES_IMAGE_NAME);
		strcat( ImageFileName, FrameNumber);
		strcat( ImageFileName, ".jpg");
	}
	else if( Resolution == BINARY)
	{
		strcpy( ImageFolderName, HIGH_RES_IMAGE_FOLDER_NAME);
		strcat( ImageFolderName, "*.jpg");
		FileCount = fileCountInFolder( ImageFolderName);

		strcpy( ImageFileName, HIGH_RES_IMAGE_FOLDER_NAME);	
		sprintf( FrameNumber, "%05i", FileCount/3);
		strcat( ImageFileName, BINARY_IMAGE_NAME);
		strcat( ImageFileName, FrameNumber);
		strcat( ImageFileName, "_b.jpg");
	}
	else if( Resolution == CONTOUR)
	{
		strcpy( ImageFolderName, HIGH_RES_IMAGE_FOLDER_NAME);
		strcat( ImageFolderName, "*.jpg");
		FileCount = fileCountInFolder( ImageFolderName);

		strcpy( ImageFileName, HIGH_RES_IMAGE_FOLDER_NAME);	
		sprintf( FrameNumber, "%05i", FileCount/3);
		strcat( ImageFileName, BINARY_IMAGE_NAME);
		strcat( ImageFileName, FrameNumber);
		strcat( ImageFileName, "_c.jpg");
	}
}