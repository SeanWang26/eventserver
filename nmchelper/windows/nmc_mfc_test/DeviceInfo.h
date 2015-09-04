#pragma once


// CDeviceInfo �Ի���

class CDeviceInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CDeviceInfo)

public:
	CDeviceInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDeviceInfo();

// �Ի�������
	enum { IDD = IDD_DIALOG_DEVICE_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

public:

	CString m_Ip;
	int m_Port;
	CString m_User;
	CString m_Password;
	CString m_DeviceName;

	afx_msg void OnBnClickedCancel();
};
