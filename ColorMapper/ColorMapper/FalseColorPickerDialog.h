#pragma once
#include "afxcmn.h"


// ColorPickerDialog dialog

class FalseColorPickerDialog : public CDialogEx
{
	DECLARE_DYNAMIC(FalseColorPickerDialog)

public:
	FalseColorPickerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~FalseColorPickerDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FALSE_COLOR_SELECTION};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void InitColorList();
	void InitImageList(int &bmpWidth);
	virtual BOOL OnInitDialog();
	int mSelectedColorScale;

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listColor;
	CImageList m_imList;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	int getSelectedColorScaleIndex();
};
