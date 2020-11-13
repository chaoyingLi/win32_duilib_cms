#ifndef _CMS_GUI_H_
#define _CMS_GUI_H_

#pragma once
#include <UIlib.h>
#include <stack>
using namespace DuiLib;

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_ud.lib")
#   else
#       pragma comment(lib, "DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_u.lib")
#   else
#       pragma comment(lib, "DuiLib.lib")
#   endif
#endif

//定义样式的XML文件位置
#define	DUI_XML_LOGIN						"../Resource/cms-login.xml"
#define	DUI_XML_OPERATOR				"../Resource/cms-operator.xml"
#define	DUI_XML_OPERATOR_HOME	"../Resource/cms-operator-home.xml"
#define	DUI_XML_OPERATOR_CHARGE	"../Resource/cms-operator-charge.xml"
#define	DUI_XML_OPERATOR_USERMGR	"../Resource/cms-operator-usermgr.xml"
#define	DUI_XML_OPERATOR_ADMIN	"../Resource/cms-operator-admin.xml"
#define	DUI_XML_ADMIN					"../Resource/cms-admin.xml"
#define	DUI_XML_SUB_CHANGEPWD			"../Resource/cms-sub-changepwd.xml"
#define	DUI_XML_SUB_CHARGED			"../Resource/cms-sub-charged.xml"
#define	DUI_XML_SUB_NEW					"../Resource/cms-sub-new.xml"
#define	DUI_XML_SUB_DEL						"../Resource/cms-sub-del.xml"
#define	DUI_XML_SUB_QNM					"../Resource/cms-sub-qnm.xml"
#define	DUI_XML_SUB_QR						"../Resource/cms-sub-qr.xml"
#define	DUI_XML_SUB_OPERATORMGR	"../Resource/cms-sub-OperatorMgr.xml"
#define	DUI_XML_SUB_OPERATORMGR_NEW	"../Resource/cms-sub-OperatorMgr-New.xml"
#define	DUI_XML_SUB_OPERATORMGR_QMD	"../Resource/cms-sub-OperatorMgr-QMD.xml"
#define	DUI_XML_SUB_MODIFYOPERATOR		"../Resource/cms-sub-ModifyOperator.xml"
#define	DUI_XML_SUB_PLANMGR	"../Resource/cms-sub-PlanMgr.xml"
#define	DUI_XML_SUB_PLANMGR_NEW	"../Resource/cms-sub-PlanMgr-New.xml"
#define	DUI_XML_SUB_PLANMGR_QMD	"../Resource/cms-sub-PlanMgr-QMD.xml"
#define	DUI_XML_SUB_MODIFYPLAN		"../Resource/cms-sub-ModifyPlan.xml"
#define	DUI_XML_SUB_IOUSER				"../Resource/cms-sub-IOuser.xml"



//自定义消息
#define	WM_CMS_BACKTOPARENT	(WM_USER + 233)

//XML文件中<Window>标签的"mininfo"属性表示窗口重绘之后的大小

struct ChargedInfo
{
	wchar_t MSISDN_NAME[128];
	wchar_t amount[32];
	wchar_t method[16];
	wchar_t OperatorNo[16];
};

//线程创建
void MainThread(void * pParam);					//创建主窗口线程
void ChangePwdThread(void * pParam);		//创建修改密码线程
void ChargedThread(void * pParam);				//创建缴费成功线程
void NewThread(void * pParam);					//创建开户线程
void DelThread(void * pParam);						//创建销户线程
void QnMThread(void * pParam);					//创建查询/修改线程
void QRThread(void * pParam);						//创建查询缴费记录线程
void OperatorMgrThread(void * pParam);		//创建收费员管理线程
void ModifyOperatorThread(void * pParam);	//创建修改收费员线程
void PlanMgrThread(void * pParam);				//创建收费员管理线程
void ModifyPlanThread(void * pParam);			//创建修改收费员线程
void IOuserThread(void * pParam);					//创建导入导出用户数据线程


