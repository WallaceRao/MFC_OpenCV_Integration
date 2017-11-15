// MatImageDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ColorMapper.h"
#include "MatImageDialog.h"
#include "afxdialogex.h"


// MatImageDialog dialog

IMPLEMENT_DYNAMIC(MatImageDialog, CDialogEx)

MatImageDialog::MatImageDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MAT_IMAGE, pParent)
{

}

MatImageDialog::~MatImageDialog()
{
}

void MatImageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMAGE, mStaticImage);
}


BEGIN_MESSAGE_MAP(MatImageDialog, CDialogEx)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()


// MatImageDialog message handlers

BOOL MatImageDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;
}

// Draw picture control by ourselves
void MatImageDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDCtl = IDC_STATIC_IMAGE)
	{
		RECT r;
		mStaticImage.GetClientRect(&r);
		int width = r.right - r.left;
		int height = r.bottom - r.top;
		mImage.StretchBlt(::GetDC(mStaticImage.m_hWnd), 0, 0, width, height);
	}
	else {
		CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}
