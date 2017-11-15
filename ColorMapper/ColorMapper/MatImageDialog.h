#pragma once
#include "afxwin.h"

// MatImageDialog dialog

class MatImageDialog : public CDialogEx
{
	DECLARE_DYNAMIC(MatImageDialog)

public:
	MatImageDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~MatImageDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MAT_IMAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CStatic mStaticImage;
	CImage mImage;
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};
