
// RFIDDlg.cpp: 구현 파일
//
#include <iostream>

#include "is_d2xx.h"
#include "pch.h"
#include "framework.h"
#include "RFID.h"
#include "RFIDDlg.h"
#include "afxdialogex.h"

// sound 출력용
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include "mmsystem.h"
#pragma comment(lib,"winmm.lib") // 라이브러리 불러오기

#ifdef _DEBUG
#define new DEBUG_NEW

// console
#pragma comment(linker, "/ENTRY:WinMainCRTStartup /subsystem:console")
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRFIDDlg 대화 상자



CRFIDDlg::CRFIDDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RFID_DIALOG, pParent)
	, m_strBookName(_T(""))
	, m_strRentalDate(_T(""))
	, m_strExpireDate(_T(""))
	, m_Administrator_Book_Name(_T(""))
	, m_Administrator_Rental_Date(_T(""))
	, m_Administrator_Expire_Date(_T(""))
	, m_strUserUID(_T(""))
	, m_strUserName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// for Thread flag
	m_ThreadFlag = false;
}
CRFIDDlg::~CRFIDDlg()
{
	OnDisconnect();
}

void CRFIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USER_UID, m_strUserUID);
	DDX_Text(pDX, IDC_USER_NAME, m_strUserName);
	DDX_Text(pDX, IDC_USER_BOOK_NAME, m_strBookName);
	DDX_Text(pDX, IDC_USER_RENTAL_DATE, m_strRentalDate);
	DDX_Text(pDX, IDC_USER_EXPIRE_DATE, m_strExpireDate);
	DDX_Control(pDX, IDC_RENTAL_LIST, m_RentalList);
	DDX_Control(pDX, IDC_RETURN_BUTTON, m_ReturnButton);
	DDX_Control(pDX, IDC_RENTAL_START_BUITTON, m_RentalStart);

	DDX_Text(pDX, IDC_ADMINISTRATOR_BOOK_NAME, m_Administrator_Book_Name);
	DDX_Text(pDX, IDC_ADMINISTRATOR_RENTAL_DATE, m_Administrator_Rental_Date);
	DDX_Text(pDX, IDC_ADMINISTRATOR_EXPIRE_DATE, m_Administrator_Expire_Date);

	DDX_Control(pDX, IDC_RENTAL_COMPLETE, m_Rental_Complete_Button);
	DDX_Text(pDX, IDC_USER_UID, m_strUserUID);
	DDX_Text(pDX, IDC_USER_NAME, m_strUserName);
}

BEGIN_MESSAGE_MAP(CRFIDDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(MESSAGE_RENTAL_BOOK, &CRFIDDlg::Rental_Book)
	ON_BN_CLICKED(IDC_RFID_CONNECT, &CRFIDDlg::OnConnect)
	ON_BN_CLICKED(IDC_RFID_DISCONNECT, &CRFIDDlg::OnDisconnect)
	ON_BN_CLICKED(IDC_READ_USER, &CRFIDDlg::OnReadUser)
	ON_CBN_SELCHANGE(IDC_RENTAL_LIST, &CRFIDDlg::OnCbnSelchangeBookList)
	ON_BN_CLICKED(IDC_RETURN_BUTTON, &CRFIDDlg::OnBnClickedReturnButton)
	ON_BN_CLICKED(IDC_RENTAL_START_BUITTON, &CRFIDDlg::OnBnClickedRentalStartBuitton)
	ON_BN_CLICKED(IDC_RENTAL_COMPLETE, &CRFIDDlg::OnBnClickedRentalComplete)
END_MESSAGE_MAP()


// CRFIDDlg 메시지 처리기

BOOL CRFIDDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// 숨길 내용
	m_ReturnButton.ShowWindow(SW_HIDE);
	m_RentalStart.ShowWindow(SW_HIDE);
	m_Rental_Complete_Button.ShowWindow(SW_HIDE);


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRFIDDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CRFIDDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CRFIDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CRFIDDlg::OnConnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		//열린 포트번호 찾기
	if (is_GetDeviceNumber(&usbnumber) == IS_OK)
	{
		printf("FTDI USB To Serial 연결된 개수 : %d\n", usbnumber);
		if (is_GetSerialNumber(0, readSerialNumber) == IS_OK)
		{
			if (memcmp(changeSerialNumber, readSerialNumber, sizeof(changeSerialNumber) != 0))
			{
				if (is_SetSerialNumber(0, changeSerialNumber) == IS_OK)
				{
					printf(" USB To Serial Number 를 변경 하였습니다.\n");
					printf(" FTDI SerialNumber :  %s \n", changeSerialNumber);
				}
			}
			else
				printf(" FTDI SerialNumber :  %s \n", changeSerialNumber);
		}
	}

	//열린 포트번호로 연결
	unsigned long portNumber;
	if (is_GetCOMPort_NoConnect(0, &portNumber) == IS_OK)
	{
		printf("COM Port : %d\n", portNumber);
	}
	if (is_OpenSerialNumber(&ftHandle, readSerialNumber, 115200) != IS_OK)
	{
		printf("USB To Serial과 통신 연결 실패\n");
		//return -1;
	}
	else {
		printf("Serial포트 %d와 통신 연결성공!! \n", portNumber);
	}
	Sleep(100);
}


