// DialogSurveyTest.cpp : 实现文件
//

#include "stdafx.h"
#include "nmc_mfc_test.h"
#include "DialogSurveyTest.h"
#include "afxdialogex.h"
#include "NMCUser.h"
#include <string>

using namespace std;

//多字节转为utf-8
void ConvertGBKToUtf8dgg(std::string& amp, std::string strGBK)
{ 
	int len=MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strGBK.c_str(), -1, NULL,0); 
	unsigned short * wszUtf8 = new unsigned short[len+1]; 
	memset(wszUtf8, 0, len * 2 + 2); 
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strGBK.c_str(), -1, (LPWSTR)wszUtf8, len); 
	len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL); 
	char *szUtf8=new char[len + 1]; 
	memset(szUtf8, 0, len + 1); 
	WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, szUtf8, len, NULL,NULL); 
	//strGBK = szUtf8; 
	amp=szUtf8;

	delete[] szUtf8; 
	delete[] wszUtf8; 
}

//utf-8转为多字节
int GetUtf8Doc_dvserd(char** pDoc, int& nDocLen, const char* in)
{
	int nMaxTransLen = 2 * (strlen(in)+1);

	WCHAR *pTmpUnicode = new WCHAR[nMaxTransLen];
	memset(pTmpUnicode, 0, sizeof(WCHAR));

	int nUniLen = MultiByteToWideChar(CP_UTF8, 0, in, strlen(in)+1, pTmpUnicode, nMaxTransLen);

	CHAR* pTmpAnsi = new CHAR[nMaxTransLen];
	memset(pTmpAnsi, 0, sizeof(CHAR));

	int nAnsiLen = WideCharToMultiByte(CP_ACP, 0, pTmpUnicode, nUniLen, pTmpAnsi, nMaxTransLen, NULL,0); 

	*pDoc = pTmpAnsi;
	return 0;
}

IMPLEMENT_DYNAMIC(CDialogSurveyTest, CDialogEx)

CDialogSurveyTest::CDialogSurveyTest(long Handle)
	: CDialogEx(CDialogSurveyTest::IDD, NULL)
	, m_SurveyWindowId(0)
{
	m_Handle = Handle;
	ppsurvey_info = NULL;
	survey_info_cnt = 0;
}

CDialogSurveyTest::CDialogSurveyTest(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogSurveyTest::IDD, pParent)
{

}

CDialogSurveyTest::~CDialogSurveyTest()
{
}

void CDialogSurveyTest::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SURVEY_PLAN, m_SurveyPlanList);
	DDX_Control(pDX, IDC_LIST_SURVEY_DEVICES, m_SurveyDeviceS);
	DDX_Control(pDX, IDC_COMBO_Survey_operate, m_CombCtrlSurvey);
	DDX_Text(pDX, IDC_EDIT_Survey_WndId, m_SurveyWindowId);
}


BEGIN_MESSAGE_MAP(CDialogSurveyTest, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDialogSurveyTest::OnBnClickedButton1)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SURVEY_PLAN, &CDialogSurveyTest::OnNMClickListSurveyPlan)
	ON_BN_CLICKED(IDC_BUTTON_Del_Survey, &CDialogSurveyTest::OnBnClickedButtonDelSurvey)
	ON_BN_CLICKED(IDC_BUTTON_Add_Survey, &CDialogSurveyTest::OnBnClickedButtonAddSurvey)
	ON_BN_CLICKED(IDC_BUTTON_Ctrl_Survey, &CDialogSurveyTest::OnBnClickedButtonCtrlSurvey)
	ON_BN_CLICKED(IDOK, &CDialogSurveyTest::OnBnClickedOk)
END_MESSAGE_MAP()



BOOL CDialogSurveyTest::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_SurveyPlanList.InsertColumn(0,_T("轮询id"));
	m_SurveyPlanList.SetColumnWidth(0, 50);
	m_SurveyPlanList.InsertColumn(1,_T("轮询名称"));
	m_SurveyPlanList.SetColumnWidth(1, 80);
	m_SurveyPlanList.InsertColumn(2,_T("轮询间隔(秒)"));
	m_SurveyPlanList.SetColumnWidth(2, 80);
	m_SurveyPlanList.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);

	m_SurveyDeviceS.InsertColumn(0,_T("设备id"));
	m_SurveyDeviceS.SetColumnWidth(0, 50);
	m_SurveyDeviceS.InsertColumn(1,_T("源id"));
	m_SurveyDeviceS.SetColumnWidth(1, 80);
	m_SurveyDeviceS.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);

	m_CombCtrlSurvey.AddString(_T("启动"));
	m_CombCtrlSurvey.AddString(_T("停止"));
	m_CombCtrlSurvey.SetCurSel(0);
	return 0;
}
void CDialogSurveyTest::OnBnClickedButton1()
{
	m_SurveyPlanList.DeleteAllItems();
	m_SurveyDeviceS.DeleteAllItems();

	if(ppsurvey_info)
	{
		nmc_free_survey_plan_info(ppsurvey_info, survey_info_cnt);
		ppsurvey_info = NULL;
		survey_info_cnt = 0;
	}

	int res = nmc_get_survey_plan(m_Handle, -1, &ppsurvey_info, &survey_info_cnt);
	if(res)
	{
		MessageBox(_T("nmc_get_survey_plan failed"));
		return;
	}

	for(int i=0; i<survey_info_cnt; ++i)
	{
		int nItem = m_SurveyPlanList.GetItemCount();

		char tmp[128] = {0};
		sprintf(tmp, "%d", ppsurvey_info[i].survey_id);

		nItem = m_SurveyPlanList.InsertItem(nItem, tmp);
		m_SurveyPlanList.SetItemText(nItem, 0, tmp);

		char* pDoc=NULL;
		int nDocLen = 0;
		GetUtf8Doc_dvserd(&pDoc, nDocLen, ppsurvey_info[i].surveyname);
		m_SurveyPlanList.SetItemText(nItem, 1, pDoc);

		sprintf(tmp, "%d", ppsurvey_info[i].surveyInterval);
		m_SurveyPlanList.SetItemText(nItem, 2, tmp);
	}

	///nmc_free_survey_plan_info(ppsurvey_info, survey_info_cnt);
}


