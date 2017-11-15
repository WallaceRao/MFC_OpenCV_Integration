
// ColorMapperDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ColorMapper.h"
#include "ColorMapperDlg.h"
#include "afxdialogex.h"
#include "FalseColorPickerDialog.h"
#include "MatImageDialog.h"
#include "MatCImageConverter.h"
#include <opencv2/opencv.hpp>
#include "DMReader.h"
using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnClose();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CColorMapperDlg dialog



CColorMapperDlg::CColorMapperDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_COLORMAPPER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CColorMapperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, mOutputEdit);
}

BEGIN_MESSAGE_MAP(CColorMapperDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_FALSE_COLOR, &CColorMapperDlg::OnBnClickedButtonFalseColor)
	ON_BN_CLICKED(IDC_BUTTON_HSB_COLOR, &CColorMapperDlg::OnBnClickedButtonHsbColor)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_PICKER, &CColorMapperDlg::OnBnClickedButtonColorPicker)
//	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_MAT_IMAGE, &CColorMapperDlg::OnBnClickedButtonMatImage)
	ON_BN_CLICKED(IDC_BUTTON_DM_FILE_READER, &CColorMapperDlg::OnBnClickedButtonDmFileReader)
END_MESSAGE_MAP()


// CColorMapperDlg message handlers

BOOL CColorMapperDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CColorMapperDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CColorMapperDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR CColorMapperDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Show false color scales for choosing
void CColorMapperDlg::OnBnClickedButtonFalseColor()
{
	destroyAllWindows();
	FalseColorPickerDialog dialog;
	dialog.DoModal();
	int chosenColorScale = dialog.getSelectedColorScaleIndex();
	// Output result for user
	CString message;
	message.Format(_T("You have chosen number %d false color scale.\n"), chosenColorScale);
	mOutputEdit.SetWindowTextW(message);
}

// Combine two one-channel mat to one HSV image
BOOL combineHSV(Mat intensity, Mat angle, Mat &hsvImag)
{
	hsvImag = Mat(intensity.size(), CV_8UC3, Scalar(0, 0, 255));
	// Check whether input mats have one channel, and the size is same.
	if (intensity.channels() != 1 || angle.channels() != 1)
		return FALSE;
	if (intensity.size() != angle.size())
		return FALSE;

	// We assume the type of input mats should be CV_32FC1, if not, convert them
	Mat cloneIntensity = intensity.clone();
	Mat cloneAngle = angle.clone();
	if(cloneIntensity.type() != CV_32FC1)
		cloneIntensity.convertTo(cloneIntensity, CV_32FC1);
	if(cloneAngle.type() != CV_32FC1)
		cloneAngle.convertTo(cloneAngle, CV_32FC1);
	
	// In OpenCV, Hue ranges from 0 to 180, Saturation and Brightness range from 0 to 255. 
	Mat normIntensity, normAngle;
	normalize(cloneIntensity, normIntensity, 0, 255, CV_MINMAX);
	normalize(cloneAngle, normAngle, 0, 180, CV_MINMAX);
	int w = hsvImag.cols;
	int h = hsvImag.rows;
	for (int j = 0; j < w; j++)
	{
		for (int i = 0; i < h; i++)
		{
			hsvImag.at<Vec3b>(i, j)[0] = (uchar)normAngle.at<float>(i, j);
			hsvImag.at<Vec3b>(i, j)[1] = (uchar)normIntensity.at<float>(i, j); // Pick the surface of the HSV Cone.
			hsvImag.at<Vec3b>(i, j)[2] = (uchar)normIntensity.at<float>(i, j);
		}
	}
	return TRUE;
}

// Perform Fourier transform of a 2D image, combine intensity and phase angle in one image for display
void CColorMapperDlg::OnBnClickedButtonHsbColor()
{
	destroyAllWindows();
	CString message;
	Mat grayImage = imread("res\\Lenna.png", IMREAD_GRAYSCALE);						
	if (grayImage.empty())
	{
		message.Format(_T("Unable to load Lenna.png \n"));
		mOutputEdit.SetWindowTextW(message);
		return;
	}
	Mat padded; //Expand input image to optimal size            
	int m = getOptimalDFTSize(grayImage.rows);
	int n = getOptimalDFTSize(grayImage.cols);
	// On the border add zero values
	copyMakeBorder(grayImage, padded, 0, m - grayImage.rows, 0, n - grayImage.cols, BORDER_CONSTANT, Scalar::all(0));

	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(),CV_32F) };
	Mat complexI;
	// Add to the expanded another plane with zeros
	merge(planes, 2, complexI);  
	// Fourier transform
	dft(complexI, complexI);  
	// Compute the magnitude and switch to logarithmic scale
	split(complexI, planes);   							
	magnitude(planes[0], planes[1], planes[0]);     
	for (int i = 0; i < 2; i++)
	{
		Mat planeImage = planes[i];
		// Switch to logarithmic scale
		planeImage += Scalar::all(1);
		log(planeImage, planeImage);
		// Rearrange the quadrants of Fourier image  so that the origin is at the image center
		planeImage = planeImage(Rect(0, 0, planeImage.cols&-2, planeImage.rows&-2));
		int cx = planeImage.cols / 2;
		int cy = planeImage.rows / 2;
		Mat q0(planeImage, Rect(0, 0, cx, cy));
		Mat q1(planeImage, Rect(cx, 0, cx, cy));
		Mat q2(planeImage, Rect(0, cy, cx, cy));
		Mat q3(planeImage, Rect(cx, cy, cx, cy));
		Mat tmp;
		q0.copyTo(tmp);
		q3.copyTo(q0);
		tmp.copyTo(q3);
		q1.copyTo(tmp);
		q2.copyTo(q1);
		tmp.copyTo(q2);
		normalize(planeImage, planeImage, 0, 1, CV_MINMAX);
	}
	
	Mat HSVImage;
	if (!combineHSV(planes[0], planes[1], HSVImage))
	{
		// Output error, can not combine inputs to one HSV image
		message.Format(_T("Unable to generate HSV image.\n"));
		mOutputEdit.SetWindowTextW(message);
		return;
	}
	Mat BGRImage;
	cvtColor(HSVImage, BGRImage, CV_HSV2BGR);
	imshow("Original Gray Image", grayImage);
	imshow("Combined Image after Fourier Transform", BGRImage);

	// Output result for user
	message.Format(_T("Succeed to generate HSV image.\n"));
	mOutputEdit.SetWindowTextW(message);

	waitKey(0);
	destroyAllWindows();
}