void CRFIDDlg::OnDisconnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		// 무선파워를 끊어요.
	is_RfOff(ftHandle);
	//USB 포트를 Close
	if (is_Close(ftHandle) == IS_OK)
	{
		printf("연결을 닫습니다. ");
	}

}


void CRFIDDlg::OnReadUser()
{
	int i, length = 0;
	CString temp;
	CString uid = _T("");
	CStringA uid_ascii;

	// fetched DB data
	CString user_uid, user_name;
	 
	// Variables using DB
	MYSQL* conn;
	MYSQL* conn_result;
	unsigned int timeout_sec = 1;
	char query[1024];
	MYSQL_RES* result;
	MYSQL_ROW row;

	// vector initialization
	book_name.clear();
	rental_date.clear();
	expire_date.clear();
	book_image.clear();

	// list initialization
	m_RentalList.ResetContent();

	//ISO15693모드로 읽기( 유저 )
	if (is_WriteReadCommand(ftHandle, CM1_ISO15693, CM2_ISO15693_ACTIVE + BUZZER_ON,
		writeLength, wirteData, &readLength, readData) == IS_OK)
	{
		for (i = 0; i < readLength; i++)
		{
			temp.Format(_T("%02x "), readData[i]);
			uid += temp;
		}

		uid_ascii = CStringA(uid);
		std::cout << uid_ascii << std::endl;
		if (!(uid.IsEmpty() || uid.GetLength() < 16))
		{
			// DB Connection
			conn = mysql_init(NULL);
			mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
			conn_result = mysql_real_connect(conn, "127.0.0.1", "root", "tbd9226844", "mysql", 3306, NULL, 0);

			// DB
			if (NULL != conn_result)
			{
				std::cout << "DB Connection Success\n" << std::endl;
				
				m_RentalStart.ShowWindow(SW_SHOW);

				sprintf_s(query, 1024, "SELECT * FROM library_user WHERE UID = '%s'", uid_ascii.GetString());

				// Send Query
				if (mysql_query(conn, query))
				{
					std::cout << "SELECT Fail" << std::endl;
				}
				else
				{
					// Get Response
					result = mysql_store_result(conn);

					while (row = mysql_fetch_row(result))     // 모든 레코드 탐색
					{
						user_uid = row[0];
						user_name = row[1];

						m_RentalList.AddString((CString)row[2]);
						book_name.push_back((CString)row[2]);

						rental_date.push_back((CString)row[3]);

						expire_date.push_back((CString)row[4]);

						user_image = row[5];

						book_image.push_back((CString)row[6]);
					}

					mysql_free_result(result);
				}
			}
			else
			{
				std::cout << "DB Connection Fail\n" << std::endl;
			}
			mysql_close(conn);

			m_strUserUID = user_uid;
			m_strUserName = user_name;

			show_image_user(user_image);

			UpdateData(false);
		}
	}
}


void CRFIDDlg::show_image_user(CString image_url)
{
	CRect m_rect;
	CClientDC dc(this);

	if (m_image)
	{
		m_image.Destroy();
		DeleteObject(m_image);
	}

	HBITMAP m_Bmp = ((CStatic*)GetDlgItem(IDC_USER_IMAGE))->SetBitmap(m_image);
	if (m_Bmp)
		::DeleteObject(m_Bmp);

	m_image.Load(image_url);

	if (m_image == NULL) return;

	((CStatic*)GetDlgItem(IDC_USER_IMAGE))->GetWindowRect(m_rect);
	ScreenToClient(&m_rect);

	m_image.Draw(dc, m_rect);
}

