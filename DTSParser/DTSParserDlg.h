
// DTSParserDlg.h : header file
//

#pragma once

#include "BitStreamReader.h"
#include "BitStreamWriter.h"

// CDTSParserDlg dialog
class CDTSParserDlg : public CDialogEx
{
// Construction
public:
	CDTSParserDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DTSPARSER_DIALOG };
#endif
	//{{AFX_DATA(CDTSParserDlg)	
	CProgressCtrl	m_ctrlProgress;
	CButton	m_ctrlRebuildStream;
	CButton	m_ctrlRemoveDiag;
	CButton	m_ctrlRemoveCRC;
	CButton	m_ctrlEnableLog;
	CListCtrl	m_batch;
	CStatic	m_outfile_ctrl;
	CStatic	m_frame_ctrl;
	CListCtrl	m_log;
	BOOL	m_swap_word;
	//}}AFX_DATA

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAddDts();
	afx_msg void OnStartBatch();
	afx_msg void OnRebuildStream();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()

private:
	CString GetFileSize(CString filename);
	void CheckSettings();
	void NewEvent(int offset, int frame, CString rate, CString channels, int Framesize, CString dial, int cpf, CString format);
	void ResizeGrid(CListCtrl& grid);
	void PumpMessages();
	void ParseDTS(int batch_entry);
	CString m_filename;
	CString m_outfilename;
	int m_size = 0;
	bool FileOpen(char* filter, CStringList& filelist);
	BitStreamReader m_bitstr;
	BitStreamWriter m_bitwrt;
	bool m_terminate = false;
};