// Show color picker dialog used for scale bar
void CColorMapperDlg::OnBnClickedButtonColorPicker()
{
	destroyAllWindows();
	CColorDialog m_setClrDlg;
    m_setClrDlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;  
    m_setClrDlg.m_cc.rgbResult = mPickedColor; // Pre_select the last decision
	if (IDOK == m_setClrDlg.DoModal())
	{
		mPickedColor = m_setClrDlg.m_cc.rgbResult;
		// Output result for user
		BYTE Red = GetRValue(mPickedColor);
		BYTE Green = GetGValue(mPickedColor);
		BYTE Blue = GetBValue(mPickedColor);
		CString message;
		message.Format(_T("You have chosen color with RGB(%d, %d, %d) for scale bar.\n"), Red, Green, Blue);
		mOutputEdit.SetWindowTextW(message);
	}
	else {
		CString message;
		message.Format(_T("You did not choose any color\n"));
		mOutputEdit.SetWindowTextW(message);
	}
}

// Close dialog and all opencv windows
void CColorMapperDlg::OnClose()
{
	destroyAllWindows();
	CDialogEx::OnClose();
}




void CColorMapperDlg::OnBnClickedButtonMatImage()
{
	// TODO: Add your control notification handler code here
	Mat mat = imread("res\\Lenna.png");
	CString message;
	if (mat.empty())
	{
		// Output error, can not load Lenna.png.
		message.Format(_T("Can not load Lenna.png\n"));
		mOutputEdit.SetWindowTextW(message);
		return;
	}
	MatCImageConverter converter;
	CImage image;
	converter.MatToCImage(mat, image);
	MatImageDialog dialog;
	dialog.mImage = image;
	message.Format(_T("Show OpenCV Mat in MFC dialog\n"));
	mOutputEdit.SetWindowTextW(message);
	dialog.DoModal();
}


// Calculate a suitable scale, the scale bar should be shorter than 200 pixels.
long getSuitableScaleLength(float scaleRow, float &nm)
{
	nm = 2.0;
	long pixels = (long)(nm / scaleRow);
	while (pixels > 200)
	{
		pixels /= 10.0;
		nm /= 10.0;
	}
	return pixels;
}



// Add scale bar  
void addScaleBar(Mat &mat, float scaleRow)
{
	float nm;
	long pixels = getSuitableScaleLength(scaleRow, nm);
	char str[25];
	snprintf(str, sizeof(str), "%.2f nm", nm);
	putText(mat, str, cvPoint(10, 30), CV_FONT_HERSHEY_SIMPLEX, 1.0f, CV_RGB(255, 255, 255), 2);
	line(mat, cvPoint(10, 50), cvPoint(10 + pixels, 50), CV_RGB(255, 255, 255), 3);
}


// Scale dm image with any ratio and add scale bar 
void resizeDMImageWithScaleBar(Mat &mat, float scaleRow, float scaleRatio)
{
	Size oldSize = mat.size();
	Size newSize;
	newSize.width = oldSize.width * scaleRatio;
	newSize.height = oldSize.height * scaleRatio;
	scaleRow /= scaleRatio;
	resize(mat, mat, newSize);
	addScaleBar(mat, scaleRow);

}

// DM reader demo
void CColorMapperDlg::OnBnClickedButtonDmFileReader()
{
	// TODO: Add your control notification handler code here
	DMReader reader[2];
	reader[0].readDMfile("res\\1.dm3");
	reader[1].readDMfile("res\\1.dm4");
	
	Mat image[2];
	for (int i = 0; i < 2; i++)
	{
		Size size;
		reader[i].getImageDimension(size);
		image[i] = Mat::zeros(size, CV_32F);
		reader[i].getImageData(image[i]);
		normalize(image[i], image[i], 0, 255, CV_MINMAX);
		image[i].convertTo(image[i], CV_8U);

		float scaleRow = reader[i].getScaleRow();
		resizeDMImageWithScaleBar(image[i], scaleRow, 0.5);
	}

	imshow("Scaled DM3 Image", image[0]);
	imshow("Scaled DM4 Image", image[1]);
	waitKey(0);
	destroyAllWindows();
}