void CRFIDDlg::show_image_administrator(CString image_url)
{
	CRect m_rect;
	CClientDC dc(this);

	if (m_image)
	{
		m_image.Destroy();
		DeleteObject(m_image);
	}

	HBITMAP m_Bmp = ((CStatic*)GetDlgItem(IDC_ADMINISTRATOR_IMAGE))->SetBitmap(m_image);
	if (m_Bmp)
		::DeleteObject(m_Bmp);

	m_image.Load(image_url);

	if (m_image == NULL) return;

	((CStatic*)GetDlgItem(IDC_ADMINISTRATOR_IMAGE))->GetWindowRect(m_rect);
	ScreenToClient(&m_rect);

	m_image.Draw(dc, m_rect);
}


UINT ThreadForRead_Card(LPVOID param)
{
	CRFIDDlg* pMain = (CRFIDDlg*)param;

	if (pMain == NULL || !pMain->IsKindOf(RUNTIME_CLASS(CRFIDDlg)))
		return 1;

	while (pMain->m_ThreadFlag)
	{
		PostMessage(pMain->m_hWnd, MESSAGE_RENTAL_BOOK, NULL, NULL);

		Sleep(1000);
	}
	return 0;
}


LRESULT CRFIDDlg::Rental_Book(WPARAM wParam, LPARAM lParam)
{
	int i, length = 0;
	CString temp;
	CString uid = _T("");
	CStringA uid_ascii;

	// fetched DB data
	CString book_name;

	// Variables using DB
	MYSQL* conn;
	MYSQL* conn_result;
	unsigned int timeout_sec = 1;
	char query[1024];
	MYSQL_RES* result;
	MYSQL_ROW row;

	//ISO14443A모드로 읽기 ( 관리자 )
	if (is_WriteReadCommand(ftHandle, CM1_ISO14443AB, CM2_ISO14443A_ACTIVE + BUZZER_ON,
		writeLength, wirteData, &readLength, readData) == IS_OK)
	{
		for (i = 0; i < readLength; i++)
		{
			temp.Format(_T("%02x "), readData[i]);
			uid += temp;
		}

		uid_ascii = CStringA(uid);

		std::cout << uid_ascii << std::endl;

		if (!(uid.IsEmpty() || uid.GetLength() < 8))
		{
			// DB Connection
			conn = mysql_init(NULL);
			mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
			conn_result = mysql_real_connect(conn, "127.0.0.1", "root", "tbd9226844", "mysql", 3306, NULL, 0);

			// DB
			if (NULL != conn_result)
			{
				std::cout << "DB Connection Success\n" << std::endl;

				m_Rental_Complete_Button.ShowWindow(SW_SHOW);

				sprintf_s(query, 1024, "SELECT * FROM administrator_book WHERE UID = '%s'", uid_ascii.GetString());

				// Send Query
				if (mysql_query(conn, query))
				{
					std::cout << "SELECT Fail" << std::endl;
				}
				else
				{
					// Get Response
					result = mysql_store_result(conn);

					row = mysql_fetch_row(result);
					
					book_name = row[1];
					administrator_book_image = row[2];
					
					mysql_free_result(result);
				}
			}
			else
			{
				std::cout << "DB Connection Fail\n" << std::endl;
			}
			mysql_close(conn);

			show_image_administrator(administrator_book_image);

			m_Administrator_Book_Name = book_name;

			UpdateData(false);
		}
	}
	return 0;
}

void CRFIDDlg::OnCbnSelchangeBookList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int index;
	int num = m_RentalList.GetCurSel();
	CString bName;
	m_RentalList.GetLBText(num, bName);

	for (index = 0; index < book_name.size(); index++)
	{
		if (!strcmp((CStringA)bName, (CStringA)book_name.at(index))) break;
	}

	m_strBookName = book_name.at(index);
	m_strRentalDate = rental_date.at(index);
	m_strExpireDate = expire_date.at(index);
	show_image_user(book_image.at(index));

	UpdateData(false);

	m_ReturnButton.ShowWindow(SW_SHOW);
}