void CDialogSurveyTest::OnNMClickListSurveyPlan(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	m_SurveyDeviceS.DeleteAllItems();

	POSITION pos = m_SurveyPlanList.GetFirstSelectedItemPosition();  
	if(pos == NULL) return;  
	int item = m_SurveyPlanList.GetNextSelectedItem(pos);  
	
	CString strTmp = m_SurveyPlanList.GetItemText(item, 0);
	int SurveyId = atoi(strTmp.GetBuffer());

	if(ppsurvey_info)
	{
		for(int i=0; i<survey_info_cnt; ++i)
		{
			if(ppsurvey_info[i].survey_id==SurveyId)
			{
				for(int j=0; j<ppsurvey_info[i].sourceinfo_cnt; ++j)
				{
					int nItem = m_SurveyDeviceS.GetItemCount();

					char tmp[128] = {0};
					sprintf(tmp, "%d", ppsurvey_info[i].pst_xy_sourceinfo[j].sourceid);

					nItem = m_SurveyDeviceS.InsertItem(nItem, tmp);
					m_SurveyDeviceS.SetItemText(nItem, 0, tmp);					

					sprintf(tmp, "%d", ppsurvey_info[i].pst_xy_sourceinfo[j].subsourceid);
					m_SurveyDeviceS.SetItemText(nItem, 1, tmp);
				}
			
				break;
			}
		}
	}
}

void CDialogSurveyTest::OnBnClickedButtonDelSurvey()
{
	int nItem = -1;

	POSITION pos = m_SurveyPlanList.GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
		nItem		= m_SurveyPlanList.GetNextSelectedItem(pos);
		CString strId = m_SurveyPlanList.GetItemText(nItem, 0);

		int SurveyId = atoi(strId.GetBuffer());

		if(ppsurvey_info==NULL)
		{
			return;
		}

		for(int i=0; i<survey_info_cnt; ++i)
		{
			if(ppsurvey_info[i].survey_id==SurveyId)
			{
				//nmc_remov,删除
				int res = nmc_set_survey_plan(m_Handle, nmc_remov, ppsurvey_info+i);
				if(res)
				{
					MessageBox(_T("删除失败"));
					return;
				}
				else
				{
					MessageBox(_T("删除成功"));
				}
			}
		}
	}
}


void CDialogSurveyTest::OnBnClickedButtonAddSurvey()
{
	struct st_xy_survey_info survey_info;
	survey_info.survey_id = -1;
	survey_info.surveyInterval = 30; //30秒

	std::string strSurveyName;
	ConvertGBKToUtf8dgg(strSurveyName, _T("轮询方案x"));
	strcpy(survey_info.surveyname, strSurveyName.c_str());

	//这里只是测试,使用时应从
	struct st_xy_sourceinfo sourceinfo[3];
	sourceinfo[0].sourceid = 1;
	sourceinfo[0].subsourceid = 2;
	sourceinfo[1].sourceid = 1;
	sourceinfo[1].subsourceid = 19;
	sourceinfo[2].sourceid = 1;
	sourceinfo[2].subsourceid = 32;

	survey_info.pst_xy_sourceinfo = sourceinfo;
	survey_info.sourceinfo_cnt = 3;

	int res = nmc_set_survey_plan(m_Handle, nmc_add, &survey_info);
	if(res)
	{
		MessageBox(_T("nmc_set_survey_plan failed"));
		return;
	}

	MessageBox(_T("nmc_set_survey_plan ok"));

}


void CDialogSurveyTest::OnBnClickedButtonCtrlSurvey()
{
	UpdateData(TRUE);
	if(m_SurveyWindowId<=0)
	{
		MessageBox(_T("请从父对话框的窗口id中选取"));
	}

	int nItem = -1;
	POSITION pos = m_SurveyPlanList.GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
		nItem		= m_SurveyPlanList.GetNextSelectedItem(pos);
		CString strId = m_SurveyPlanList.GetItemText(nItem, 0);

		int SurveyId = atoi(strId.GetBuffer());

		int Ctrl = m_CombCtrlSurvey.GetCurSel();
		//m_WndId的值
		int res = nmc_ctrl_window_survey(m_Handle, Ctrl,  SurveyId, 0, m_SurveyWindowId);
		if(res)
		{
			MessageBox(_T("nmc_ctrl_window_survey failed"));
		}
		else
			MessageBox(_T("nmc_ctrl_window_survey ok"));
	}
}


void CDialogSurveyTest::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}
