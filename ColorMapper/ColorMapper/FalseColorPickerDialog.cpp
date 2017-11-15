// ColorPickerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ColorMapper.h"
#include "FalseColorPickerDialog.h"
#include "afxdialogex.h"
#include "string.h"
#include <opencv2/opencv.hpp>

using namespace cv;
#define COLORMAP_COUNT 12

/* 
	The color map we support:
	COLORMAP_AUTUMN = 0,
	COLORMAP_BONE = 1,
	COLORMAP_JET = 2,
	COLORMAP_WINTER = 3,
	COLORMAP_RAINBOW = 4,
	COLORMAP_OCEAN = 5,
	COLORMAP_SUMMER = 6,
	COLORMAP_SPRING = 7,
	COLORMAP_COOL = 8,
	COLORMAP_HSV = 9,
	COLORMAP_PINK = 10,
	COLORMAP_HOT = 11

	Our bitmap resource files follow this order, too.
*/


IMPLEMENT_DYNAMIC(FalseColorPickerDialog, CDialogEx)


FalseColorPickerDialog::FalseColorPickerDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_FALSE_COLOR_SELECTION, pParent)
{
	mSelectedColorScale = 0;
}

FalseColorPickerDialog::~FalseColorPickerDialog()
{
}

void FalseColorPickerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_COLOR, m_listColor);
}

// Init image list for displaying color scale
void FalseColorPickerDialog::InitImageList(int &bmpWidth)
{
	m_imList.DeleteImageList();
	CBitmap pBmp[12];
	pBmp[0].LoadBitmapW(IDB_BITMAP_0);
	pBmp[1].LoadBitmapW(IDB_BITMAP_1);
	pBmp[2].LoadBitmapW(IDB_BITMAP_2);
	pBmp[3].LoadBitmapW(IDB_BITMAP_3);
	pBmp[4].LoadBitmapW(IDB_BITMAP_4);
	pBmp[5].LoadBitmapW(IDB_BITMAP_5);
	pBmp[6].LoadBitmapW(IDB_BITMAP_6);
	pBmp[7].LoadBitmapW(IDB_BITMAP_7);
	pBmp[8].LoadBitmapW(IDB_BITMAP_8);
	pBmp[9].LoadBitmapW(IDB_BITMAP_9);
	pBmp[10].LoadBitmapW(IDB_BITMAP_10);
	pBmp[11].LoadBitmapW(IDB_BITMAP_11);
	BITMAP bm;
	pBmp[0].GetObject(sizeof(BITMAP), &bm);
	m_imList.Create(bm.bmWidth, bm.bmHeight, ILC_COLOR24, 10, 20);
	bmpWidth = bm.bmWidth;
	// Add all bmps
	for (int i = 0; i < COLORMAP_COUNT; i++)
	{
		m_imList.Add(&pBmp[i], RGB(0, 0, 0));
	}
}

// Init color list for displaying color mappers
void FalseColorPickerDialog::InitColorList(void)
{
	DWORD dwStyle;
	dwStyle = m_listColor.GetExtendedStyle();
	dwStyle = dwStyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES;
	m_listColor.SetExtendedStyle(dwStyle);

	int bmpWidth;
	InitImageList(bmpWidth);
	m_listColor.SetImageList(&m_imList, LVSIL_SMALL);

	CRect mRect;
	m_listColor.GetWindowRect(&mRect);
	int length = mRect.Width();
	m_listColor.InsertColumn(0, _T("Scale"), LVCFMT_LEFT, bmpWidth, -1);
	m_listColor.InsertColumn(1, _T("Class"), LVCFMT_LEFT, length - bmpWidth - 4, -1);

	// Init color scale names
	CString color_map_names[12];
	color_map_names[0] = CString(_T("Autumn"));
	color_map_names[1] = CString(_T("Bone"));
	color_map_names[2] = CString(_T("Jet"));
	color_map_names[3] = CString(_T("Winter"));
	color_map_names[4] = CString(_T("Rainbow"));
	color_map_names[5] = CString(_T("Ocean"));
	color_map_names[6] = CString(_T("Summer"));
	color_map_names[7] = CString(_T("Spring"));
	color_map_names[8] = CString(_T("Cool"));
	color_map_names[9] = CString(_T("HSV"));
	color_map_names[10] = CString(_T("Pink"));
	color_map_names[11] = CString(_T("Hot"));
	
	// Insert color scales to list control
	int nRow;
	LVITEM lvItem = { 0 };
	for (int i = 0; i < COLORMAP_COUNT; i++)
	{
		lvItem.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_STATE;
		lvItem.iItem = i;
		lvItem.iImage = i;
		lvItem.iSubItem = 0;
		nRow = m_listColor.InsertItem(&lvItem);
		m_listColor.SetItemText(nRow, 1, color_map_names[i]);
	}
	m_listColor.SetItemState(mSelectedColorScale, LVIS_SELECTED, LVIS_SELECTED);
}

BEGIN_MESSAGE_MAP(FalseColorPickerDialog, CDialogEx)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_TEST, &FalseColorPickerDialog::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDOK, &FalseColorPickerDialog::OnBnClickedOk)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// ColorPickerDialog message handlers

int FalseColorPickerDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}


// Init Dialog
BOOL FalseColorPickerDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitColorList();
	return TRUE;
}

//Test with the selected color mapper
void FalseColorPickerDialog::OnBnClickedButtonTest()
{
	Mat img = imread("res\\Lenna.png");
	if (img.empty())
	{
		// Output error, can not load Lenna.png.
		return;
	}
	Mat grayImg, reColoredImg;
	cvtColor(img, grayImg, CV_BGR2GRAY);
	
	int selected_index = -1;
	if ((selected_index = m_listColor.GetNextItem(selected_index, LVNI_SELECTED)) != -1)
	{
		applyColorMap(grayImg, reColoredImg, selected_index);
		imshow("Color_Scaled Image", reColoredImg);
	}
	else {
		// Output error, no color scale is selected.
		return;
	}
	imshow("Gray Image", grayImg);
	waitKey(0);
	destroyAllWindows();
}
 
// Save selected color class and close dialog
void FalseColorPickerDialog::OnBnClickedOk()
{
	int selected_index = -1;
	if ((selected_index = m_listColor.GetNextItem(selected_index, LVNI_SELECTED)) != -1)
	{
		mSelectedColorScale = selected_index;
	}
	destroyAllWindows();
	CDialogEx::OnOK();
}

// Close dialog and all OpenCV Windows
void FalseColorPickerDialog::OnClose()
{
	destroyAllWindows();
	CDialogEx::OnClose();
}

// Return selected color scale
int FalseColorPickerDialog::getSelectedColorScaleIndex()
{
	return mSelectedColorScale;
}