static int OperatorNo;			//当前操作员/管理员工号
static int OperatorClass;		//当前操作员/管理员等级
stack<HWND> stackHwnd;

class CLoginFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void	Notify(TNotifyUI& msg);
	//处理其他系统消息
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	//处理系统消息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	static bool OnLogin(LPCWSTR OperatorNo, LPCWSTR Password);

private:
	CEditUI* pNoEdit;
	CEditUI* pPwdEdit;
	CButtonUI* btnMin;
	CButtonUI* btnClose;
};

class CMainFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void	Notify(TNotifyUI& msg);
	void Init();
	//处理其他系统消息
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	//处理系统消息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	//切换到“首页”标签页
	bool OnSwitchHome();
	//切换到“用户缴费”标签页
	bool OnSwitchCharge();
	//切换到“用户管理”标签页
	bool OnSwitchUserMgr();
	//切换到“综合管理”标签页
	bool OnSwitchAdmin();

	bool OnUserCharge(struct ChargedInfo * ci);
	void OnQueryChargeMSISDN(LPCWSTR MSISDN);
	void OnPrepare();
private:
	CButtonUI* btnMin;
	CButtonUI* btnClose;
	CEditUI* editMSISDN;
	CEditUI* editCharge;
	CContainerUI* containerCharge;
	COptionUI* optHome;
	CWndShadow m_WndShadow;
	CControlUI* ctrlGray;
	CControlUI* ctrlGrayHome;
	CControlUI* ctrlGrayCharge;
	CControlUI* ctrlGrayUserMgr;
	CControlUI* ctrlGrayAdmin;
};

