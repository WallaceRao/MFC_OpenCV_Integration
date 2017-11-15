
// ColorMapperDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CColorMapperDlg dialog
class CColorMapperDlg : public CDialogEx
{
// Construction
public:
	CColorMapperDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COLORMAPPER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	COLORREF mPickedColor;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButtonFalseColor();
	afx_msg void OnBnClickedButtonHsbColor();
	afx_msg void OnBnClickedButtonColorPicker();
//	afx_msg void OnDestroy();
	afx_msg void OnClose();
	CEdit mOutputEdit;
	afx_msg void OnBnClickedButtonMatImage();
	afx_msg void OnBnClickedButtonDmFileReader();
};
