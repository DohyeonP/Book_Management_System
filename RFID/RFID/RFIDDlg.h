
// RFIDDlg.h: 헤더 파일
//

#pragma once
#include "is_d2xx.h"

#include <vector>

// DB
#include <mysql.h>

#define MESSAGE_RENTAL_BOOK WM_USER

// CRFIDDlg 대화 상자
class CRFIDDlg : public CDialogEx
{
// 생성입니다.
public:
	CRFIDDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	~CRFIDDlg();
	IS_HANDLE ftHandle = 0;
	char readSerialNumber[100] = "COM07";
	char changeSerialNumber[100] = "RFID01";
	short usbnumber;
	unsigned char wirteData[1024];
	unsigned short writeLength = 0;
	unsigned char readData[1024];
	unsigned short readLength = 0;

	// Thread
	CWinThread* m_pThread;
	BOOL m_ThreadFlag;

	// Image
	CImage m_image;

	CString user_image;
	CString administrator_book_image;

	CString m_strBookName;
	CString m_strRentalDate;
	CString m_strExpireDate;
	CComboBox m_RentalList;
	CButton m_ReturnButton;

	std::vector<CString> book_name;
	std::vector<CString> rental_date;
	std::vector<CString> expire_date;
	std::vector<CString> book_image;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RFID_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
	afx_msg void OnReadUser();

	void show_image_user(CString image_url);
	void show_image_administrator(CString image_url);

	// Read continue for thread
	LRESULT Rental_Book(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCbnSelchangeBookList();
	afx_msg void OnBnClickedReturnButton();
	CButton m_RentalStart;

	CString m_Administrator_Book_Name;
	CString m_Administrator_Rental_Date;
	CString m_Administrator_Expire_Date;
	CButton m_Rental_Complete_Button;
	afx_msg void OnBnClickedRentalStartBuitton();
	afx_msg void OnBnClickedRentalComplete();
	CString m_strUserUID;
	CString m_strUserName;
};