class OperatorHomeUI : public CContainerUI
{
public:
	OperatorHomeUI()
	{
		CDialogBuilder builder;
		CContainerUI* pOperatorHome = static_cast<CContainerUI*>(builder.Create(_T(DUI_XML_OPERATOR_HOME), (UINT)0));
		if (pOperatorHome) 
		{
			this->Add(pOperatorHome);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

class OperatorChargeUI : public CContainerUI
{
public:
	OperatorChargeUI()
	{
		CDialogBuilder builder;
		CContainerUI* pOperatorCharge = static_cast<CContainerUI*>(builder.Create(_T(DUI_XML_OPERATOR_CHARGE), (UINT)0));
		if (pOperatorCharge)
		{
			this->Add(pOperatorCharge);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

class OperatorUserMgrUI : public CContainerUI
{
public:
	OperatorUserMgrUI()
	{
		CDialogBuilder builder;
		CContainerUI* pOperatorUserMgr = static_cast<CContainerUI*>(builder.Create(_T(DUI_XML_OPERATOR_USERMGR), (UINT)0));
		if (pOperatorUserMgr)
		{
			this->Add(pOperatorUserMgr);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

class OperatorAdminUI : public CContainerUI
{
public:
	OperatorAdminUI()
	{
		CDialogBuilder builder;
		CContainerUI* pOperatorAdmin = static_cast<CContainerUI*>(builder.Create(_T(DUI_XML_OPERATOR_ADMIN), (UINT)0));
		if (pOperatorAdmin)
		{
			this->Add(pOperatorAdmin);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};
//创建自定义控件 嵌套XML
class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		//与XML中的switch占位标签名对应
		if (_tcscmp(pstrClass, _T("OperatorHome")) == 0) return new OperatorHomeUI;
		if (_tcscmp(pstrClass, _T("OperatorCharge")) == 0) return new OperatorChargeUI;
		if (_tcscmp(pstrClass, _T("OperatorUserMgr")) == 0) return new OperatorUserMgrUI;
		if (_tcscmp(pstrClass, _T("OperatorAdmin")) == 0) return new OperatorAdminUI;


		return NULL;
	}
};

//定义子窗口类
//修改密码窗口类
class CSubChangePwdFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void	Notify(TNotifyUI& msg);
	//处理其他系统消息
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	//处理系统消息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//处理创建窗口消息
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	//处理修改密码请求
	bool OnChangePwd();

private:
	CEditUI* pRawPwdEdit;
	CEditUI* pNewPwdEdit;
	CEditUI* pNewPwd2Edit;
	CButtonUI* btnClose;
	CButtonUI* btnChange;
	CButtonUI* btnReset;
	CWndShadow m_WndShadow;
};

//缴费成功窗口类
class CSubChargedFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void	Notify(TNotifyUI& msg);

	//初始化类
	CSubChargedFrameWnd(struct ChargedInfo * ci);
	//初始化窗口
	void Init();
	//处理其他系统消息
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	//处理系统消息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//处理创建窗口消息
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//处理打印发票请求
	bool OnCreateReceipt();

private:
	CLabelUI* labelMSISDN_NAME;
	CLabelUI* labelAmount;
	CButtonUI* btnClose;
	CButtonUI* btnPrint;

	struct ChargedInfo * pci;

};

//开户类
class CSubNewFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void	Notify(TNotifyUI& msg);

	//初始化窗口
	void Init();
	//处理其他系统消息
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	//处理系统消息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//处理创建窗口消息
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//处理开户请求
	bool OnNewUser();
};

//销户类
class CSubDelFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void	Notify(TNotifyUI& msg);

	//初始化窗口
	void Init();
	//处理其他系统消息
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	//处理系统消息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//处理创建窗口消息
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//处理查询用户信息请求
	bool OnQueryUserInfo(LPCWSTR MSISDN);
	//处理销户请求
	bool OnDelUser(LPCWSTR MSISDN);
};

//修改/查询类
class CSubQnMFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void	Notify(TNotifyUI& msg);

	//初始化窗口
	void Init();
	//处理其他系统消息
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	//处理系统消息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//处理创建窗口消息
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//处理查询用户信息请求
	bool OnQueryUserInfo(LPCWSTR MSISDN);
	//处理修改请求
	bool OnModifyUserInfo(LPCWSTR MSISDN);
private:
	bool isQueried;
};

//查询缴费记录类
class CSubQRFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void	Notify(TNotifyUI& msg);

	//初始化窗口
	void Init();
	//处理其他系统消息
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	//处理系统消息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//处理创建窗口消息
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//处理查询用户信息请求
	bool OnQueryRecords(LPCWSTR MSISDN);
	//处理导出请求
	bool OnOut(LPCWSTR MSISDN);
private:
	bool isQueried;
	struct CMS_DB_HEADER * cdh;
};

class CSubOperatorMgrFrameWnd :public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void	Notify(TNotifyUI& msg);

	~CSubOperatorMgrFrameWnd();
	//初始化窗口
	void Init();
	//处理其他系统消息
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	//处理系统消息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//处理创建窗口消息
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	//标签页切换
	bool OnSwitchNew();
	bool OnSwitchQMD();
	// 响应“新增”标签页中的“增加”按钮事件
	bool OnNewOperator();
	//响应“增删改”标签页中的“查询”按钮事件
	bool OnQueryOperator();
private:
	struct CMS_DB_HEADER * cdh;
	CControlUI* ctrlGrayOpMgrQMD;
	CControlUI* ctrlGrayOpMgr;

};

//收费员管理 “新建”标签页
class OperatorMgrNewUI : public CContainerUI
{
public:
	OperatorMgrNewUI()
	{
		CDialogBuilder builder;
		CContainerUI* pOperatorMgrNew =
			static_cast<CContainerUI*>(builder.Create(_T(DUI_XML_SUB_OPERATORMGR_NEW), (UINT)0));
		if (pOperatorMgrNew)
		{
			this->Add(pOperatorMgrNew);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

//收费员管理 “查改删”标签页
class OperatorMgrQMDUI : public CContainerUI
{
public:
	OperatorMgrQMDUI()
	{
		CDialogBuilder builder;
		CContainerUI* pOperatorMgrQMD =
			static_cast<CContainerUI*>(builder.Create(_T(DUI_XML_SUB_OPERATORMGR_QMD), (UINT)0));
		if (pOperatorMgrQMD)
		{
			this->Add(pOperatorMgrQMD);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

//创建自定义控件 嵌套XML
class CSubOperatorMgrBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		//与XML中的switch占位标签名对应
		if (_tcscmp(pstrClass, _T("OperatorMgrNew")) == 0) return new OperatorMgrNewUI;
		if (_tcscmp(pstrClass, _T("OperatorMgrQMD")) == 0) return new OperatorMgrQMDUI;

		return NULL;
	}
};

//收费员管理“修改”子窗口
class CModifyOperatorFrameWnd :public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void	Notify(TNotifyUI& msg);

	CModifyOperatorFrameWnd(struct CMS_DB_NODE * cdn);
	//初始化窗口
	void Init();
	//处理其他系统消息
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	//处理系统消息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//处理创建窗口消息
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	struct Operator* pOperator;
	CEditUI * editName;
	CEditUI * editGender;
	CEditUI * editPwd;
	CListLabelElementUI* pItemEle;
public:
	// 响应“修改”按钮
	bool OnModify();
};

//套餐管理
class CPlanMgrFrameWnd :public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void	Notify(TNotifyUI& msg);

	~CPlanMgrFrameWnd();
	//初始化窗口
	void Init();
	//处理其他系统消息
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	//处理系统消息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//处理创建窗口消息
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// 响应“新增”标签页中的“增加”按钮事件
	bool OnNewPlan();
	//响应“增删改”标签页中的“查询”按钮事件
	bool OnQueryPlan();
private:
	struct CMS_DB_HEADER * cdh;
	CControlUI* ctrlGrayPlanMgrQMD;
	CControlUI* ctrlGrayPlanMgr;
public:
	// 删除选中套餐
	bool OnDeletePlan();
	// 获取选中元素在链表中的指针
	bool CDNCallPlanWalker(struct CMS_DB_NODE * &cdn);
};

//套餐管理 “新建”标签页
class PlanMgrNewUI : public CContainerUI
{
public:
	PlanMgrNewUI()
	{
		CDialogBuilder builder;
		CContainerUI* pPlanMgrNew =
			static_cast<CContainerUI*>(builder.Create(_T(DUI_XML_SUB_PLANMGR_NEW), (UINT)0));
		if (pPlanMgrNew)
		{
			this->Add(pPlanMgrNew);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

//套餐管理 “查改删”标签页
class PlanMgrQMDUI : public CContainerUI
{
public:
	PlanMgrQMDUI()
	{
		CDialogBuilder builder;
		CContainerUI* pPlanMgrQMD =
			static_cast<CContainerUI*>(builder.Create(_T(DUI_XML_SUB_PLANMGR_QMD), (UINT)0));
		if (pPlanMgrQMD)
		{
			this->Add(pPlanMgrQMD);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

//创建自定义控件 嵌套XML
class CPlanMgrBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		//与XML中的switch占位标签名对应
		if (_tcscmp(pstrClass, _T("PlanMgrNew")) == 0) return new PlanMgrNewUI;
		if (_tcscmp(pstrClass, _T("PlanMgrQMD")) == 0) return new PlanMgrQMDUI;

		return NULL;
	}
};

//套餐管理“修改”子窗口
class CModifyPlanFrameWnd :public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void	Notify(TNotifyUI& msg);

	CModifyPlanFrameWnd(struct CMS_DB_NODE * cdn);
	//初始化窗口
	void Init();
	//处理其他系统消息
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	//处理系统消息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//处理创建窗口消息
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	struct CallPlan* pPlan;
	CEditUI * editRemote;
	CEditUI * editLocal;
	CEditUI * editRoaming;
	CEditUI * editName;
public:
	// 响应“修改”按钮
	bool OnModify();
};

//套餐管理“导入导出用户数据”子窗口
class CIOuserFrameWnd :public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void	Notify(TNotifyUI& msg);

	//处理其他系统消息
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	//处理系统消息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	// 导出用户数据
	bool OnOut();
	// 导入用户数据
	bool OnIn();
};
#endif