void CRFIDDlg::OnBnClickedReturnButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(true);

	int index;
	for (index = 0; index < book_name.size(); index++)
	{
		if (!strcmp((CStringA)m_strBookName, (CStringA)book_name.at(index))) break;
	}

	if (strcmp((CStringA)book_name.at(index), ""))
	{
		book_name.erase(book_name.begin() + index);
		rental_date.erase(rental_date.begin() + index);
		expire_date.erase(expire_date.begin() + index);
		book_image.erase(book_image.begin() + index);

		// Variables using DB
		MYSQL* conn;
		MYSQL* conn_result;
		unsigned int timeout_sec = 1;
		char query[1024];

		// DB Connection
		conn = mysql_init(NULL);
		mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
		conn_result = mysql_real_connect(conn, "127.0.0.1", "root", "tbd9226844", "mysql", 3306, NULL, 0);

		// DB
		if (NULL != conn_result)
		{
			std::cout << "DB Connection Success\n" << std::endl;

			sprintf_s(query, 1024, "DELETE FROM library_user where BOOK_NAME = '%s'", (CStringA)m_strBookName.GetString());

			// Send Query
			if (mysql_query(conn, query)) std::cout << "DELETE Fail" << std::endl;
		}
		else
		{
			std::cout << "DB Connection Fail\n" << std::endl;
		}
		mysql_close(conn);

		m_RentalList.DeleteString(index);
		m_strBookName = _T("");
		m_strRentalDate = _T("");
		m_strExpireDate = _T("");

		UpdateData(false);
	}
}


void CRFIDDlg::OnBnClickedRentalStartBuitton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString button;
	GetDlgItemText(IDC_RENTAL_START_BUITTON, button);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (button == _T("대여 하기"))
	{
		m_ThreadFlag = true;

		m_pThread = AfxBeginThread(ThreadForRead_Card, this);

		m_Rental_Complete_Button.ShowWindow(SW_SHOW);

		SetDlgItemText(IDC_RENTAL_START_BUITTON, _T("stop"));
	}
	else
	{
		m_ThreadFlag = false;

		WaitForSingleObject(m_pThread->m_hThread, 1000);

		SetDlgItemText(IDC_RENTAL_START_BUITTON, _T("대여 하기"));

		m_Rental_Complete_Button.ShowWindow(SW_HIDE);
	}
}


void CRFIDDlg::OnBnClickedRentalComplete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(true);

	//db 연결, 데이터를 db에 삽입 중복된게 있으면 삽입 x

	// Variables using DB
	MYSQL* conn;
	MYSQL* conn_result;
	unsigned int timeout_sec = 1;
	char query[1024];
	MYSQL_RES* result;
	MYSQL_ROW row;

	// DB Connection
	conn = mysql_init(NULL);
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
	conn_result = mysql_real_connect(conn, "127.0.0.1", "root", "tbd9226844", "mysql", 3306, NULL, 0);

	if (NULL != conn_result)
	{
		std::cout << "DB Connection Success\n" << std::endl;

		m_Rental_Complete_Button.ShowWindow(SW_SHOW);

		sprintf_s(query, 1024, "SELECT BOOK_NAME FROM library_user WHERE UID = '%s'", (CStringA)m_strUserUID);

		// Send Query
		if (mysql_query(conn, query))
		{
			std::cout << "SELECT Fail" << std::endl;
		}
		else
		{
			// Get Response
			result = mysql_store_result(conn);

			while (row = mysql_fetch_row(result))     // 모든 레코드 탐색
			{
				if (!strcmp(row[0], (CStringA)m_Administrator_Book_Name))
				{
					AfxMessageBox(_T("이미 대여한 책입니다!"));
					return;
				}
			}
			mysql_free_result(result);
		}

		sprintf_s(query, 1024, "INSERT INTO library_user VALUES('%s', '%s', '%s', '%s', '%s', '%s', '%s')",
			(CStringA)m_strUserUID, (CStringA)m_strUserName, (CStringA)m_Administrator_Book_Name,
			(CStringA)m_Administrator_Rental_Date, (CStringA)m_Administrator_Expire_Date,
			(CStringA)user_image, (CStringA)administrator_book_image);

		// Send Query
		if (mysql_query(conn, query))
		{
			std::cout << "SELECT Fail" << std::endl;
		}
	}
	else
	{
		std::cout << "DB Connection Fail\n" << std::endl;
	}
	mysql_close(conn);

	AfxMessageBox(_T("대여가 완료되었습니다!"));
}
