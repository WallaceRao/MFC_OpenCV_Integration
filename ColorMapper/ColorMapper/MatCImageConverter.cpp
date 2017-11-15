#include "stdafx.h"
#include "MatCImageConverter.h"


MatCImageConverter::MatCImageConverter()
{
}


MatCImageConverter::~MatCImageConverter()
{
}


void MatCImageConverter::MatToCImage(Mat& mat, CImage& cimage)
{
	if (0 == mat.total())
	{
		return;
	}
	int nChannels = mat.channels();
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = mat.cols;
	int nHeight = mat.rows;

	// rebuild cimage  
	cimage.Destroy();
	cimage.Create(nWidth, nHeight, 8 * nChannels);

	//Copy data
	uchar* pucRow;                                  // point to the row of data 
	uchar* pucImage = (uchar*)cimage.GetBits();     // point to data  
	int nStep = cimage.GetPitch();                  // the bytes of each row 
	if (1 == nChannels)
	{
		RGBQUAD* rgbquadColorTable;
		int nMaxColors = 256;
		rgbquadColorTable = new RGBQUAD[nMaxColors];
		cimage.GetColorTable(0, nMaxColors, rgbquadColorTable);
		for (int nColor = 0; nColor < nMaxColors; nColor++)
		{
			rgbquadColorTable[nColor].rgbBlue = (uchar)nColor;
			rgbquadColorTable[nColor].rgbGreen = (uchar)nColor;
			rgbquadColorTable[nColor].rgbRed = (uchar)nColor;
		}
		cimage.SetColorTable(0, nMaxColors, rgbquadColorTable);
		delete[]rgbquadColorTable;
	}

	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (mat.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				*(pucImage + nRow * nStep + nCol) = pucRow[nCol];
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
					*(pucImage + nRow * nStep + nCol * 3 + nCha) = pucRow[nCol * 3 + nCha];
			}
		}
	}
}

void MatCImageConverter::CImageToMat(CImage& cimage, Mat& mat)
{
	if (true == cimage.IsNull())
	{
		return;
	}

	int nChannels = cimage.GetBPP() / 8;
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = cimage.GetWidth();
	int nHeight = cimage.GetHeight();

	// rebuild mat  
	if (1 == nChannels)
		mat.create(nHeight, nWidth, CV_8UC1);
	else if (3 == nChannels)
		mat.create(nHeight, nWidth, CV_8UC3);
	// copy data
	uchar* pucRow;                                  // point to data row  
	uchar* pucImage = (uchar*)cimage.GetBits();     // point to data  
	int nStep = cimage.GetPitch();                  // byte count of each row 
	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (mat.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				pucRow[nCol] = *(pucImage + nRow * nStep + nCol);
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
					pucRow[nCol * 3 + nCha] = *(pucImage + nRow * nStep + nCol * 3 + nCha);
			}
		}
	}
}