#include "CMS-gui.h"
#include "CMS-db.h"
#include "cms-pdf.h"
#include "cms-xls.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CWndShadow::Initialize(hInstance);

	CLoginFrameWnd loginFrame;
	loginFrame.Create(NULL, _T("收费管理系统"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	loginFrame.CenterWindow();

	loginFrame.ShowModal();

	//CMainFrameWnd mainFrame;
	//mainFrame.Create(NULL, _T("收费管理系统"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	//mainFrame.CenterWindow();
	//mainFrame.ShowModal();
	return 0;
}

LPCTSTR CLoginFrameWnd::GetWindowClassName() const {return _T("DUILoginFrame");}

CDuiString CLoginFrameWnd::GetSkinFile(){	return _T(DUI_XML_LOGIN);}

CDuiString CLoginFrameWnd::GetSkinFolder(){	return _T("");}

void CLoginFrameWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("click"))
	{
		//登录按钮
		if (msg.pSender->GetName() == _T("btnLogin"))
		{
			pNoEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNo")));
			pPwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editPwd")));
			//::MessageBox(NULL, pNoEdit->GetText(), pPwdEdit->GetText(), NULL);
			if (pNoEdit->GetText().IsEmpty())
				pNoEdit->SetFocus();
			else if (pPwdEdit->GetText().IsEmpty())
				pPwdEdit->SetFocus();
			else
			{
				//if(OnLogin(pNoEdit->GetText(), pPwdEdit->GetText()) == true)
				if (true)			//绕过登录校验 原校验语句见46行
				{
					HWND m_hwnd = GetHWND();
					//将当前窗口句柄加入栈
					stackHwnd.push(m_hwnd);
					//启动主窗口线程
					_beginthread(MainThread, 0, m_hwnd);
				}
				else
					::MessageBox(NULL, L"登录失败：工号或密码错误！", L"提示", MB_OK | MB_ICONERROR);
			}
		}
		//重置按钮
		else if (msg.pSender->GetName() == _T("btnReset"))
		{
			pNoEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNo")));
			pPwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editPwd")));
			pNoEdit->SetText(_T(""));
			pPwdEdit->SetText(_T(""));
		}
		//最小化按钮
		else if (msg.pSender->GetName() == _T("minbtn")) { SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return; }
		//关闭按钮
		else if (msg.pSender->GetName() == _T("closebtn")) {Close(0); }
	}//if click
	else if (msg.sType == _T("setfocus"))
	{
		if (msg.pSender->GetName() == _T("editNo"))
		{
			pNoEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNo")));
			pNoEdit->SetBorderSize(2);
		}
		else if (msg.pSender->GetName() == _T("editPwd"))
		{
			pPwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editPwd")));
			pPwdEdit->SetBorderSize(2);
		}
	}
	else if (msg.sType == _T("killfocus"))
	{
		if (msg.pSender->GetName() == _T("editNo"))
		{
			pNoEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNo")));
			pNoEdit->SetBorderSize(1);
		}
		else if (msg.pSender->GetName() == _T("editPwd"))
		{
			pPwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editPwd")));
			pPwdEdit->SetBorderSize(1);
		}
	}
}

//处理其他系统消息
LRESULT CLoginFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			//CEditUI* pEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNo")));
			//if (pEdit->GetText().IsEmpty()) pEdit->SetFocus();
			//else {
			//	pEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editPwd")));
			//	if (pEdit->GetText().IsEmpty()) pEdit->SetFocus();
			//	else {
			//		pNoEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNo")));
			//		pPwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editPwd")));
			//		OnLogin(pNoEdit->GetText(), pPwdEdit->GetText());
				//}
			//}
			return true;
		}
		else if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return true;
		}
	default:
		return false;
	}
}

//处理系统消息
LRESULT CLoginFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;	//标志着消息是否被处理
	switch (uMsg)
	{
	case WM_NCLBUTTONDBLCLK: return 0;//禁用双击标题栏窗口最大化：屏蔽非客户区左键双击事件
	case WM_CMS_BACKTOPARENT:
		pNoEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNo")));
		pPwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editPwd")));
		pNoEdit->SetText(L"");
		pPwdEdit->SetText(L"");
		break;
	default: bHandled = FALSE;
	}
	//如果已处理消息 则直接返回
	if (bHandled) return lRes;
	//如果由前述MessageHandler处理了消息 则直接返回
	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	//其余的还给WIB处理...
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

//处理登录过程
bool CLoginFrameWnd::OnLogin(LPCWSTR wszOperatorNo, LPCWSTR Password)
{
	bool isSucceed = false;
	//连接数据库
	DBgo * pDbgo = new DBgo();
	struct Operator * pO = new struct Operator;
	if (pDbgo == NULL || pO == NULL)
		return false;
	//获取密码
	CString szStr;
	szStr.Format(L"%s", Password);
	std::wstring wszStr = szStr.GetBuffer(szStr.GetLength());
	//计算密码MD5
	MD5 * pMD5 = new MD5(ws2s(wszStr));
	if (pMD5 == NULL)
	{
		if (pDbgo)
		{
			delete pDbgo;
			pDbgo = NULL;
		}
		if (pO)
		{
			delete pO;
			pO = NULL;
		}
		return false;
	}
	wszStr = s2ws(pMD5->md5());
	//获取工号
	memset(pO, 0, sizeof(struct Operator));
	pO->OperatorNo = _wtoi(wszOperatorNo);
	wcsncpy_s(pO->password, wszStr.c_str(), wszStr.length() * 2);
	int LoginResult = 0;
	//检索数据库 判断登录结果
	pDbgo->Login(pO, LoginResult);
	if (LoginResult == 1)
	{
		//::MessageBox(NULL, L"操作员", L"登录结果", MB_OK | MB_ICONINFORMATION);
		OperatorNo = pO->OperatorNo;
		OperatorClass = 1;
		isSucceed = true;
	}
	else if (LoginResult == 666)
	{
		//::MessageBox(NULL, L"管理员", L"登录结果", MB_OK | MB_ICONINFORMATION);
		OperatorNo = pO->OperatorNo;
		OperatorClass = 666;
		isSucceed = true;
	}
	else
		isSucceed = false;
	//清理
	if (pMD5)
	{
		delete pMD5;
		pMD5 = NULL;
	}
	if (pO)
	{
		delete pO;
		pO = NULL;
	}
	if (pDbgo)
	{
		delete pDbgo;
		pDbgo = NULL;
	}

	return isSucceed;
}

LPCTSTR CMainFrameWnd::GetWindowClassName() const{ return _T("DUIMainFrame"); }

CDuiString CMainFrameWnd::GetSkinFile(){	return _T(DUI_XML_OPERATOR);}

CDuiString CMainFrameWnd::GetSkinFolder(){ return _T(""); }

void CMainFrameWnd::Notify(TNotifyUI & msg)
{
	//初始化 首先显示登录窗口
	//if (msg.sType == _T("windowinit")) OnPrepare();
	if (msg.sType == _T("click"))
	{
		//“修改密码”按钮
		if (msg.pSender->GetName() == L"btnChangePwd")
		{
			//显示灰色遮罩
			ctrlGray = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGray"));
			ctrlGray->SetVisible(true);
			ctrlGrayHome = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayHome"));
			ctrlGrayHome->SetVisible(true);

			//获取当前窗口句柄
			HWND m_hwnd = GetHWND();
			//将当前窗口句柄加入栈
			stackHwnd.push(m_hwnd);
			//启动修改密码子窗口线程
			_beginthread(ChangePwdThread, 0, m_hwnd);
		}
		//“查询号码”按钮
		else if (msg.pSender->GetName() == _T("btnMSISDN"))
		{
			//获取号码 调用处理函数
			editMSISDN = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editMSISDN")));
			if (editMSISDN && editMSISDN->GetText().GetLength() > 0)
				OnQueryChargeMSISDN(editMSISDN->GetText());
			else
				::MessageBox(GetHWND(), L"请填写用户电话号码...", L"注意", MB_OK | MB_ICONINFORMATION);
		}
		//“缴费”按钮
		else if (msg.pSender->GetName() == _T("btnCharge"))
		{	
			//创建ci 准备传给子线程
			struct ChargedInfo* ci = new struct ChargedInfo;
			if(ci == NULL)
				::MessageBox(GetHWND(), L"内存不足？", L"提示", MB_OK | MB_ICONERROR);
			memset(ci, 0, sizeof(struct ChargedInfo));

			if (this->OnUserCharge(ci) == true)
			{
				//缴费成功 显示自绘窗口
				//::MessageBox(GetHWND(), L"缴费成功", L"提示", MB_OK | MB_ICONINFORMATION);
				//显示灰色遮罩
				ctrlGray = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGray"));
				ctrlGray->SetVisible(true);
				ctrlGrayCharge = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayCharge"));
				ctrlGrayCharge->SetVisible(true);
				//获取当前窗口句柄
				HWND m_hwnd = GetHWND();
				//将当前窗口句柄加入栈
				stackHwnd.push(m_hwnd);
				//启动缴费成功子窗口线程
				_beginthread(ChargedThread, 0, ci);
			}
			else//缴费失败
				::MessageBox(GetHWND(), L"缴费失败", L"提示", MB_OK | MB_ICONERROR);
		}
		//“开户”按钮
		else if (msg.pSender->GetName() == L"btnMgrNew")
		{
			//显示灰色遮罩
			ctrlGray = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGray"));
			ctrlGray->SetVisible(true);
			ctrlGrayUserMgr = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayUserMgr"));
			ctrlGrayUserMgr->SetVisible(true);

			//获取当前窗口句柄
			//HWND m_hwnd = GetHWND();
			//将当前窗口句柄加入栈
			stackHwnd.push(m_hWnd);
			//启动修改密码子窗口线程
			_beginthread(NewThread, 0, m_hWnd);
		}
		//“销户”按钮
		else if (msg.pSender->GetName() == L"btnMgrDel") 
		{
			//显示灰色遮罩
			ctrlGray = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGray"));
			ctrlGray->SetVisible(true);
			ctrlGrayUserMgr = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayUserMgr"));
			ctrlGrayUserMgr->SetVisible(true);

			//获取当前窗口句柄
			//HWND m_hwnd = GetHWND();
			//将当前窗口句柄加入栈
			stackHwnd.push(m_hWnd);
			//启动修改密码子窗口线程
			_beginthread(DelThread, 0, m_hWnd);
		}
		//“查询/修改”按钮
		else if (msg.pSender->GetName() == L"btnMgrQnM")
		{
			//显示灰色遮罩
			ctrlGray = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGray"));
			ctrlGray->SetVisible(true);
			ctrlGrayUserMgr = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayUserMgr"));
			ctrlGrayUserMgr->SetVisible(true);

			//获取当前窗口句柄
			//HWND m_hwnd = GetHWND();
			//将当前窗口句柄加入栈
			stackHwnd.push(m_hWnd);
			//启动修改密码子窗口线程
			_beginthread(QnMThread, 0, m_hWnd);
		}
		// “缴费记录”按钮
		else if (msg.pSender->GetName() == L"btnMgrQR")
		{
			//显示灰色遮罩
			ctrlGray = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGray"));
			ctrlGray->SetVisible(true);
			ctrlGrayUserMgr = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayUserMgr"));
			ctrlGrayUserMgr->SetVisible(true);

			//获取当前窗口句柄
			//HWND m_hwnd = GetHWND();
			//将当前窗口句柄加入栈
			stackHwnd.push(m_hWnd);
			//启动修改密码子窗口线程
			_beginthread(QRThread, 0, m_hWnd);
		}
		//“收费员管理”按钮
		else if (msg.pSender->GetName() == L"btnAdminOperatorMgr")
		{
			//显示灰色遮罩
			ctrlGray = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGray"));
			ctrlGray->SetVisible(true);
			ctrlGrayAdmin = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayAdmin"));
			ctrlGrayAdmin->SetVisible(true);

			//将当前窗口句柄加入栈
			stackHwnd.push(m_hWnd);
			//启动收费员管理子窗口线程
			_beginthread(OperatorMgrThread, 0, m_hWnd);
		}
		//“套餐管理”按钮
		else if (msg.pSender->GetName() == L"btnAdminPlanMgr")
		{
			//显示灰色遮罩
			ctrlGray = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGray"));
			ctrlGray->SetVisible(true);
			ctrlGrayAdmin = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayAdmin"));
			ctrlGrayAdmin->SetVisible(true);

			//将当前窗口句柄加入栈
			stackHwnd.push(m_hWnd);
			//启动套餐管理子窗口线程
			_beginthread(PlanMgrThread, 0, m_hWnd);
		}
		//"导入导出用户数据"按钮
		else if (msg.pSender->GetName() == L"btnAdminIOuser")
		{
			//显示灰色遮罩
			ctrlGray = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGray"));
			ctrlGray->SetVisible(true);
			ctrlGrayAdmin = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayAdmin"));
			ctrlGrayAdmin->SetVisible(true);

			//将当前窗口句柄加入栈
			stackHwnd.push(m_hWnd);
			//启动导入导出用户数据子窗口线程
			_beginthread(IOuserThread, 0, m_hWnd);
		}
		//导入导出操作员按钮
		else if(msg.pSender->GetName() == L"btnAdminIOoperator")
			::MessageBox(m_hWnd, L"敬请期待", L"提示", MB_OK | MB_ICONINFORMATION);
		//导入导出收费记录
		else if (msg.pSender->GetName() == L"btnAdminIOcharge")
			::MessageBox(m_hWnd, L"敬请期待", L"提示", MB_OK | MB_ICONINFORMATION);
		//备份恢复数据库
		else if (msg.pSender->GetName() == L"btnAdminBnR")
		{
			int retVal = ::MessageBox(m_hWnd,
				L"【是】备份数据库至D:/CMS.bak\n【否】从D:/CMS.bak恢复数据库\n【取消】不执行操作",
				L"操作选择",
				MB_YESNOCANCEL | MB_ICONQUESTION);

			if (retVal == IDYES)
			{
				//备份
				DBgo * dbgo = new DBgo;
				if (dbgo->BackupAndRestore(true) == true)
				{
					::MessageBox(m_hWnd, L"备份成功", L"提示", MB_OK | MB_ICONINFORMATION);
				}
				else
				{
					::MessageBox(m_hWnd, L"备份失败", L"提示", MB_OK | MB_ICONINFORMATION);
				}
				delete dbgo;
				dbgo = NULL;
			}
			else if (retVal == IDNO)
			{
				//恢复
				DBgo * dbgo = new DBgo;
				if (dbgo->BackupAndRestore(true) == true)
				{
					::MessageBox(m_hWnd, L"还原成功", L"提示", MB_OK | MB_ICONINFORMATION);
				}
				else
				{
					::MessageBox(m_hWnd, L"还原失败", L"提示", MB_OK | MB_ICONINFORMATION);
				}
				delete dbgo;
				dbgo = NULL;
			}
			else
			{
				::MessageBox(m_hWnd, L"用户取消了操作", L"提示", MB_OK | MB_ICONINFORMATION);
			}
		}
		//最小化按钮：//为避免最小化主程序界面后 从登录界面重复登录 将不响应最小化按钮
		else if (msg.pSender->GetName() == _T("minbtn")) { SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return; }
		//关闭按钮
		else if (msg.pSender->GetName() == _T("closebtn")) 
		{ 
			//从栈顶获取父窗口句柄并出栈
			HWND m_hwnd = stackHwnd.top();
			stackHwnd.pop();

			::EnableWindow(m_hwnd , TRUE);												// 父窗口还原 m_hwnd为子窗口句柄
			::SetFocus(m_hwnd);																	//设置焦点到父窗口
			::PostMessage(m_hwnd, WM_CMS_BACKTOPARENT, 0, 0);		//向父窗口发送消息
			//PostQuitMessage(0); 
			Close();
		}
	}//if click
	else if (msg.sType == _T("selectchanged"))
	{
		//与xml中的opt对应 根据所选opt 切换switch内容
		CDuiString name = msg.pSender->GetName();
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
		if (name == _T("optHome")) { pControl->SelectItem(0); this->OnSwitchHome(); }
		else if (name == _T("optCharge"))	{pControl->SelectItem(1);	this->OnSwitchCharge();	}
		else if (name == _T("optUserMgr")) { pControl->SelectItem(2); this->OnSwitchUserMgr(); }
		else if (name == _T("optAdmin")) 
		{ 
			if (OperatorClass == 666)
			{
				pControl->SelectItem(3);
				this->OnSwitchAdmin();
			}
			else
				::MessageBox(m_hWnd, L"敬请期待~", L"提示", MB_OK | MB_ICONINFORMATION);
		}
	}
	else if (msg.sType == _T("setfocus"))
	{
		if (msg.pSender->GetName() == _T("editMSISDN"))
		{
			editMSISDN = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editMSISDN")));
			editMSISDN->SetBorderSize(2);
		}
		else if (msg.pSender->GetName() == _T("editCharge"))
		{
			editCharge = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editCharge")));
			editCharge->SetBorderSize(2);
		}
	}
	else if (msg.sType == _T("killfocus"))
	{
		if (msg.pSender->GetName() == _T("editMSISDN"))
		{
			editMSISDN = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editMSISDN")));
			editMSISDN->SetBorderSize(1);
		}
		else if (msg.pSender->GetName() == _T("editCharge"))
		{
			editCharge = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editCharge")));
			editCharge->SetBorderSize(1);
		}
	}
	else if (msg.sType == _T("itemselect")) {
		if (msg.pSender->GetName() == _T("optHome")) 
		{
			//CComboUI* pCombox = (CComboUI*)msg.pSender;
			//int nsel = pCombox->GetCurSel();
			//CListLabelElementUI* pItemEle = (CListLabelElementUI*)pCombox->GetItemAt(nsel);
			//CDuiString strText = pItemEle->GetText();
		}
	}
}

void CMainFrameWnd::Init()
{
	//OperatorNo = 2;
	//执行首页动作
	this->OnSwitchHome();
	////将灰色控件设置为不可见
	//ctrlGray = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGray"));
	//ctrlGray->SetVisible(false);
}

LRESULT CMainFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			//CEditUI* pEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNo")));
			//if (pEdit->GetText().IsEmpty()) pEdit->SetFocus();
			//else {
			//	pEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editPwd")));
			//	if (pEdit->GetText().IsEmpty()) pEdit->SetFocus();
			//	else {
			//		//pNoEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNo")));
			//		//pPwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editPwd")));
			//		//OnLogin(pNoEdit->GetText(), pPwdEdit->GetText());
			//	}
			//}
			return true;
		}
		else if (wParam == VK_ESCAPE) {
			//PostQuitMessage(0);
			return true;
		}
	default:
		return false;
	}
}

LRESULT CMainFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;	//标志着消息是否被处理
	switch (uMsg)
	{
		//禁用双击标题栏窗口最大化：屏蔽非客户区左键双击事件
	case WM_NCLBUTTONDBLCLK: return 0;
	case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_CMS_BACKTOPARENT:
		//响应子窗口退出消息 隐藏灰色遮罩
		ctrlGray = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGray"));
		ctrlGray->SetVisible(false);
		ctrlGrayHome = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayHome"));
		ctrlGrayHome->SetVisible(false);
		ctrlGrayCharge = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayCharge"));
		ctrlGrayCharge->SetVisible(false);
		ctrlGrayUserMgr = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayUserMgr"));
		ctrlGrayUserMgr->SetVisible(false);
		ctrlGrayAdmin = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayAdmin"));
		ctrlGrayAdmin->SetVisible(false);
		break;
	default: bHandled = FALSE;
	}
	//如果已处理消息 则直接返回
	if (bHandled) return lRes;
	//如果由前述MessageHandler处理了消息 则直接返回
	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	//其余的还给WIB处理...
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CMainFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	
	m_PaintManager.Init(m_hWnd);
	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb;
	CControlUI* pRoot = builder.Create(_T(DUI_XML_OPERATOR), (UINT)0, &cb, &m_PaintManager);
	ASSERT(pRoot && "Failed to parse XML");
	m_PaintManager.AttachDialog(pRoot);
	m_PaintManager.AddNotifier(this);


	m_WndShadow.Create(m_hWnd);
	m_WndShadow.SetSize(3);
	m_WndShadow.SetPosition(0, 0);

	Init();
	return 0;
}

LRESULT CMainFrameWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	RECT rcCaption = m_PaintManager.GetCaptionRect();
	if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
		CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(pt));
		if (pControl && _tcscmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0 &&
			_tcscmp(pControl->GetClass(), DUI_CTR_OPTION) != 0 &&
			_tcscmp(pControl->GetClass(), DUI_CTR_TEXT) != 0 &&
			_tcscmp(pControl->GetClass(), DUI_CTR_COMBO) != 0)
			return HTCAPTION;
	}
	return HTCLIENT;
}

LRESULT CMainFrameWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	return 0;
}

LRESULT CMainFrameWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	return 0;
}

LRESULT CMainFrameWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	if (::IsIconic(*this)) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

bool CMainFrameWnd::OnSwitchHome()
{
	ctrlGrayHome = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayHome"));
	ctrlGrayHome->SetVisible(false);

	CLabelUI * labelWelcome = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"labelWelcome"));
	CLabelUI * labelClass = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"labelClass"));

	if (labelWelcome == NULL ||
		labelClass == NULL)
		return false;

	DBgo* dbgo = new DBgo();
	if (dbgo == NULL)
		return false;

	struct CMS_DB_HEADER * cdh = new struct CMS_DB_HEADER;
	if (cdh == NULL)
	{
		if (dbgo != NULL)
		{
			delete dbgo;
			dbgo = NULL;
		}
		return false;
	}
	InitCDH(cdh);

	CString wszStr;
	wszStr.Format(L"%d", OperatorNo);
	if (dbgo->QueryOperator(wszStr.GetBuffer(wszStr.GetLength()),QO_BY_OPERATORNO, cdh) 
		== true &&
		cdh->n == 1)
	{
		wszStr.Format(L"%s，您好", (((struct Operator*)(cdh->next->content))->OperatorName));
		labelWelcome->SetText(wszStr.GetBuffer(wszStr.GetLength()));
		wszStr.Format(L"账户权限：%s",
			(((struct Operator*)(cdh->next->content))->Class)==1?L"收费员":L"管理员");
		labelClass->SetText(wszStr.GetBuffer(wszStr.GetLength()));
	}
	else
	{
		labelWelcome->SetText(L"数据库服务异常，请联系管理员！");
		return false;
	}

	DeleteCDH(cdh);
	if (dbgo != NULL)
	{
		delete dbgo;
		dbgo = NULL;
	}
	return true;
}

bool CMainFrameWnd::OnSwitchCharge()
{
	ctrlGrayCharge = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayCharge"));
	ctrlGrayCharge->SetVisible(false);

	editMSISDN = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editMSISDN")));
	if (editMSISDN)
	{
		editMSISDN->SetText(L"");
	}
	//初始 隐藏下半部分信息
	containerCharge = static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("containerCharge")));
	if (containerCharge)
	{
		containerCharge->SetVisible(false);
	}
	return true;
}

bool CMainFrameWnd::OnSwitchUserMgr()
{
	ctrlGrayUserMgr = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayUserMgr"));
	ctrlGrayUserMgr->SetVisible(false);
	return false;
}

bool CMainFrameWnd::OnSwitchAdmin()
{
	return false;
}

bool CMainFrameWnd::OnUserCharge(struct ChargedInfo * ci)
{
	//获取缴费金额、用户号码、缴费方式、用户余额
	editCharge = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editCharge")));
	editMSISDN = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editMSISDN")));
	CComboUI * pCombox = static_cast<CComboUI*>(m_PaintManager.FindControl(L"comboPayMethod"));
	int nsel = pCombox->GetCurSel();
	CListLabelElementUI* pItemEle = (CListLabelElementUI*)pCombox->GetItemAt(nsel);
	CTextUI* textBalance = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("textBalance")));
	bool isSucceed = false;
	
	if (editCharge != NULL && editMSISDN != NULL && pItemEle != NULL && textBalance != NULL)
	{
		//从控件获取信息
		double dCharge = 0.0;
		double dBalance = 0.0;
		swscanf_s(editCharge->GetText(), L"%lf", &dCharge);
		swscanf_s(textBalance->GetText(), L"%lf", &dBalance);
		if (dCharge <= 0.0)
		{
			//::MessageBox(GetHWND(), L"请检查“缴费金额”是否正确。", L"注意", MB_OK | MB_ICONQUESTION);
			return false;
		}
		//创建、初始化、填充结构体
		struct Charge* c = new struct Charge;
		if (c == NULL)
		{
			//::MessageBox(GetHWND(), L"无法创建Charge结构体", L"注意", MB_OK | MB_ICONQUESTION);
			return false;
		}
		memset(c, 0, sizeof(struct Charge));
		c->OperatorNo = OperatorNo;
		c->PayAmount = dCharge;
		wcscpy_s(c->MSISDN, editMSISDN->GetText());
		wcscpy_s(c->PayMethod, pItemEle->GetText());

		//写入数据库
		DBgo * dbgo = new DBgo;
		if (dbgo == NULL)
		{
			//::MessageBox(GetHWND(), L"无法创建dbgo", L"注意", MB_OK | MB_ICONQUESTION);
			if (c != NULL)
			{
				delete c;
				c = NULL;
			}
			return false;
		}

		if (dbgo->OnUserCharge(c, dBalance) == true)
		{
			//缴费成功 获取信息以填写ci
			CTextUI* textName = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("textName")));
			CString wszStr;
			std::wstring szStr1 = editMSISDN->GetText();
			std::wstring szStr2 = textName->GetText();
			wszStr.Format(L"%s （%s）", szStr1.c_str(), szStr2.c_str());
			//填写ci
			wcsncpy_s(ci->amount, editCharge->GetText(), 32);
			wcsncpy_s(ci->MSISDN_NAME, wszStr.GetBuffer(wszStr.GetLength()), wszStr.GetLength());
			wcscpy_s(ci->method, pItemEle->GetText());
			wszStr.Format(L"%d", OperatorNo);
			wcsncpy_s(ci->OperatorNo, wszStr.GetBuffer(wszStr.GetLength()), wszStr.GetLength());

			//设定返回值
			isSucceed = true;
		}
		else
			isSucceed = false;			//缴费失败

		//释放内存
		if (dbgo)
		{
			delete dbgo;
			dbgo = NULL;
		}
		if (c)
		{
			delete c;
			c = NULL;
		}
	}
	return isSucceed;
}

void CMainFrameWnd::OnQueryChargeMSISDN(LPCWSTR MSISDN)
{
	//emmmm开查数据库
	DBgo * dbgo = new DBgo();
	struct CMS_DB_HEADER * cdh = new struct CMS_DB_HEADER;
	InitCDH(cdh);

	//执行查询 根据MSISDN精确查找
	dbgo->QueryUserInfo((wchar_t*)MSISDN, QUI_BY_MSISDN | QUI_WAY_ACCURATE, cdh);

	if (cdh->n == 0 || cdh->next == NULL)
		::MessageBox(GetHWND(), L"没有找到该号码对应的用户。", L"提示", MB_OK | MB_ICONINFORMATION);
	else
	{
		//获取界面Text句柄
		CTextUI* textName = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("textName")));
		CTextUI* textBalance = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("textBalance")));
		CTextUI* textStatus = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("textStatus")));
		if (textName==NULL || textBalance == NULL ||textStatus == NULL)
		{
			::MessageBox(GetHWND(), L"一个或多个句柄分配失败。检查控件名称", L"提示", MB_OK | MB_ICONINFORMATION);
			goto EXITQUERY;
		}

		//转换“余额”格式
		CString wszStr;
		wszStr.Format(L"%.2lf", ((struct UserInfo*)(cdh->next->content))->balance);

		//赋值
		textName->SetText(((struct UserInfo*)(cdh->next->content))->name);
		textBalance->SetText(wszStr.GetBuffer(wszStr.GetLength()));
		textStatus->SetText(((struct UserInfo*)(cdh->next->content))->status);

		//根据用户套餐编号 查询套餐名称及资费
		wszStr.Format(L"%d", ((struct UserInfo*)(cdh->next->content))->PlanNo);
		dbgo->QueryPlan(wszStr.GetBuffer(wszStr.GetLength()), QP_BY_PLANNO, cdh);

		//检查是否存在该套餐
		if (cdh->n != 2)
		{
			::MessageBox(GetHWND(), L"用户当前使用的套餐已不存在，请修改为其他套餐。", L"提示", MB_OK | MB_ICONINFORMATION);
			goto EXITQUERY;
		}

		//填写套餐信息
		wszStr.Format(L"%s（长：%.2lf，市：%.2lf，漫：%.2lf）",
			((struct CallPlan*)(cdh->next->content))->PlanName, ((struct CallPlan*)(cdh->next->content))->RemoteFee,
			((struct CallPlan*)(cdh->next->content))->LocalFee, ((struct CallPlan*)(cdh->next->content))->RoamingFee);

		//赋值x2
		CTextUI* textPlan = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("textPlan")));
		if (textPlan == NULL)
		{
			::MessageBox(GetHWND(), L"一个或多个句柄分配失败。检查控件名称", L"提示", MB_OK | MB_ICONINFORMATION);
			goto EXITQUERY;
		}
		textPlan->SetText(wszStr.GetBuffer(wszStr.GetLength()));

		//显示容器中的内容
		containerCharge = static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("containerCharge")));
		if (containerCharge)
			containerCharge->SetVisible(true);
	}

EXITQUERY:
	DeleteCDH(cdh);
	if (dbgo)
	{
		delete dbgo;
		dbgo = NULL;
	}
	return;
}

void CMainFrameWnd::OnPrepare()
{
	//CLoginFrameWnd duiFrame;
	//duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	//duiFrame.CenterWindow();
	//duiFrame.ShowModal();

	return;
}

void MainThread(void * pParam)
{
	::EnableWindow((HWND)pParam, false);    //父窗口无效化
	//建立主窗口
	CMainFrameWnd *pMainWnd = new CMainFrameWnd();
	pMainWnd->Create((HWND)pParam, _T("收费管理系统"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pMainWnd->CenterWindow();
	CPaintManagerUI::MessageLoop();
}

void ChangePwdThread(void * pParam)
{
	::EnableWindow((HWND)pParam, false);    //父窗口无效化
	CSubChangePwdFrameWnd *pChangePwdWnd = new CSubChangePwdFrameWnd();
	pChangePwdWnd->Create((HWND)pParam, _T("修改密码"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pChangePwdWnd->CenterWindow();
	CPaintManagerUI::MessageLoop();
}

void ChargedThread(void * pParam)
{
	HWND m_hwnd = stackHwnd.top();
	::EnableWindow(m_hwnd, false);
	CSubChargedFrameWnd * pChargedWnd = new CSubChargedFrameWnd((struct ChargedInfo*)pParam);
	pChargedWnd->Create(m_hwnd, _T("缴费成功"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pChargedWnd->CenterWindow();
	CPaintManagerUI::MessageLoop();
}

void NewThread(void * pParam)
{
	HWND m_hwnd = stackHwnd.top();
	::EnableWindow(m_hwnd, false);
	CSubNewFrameWnd * pNewWnd = new CSubNewFrameWnd();
	pNewWnd->Create(m_hwnd, _T("开户"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pNewWnd->CenterWindow();
	CPaintManagerUI::MessageLoop();
}

void DelThread(void * pParam)
{
	HWND m_hwnd = stackHwnd.top();
	::EnableWindow(m_hwnd, false);
	CSubDelFrameWnd * pDelWnd = new CSubDelFrameWnd();
	pDelWnd->Create(m_hwnd, _T("销户"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pDelWnd->CenterWindow();
	CPaintManagerUI::MessageLoop();
}

void QnMThread(void * pParam)
{
	HWND m_hwnd = stackHwnd.top();
	::EnableWindow(m_hwnd, false);
	CSubQnMFrameWnd * pQnMWnd = new CSubQnMFrameWnd();
	pQnMWnd->Create(m_hwnd, _T("查询/修改"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pQnMWnd->CenterWindow();
	CPaintManagerUI::MessageLoop();
}

void QRThread(void * pParam)
{
	HWND m_hwnd = stackHwnd.top();
	::EnableWindow(m_hwnd, false);
	CSubQRFrameWnd * pQRWnd = new CSubQRFrameWnd();
	pQRWnd->Create(m_hwnd, _T("查询缴费记录"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pQRWnd->CenterWindow();
	CPaintManagerUI::MessageLoop();
}

void OperatorMgrThread(void * pParam)
{
	HWND m_hwnd = stackHwnd.top();
	::EnableWindow(m_hwnd, false);
	CSubOperatorMgrFrameWnd * pOperatorMgrWnd = new CSubOperatorMgrFrameWnd();
	pOperatorMgrWnd->Create(m_hwnd, _T("收费员管理"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pOperatorMgrWnd->CenterWindow();
	CPaintManagerUI::MessageLoop();
}

void ModifyOperatorThread(void * pParam)
{
	HWND m_hwnd = stackHwnd.top();
	::EnableWindow(m_hwnd, false);
	CModifyOperatorFrameWnd * pModifyOperatorWnd = 
		new CModifyOperatorFrameWnd((struct CMS_DB_NODE*)pParam);
	pModifyOperatorWnd->Create(m_hwnd, _T("修改收费员"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pModifyOperatorWnd->CenterWindow();
	CPaintManagerUI::MessageLoop();
}

void PlanMgrThread(void * pParam)
{
	HWND m_hwnd = stackHwnd.top();
	::EnableWindow(m_hwnd, false);
	CPlanMgrFrameWnd * pPlanMgrWnd = new CPlanMgrFrameWnd();
	pPlanMgrWnd->Create(m_hwnd, _T("套餐管理"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pPlanMgrWnd->CenterWindow();
	CPaintManagerUI::MessageLoop();
}

void ModifyPlanThread(void * pParam)
{
	HWND m_hwnd = stackHwnd.top();
	::EnableWindow(m_hwnd, false);
	CModifyPlanFrameWnd * pModifyPlanWnd =
		new CModifyPlanFrameWnd((struct CMS_DB_NODE*)pParam);
	pModifyPlanWnd->Create(m_hwnd, _T("修改套餐"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pModifyPlanWnd->CenterWindow();
	CPaintManagerUI::MessageLoop();
}

void IOuserThread(void * pParam)
{
	HWND m_hwnd = stackHwnd.top();
	::EnableWindow(m_hwnd, false);
	CIOuserFrameWnd * pIOuserWnd = new CIOuserFrameWnd();
	pIOuserWnd->Create(m_hwnd, _T("套餐管理"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pIOuserWnd->CenterWindow();
	CPaintManagerUI::MessageLoop();
}

LPCTSTR CSubChangePwdFrameWnd::GetWindowClassName() const{ return _T("DUILoginFrame"); }

CDuiString CSubChangePwdFrameWnd::GetSkinFile() { return _T(DUI_XML_SUB_CHANGEPWD); }

CDuiString CSubChangePwdFrameWnd::GetSkinFolder() { return _T(""); }

void CSubChangePwdFrameWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("click"))
	{
		//“修改密码”按钮
		if (msg.pSender->GetName() == _T("btnChange"))
		{
			pRawPwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editRawPwd")));
			pNewPwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNewPwd")));
			pNewPwd2Edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNewPwd2")));
			
			if (pRawPwdEdit->GetText().IsEmpty())
				pRawPwdEdit->SetFocus();
			else if (pNewPwdEdit->GetText().IsEmpty())
				pNewPwdEdit->SetFocus();
			else if (pNewPwd2Edit->GetText().IsEmpty())
				pNewPwd2Edit->SetFocus();
			else
				if (OnChangePwd() == true)
					::MessageBox(GetHWND(), L"修改成功", L"提示", MB_OK | MB_ICONINFORMATION);


		}
		//重置按钮
		else if (msg.pSender->GetName() == _T("btnReset"))
		{
			pRawPwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editRawPwd")));
			pNewPwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNewPwd")));
			pNewPwd2Edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNewPwd2")));

			pNewPwdEdit->SetText(_T(""));
			pNewPwd2Edit->SetText(_T(""));
			pRawPwdEdit->SetText(_T(""));
		}
		//关闭按钮
		else if (msg.pSender->GetName() == _T("closebtn"))
		{
			//从栈顶获取父窗口句柄并出栈
			HWND m_hwnd = stackHwnd.top();
			stackHwnd.pop();

			::EnableWindow(m_hwnd, TRUE);												// 父窗口还原 m_hwnd为子窗口句柄
			::SetFocus(m_hwnd);																	//设置焦点到父窗口
			::PostMessage(m_hwnd, WM_CMS_BACKTOPARENT, 0, 0);		//向父窗口发送消息
			Close(); 
		}
	}//if click
}

LRESULT CSubChangePwdFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			return true;
		}
		else if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return true;
		}
	default:
		return false;
	}
}

LRESULT CSubChangePwdFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;	//标志着消息是否被处理
	switch (uMsg)
	{
	case WM_NCLBUTTONDBLCLK: return 0;//禁用双击标题栏窗口最大化：屏蔽非客户区左键双击事件
	default: bHandled = FALSE;
	}
	//如果已处理消息 则直接返回
	if (bHandled) return lRes;
	//如果由前述MessageHandler处理了消息 则直接返回
	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	//其余的还给WIB处理...
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CSubChangePwdFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_PaintManager.Init(m_hWnd);
	CDialogBuilder builder;
	//CDialogBuilderCallbackEx cb;
	CControlUI* pRoot = builder.Create(_T(DUI_XML_SUB_CHANGEPWD), (UINT)0, NULL, &m_PaintManager);
	ASSERT(pRoot && "Failed to parse XML");
	m_PaintManager.AttachDialog(pRoot);
	m_PaintManager.AddNotifier(this);


	m_WndShadow.Create(m_hWnd);
	m_WndShadow.SetSize(3);
	m_WndShadow.SetPosition(0, 0);

	//Init();
	return 0;
}

bool CSubChangePwdFrameWnd::OnChangePwd()
{
	bool isSucceed = false;
	pRawPwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editRawPwd")));
	pNewPwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNewPwd")));
	pNewPwd2Edit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editNewPwd2")));

	//检查两次输入的新密码是否一致
	if (pNewPwdEdit->GetText() != pNewPwd2Edit->GetText())
	{
		::MessageBox(GetHWND(), L"两次输入的新密码不一样哦~", L"提示", MB_OK | MB_ICONQUESTION);
		return false;
	}
	//检查原密码是否正确
	CString wszStr;
	wszStr.Format(L"%d", OperatorNo);
	if (CLoginFrameWnd::OnLogin(wszStr.GetBuffer(wszStr.GetLength()), pRawPwdEdit->GetText())
		== false)
	{
		::MessageBox(GetHWND(), L"原密码不对哦~", L"提示", MB_OK | MB_ICONQUESTION);
		return false;
	}

	//连接数据库 执行更新语句
	DBgo * dbgo = new DBgo;
	if (dbgo == NULL)
		return false;
	//
	if (dbgo->OnOperatorChangePwd(OperatorNo, pNewPwdEdit->GetText()) 
		== true)
		isSucceed = true;
	else
	{
		::MessageBox(GetHWND(), L"数据库语句执行失败", L"提示", MB_OK | MB_ICONERROR);
		isSucceed = false;
	}
	
	if (dbgo)
	{
		delete dbgo;
		dbgo = NULL;
	}
	return isSucceed;
}

LPCTSTR CSubChargedFrameWnd::GetWindowClassName() const { return _T("DUILoginFrame"); }

CDuiString CSubChargedFrameWnd::GetSkinFile() { return _T(DUI_XML_SUB_CHANGEPWD); }

CDuiString CSubChargedFrameWnd::GetSkinFolder() { return _T(""); }

void CSubChargedFrameWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("click"))
	{
		//“确定”按钮
		if (msg.pSender->GetName() == _T("btnOK"))
		{
			//从栈顶获取父窗口句柄并出栈
			HWND m_hwnd = stackHwnd.top();
			stackHwnd.pop();

			::EnableWindow(m_hwnd, TRUE);												// 父窗口还原 m_hwnd为子窗口句柄
			::SetFocus(m_hwnd);																	//设置焦点到父窗口
			::PostMessage(m_hwnd, WM_CMS_BACKTOPARENT, 0, 0);		//向父窗口发送消息
			Close();
		}
		//“打印发票”按钮
		else if (msg.pSender->GetName() == _T("btnPrint"))
		{
			//如果失败 尝试打印3次
			if (OnCreateReceipt() == false)
			{
				if (OnCreateReceipt() == false)
				{
					if (OnCreateReceipt() == false)
					{
						::MessageBox(m_hWnd, L"发票打印失败！", L"注意", MB_OK | MB_ICONERROR);
					}
				}
			}
		}
		//关闭按钮
		else if (msg.pSender->GetName() == _T("closebtn"))
		{
			//从栈顶获取父窗口句柄并出栈
			HWND m_hwnd = stackHwnd.top();
			stackHwnd.pop();

			::EnableWindow(m_hwnd, TRUE);												// 父窗口还原 m_hwnd为子窗口句柄
			::SetFocus(m_hwnd);																	//设置焦点到父窗口
			::PostMessage(m_hwnd, WM_CMS_BACKTOPARENT, 0, 0);		//向父窗口发送消息
			Close();
		}
	}//if click
}

CSubChargedFrameWnd::CSubChargedFrameWnd(ChargedInfo * ci)
{
	pci = new struct ChargedInfo;

	if (pci)
	{
		memset(pci, 0, sizeof(struct ChargedInfo));
		memcpy(pci, ci, sizeof(struct ChargedInfo));
	}
}

void CSubChargedFrameWnd::Init()
{
	labelMSISDN_NAME = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"labelMSISDN_NAME"));
	labelAmount = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"labelAmount"));

	if (labelMSISDN_NAME == NULL ||
		labelAmount == NULL ||
		pci == NULL)
	{
		::MessageBox(m_hWnd, L"???", L"...", MB_OK | MB_ICONERROR);
		return;
	}

	labelMSISDN_NAME->SetText(pci->MSISDN_NAME);
	labelAmount->SetText(pci->amount);
	return;
}

LRESULT CSubChargedFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			return true;
		}
		else if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return true;
		}
	default:
		return false;
	}
}

LRESULT CSubChargedFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;	//标志着消息是否被处理
	switch (uMsg)
	{
	case WM_NCLBUTTONDBLCLK: return 0;//禁用双击标题栏窗口最大化：屏蔽非客户区左键双击事件
	default: bHandled = FALSE;
	}
	//如果已处理消息 则直接返回
	if (bHandled) return lRes;
	//如果由前述MessageHandler处理了消息 则直接返回
	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	//其余的还给WIB处理...
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CSubChargedFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_PaintManager.Init(m_hWnd);
	CDialogBuilder builder;
	//CDialogBuilderCallbackEx cb;
	CControlUI* pRoot = builder.Create(_T(DUI_XML_SUB_CHARGED), (UINT)0, NULL, &m_PaintManager);
	ASSERT(pRoot && "Failed to parse XML");
	m_PaintManager.AttachDialog(pRoot);
	m_PaintManager.AddNotifier(this);


	//m_WndShadow.Create(m_hWnd);
	//m_WndShadow.SetSize(3);
	//m_WndShadow.SetPosition(0, 0);

	Init();
	return 0;
}

bool CSubChargedFrameWnd::OnCreateReceipt()
{
	bool isSucceed = false;
	//获取当前时间
	SYSTEMTIME st;
	GetLocalTime(&st);
	CString wszStr;
	wszStr.Format(L"%4d-%2d-%2d %2d:%2d:%2d",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	//转换金额
	double dCharge = 0.0;
	swscanf_s(pci->amount, L"%lf", &dCharge);

	CString ReceiptContent = "邮电局收费发票\n";
	ReceiptContent += "-------------------------------------\n时间：";
	ReceiptContent += wszStr;																//自动获取时间
	ReceiptContent += "\n用户：";
	ReceiptContent += pci->MSISDN_NAME;										//号码、姓名
	ReceiptContent += "\n缴费金额：";
	ReceiptContent += pci->amount;													//金额
	ReceiptContent += "元\n金额大写：";
	ReceiptContent += ConvertMoneyCaps(dCharge).c_str();				//大写金额
	ReceiptContent += "\n缴费方式：";
	ReceiptContent += pci->method;													//缴费方式
	ReceiptContent += "\n收费员工号：";
	ReceiptContent += pci->OperatorNo;											//工号
	ReceiptContent += "\n-------------------------------------\n软件开发实践3\n150420220 魏子豪";

	isSucceed = CreatePDF(ReceiptContent);
	return isSucceed;
}

LPCTSTR CSubNewFrameWnd::GetWindowClassName() const { return _T("DUILoginFrame"); }

CDuiString CSubNewFrameWnd::GetSkinFile() { return _T(DUI_XML_SUB_NEW); }

CDuiString CSubNewFrameWnd::GetSkinFolder() { return _T(""); }

void CSubNewFrameWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("click"))
	{
		//“开户”按钮
		if (msg.pSender->GetName() == _T("btnNew"))
		{	
			if (OnNewUser() == true)
				::MessageBox(m_hWnd, L"开户成功~", L"提示", MB_OK | MB_ICONINFORMATION);
			else
				::MessageBox(m_hWnd, L"开户失败了...看看有没有没填的、有没有选套餐~？", L"提示", MB_OK | MB_ICONERROR);
		}
		//“重置”按钮
		else if (msg.pSender->GetName() == _T("btnReset"))
		{
			CEditUI * edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editName")));
			edits->SetText(L"");
			edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editGender")));
			edits->SetText(L"");
			edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editAddr")));
			edits->SetText(L"");
			edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editMSISDN")));
			edits->SetText(L"");
			edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editCharge")));
			edits->SetText(L"");
		}
		//关闭按钮
		else if (msg.pSender->GetName() == _T("closebtn"))
		{
			//从栈顶获取父窗口句柄并出栈
			HWND m_hwnd = stackHwnd.top();
			stackHwnd.pop();

			::EnableWindow(m_hwnd, TRUE);												// 父窗口还原 m_hwnd为子窗口句柄
			::SetFocus(m_hwnd);																	//设置焦点到父窗口
			::PostMessage(m_hwnd, WM_CMS_BACKTOPARENT, 0, 0);		//向父窗口发送消息
			Close();
		}
	}//if click
}

void CSubNewFrameWnd::Init()
{
	////初始化：搞一下list中的元素
	//获取数据库中的套餐内容
	struct CMS_DB_HEADER * cdh = new struct CMS_DB_HEADER;
	InitCDH(cdh);
	DBgo * dbgo = new DBgo;
	dbgo->QueryPlan(L"", QP_BY_PLANNAME, cdh);

	//添加list列表内容
	CListUI * listPlan = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listPlan")));
	struct CMS_DB_NODE * cdn = cdh->next;
	CString wszStr;
	// 添加List列表内容，必须先Add(pListElement)，再SetText
	while(cdn != NULL)
	{
		CListTextElementUI* pListElement = new CListTextElementUI;
		pListElement->SetTag(((struct CallPlan*)(cdn->content))->PlanNo);		//用PlanNo作为tag
		listPlan->Add(pListElement);

		wszStr.Format(L"   %d", ((struct CallPlan*)(cdn->content))->PlanNo);
		pListElement->SetText(0, wszStr.GetBuffer(wszStr.GetLength()));
		pListElement->SetText(1, ((struct CallPlan*)(cdn->content))->PlanName);
		wszStr.Format(L"   %.2lf", ((struct CallPlan*)(cdn->content))->RemoteFee);
		pListElement->SetText(2, wszStr.GetBuffer(wszStr.GetLength()));
		wszStr.Format(L"   %.2lf", ((struct CallPlan*)(cdn->content))->LocalFee);
		pListElement->SetText(3, wszStr.GetBuffer(wszStr.GetLength()));
		wszStr.Format(L"   %.2lf", ((struct CallPlan*)(cdn->content))->RoamingFee);
		pListElement->SetText(4, wszStr.GetBuffer(wszStr.GetLength()));

		cdn = cdn->next;
	}

	DeleteCDH(cdh);
	delete dbgo;
	dbgo = NULL;

	return;
}

LRESULT CSubNewFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			return true;
		}
		else if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return true;
		}
	default:
		return false;
	}
}

LRESULT CSubNewFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;	//标志着消息是否被处理
	switch (uMsg)
	{
	case WM_NCLBUTTONDBLCLK: return 0;//禁用双击标题栏窗口最大化：屏蔽非客户区左键双击事件
	default: bHandled = FALSE;
	}
	//如果已处理消息 则直接返回
	if (bHandled) return lRes;
	//如果由前述MessageHandler处理了消息 则直接返回
	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	//其余的还给WIB处理...
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CSubNewFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_PaintManager.Init(m_hWnd);
	CDialogBuilder builder;
	//CDialogBuilderCallbackEx cb;
	CControlUI* pRoot = builder.Create(_T(DUI_XML_SUB_NEW), (UINT)0, NULL, &m_PaintManager);
	ASSERT(pRoot && "Failed to parse XML");
	m_PaintManager.AttachDialog(pRoot);
	m_PaintManager.AddNotifier(this);


	//m_WndShadow.Create(m_hWnd);
	//m_WndShadow.SetSize(3);
	//m_WndShadow.SetPosition(0, 0);

	Init();
	return 0;
}

bool CSubNewFrameWnd::OnNewUser()
{
	bool isSucceed = false;
	////获取所选套餐
	CListUI * listPlan = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listPlan")));
	CListTextElementUI * pListElement = new CListTextElementUI;
	int nIndex = listPlan->GetCurSel();
	if (nIndex < 0)
		return false;
	pListElement = (CListTextElementUI*)listPlan->GetItemAt(nIndex);
	CLabelUI* labelPlan = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelPlan")));
	labelPlan->SetText(pListElement->GetText(1));

	////创建UserInfo
	struct UserInfo * pui = new struct UserInfo;
	memset(pui, 0, sizeof(struct UserInfo));

	////填充UserInfo
	//获取字符串
	CEditUI * edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editName")));
	if (edits->GetText().GetLength() == 0)
	{
		edits->SetFocus();
		return false;
	}
	wcsncpy_s(pui->name, edits->GetText(), edits->GetText().GetLength());
	edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editGender")));
	if (edits->GetText().GetLength() == 0)
	{
		edits->SetFocus();
		return false;
	}
	wcsncpy_s(pui->gender, edits->GetText(), edits->GetText().GetLength());
	edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editAddr")));
	if (edits->GetText().GetLength() == 0)
	{
		edits->SetFocus();
		return false;
	}
	wcsncpy_s(pui->address, edits->GetText(), edits->GetText().GetLength());
	edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editMSISDN")));
	if (edits->GetText().GetLength() == 0)
	{
		edits->SetFocus();
		return false;
	}
	wcsncpy_s(pui->MSISDN, edits->GetText(), edits->GetText().GetLength());
	wcsncpy_s(pui->status, L"正常", wcslen(L"正常"));
	
	//获取数字
	int PlanNO = 0;
	double dCharge = 0.0;
	edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editCharge")));
	swscanf_s(edits->GetText(), L"%lf", &dCharge);
	swscanf_s(pListElement->GetText(0), L"%d", &PlanNO);
	pui->PlanNo = PlanNO;
	pui->balance = dCharge;

	////连接数据库 
	DBgo * dbgo = new DBgo;
	if (dbgo->AddUserInfo(pui) == true)
		isSucceed = true;
	else
		isSucceed = false;

	////结束
	delete dbgo;
	dbgo = NULL;
	delete pui;
	pui = NULL;
	return true;
}

LPCTSTR CSubDelFrameWnd::GetWindowClassName() const { return _T("DUILoginFrame"); }

CDuiString CSubDelFrameWnd::GetSkinFile() { return _T(DUI_XML_SUB_DEL); }

CDuiString CSubDelFrameWnd::GetSkinFolder() { return _T(""); }

void CSubDelFrameWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("click"))
	{
		//“查询”按钮
		if (msg.pSender->GetName() == _T("btnQuery"))
		{
			//获取号码 调用处理函数
			CEditUI* editMSISDN = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editMSISDN")));
			if (editMSISDN && editMSISDN->GetText().GetLength() > 0)
			{
				if (!OnQueryUserInfo(editMSISDN->GetText()))
					::MessageBox(GetHWND(), L"查询失败...号码是否存在？", L"注意", MB_OK | MB_ICONINFORMATION);
			}
			else
				::MessageBox(GetHWND(), L"请填写用户电话号码...", L"注意", MB_OK | MB_ICONINFORMATION);
		}
		//“销户”按钮
		else if (msg.pSender->GetName() == _T("btnDel"))
		{
			//获取号码 调用处理函数
			CEditUI* editMSISDN = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editMSISDN")));
			if (editMSISDN && editMSISDN->GetText().GetLength() > 0)
				if(OnDelUser(editMSISDN->GetText()))
					::MessageBox(GetHWND(), L"销户成功", L"注意", MB_OK | MB_ICONINFORMATION);
				else
					::MessageBox(GetHWND(), L"销户失败...号码是否存在？", L"注意", MB_OK | MB_ICONINFORMATION);
			else
			{
				::MessageBox(GetHWND(), L"请填写用户电话号码...", L"注意", MB_OK | MB_ICONINFORMATION);
				editMSISDN->SetFocus();
			}
		}
		//关闭按钮
		else if (msg.pSender->GetName() == _T("closebtn"))
		{
			//从栈顶获取父窗口句柄并出栈
			HWND m_hwnd = stackHwnd.top();
			stackHwnd.pop();

			::EnableWindow(m_hwnd, TRUE);												// 父窗口还原 m_hwnd为子窗口句柄
			::SetFocus(m_hwnd);																	//设置焦点到父窗口
			::PostMessage(m_hwnd, WM_CMS_BACKTOPARENT, 0, 0);		//向父窗口发送消息
			Close();
		}
	}//if click
}

void CSubDelFrameWnd::Init()
{
	return;
}

LRESULT CSubDelFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			return true;
		}
		else if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return true;
		}
	default:
		return false;
	}
}

LRESULT CSubDelFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;	//标志着消息是否被处理
	switch (uMsg)
	{
	case WM_NCLBUTTONDBLCLK: return 0;//禁用双击标题栏窗口最大化：屏蔽非客户区左键双击事件
	default: bHandled = FALSE;
	}
	//如果已处理消息 则直接返回
	if (bHandled) return lRes;
	//如果由前述MessageHandler处理了消息 则直接返回
	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	//其余的还给WIB处理...
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CSubDelFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_PaintManager.Init(m_hWnd);
	CDialogBuilder builder;
	//CDialogBuilderCallbackEx cb;
	CControlUI* pRoot = builder.Create(_T(DUI_XML_SUB_DEL), (UINT)0, NULL, &m_PaintManager);
	ASSERT(pRoot && "Failed to parse XML");
	m_PaintManager.AttachDialog(pRoot);
	m_PaintManager.AddNotifier(this);


	//m_WndShadow.Create(m_hWnd);
	//m_WndShadow.SetSize(3);
	//m_WndShadow.SetPosition(0, 0);

	Init();
	return 0;
}

bool CSubDelFrameWnd::OnQueryUserInfo(LPCWSTR MSISDN)
{
	bool isSucceed = false;
	//emmmm开查数据库
	DBgo * dbgo = new DBgo();
	struct CMS_DB_HEADER * cdh = new struct CMS_DB_HEADER;
	InitCDH(cdh);

	//执行查询 根据MSISDN精确查找
	dbgo->QueryUserInfo((wchar_t*)MSISDN, QUI_BY_MSISDN | QUI_WAY_ACCURATE, cdh);

	if (cdh->n == 0 || cdh->next == NULL)
	{
		::MessageBox(GetHWND(), L"没有找到该号码对应的用户。", L"提示", MB_OK | MB_ICONINFORMATION);
		isSucceed = false;
	}
	else
	{
		//获取界面Text句柄
		CLabelUI* textName = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("textName")));
		CLabelUI* textBalance = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("textBalance")));
		CLabelUI* textStatus = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("textStatus")));
		if (textName == NULL || textBalance == NULL || textStatus == NULL)
		{
			::MessageBox(GetHWND(), L"一个或多个句柄分配失败。检查控件名称", L"提示", MB_OK | MB_ICONINFORMATION);
			isSucceed = false;
			goto EXITQUERY;
		}

		//转换“余额”格式
		CString wszStr;
		wszStr.Format(L"%.2lf", ((struct UserInfo*)(cdh->next->content))->balance);

		//赋值
		textName->SetText(((struct UserInfo*)(cdh->next->content))->name);
		textBalance->SetText(wszStr.GetBuffer(wszStr.GetLength()));
		textStatus->SetText(((struct UserInfo*)(cdh->next->content))->status);

		//根据用户套餐编号 查询套餐名称及资费
		wszStr.Format(L"%d", ((struct UserInfo*)(cdh->next->content))->PlanNo);
		dbgo->QueryPlan(wszStr.GetBuffer(wszStr.GetLength()), QP_BY_PLANNO, cdh);

		//检查是否存在该套餐
		if (cdh->n != 2)
		{
			::MessageBox(GetHWND(), L"用户当前使用的套餐已不存在，请修改为其他套餐。", L"提示", MB_OK | MB_ICONINFORMATION);
			isSucceed = false;
			goto EXITQUERY;
		}

		//填写套餐信息
		wszStr.Format(L"%s（长：%.2lf，市：%.2lf，漫：%.2lf）",
			((struct CallPlan*)(cdh->next->content))->PlanName, ((struct CallPlan*)(cdh->next->content))->RemoteFee,
			((struct CallPlan*)(cdh->next->content))->LocalFee, ((struct CallPlan*)(cdh->next->content))->RoamingFee);

		//赋值x2
		CLabelUI* textPlan = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("textPlan")));
		if (textPlan == NULL)
		{
			::MessageBox(GetHWND(), L"一个或多个句柄分配失败。检查控件名称", L"提示", MB_OK | MB_ICONINFORMATION);
			isSucceed = false;
			goto EXITQUERY;
		}
		textPlan->SetText(wszStr.GetBuffer(wszStr.GetLength()));
	}
	isSucceed = true;

EXITQUERY:
	DeleteCDH(cdh);
	if (dbgo)
	{
		delete dbgo;
		dbgo = NULL;
	}
	return isSucceed;
}

bool CSubDelFrameWnd::OnDelUser(LPCWSTR MSISDN)
{
	bool isSucceed = false;

	DBgo * dbgo = new DBgo;
	isSucceed = dbgo->DropUserInfo((wchar_t*)MSISDN, DUI_WAY_BY_MSISDN);

	delete dbgo;
	dbgo = NULL;
	return isSucceed;
}

LPCTSTR CSubQnMFrameWnd::GetWindowClassName() const { return _T("DUILoginFrame"); }

CDuiString CSubQnMFrameWnd::GetSkinFile() { return _T(DUI_XML_SUB_QNM); }

CDuiString CSubQnMFrameWnd::GetSkinFolder() { return _T(""); }

void CSubQnMFrameWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("click"))
	{
		//“查询”按钮
		if (msg.pSender->GetName() == _T("btnQuery"))
		{
			//获取号码 调用处理函数
			CEditUI* editMSISDN = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editMSISDN")));
			if (editMSISDN && editMSISDN->GetText().GetLength() > 0)
			{
				if (!OnQueryUserInfo(editMSISDN->GetText()))
					::MessageBox(GetHWND(), L"查询失败...号码是否存在？", L"注意", MB_OK | MB_ICONINFORMATION);
				else
					isQueried = true;
			}
			else
				::MessageBox(GetHWND(), L"请填写用户电话号码...", L"注意", MB_OK | MB_ICONINFORMATION);
		}
		//“修改”按钮
		else if (msg.pSender->GetName() == _T("btnModify"))
		{
			//if (!isQueried)
			//{
			//	::MessageBox(m_hWnd, L"请先查询...", L"提示", MB_OK | MB_ICONERROR);
			//	return;
			//}
			//获取号码 调用处理函数
			CEditUI* editMSISDN = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editMSISDN")));
			if (editMSISDN && editMSISDN->GetText().GetLength() > 0)
			{
				if (OnModifyUserInfo(editMSISDN->GetText()))
				{
					::MessageBox(GetHWND(), L"修改成功", L"注意", MB_OK | MB_ICONINFORMATION);
					isQueried = false;
				}
				else
					::MessageBox(GetHWND(), 
						L"修改失败...请确认号码是否存在、是否已选择套餐、是否填写了所有字段",
						L"注意",
						MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				::MessageBox(GetHWND(), L"请填写用户电话号码...", L"注意", MB_OK | MB_ICONINFORMATION);
				editMSISDN->SetFocus();
			}
		}
		//关闭按钮
		else if (msg.pSender->GetName() == _T("closebtn"))
		{
			//从栈顶获取父窗口句柄并出栈
			HWND m_hwnd = stackHwnd.top();
			stackHwnd.pop();

			::EnableWindow(m_hwnd, TRUE);												// 父窗口还原 m_hwnd为子窗口句柄
			::SetFocus(m_hwnd);																	//设置焦点到父窗口
			::PostMessage(m_hwnd, WM_CMS_BACKTOPARENT, 0, 0);		//向父窗口发送消息
			Close();
		}
	}//if click
}

void CSubQnMFrameWnd::Init()
{
	isQueried = false;

	////初始化：搞一下list中的元素
	//获取数据库中的套餐内容
	struct CMS_DB_HEADER * cdh = new struct CMS_DB_HEADER;
	InitCDH(cdh);
	DBgo * dbgo = new DBgo;
	dbgo->QueryPlan(L"", QP_BY_PLANNAME, cdh);

	//添加list列表内容
	CListUI * listPlan = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listPlan")));
	struct CMS_DB_NODE * cdn = cdh->next;
	CString wszStr;
	// 添加List列表内容，必须先Add(pListElement)，再SetText
	while (cdn != NULL)
	{
		CListTextElementUI* pListElement = new CListTextElementUI;
		pListElement->SetTag(((struct CallPlan*)(cdn->content))->PlanNo);		//用PlanNo作为tag
		listPlan->Add(pListElement);

		wszStr.Format(L"   %d", ((struct CallPlan*)(cdn->content))->PlanNo);
		pListElement->SetText(0, wszStr.GetBuffer(wszStr.GetLength()));
		pListElement->SetText(1, ((struct CallPlan*)(cdn->content))->PlanName);
		wszStr.Format(L"   %.2lf", ((struct CallPlan*)(cdn->content))->RemoteFee);
		pListElement->SetText(2, wszStr.GetBuffer(wszStr.GetLength()));
		wszStr.Format(L"   %.2lf", ((struct CallPlan*)(cdn->content))->LocalFee);
		pListElement->SetText(3, wszStr.GetBuffer(wszStr.GetLength()));
		wszStr.Format(L"   %.2lf", ((struct CallPlan*)(cdn->content))->RoamingFee);
		pListElement->SetText(4, wszStr.GetBuffer(wszStr.GetLength()));

		cdn = cdn->next;
	}

	DeleteCDH(cdh);
	delete dbgo;
	dbgo = NULL;

	return;
}

LRESULT CSubQnMFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			return true;
		}
		else if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return true;
		}
	default:
		return false;
	}
}

LRESULT CSubQnMFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;	//标志着消息是否被处理
	switch (uMsg)
	{
	case WM_NCLBUTTONDBLCLK: return 0;//禁用双击标题栏窗口最大化：屏蔽非客户区左键双击事件
	default: bHandled = FALSE;
	}
	//如果已处理消息 则直接返回
	if (bHandled) return lRes;
	//如果由前述MessageHandler处理了消息 则直接返回
	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	//其余的还给WIB处理...
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CSubQnMFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_PaintManager.Init(m_hWnd);
	CDialogBuilder builder;
	//CDialogBuilderCallbackEx cb;
	CControlUI* pRoot = builder.Create(_T(DUI_XML_SUB_QNM), (UINT)0, NULL, &m_PaintManager);
	ASSERT(pRoot && "Failed to parse XML");
	m_PaintManager.AttachDialog(pRoot);
	m_PaintManager.AddNotifier(this);


	//m_WndShadow.Create(m_hWnd);
	//m_WndShadow.SetSize(3);
	//m_WndShadow.SetPosition(0, 0);

	Init();
	return 0;
}

bool CSubQnMFrameWnd::OnQueryUserInfo(LPCWSTR MSISDN)
{
	bool isSucceed = false;
	//emmmm开查数据库
	DBgo * dbgo = new DBgo();
	struct CMS_DB_HEADER * cdh = new struct CMS_DB_HEADER;
	InitCDH(cdh);

	//执行查询 根据MSISDN精确查找
	dbgo->QueryUserInfo((wchar_t*)MSISDN, QUI_BY_MSISDN | QUI_WAY_ACCURATE, cdh);

	if (cdh->n == 0 || cdh->next == NULL)
	{
		::MessageBox(GetHWND(), L"没有找到该号码对应的用户。", L"提示", MB_OK | MB_ICONINFORMATION);
		isSucceed = false;
	}
	else
	{
		//获取界面Text句柄
		CEditUI* textName = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editName")));
		CEditUI* textGender = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editGender")));
		CEditUI* textAddr = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editAddr")));
		CEditUI* textStatus = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editStatus")));
		CLabelUI* textBalance = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"labelBalance"));
		if (textName == NULL || textGender == NULL || textAddr == NULL)
		{
			::MessageBox(GetHWND(), L"一个或多个句柄分配失败。检查控件名称", L"提示", MB_OK | MB_ICONINFORMATION);
			isSucceed = false;
			goto EXITQUERY;
		}

		//转换“余额”格式
		CString wszStr;
		wszStr.Format(L"%.2lf", ((struct UserInfo*)(cdh->next->content))->balance);

		//赋值
		textName->SetText(((struct UserInfo*)(cdh->next->content))->name);
		textGender->SetText(((struct UserInfo*)(cdh->next->content))->gender);
		textAddr->SetText(((struct UserInfo*)(cdh->next->content))->address);
		textBalance->SetText(wszStr.GetBuffer(wszStr.GetLength()));
		textStatus->SetText(((struct UserInfo*)(cdh->next->content))->status);

		//根据用户套餐编号 查询套餐名称及资费
		wszStr.Format(L"%d", ((struct UserInfo*)(cdh->next->content))->PlanNo);
		dbgo->QueryPlan(wszStr.GetBuffer(wszStr.GetLength()), QP_BY_PLANNO, cdh);

		//检查是否存在该套餐
		if (cdh->n != 2)
		{
			::MessageBox(GetHWND(), L"用户当前使用的套餐已不存在，请修改为其他套餐。", L"提示", MB_OK | MB_ICONINFORMATION);
			isSucceed = false;
			goto EXITQUERY;
		}

		//填写套餐信息
		wszStr.Format(L"%s（长：%.2lf，市：%.2lf，漫：%.2lf）",
			((struct CallPlan*)(cdh->next->content))->PlanName, ((struct CallPlan*)(cdh->next->content))->RemoteFee,
			((struct CallPlan*)(cdh->next->content))->LocalFee, ((struct CallPlan*)(cdh->next->content))->RoamingFee);

		//赋值x2
		CLabelUI* textPlan = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelPlan")));
		if (textPlan == NULL)
		{
			::MessageBox(GetHWND(), L"一个或多个句柄分配失败。检查控件名称", L"提示", MB_OK | MB_ICONINFORMATION);
			isSucceed = false;
			goto EXITQUERY;
		}
		textPlan->SetText(wszStr.GetBuffer(wszStr.GetLength()));
	}
	isSucceed = true;

EXITQUERY:
	DeleteCDH(cdh);
	if (dbgo)
	{
		delete dbgo;
		dbgo = NULL;
	}
	return isSucceed;
}

bool CSubQnMFrameWnd::OnModifyUserInfo(LPCWSTR MSISDN)
{
	bool isSucceed = false;
	////获取所选套餐
	CListUI * listPlan = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listPlan")));
	CListTextElementUI * pListElement = new CListTextElementUI;
	int nIndex = listPlan->GetCurSel();
	if (nIndex < 0)
		return false;
	pListElement = (CListTextElementUI*)listPlan->GetItemAt(nIndex);
	CLabelUI* labelPlan = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelPlan")));
	labelPlan->SetText(pListElement->GetText(1));

	////创建UserInfo
	struct UserInfo * pui = new struct UserInfo;
	memset(pui, 0, sizeof(struct UserInfo));

	////填充UserInfo
	//获取字符串
	CEditUI * 	edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editMSISDN")));
	if (edits->GetText().GetLength() == 0)
	{
		edits->SetFocus();
		return false;
	}
	wcsncpy_s(pui->MSISDN, edits->GetText(), edits->GetText().GetLength());
	//首先删除原信息 再插入新信息 完成修改
	DBgo * dbgo = new DBgo;
	isSucceed = dbgo->DropUserInfo((wchar_t*)MSISDN, DUI_WAY_BY_MSISDN);
	//继续填充
	 edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editName")));
	if (edits->GetText().GetLength() == 0)
	{
		edits->SetFocus();
		return false;
	}
	wcsncpy_s(pui->name, edits->GetText(), edits->GetText().GetLength());
	edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editGender")));
	if (edits->GetText().GetLength() == 0)
	{
		edits->SetFocus();
		return false;
	}
	wcsncpy_s(pui->gender, edits->GetText(), edits->GetText().GetLength());
	edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editAddr")));
	if (edits->GetText().GetLength() == 0)
	{
		edits->SetFocus();
		return false;
	}
	wcsncpy_s(pui->address, edits->GetText(), edits->GetText().GetLength());
	edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editStatus")));
	if (edits->GetText().GetLength() == 0)
	{
		edits->SetFocus();
		return false;
	}
	wcsncpy_s(pui->status, edits->GetText(), edits->GetText().GetLength());

	//获取数字
	int PlanNO = 0;
	double dCharge = 0.0;
	//edits = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editCharge")));
	CLabelUI * labels = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"labelBalance"));
	swscanf_s(labels->GetText(), L"%lf", &dCharge);
	swscanf_s(pListElement->GetText(0), L"%d", &PlanNO);
	pui->PlanNo = PlanNO;
	pui->balance = dCharge;

	////连接数据库 
	//DBgo * dbgo = new DBgo;
	if (dbgo->AddUserInfo(pui) == true)
		isSucceed = true;
	else
		isSucceed = false;

	////结束
	delete dbgo;
	dbgo = NULL;
	delete pui;
	pui = NULL;
	return true;
}

LPCTSTR CSubQRFrameWnd::GetWindowClassName() const { return _T("DUILoginFrame"); }

CDuiString CSubQRFrameWnd::GetSkinFile() { return _T(DUI_XML_SUB_QR); }

CDuiString CSubQRFrameWnd::GetSkinFolder() { return _T(""); }

void CSubQRFrameWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("click"))
	{
		//“查询”按钮
		if (msg.pSender->GetName() == _T("btnQuery"))
		{
			CEditUI* editMSISDN = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editMSISDN"));
			if (editMSISDN && editMSISDN->GetText().GetLength() != 0)
			{
				if (OnQueryRecords(editMSISDN->GetText()))
					//::MessageBox(m_hWnd, L"数据库出错啦", L"提示", MB_OK | MB_ICONERROR);

				//else
					isQueried = true;
			}
			else
				::MessageBox(m_hWnd, L"请先输入号码~", L"提示", MB_OK | MB_ICONINFORMATION);
		}
		//“导出”按钮
		else if (msg.pSender->GetName() == _T("btnOut"))
		{
			CEditUI* editMSISDN = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editMSISDN"));
			if (editMSISDN && editMSISDN->GetText().GetLength() != 0)
			{
				if (!OnOut(editMSISDN->GetText()))
					::MessageBox(m_hWnd, L"缴费记录导出失败", L"提示", MB_OK | MB_ICONERROR);
				else
					::MessageBox(m_hWnd, L"缴费记录导出成功", L"提示", MB_OK | MB_ICONINFORMATION);
			}
			else
				::MessageBox(m_hWnd, L"请先输入号码~", L"提示", MB_OK | MB_ICONINFORMATION);
		}
		//关闭按钮
		else if (msg.pSender->GetName() == _T("closebtn") ||
			msg.pSender->GetName() == L"btnClose")
		{
			//从栈顶获取父窗口句柄并出栈
			HWND m_hwnd = stackHwnd.top();
			stackHwnd.pop();

			::EnableWindow(m_hwnd, TRUE);												// 父窗口还原 m_hwnd为子窗口句柄
			::SetFocus(m_hwnd);																	//设置焦点到父窗口
			::PostMessage(m_hwnd, WM_CMS_BACKTOPARENT, 0, 0);		//向父窗口发送消息
			Close();
		}
	}
}

void CSubQRFrameWnd::Init()
{
	isQueried = false;
}

LRESULT CSubQRFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			return true;
		}
		else if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return true;
		}
	default:
		return false;
	}
}

LRESULT CSubQRFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;	//标志着消息是否被处理
	switch (uMsg)
	{
	case WM_NCLBUTTONDBLCLK: return 0;//禁用双击标题栏窗口最大化：屏蔽非客户区左键双击事件
	default: bHandled = FALSE;
	}
	//如果已处理消息 则直接返回
	if (bHandled) return lRes;
	//如果由前述MessageHandler处理了消息 则直接返回
	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	//其余的还给WIB处理...
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CSubQRFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_PaintManager.Init(m_hWnd);
	CDialogBuilder builder;
	//CDialogBuilderCallbackEx cb;
	CControlUI* pRoot = builder.Create(_T(DUI_XML_SUB_QR), (UINT)0, NULL, &m_PaintManager);
	ASSERT(pRoot && "Failed to parse XML");
	m_PaintManager.AttachDialog(pRoot);
	m_PaintManager.AddNotifier(this);


	//m_WndShadow.Create(m_hWnd);
	//m_WndShadow.SetSize(3);
	//m_WndShadow.SetPosition(0, 0);

	Init();
	return 0;
}

bool CSubQRFrameWnd::OnQueryRecords(LPCWSTR MSISDN)
{
	bool isSucceed = false;

	cdh = new struct CMS_DB_HEADER;
	DBgo * dbgo = new DBgo;

	if (cdh == NULL || dbgo == NULL)
		isSucceed = false;
	else
	{
		InitCDH(cdh);
		dbgo->QueryCharge((wchar_t*)MSISDN, cdh);
		isSucceed = true;
	}
	
	if (dbgo)
	{
		delete dbgo;
		dbgo = NULL;
	}

	if (cdh->n == 0)
	{
		::MessageBox(m_hWnd, L"暂无缴费信息", L"提示", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	//插入列表
	CListUI * listPlan = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listPlan")));
	struct CMS_DB_NODE * cdn = cdh->next;
	CString wszStr;
	int i = 0;
	// 添加List列表内容，必须先Add(pListElement)，再SetText
	while (cdn != NULL)
	{
		CListTextElementUI* pListElement = new CListTextElementUI;
		pListElement->SetTag(i++);		//用PlanNo作为tag
		listPlan->Add(pListElement);

		pListElement->SetText(0,  ((struct Charge*)(cdn->content))->PayTime);
		wszStr.Format(L"   %.2lf", ((struct Charge*)(cdn->content))->PayAmount);
		pListElement->SetText(1, wszStr.GetBuffer(wszStr.GetLength()));
		pListElement->SetText(2, ((struct Charge*)(cdn->content))->PayMethod);
		wszStr.Format(L"   %d", ((struct Charge*)(cdn->content))->OperatorNo);
		pListElement->SetText(3, wszStr.GetBuffer(wszStr.GetLength()));

		cdn = cdn->next;
	}

	return isSucceed;
}

bool CSubQRFrameWnd::OnOut(LPCWSTR MSISDN)
{
	bool isSucceed = false;
	Book* book = xlCreateBook();
	if (book)
	{
		Sheet* sheet = book->addSheet(L"Sheet1");
		if (sheet)
		{
			//输出“用户号码：13800138000”
			sheet->writeStr(0, 0, L"用户号码：");
			sheet->writeStr(0, 1, MSISDN);
			//输出表头“缴费时间 缴费金额 支付方式 操作员工号”
			sheet->writeStr(1, 0, L"缴费时间");
			sheet->writeStr(1, 1, L"缴费金额");
			sheet->writeStr(1, 2, L"支付方式");
			sheet->writeStr(1, 3, L"操作员工号");
			//循环输出各行内容
			struct CMS_DB_NODE * cdn = cdh->next;
			int i = 2;
			while (cdn != NULL)
			{
				sheet->writeStr(i, 0, ((struct Charge*)(cdn->content))->PayTime);
				sheet->writeNum(i, 1, ((struct Charge*)(cdn->content))->PayAmount);
				sheet->writeStr(i, 2, ((struct Charge*)(cdn->content))->PayMethod);
				sheet->writeNum(i++, 3, ((struct Charge*)(cdn->content))->OperatorNo);
				cdn = cdn->next;
			}

			sheet->setCol(1, 1, 12);
		}
		//将用户电话号码作为文件名
		CString wszFileName = MSISDN;
		wszFileName += ".xls";
		if (book->save(wszFileName.GetBuffer(wszFileName.GetLength())))
		{
			::ShellExecute(NULL, L"open", wszFileName.GetBuffer(wszFileName.GetLength()), NULL, NULL, SW_SHOW);
			isSucceed = true;
		}
		else
		{
			//std::cout << book->errorMessage() << std::endl;
			::MessageBox(m_hWnd, s2ws(book->errorMessage()).c_str(), L"XLS FAILED", MB_OK | MB_ICONERROR);
			isSucceed = false;
		}

		book->release();
	}
	else
		isSucceed = false;
	return isSucceed;
}

LPCTSTR CSubOperatorMgrFrameWnd::GetWindowClassName() const { return _T("DUILoginFrame"); }

CDuiString CSubOperatorMgrFrameWnd::GetSkinFile() { return _T(DUI_XML_SUB_OPERATORMGR); }

CDuiString CSubOperatorMgrFrameWnd::GetSkinFolder() { return _T(""); }

void CSubOperatorMgrFrameWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("click"))
	{
		//关闭按钮
		if (msg.pSender->GetName() == _T("closebtn"))
		{
			//从栈顶获取父窗口句柄并出栈
			HWND m_hwnd = stackHwnd.top();
			stackHwnd.pop();

			::EnableWindow(m_hwnd, TRUE);												// 父窗口还原 m_hwnd为子窗口句柄
			::SetFocus(m_hwnd);																	//设置焦点到父窗口
			::PostMessage(m_hwnd, WM_CMS_BACKTOPARENT, 0, 0);		//向父窗口发送消息
			//PostQuitMessage(0); 
			Close();
		}
		//添加收费员
		else if (msg.pSender->GetName() == _T("btnAdd"))
		{
			if (OnNewOperator() == true)
				::MessageBox(m_hWnd, L"添加成功", L"提示", MB_OK | MB_ICONINFORMATION);
			else
				::MessageBox(m_hWnd, L"添加失败。是否有未填项？", L"提示", MB_OK | MB_ICONINFORMATION);
		}
		//重置
		else if (msg.pSender->GetName() == _T("btnReset"))
		{
			CEditUI * editName = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editName"));
			CEditUI * editGender = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editGender"));
			CEditUI * editPwd = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editPwd"));
			if (editName != NULL &&
				editGender != NULL &&
				editPwd != NULL)
			{
				editName->SetText(L"");
				editGender->SetText(L"");
				editPwd->SetText(L"");
			}
		}
		//查询
		else if (msg.pSender->GetName() == _T("btnQuery"))
		{
			if(OnQueryOperator() == false)
				::MessageBox(m_hWnd, L"查询失败。\n是否未填待查内容、待查内容与查询方式是否匹配？", 
					L"提示", MB_OK | MB_ICONINFORMATION);
		}
		//修改
		else if (msg.pSender->GetName() == _T("btnModify"))
		{
			if(cdh == NULL)
				::MessageBox(m_hWnd, L"请先执行查询...",	L"提示", MB_OK | MB_ICONINFORMATION);
			else
			{
				//获取选中元素的句柄
				CListUI * listOperator = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listOperator")));
				CListTextElementUI * pListElement = new CListTextElementUI;
				int nIndex = listOperator->GetCurSel();
				if (nIndex < 0)
				{
					::MessageBox(m_hWnd, L"请选中所要修改的信息...", L"提示", MB_OK | MB_ICONINFORMATION);
					return;
				}
				pListElement = (CListTextElementUI*)listOperator->GetItemAt(nIndex);
				//获取选中元素的工号
				int SelectedNo = 0;
				swscanf_s(pListElement->GetText(0), L"%d", &SelectedNo);

				//找到该元素对应的链表结点
				struct CMS_DB_NODE * cdn = cdh->next;
				while (cdn != NULL)
				{
					if (((struct Operator*)(cdn->content))->OperatorNo == SelectedNo)
						break;
					cdn = cdn->next;
				}
				if (cdn == NULL)
				{
					::MessageBox(m_hWnd, L"链表错误？", L"提示", MB_OK | MB_ICONERROR);
					return;
				}
				//显示灰色遮罩
				ctrlGrayOpMgr = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayOpMgr"));
				ctrlGrayOpMgr->SetVisible(true);
				ctrlGrayOpMgrQMD = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayOpMgrQMD"));
				ctrlGrayOpMgrQMD->SetVisible(true);

				//获取当前窗口句柄
				HWND m_hwnd = GetHWND();
				//将当前窗口句柄加入栈
				stackHwnd.push(m_hwnd);
				//启动修改收费员子窗口线程
				_beginthread(ModifyOperatorThread, 0, cdn);
			}
		}
		//删除
		else if (msg.pSender->GetName() == _T("btnDelete"))
		{
			if (cdh == NULL)
				::MessageBox(m_hWnd, L"请先执行查询...", L"提示", MB_OK | MB_ICONINFORMATION);
			else
			{
				//获取选中元素的句柄
				CListUI * listOperator = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listOperator")));
				CListTextElementUI * pListElement = new CListTextElementUI;
				int nIndex = listOperator->GetCurSel();
				if (nIndex < 0)
				{
					::MessageBox(m_hWnd, L"请选中所要修改的信息...", L"提示", MB_OK | MB_ICONINFORMATION);
					return;
				}
				pListElement = (CListTextElementUI*)listOperator->GetItemAt(nIndex);
				//获取选中元素的工号
				int SelectedNo = 0;
				swscanf_s(pListElement->GetText(0), L"%d", &SelectedNo);

				//找到该元素对应的链表结点
				struct CMS_DB_NODE * cdn = cdh->next;
				while (cdn != NULL)
				{
					if (((struct Operator*)(cdn->content))->OperatorNo == SelectedNo)
						break;
					cdn = cdn->next;
				}
				if (cdn == NULL)
				{
					::MessageBox(m_hWnd, L"链表错误？", L"提示", MB_OK | MB_ICONERROR);
					return;
				}
				//获取一下工号
				int OpNoToDel = ((struct Operator*)(cdn->content))->OperatorNo;
				//给老子删！
				DBgo * dbgo = new DBgo;
				if (dbgo == NULL)
				{
					::MessageBox(m_hWnd, L"dbgo Null Pointer？", L"提示", MB_OK | MB_ICONERROR);
					return;
				}
				if(dbgo->DropOperator(OpNoToDel) == true)
					::MessageBox(m_hWnd, L"删除成功", L"提示", MB_OK | MB_ICONINFORMATION);
				else
					::MessageBox(m_hWnd, L"删除失败\n数据库错误？", L"提示", MB_OK | MB_ICONERROR);
				delete dbgo;
				dbgo = NULL;
			}
			return;
		}
	}
	else if (msg.sType == _T("selectchanged"))
	{
		//与xml中的opt对应 根据所选opt 切换switch内容
		CDuiString name = msg.pSender->GetName();
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
		if (name == _T("optNew")) { pControl->SelectItem(0); this->OnSwitchNew(); }
		else if (name == _T("optQMD")) { pControl->SelectItem(1);	this->OnSwitchQMD(); }
	}
}

CSubOperatorMgrFrameWnd::~CSubOperatorMgrFrameWnd()
{
	if (cdh != NULL)
		DeleteCDH(cdh);
}

void CSubOperatorMgrFrameWnd::Init()
{
	cdh = NULL;
	return;
}

LRESULT CSubOperatorMgrFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			return true;
		}
		else if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return true;
		}
	default:
		return false;
	}
}

LRESULT CSubOperatorMgrFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;	//标志着消息是否被处理
	switch (uMsg)
	{
	case WM_NCLBUTTONDBLCLK: return 0;//禁用双击标题栏窗口最大化：屏蔽非客户区左键双击事件
	case WM_CMS_BACKTOPARENT:
		ctrlGrayOpMgr = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayOpMgr"));
		ctrlGrayOpMgr->SetVisible(false);
		ctrlGrayOpMgrQMD = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayOpMgrQMD"));
		ctrlGrayOpMgrQMD->SetVisible(false);
		break;
	default: bHandled = FALSE;
	}
	//如果已处理消息 则直接返回
	if (bHandled) return lRes;
	//如果由前述MessageHandler处理了消息 则直接返回
	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	//其余的还给WIB处理...
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CSubOperatorMgrFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_PaintManager.Init(m_hWnd);
	CDialogBuilder builder;
	CSubOperatorMgrBuilderCallbackEx cb;
	CControlUI* pRoot = builder.Create(_T(DUI_XML_SUB_OPERATORMGR), (UINT)0, &cb, &m_PaintManager);
	ASSERT(pRoot && "Failed to parse XML");
	m_PaintManager.AttachDialog(pRoot);
	m_PaintManager.AddNotifier(this);


	//m_WndShadow.Create(m_hWnd);
	//m_WndShadow.SetSize(3);
	//m_WndShadow.SetPosition(0, 0);

	Init();
	return 0;
}

bool CSubOperatorMgrFrameWnd::OnSwitchNew()
{
	return false;
}

bool CSubOperatorMgrFrameWnd::OnSwitchQMD()
{
	return false;
}


// 响应“新增”标签页中的“增加”按钮事件
bool CSubOperatorMgrFrameWnd::OnNewOperator()
{
	bool isSucceed = false;
	//获取姓名、性别、密码、等级
	CEditUI * editName = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editName"));
	CEditUI * editGender = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editGender"));
	CEditUI * editPwd = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editPwd"));
	CComboUI * pCombox = static_cast<CComboUI*>(m_PaintManager.FindControl(L"comboClass"));
	int nsel = pCombox->GetCurSel();
	CListLabelElementUI* pItemEle = (CListLabelElementUI*)pCombox->GetItemAt(nsel);
	
	//合法性检查
	if (editName == NULL ||
		editGender == NULL ||
		editPwd == NULL ||
		pItemEle == NULL)
		return false;
	else if (editName->GetText().GetLength() == 0 ||
		editGender->GetText().GetLength() == 0 ||
		editPwd->GetText().GetLength() == 0)
		return false;

	//获取密码MD5
	std::wstring wszStr = editPwd->GetText();
	MD5 * md5 = new MD5(ws2s(wszStr));
	wszStr = s2ws(md5->md5());

	//获取当前最大工号
	DBgo * dbgo = new DBgo;
	if(dbgo == NULL)
	{
		if (md5)
		{
			delete md5;
			md5 = NULL;
		}
		return false;
	}
	int MaxNo = dbgo->GetMaxNo(L"OperatorNo", L"Operator");
	if (MaxNo == -1)
	{
		if (md5)
		{
			delete md5;
			md5 = NULL;
		}
		if (dbgo)
		{
			delete dbgo;
			dbgo = NULL;
		}
		return false;
	}
	++MaxNo;		//新收费员工号为：最大工号+1

	//填充结构体
	struct Operator * pOperator = new struct Operator;
	if (pOperator == NULL)
	{	
		if (md5)
		{
			delete md5;
			md5 = NULL;
		}
		return false;
	}
	memset(pOperator, 0, sizeof(struct Operator));
	pOperator->Class = (pItemEle->GetText() == L"管理员") ? 666 : 1;
	//pOperator->LoginStatus = 0;
	pOperator->OperatorNo = MaxNo;
	wcsncpy_s(pOperator->password, wszStr.c_str(), wszStr.length() * 2);
	wszStr = editName->GetText();
	wcsncpy_s(pOperator->OperatorName, wszStr.c_str(), wszStr.length() * 2);
	wszStr = editGender->GetText();
	wcsncpy_s(pOperator->OperatorGender, wszStr.c_str(), wszStr.length() * 2);
	
	//执行数据库动作
	isSucceed = dbgo->AddOperator(pOperator);

	if (md5)
	{
		delete md5;
		md5 = NULL;
	}
	if (pOperator)
	{
		delete pOperator;
		pOperator = NULL;
	}
	if (dbgo)
	{
		delete dbgo;
		dbgo = NULL;
	}
	return isSucceed;
}

bool CSubOperatorMgrFrameWnd::OnQueryOperator()
{
	//获取控件句柄
	CEditUI * editContent = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editContent"));
	CComboUI * pCombox = static_cast<CComboUI*>(m_PaintManager.FindControl(L"comboMethod"));
	int nsel = pCombox->GetCurSel();
	CListLabelElementUI* pItemEle = (CListLabelElementUI*)pCombox->GetItemAt(nsel);

	cdh = new struct CMS_DB_HEADER;

	if (editContent == NULL ||
		pItemEle == NULL ||
		cdh == NULL)
	{
		if (cdh != NULL)
		{
			delete cdh;
			cdh = NULL;
		}
		return false;
	}
	//初始化cdh
	InitCDH(cdh);

	//连接数据库 
	DBgo * dbgo = new DBgo;
	if (dbgo == NULL)
	{
		delete cdh;
		cdh = NULL;
		return false;
	}

	//获取待查内容、查询方式
	int QueryMethod = (pItemEle->GetText() == L"工号（精确查询）") ? QO_BY_OPERATORNO : QO_BY_OPERATORNAME;
	CString wszStr = editContent->GetText();
	//执行查询
	if (dbgo->QueryOperator(wszStr.GetBuffer(wszStr.GetLength()), QueryMethod, cdh)
		== false)
		::MessageBox(m_hWnd, L"查询出错啦~", L"提示", MB_OK|MB_ICONERROR);
	else if(cdh->n == 0)
		::MessageBox(m_hWnd, L"没有查到相关信息", L"提示", MB_OK | MB_ICONINFORMATION);
	else
	{
		////写入列表
		//获取句柄
		CListUI * listOperator = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listOperator")));
		struct CMS_DB_NODE * cdn = cdh->next;

		// 添加List列表内容，必须先Add(pListElement)，再SetText
		while (cdn != NULL)
		{
			CListTextElementUI* pListElement = new CListTextElementUI;
			pListElement->SetTag(((struct Operator*)(cdn->content))->OperatorNo);		//用OperatorNo作为tag
			listOperator->Add(pListElement);

			wszStr.Format(L"   %d", ((struct Operator*)(cdn->content))->OperatorNo);
			pListElement->SetText(0, wszStr.GetBuffer(wszStr.GetLength()));
			pListElement->SetText(1, ((struct Operator*)(cdn->content))->OperatorName);
			pListElement->SetText(2, ((struct Operator*)(cdn->content))->OperatorGender);
			pListElement->SetText(3, ((struct Operator*)(cdn->content))->Class==666?L"管理员":L"收费员");

			cdn = cdn->next;
		}
	}
	if (dbgo)
	{
		delete dbgo;
		dbgo = NULL;
	}
	return true;
}

LPCTSTR CModifyOperatorFrameWnd::GetWindowClassName() const { return _T("DUILoginFrame"); }

CDuiString CModifyOperatorFrameWnd::GetSkinFile() { return _T(DUI_XML_SUB_MODIFYOPERATOR); }

CDuiString CModifyOperatorFrameWnd::GetSkinFolder() { return _T(""); }

void CModifyOperatorFrameWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("click"))
	{
		//关闭按钮
		if (msg.pSender->GetName() == _T("closebtn"))
		{
			//从栈顶获取父窗口句柄并出栈
			HWND m_hwnd = stackHwnd.top();
			stackHwnd.pop();

			::EnableWindow(m_hwnd, TRUE);												// 父窗口还原 m_hwnd为子窗口句柄
			::SetFocus(m_hwnd);																	//设置焦点到父窗口
			::PostMessage(m_hwnd, WM_CMS_BACKTOPARENT, 0, 0);		//向父窗口发送消息
																	//PostQuitMessage(0); 
			Close();
		}
		//修改按钮
		else if (msg.pSender->GetName() == L"btnModify")
		{
			if (OnModify() == true)
				::MessageBox(m_hWnd, L"修改成功~", L"提示", MB_OK | MB_ICONINFORMATION);
		}
		//重置按钮
		else if (msg.pSender->GetName() == L"btnReset")
		{
			editName = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editName"));
			editGender = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editGender"));
			editPwd = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editPwd"));
			if (editName == NULL ||
				editGender == NULL ||
				editPwd == NULL)
			{
				::MessageBox(m_hWnd, L"空指针异常？", L"提示", MB_OK | MB_ICONINFORMATION);
				return;
			}
			editName->SetText(L"");
			editGender->SetText(L"");
			editPwd->SetText(L"");
		}
	}
}

CModifyOperatorFrameWnd::CModifyOperatorFrameWnd(CMS_DB_NODE * cdn)
{
	if (cdn != NULL)
		pOperator = (struct Operator*)(cdn->content);
	else
		pOperator = NULL;
}

void CModifyOperatorFrameWnd::Init()
{
	//为各控件赋值
	CLabelUI * labelOperatorNo = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"labelOperatorNo"));
	CString wszStr;
	wszStr.Format(L"%d", pOperator->OperatorNo);

	if (labelOperatorNo != NULL)
		labelOperatorNo->SetText(wszStr.GetBuffer(wszStr.GetLength()));
	CEditUI * edits = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editName"));
	if (edits != NULL)
		edits->SetText(pOperator->OperatorName);
	edits = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editGender"));
	if(edits != NULL)
		edits->SetText(pOperator->OperatorGender);
	
	return;
}

LRESULT CModifyOperatorFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			return true;
		}
		else if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return true;
		}
	default:
		return false;
	}
}

LRESULT CModifyOperatorFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;	//标志着消息是否被处理
	switch (uMsg)
	{
	case WM_NCLBUTTONDBLCLK: return 0;//禁用双击标题栏窗口最大化：屏蔽非客户区左键双击事件
	default: bHandled = FALSE;
	}
	//如果已处理消息 则直接返回
	if (bHandled) return lRes;
	//如果由前述MessageHandler处理了消息 则直接返回
	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	//其余的还给WIB处理...
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CModifyOperatorFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_PaintManager.Init(m_hWnd);
	CDialogBuilder builder;
	//CDialogBuilderCallbackEx cb;
	CControlUI* pRoot = builder.Create(_T(DUI_XML_SUB_MODIFYOPERATOR), (UINT)0, NULL, &m_PaintManager);
	ASSERT(pRoot && "Failed to parse XML");
	m_PaintManager.AttachDialog(pRoot);
	m_PaintManager.AddNotifier(this);


	//m_WndShadow.Create(m_hWnd);
	//m_WndShadow.SetSize(3);
	//m_WndShadow.SetPosition(0, 0);

	Init();
	return 0;
}


// 响应“修改”按钮
bool CModifyOperatorFrameWnd::OnModify()
{
	//接收一波内容
	editName = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editName"));
	editGender = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editGender"));
	editPwd = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editPwd"));
	if (editName == NULL ||
		editGender == NULL ||
		editPwd == NULL)
	{
		::MessageBox(m_hWnd, L"空指针异常？", L"提示", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	CComboUI * pCombox = static_cast<CComboUI*>(m_PaintManager.FindControl(L"comboClass"));
	int nsel = pCombox->GetCurSel();
	if (nsel < 0)
	{
		::MessageBox(m_hWnd, L"请选择用户等级。", L"提示", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	pItemEle = (CListLabelElementUI*)pCombox->GetItemAt(nsel);
	//先删
	DBgo * dbgo = new DBgo;
	if (dbgo == NULL)
	{
		::MessageBox(m_hWnd, L"dbgo Null Pointer", L"提示", MB_OK);
		return false;
	}

	if (dbgo->DropOperator(pOperator->OperatorNo) == false)
	{
		::MessageBox(m_hWnd, L"修改失败\n没删成功...", L"提示", MB_OK);
		delete dbgo;
		dbgo = NULL;
		return false;
	}
	//再建
	struct Operator* pO = new struct Operator;
	if (pO == NULL)
	{
		::MessageBox(m_hWnd, L"pO Null Pointer", L"提示", MB_OK);
		delete dbgo;
		dbgo = NULL;
		return false;
	}
	memset(pO, 0, sizeof(struct Operator));
	//开填
	pO->OperatorNo = pOperator->OperatorNo;
	//dbgo->AddOperator()
	pO->Class = (pItemEle->GetText() == L"管理员") ? 666 : 1;
	std::wstring wszStr = editName->GetText();
	memcpy_s(pO->OperatorName, NAME_LEN, wszStr.c_str(), wszStr.length() * 2);
	wszStr = editGender->GetText();
	memcpy_s(pO->OperatorGender, GENDER_LEN, wszStr.c_str(), wszStr.length() * 2);
	//还得算个MD5
	wszStr = editPwd->GetText();
	MD5 * md5 = new MD5(ws2s(wszStr));
	wszStr = s2ws(md5->md5());
	memcpy_s(pO->password, PWD_LEN, wszStr.c_str(), wszStr.length() * 2);
	//插入数据库
	bool isSucceed = false;
	if (dbgo->AddOperator(pO) == false)
	{
		::MessageBox(m_hWnd, L"修改失败\n没Insert成功...", L"提示", MB_OK);
		isSucceed = false;
	}
	else
		isSucceed = true;

	delete dbgo;
	dbgo = NULL;
	delete md5;
	md5 = NULL;
	delete pO;
	pO = NULL;

	return isSucceed;
}

LPCTSTR CPlanMgrFrameWnd::GetWindowClassName() const { return _T("DUILoginFrame"); }

CDuiString CPlanMgrFrameWnd::GetSkinFile() { return _T(DUI_XML_SUB_PLANMGR); }

CDuiString CPlanMgrFrameWnd::GetSkinFolder() { return _T(""); }

void CPlanMgrFrameWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("click"))
	{
		//关闭按钮
		if (msg.pSender->GetName() == _T("closebtn"))
		{
			//从栈顶获取父窗口句柄并出栈
			HWND m_hwnd = stackHwnd.top();
			stackHwnd.pop();

			::EnableWindow(m_hwnd, TRUE);												// 父窗口还原 m_hwnd为子窗口句柄
			::SetFocus(m_hwnd);																	//设置焦点到父窗口
			::PostMessage(m_hwnd, WM_CMS_BACKTOPARENT, 0, 0);		//向父窗口发送消息

			Close();
		}
		//添加套餐
		else if (msg.pSender->GetName() == _T("btnAdd"))
		{
			if (OnNewPlan() == true)
				::MessageBox(m_hWnd, L"添加成功", L"提示", MB_OK | MB_ICONINFORMATION);
			else
				::MessageBox(m_hWnd, L"添加失败。是否有未填项？", L"提示", MB_OK | MB_ICONINFORMATION);
		}
		//重置
		else if (msg.pSender->GetName() == _T("btnReset"))
		{
			//获取句柄
			CEditUI* editName = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editName"));
			CEditUI* editRemote = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editRemote"));
			CEditUI* editLocal = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editLocal"));
			CEditUI* editRoaming = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editRoaming"));
			//空指针检查
			if (editName == NULL ||
				editRemote == NULL ||
				editLocal == NULL ||
				editRoaming == NULL)
			{
				::MessageBox(m_hWnd, L"handles Null Pointer Exceptions...", L"Error", MB_OK | MB_ICONERROR);

				return ;
			}
			else
			{
				editName->SetText(L"");
				editRemote->SetText(L"");
				editLocal->SetText(L"");
				editRoaming->SetText(L"");
			}
		}
		//查询
		else if (msg.pSender->GetName() == _T("btnQuery"))
		{
			if (OnQueryPlan() == false)
				::MessageBox(m_hWnd, L"查询失败。\n是否未填待查内容？\n待查内容与查询方式是否匹配？",
					L"提示", MB_OK | MB_ICONINFORMATION);
		}
		//修改
		else if (msg.pSender->GetName() == _T("btnModify"))
		{
			if (cdh == NULL)
				::MessageBox(m_hWnd, L"请先执行查询...", L"提示", MB_OK | MB_ICONINFORMATION);
			else
			{
				//获取选中元素的句柄
				struct CMS_DB_NODE * cdn = NULL;
				if (CDNCallPlanWalker(cdn) == false)
					return;

				//显示灰色遮罩
				ctrlGrayPlanMgr = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayPlanMgr"));
				ctrlGrayPlanMgr->SetVisible(true);
				ctrlGrayPlanMgrQMD = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayPlanMgrQMD"));
				ctrlGrayPlanMgrQMD->SetVisible(true);

				//获取当前窗口句柄
				HWND m_hwnd = GetHWND();
				//将当前窗口句柄加入栈
				stackHwnd.push(m_hwnd);
				//启动修改收费员子窗口线程
				_beginthread(ModifyPlanThread, 0, cdn);
			}
		}
		//删除
		else if (msg.pSender->GetName() == _T("btnDelete"))
		{
			if (cdh == NULL)
				::MessageBox(m_hWnd, L"请先执行查询...", L"提示", MB_OK | MB_ICONINFORMATION);
			else
			{
				if (OnDeletePlan() == true)
					::MessageBox(m_hWnd, L"删除成功", L"提示", MB_OK | MB_ICONINFORMATION);
			}
			return;
		}
	}
	else if (msg.sType == _T("selectchanged"))
	{
		//与xml中的opt对应 根据所选opt 切换switch内容
		CDuiString name = msg.pSender->GetName();
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
		if (name == _T("optNew"))
		{ 
			pControl->SelectItem(0);  
		}
		else if (name == _T("optQMD"))
		{
			pControl->SelectItem(1);
		}
	}
}

CPlanMgrFrameWnd::~CPlanMgrFrameWnd()
{
	if (cdh != NULL)
		DeleteCDH(cdh);
}

void CPlanMgrFrameWnd::Init()
{
	cdh = NULL;
	return;
}

LRESULT CPlanMgrFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			return true;
		}
		else if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return true;
		}
	default:
		return false;
	}
}

LRESULT CPlanMgrFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;	//标志着消息是否被处理
	switch (uMsg)
	{
	case WM_NCLBUTTONDBLCLK: return 0;//禁用双击标题栏窗口最大化：屏蔽非客户区左键双击事件
	case WM_CMS_BACKTOPARENT:
		ctrlGrayPlanMgr = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayPlanMgr"));
		ctrlGrayPlanMgr->SetVisible(false);
		ctrlGrayPlanMgrQMD = static_cast<CControlUI*>(m_PaintManager.FindControl(L"ctrlGrayPlanMgrQMD"));
		ctrlGrayPlanMgrQMD->SetVisible(false);
		break;
	default: bHandled = FALSE;
	}
	//如果已处理消息 则直接返回
	if (bHandled) return lRes;
	//如果由前述MessageHandler处理了消息 则直接返回
	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	//其余的还给WIB处理...
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CPlanMgrFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_PaintManager.Init(m_hWnd);
	CDialogBuilder builder;
	CPlanMgrBuilderCallbackEx cb;
	CControlUI* pRoot = builder.Create(_T(DUI_XML_SUB_PLANMGR), (UINT)0, &cb, &m_PaintManager);
	ASSERT(pRoot && "Failed to parse XML");
	m_PaintManager.AttachDialog(pRoot);
	m_PaintManager.AddNotifier(this);


	//m_WndShadow.Create(m_hWnd);
	//m_WndShadow.SetSize(3);
	//m_WndShadow.SetPosition(0, 0);

	Init();
	return 0;
}

bool CPlanMgrFrameWnd::OnNewPlan()
{
	//获取句柄
	CEditUI* editName = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editName"));
	CEditUI* editRemote = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editRemote"));
	CEditUI* editLocal = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editLocal"));
	CEditUI* editRoaming = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editRoaming"));
	//空指针检查
	if (editName == NULL ||
		editRemote == NULL ||
		editLocal == NULL ||
		editRoaming == NULL)
	{
		::MessageBox(m_hWnd, L"handles Null Pointer Exceptions...", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}
	//获取内容
	std::wstring wszStr = editName->GetText();
	double dRemote = 0.0;
	double dLocal = 0.0;
	double dRoaming = 0.0;
	swscanf_s(editRemote->GetText(), L"%lf", &dRemote);
	swscanf_s(editLocal->GetText(), L"%lf", &dLocal);
	swscanf_s(editRoaming->GetText(), L"%lf", &dRoaming);
	if (dRemote == 0.0 ||
		dLocal == 0.0 ||
		dRoaming == 0.0)
	{
		::MessageBox(m_hWnd, L"费率好像不太对喔...", L"提示", MB_OK | MB_ICONERROR);
		return false;
	}
	//填结构体
	struct CallPlan *pCp = new struct CallPlan;
	if (pCp == NULL)
	{
		::MessageBox(m_hWnd, L"pCp Null Pointer Exceptions...", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}
	memset(pCp, 0, sizeof(struct CallPlan));
	pCp->LocalFee = dLocal;
	pCp->RemoteFee = dRemote;
	pCp->RoamingFee = dRoaming;
	memcpy_s(pCp->PlanName, PLANNAME_LEN, wszStr.c_str(), wszStr.length() * 2);
	//连数据库
	DBgo * dbgo = new DBgo();
	if (dbgo == NULL)
	{
		::MessageBox(m_hWnd, L"dbgo Null Pointer Exceptions...", L"Error", MB_OK | MB_ICONERROR);
		delete pCp;
		pCp = NULL;
		return false;
	}
	pCp->PlanNo = dbgo->GetMaxNo(L"PlanNo", L"CallPlan") + 1;
	if (pCp->PlanNo == -1)
	{
		::MessageBox(m_hWnd, L"dbgo->GetMaxNo Error...", L"Error", MB_OK | MB_ICONERROR);
		delete pCp;
		pCp = NULL;
		delete dbgo;
		dbgo = NULL;
		return false;
	}
	//插入数据库
	if (dbgo->AddPlan(pCp) == false)
	{
		::MessageBox(m_hWnd, L"dbgo->AddPlan Error...", L"Error", MB_OK | MB_ICONERROR);
		delete pCp;
		pCp = NULL;
		delete dbgo;
		dbgo = NULL;
		return false;
	}

	delete dbgo;
	dbgo = NULL;
	delete pCp;
	pCp = NULL;
	return true;
}

bool CPlanMgrFrameWnd::OnQueryPlan()
{
	//获取句柄
	CEditUI * editContent = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editContent"));
	CComboUI * pCombox = static_cast<CComboUI*>(m_PaintManager.FindControl(L"comboMethod"));
	if (editContent == NULL ||
		pCombox == NULL)
	{
		::MessageBox(m_hWnd, L"handle(s) Null Pointer Exception...", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}
	int nsel = pCombox->GetCurSel();
	if (nsel < 0)
	{
		::MessageBox(m_hWnd, L"请选择一个查询方式...", L"提示", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	CListLabelElementUI* pItemEle = (CListLabelElementUI*)pCombox->GetItemAt(nsel);
	//获取内容
	std::wstring wszStr = editContent->GetText();
	if (wszStr.length() == 0)
	{
		::MessageBox(m_hWnd, L"咱总得输入点啥才能查吧...", L"提示", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	int Query_Method = (pItemEle->GetText() == L"套餐号（精确查询）") ? QP_BY_PLANNO : QP_BY_PLANNAME;
	//给cdh分配内存
	if (cdh == NULL)
	{
		cdh = new struct CMS_DB_HEADER;
		if (cdh == NULL)
		{
			::MessageBox(m_hWnd, L"cdh Null Pointer Exception...", L"Error", MB_OK | MB_ICONERROR);

			return false;
		}
		InitCDH(cdh);
	}
	//连数据库
	DBgo * dbgo = new DBgo();
	if (dbgo == NULL)
	{
		::MessageBox(m_hWnd, L"dbgo Null Pointer Exception...", L"Error", MB_OK | MB_ICONERROR);

		return false;
	}
	if (dbgo->QueryPlan((wchar_t*)wszStr.c_str(), Query_Method, cdh) == false)
	{
		::MessageBox(m_hWnd, L"SQL错误", L"提示", MB_OK | MB_ICONERROR);
		delete dbgo;
		dbgo = NULL;
		return false;
	}
	else if (cdh->n == 0)
	{
		::MessageBox(m_hWnd, L"没查着...换个词儿试试？", L"提示", MB_OK | MB_ICONINFORMATION);
		delete dbgo;
		dbgo = NULL;
		return false;
	}
	//获取句柄
	CListUI * listPlan = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listPlan")));

	struct CMS_DB_NODE * cdn = cdh->next;
	CString szStr;
	// 添加List列表内容，必须先Add(pListElement)，再SetText
	while (cdn != NULL)
	{
		CListTextElementUI* pListElement = new CListTextElementUI;
		pListElement->SetTag(((struct CallPlan*)(cdn->content))->PlanNo);		//用PlanNo作为tag
		listPlan->Add(pListElement);

		szStr.Format(L"   %d", ((struct CallPlan*)(cdn->content))->PlanNo);
		pListElement->SetText(0, szStr.GetBuffer(szStr.GetLength()));
		pListElement->SetText(1, ((struct CallPlan*)(cdn->content))->PlanName);
		szStr.Format(L"   %.2lf", ((struct CallPlan*)(cdn->content))->RemoteFee);
		pListElement->SetText(2, szStr.GetBuffer(szStr.GetLength()));
		szStr.Format(L"   %.2lf", ((struct CallPlan*)(cdn->content))->LocalFee);
		pListElement->SetText(3, szStr.GetBuffer(szStr.GetLength()));
		szStr.Format(L"   %.2lf", ((struct CallPlan*)(cdn->content))->RoamingFee);
		pListElement->SetText(4, szStr.GetBuffer(szStr.GetLength()));
		
		cdn = cdn->next;
	}
	delete dbgo;
	dbgo = NULL;
	return true;
}

LPCTSTR CModifyPlanFrameWnd::GetWindowClassName() const { return _T("DUILoginFrame"); }

CDuiString CModifyPlanFrameWnd::GetSkinFile() { return _T(DUI_XML_SUB_MODIFYPLAN); }

CDuiString CModifyPlanFrameWnd::GetSkinFolder() { return _T(""); }

void CModifyPlanFrameWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("click"))
	{
		//关闭按钮
		if (msg.pSender->GetName() == _T("closebtn"))
		{
			//从栈顶获取父窗口句柄并出栈
			HWND m_hwnd = stackHwnd.top();
			stackHwnd.pop();

			::EnableWindow(m_hwnd, TRUE);												// 父窗口还原 m_hwnd为子窗口句柄
			::SetFocus(m_hwnd);																	//设置焦点到父窗口
			::PostMessage(m_hwnd, WM_CMS_BACKTOPARENT, 0, 0);		//向父窗口发送消息
																	//PostQuitMessage(0); 
			Close();
		}
		//修改按钮
		else if (msg.pSender->GetName() == L"btnModify")
		{
			if (OnModify() == true)
				::MessageBox(m_hWnd, L"修改成功~", L"提示", MB_OK | MB_ICONINFORMATION);
		}
		//重置按钮
		else if (msg.pSender->GetName() == L"btnReset")
		{
			editName = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editName"));
			editRemote = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editRemote"));
			editLocal = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editLocal"));
			editRoaming = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editRoaming"));

			if (editName == NULL ||
				editLocal == NULL ||
				editRoaming == NULL ||
				editRemote == NULL)
			{
				::MessageBox(m_hWnd, L"空指针异常？", L"提示", MB_OK | MB_ICONINFORMATION);
				return;
			}
			editName->SetText(L"");
			editLocal->SetText(L"");
			editRemote->SetText(L"");
			editRoaming->SetText(L"");
		}
	}
}

CModifyPlanFrameWnd::CModifyPlanFrameWnd(CMS_DB_NODE * cdn)
{
	if (cdn != NULL)
		pPlan = (struct CallPlan*)(cdn->content);
	else
		pPlan = NULL;
}

void CModifyPlanFrameWnd::Init()
{
	//为各控件赋值
	CLabelUI * labelPlanNo = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"labelPlanNo"));
	CString wszStr;
	wszStr.Format(L"%d", pPlan->PlanNo);

	if (labelPlanNo != NULL)
		labelPlanNo->SetText(wszStr.GetBuffer(wszStr.GetLength()));
	CEditUI * edits = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editName"));
	if (edits != NULL)
		edits->SetText(pPlan->PlanName);
	edits = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editRemote"));
	if (edits != NULL)
	{
		wszStr.Format(L"%.2lf", pPlan->RemoteFee);
		edits->SetText(wszStr.GetBuffer(wszStr.GetLength()));
	}
	edits = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editRoaming"));
	if (edits != NULL)
	{
		wszStr.Format(L"%.2lf", pPlan->RoamingFee);
		edits->SetText(wszStr.GetBuffer(wszStr.GetLength()));
	}
	edits = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editLocal"));
	if (edits != NULL)
	{
		wszStr.Format(L"%.2lf", pPlan->LocalFee);
		edits->SetText(wszStr.GetBuffer(wszStr.GetLength()));
	}
	return;
}

LRESULT CModifyPlanFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			return true;
		}
		else if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return true;
		}
	default:
		return false;
	}
}

LRESULT CModifyPlanFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;	//标志着消息是否被处理
	switch (uMsg)
	{
	case WM_NCLBUTTONDBLCLK: return 0;//禁用双击标题栏窗口最大化：屏蔽非客户区左键双击事件
	default: bHandled = FALSE;
	}
	//如果已处理消息 则直接返回
	if (bHandled) return lRes;
	//如果由前述MessageHandler处理了消息 则直接返回
	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	//其余的还给WIB处理...
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CModifyPlanFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_PaintManager.Init(m_hWnd);
	CDialogBuilder builder;
	//CDialogBuilderCallbackEx cb;
	CControlUI* pRoot = builder.Create(_T(DUI_XML_SUB_MODIFYPLAN), (UINT)0, NULL, &m_PaintManager);
	ASSERT(pRoot && "Failed to parse XML");
	m_PaintManager.AttachDialog(pRoot);
	m_PaintManager.AddNotifier(this);


	//m_WndShadow.Create(m_hWnd);
	//m_WndShadow.SetSize(3);
	//m_WndShadow.SetPosition(0, 0);

	Init();
	return 0;
}

bool CModifyPlanFrameWnd::OnModify()
{
	//获取句柄
	CEditUI * editName = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editName"));
	CEditUI * editRemote = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editRemote"));
	CEditUI * editLocal = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editLocal"));
	CEditUI * editRoaming = static_cast<CEditUI*>(m_PaintManager.FindControl(L"editRoaming"));
	if (editName == NULL ||
		editRemote == NULL ||
		editLocal == NULL ||
		editRoaming == NULL)
	{
		::MessageBox(m_hWnd, L"handle(s) Null Pointer Exception...", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}
	//连库
	DBgo * dbgo = new DBgo();
	if (dbgo == NULL)
	{
		::MessageBox(m_hWnd, L"dbgo Null Pointer Exception...", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}
	//先删
	if (dbgo->DropPlan(pPlan->PlanNo) == false)
	{
		::MessageBox(m_hWnd, L"SQL Error...", L"Error", MB_OK | MB_ICONERROR);
		delete dbgo;
		dbgo = NULL;
		return false;
	}
	//获取内容
	std::wstring wszStr = editName->GetText();
	double dRemote = 0.0;
	double dLocal = 0.0;
	double dRoaming = 0.0;
	swscanf_s(editRemote->GetText(), L"%lf", &dRemote);
	swscanf_s(editLocal->GetText(), L"%lf", &dLocal);
	swscanf_s(editRoaming->GetText(), L"%lf", &dRoaming);
	if (dRemote == 0.0 ||
		dLocal == 0.0 ||
		dRoaming == 0.0)
	{
		::MessageBox(m_hWnd, L"费率好像不太对喔...", L"提示", MB_OK | MB_ICONERROR);
		delete dbgo;
		dbgo = NULL;
		return false;
	}
	//创建结构体
	struct CallPlan * pCp = new struct CallPlan;
	if (pCp == NULL)
	{
		::MessageBox(m_hWnd, L"pCp Null Pointer Exception...", L"Error", MB_OK | MB_ICONERROR);
		delete dbgo;
		dbgo = NULL;
		return false;
	}
	memset(pCp, 0, sizeof(struct CallPlan));
	//填写结构体
	pCp->PlanNo = pPlan->PlanNo;
	pCp->LocalFee = dLocal;
	pCp->RemoteFee = dRemote;
	pCp->RoamingFee = dRoaming;
	memcpy_s(pCp->PlanName, PLANNAME_LEN, wszStr.c_str(), wszStr.length() * 2);
	//再插入数据库
	if (dbgo->AddPlan(pCp) == false)
	{
		::MessageBox(m_hWnd, L"dbgo->AddPlan Error...", L"Error", MB_OK | MB_ICONERROR);
		delete pCp;
		pCp = NULL;
		delete dbgo;
		dbgo = NULL;
		return false;
	}

	delete dbgo;
	dbgo = NULL;
	delete pCp;
	pCp = NULL;
	return true;
}


// 删除选中套餐
bool CPlanMgrFrameWnd::OnDeletePlan()
{
	struct CMS_DB_NODE * cdn = NULL;
	if (CDNCallPlanWalker(cdn) == false)
		return false;
	
	//获取一下工号
	int PlanNoToDel = ((struct CallPlan*)(cdn->content))->PlanNo;
	//给老子删！
	DBgo * dbgo = new DBgo;
	if (dbgo == NULL)
	{
		::MessageBox(m_hWnd, L"dbgo Null Pointer？", L"提示", MB_OK | MB_ICONERROR);
		return false;
	}
	bool isSucceed = false;
	if (dbgo->DropPlan(PlanNoToDel) == true)
		isSucceed = true;
	else
	{
		::MessageBox(m_hWnd, L"删除失败\n数据库错误？", L"提示", MB_OK | MB_ICONERROR);
		isSucceed = false;
	}

	delete dbgo;
	dbgo = NULL;
	
	return isSucceed;
}


// 获取选中元素在链表中的指针
bool CPlanMgrFrameWnd::CDNCallPlanWalker(struct CMS_DB_NODE * &cdn)
{
	if (cdh == NULL)
	{
		::MessageBox(m_hWnd, L"请先执行查询...", L"提示", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	else
	{
		//获取选中元素的句柄
		CListUI * listPlan = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listPlan")));
		if (listPlan == NULL)
		{
			::MessageBox(m_hWnd, L"listPlan Null Pointer Exception", L"Error", MB_OK | MB_ICONERROR);
			return false;
		}
		CListTextElementUI * pListElement = new CListTextElementUI;
		int nIndex = listPlan->GetCurSel();
		if (nIndex < 0)
		{
			::MessageBox(m_hWnd, L"请选中所要修改的信息...", L"提示", MB_OK | MB_ICONINFORMATION);
			return false;
		}
		pListElement = (CListTextElementUI*)listPlan->GetItemAt(nIndex);
		//获取选中元素的套餐号
		int SelectedNo = 0;
		swscanf_s(pListElement->GetText(0), L"%d", &SelectedNo);

		//找到该元素对应的链表结点
		cdn = cdh->next;
		struct CallPlan * pCp = NULL;
		while (cdn != NULL)
		{
			pCp = (struct CallPlan*)cdn->content;
			if (pCp->PlanNo == SelectedNo)
				break;
			cdn = cdn->next;
		}
		if (cdn == NULL)
		{
			::MessageBox(m_hWnd, L"链表错误？", L"提示", MB_OK | MB_ICONERROR);
			return false;
		}
	}
	return true;
}

LPCTSTR CIOuserFrameWnd::GetWindowClassName() const { return _T("DUILoginFrame"); }

CDuiString CIOuserFrameWnd::GetSkinFile() { return _T(DUI_XML_SUB_IOUSER); }

CDuiString CIOuserFrameWnd::GetSkinFolder() { return _T(""); }

void CIOuserFrameWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == _T("click"))
	{
		//关闭按钮
		if (msg.pSender->GetName() == _T("closebtn"))
		{
			//从栈顶获取父窗口句柄并出栈
			HWND m_hwnd = stackHwnd.top();
			stackHwnd.pop();

			::EnableWindow(m_hwnd, TRUE);												// 父窗口还原 m_hwnd为子窗口句柄
			::SetFocus(m_hwnd);																	//设置焦点到父窗口
			::PostMessage(m_hwnd, WM_CMS_BACKTOPARENT, 0, 0);		//向父窗口发送消息
																	//PostQuitMessage(0); 
			Close();
		}
		//导入按钮
		else if (msg.pSender->GetName() == L"btnIn")
		{
			if (::MessageBox(m_hWnd, L"导入操作将会覆盖现有数据，确定吗？", L"确认操作", MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONQUESTION)
				== IDOK)
			{
				if (OnIn() == true)
					::MessageBox(m_hWnd, L"导入成功~", L"提示", MB_OK | MB_ICONINFORMATION);
				else
					::MessageBox(m_hWnd, L"导入失败...", L"提示", MB_OK | MB_ICONINFORMATION);

			}
			else
				::MessageBox(m_hWnd, L"用户取消了操作", L"提示", MB_OK | MB_ICONINFORMATION);
		}
		//导出按钮
		else if (msg.pSender->GetName() == L"btnOut")
		{
			if (OnOut() == false)
				::MessageBox(m_hWnd, L"导出失败...", L"提示", MB_OK | MB_ICONINFORMATION);

		}
	}
}

LRESULT CIOuserFrameWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool & bHandled)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			return true;
		}
		else if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return true;
		}
	default:
		return false;
	}
}

LRESULT CIOuserFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;	//标志着消息是否被处理
	switch (uMsg)
	{
	case WM_NCLBUTTONDBLCLK: return 0;//禁用双击标题栏窗口最大化：屏蔽非客户区左键双击事件
	default: bHandled = FALSE;
	}
	//如果已处理消息 则直接返回
	if (bHandled) return lRes;
	//如果由前述MessageHandler处理了消息 则直接返回
	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	//其余的还给WIB处理...
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}


// 导出用户数据
bool CIOuserFrameWnd::OnOut()
{
	//先从数据库查出数据
	struct CMS_DB_HEADER * cdh = new struct CMS_DB_HEADER;
	DBgo * dbgo = new DBgo;
	if (cdh == NULL || dbgo == NULL)
		return false;
	InitCDH(cdh);
	
	if (dbgo->QueryUserInfo(L"%", QUI_BY_USERNAME | QUI_WAY_APPROXIMATE, cdh) == false)
	{
		delete dbgo;
		delete cdh;
		dbgo = NULL;
		cdh = NULL;
		return false;
	}
	bool isSucceed = false;
	if (cdh->n == 0)
	{
		::MessageBox(m_hWnd, L"暂无用户数据", L"提示", MB_OK | MB_ICONINFORMATION);
		isSucceed = true;
		delete dbgo;
		delete cdh;
		dbgo = NULL;
		cdh = NULL;
	}
	//写入xls文件
	else
	{
		Book* book = xlCreateBook();
		if (book)
		{
			Sheet* sheet = book->addSheet(L"Sheet1");
			if (sheet)
			{
				//输出表头
				sheet->writeStr(0, 0, L"用户号码");
				sheet->writeStr(0, 1, L"用户余额");
				sheet->writeStr(0, 2, L"套餐号");
				sheet->writeStr(0, 3, L"状态");
				sheet->writeStr(0, 4, L"姓名");
				sheet->writeStr(0, 5, L"性别");
				sheet->writeStr(0, 6, L"地址");

				//循环输出各行内容
				struct CMS_DB_NODE * cdn = cdh->next;
				struct UserInfo * ui = NULL;
				int i = 1;
				while (cdn != NULL)
				{
					ui = (struct UserInfo *)cdn->content;

					sheet->writeStr(i, 0, ui->MSISDN);
					sheet->writeNum(i, 1, ui->balance);
					sheet->writeNum(i, 2, ui->PlanNo);
					sheet->writeStr(i, 3, ui->status);
					sheet->writeStr(i, 4, ui->name);
					sheet->writeStr(i, 5, ui->gender);
					sheet->writeStr(i++, 6, ui->address);

					cdn = cdn->next;
				}

				sheet->setCol(1, 1, 12);
			}
			//将当前时间作为文件名的一部分
			//SYSTEMTIME st;
			//GetLocalTime(&st);

			CString wszFileName = L"D:/用户数据.xls";
			//wszFileName.Format(L"用户数据 %4d-%2d-%2d %2d:%2d:%2d.xls",
			//	 st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			//wszFileName += ".xls";
			if (book->save(wszFileName.GetBuffer(wszFileName.GetLength())))
			{
				::ShellExecute(NULL, L"open", wszFileName.GetBuffer(wszFileName.GetLength()), NULL, NULL, SW_SHOW);
				isSucceed = true;
			}
			else
			{
				//std::cout << book->errorMessage() << std::endl;
				::MessageBox(m_hWnd, s2ws(book->errorMessage()).c_str(), L"XLS FAILED", MB_OK | MB_ICONERROR);
				isSucceed = false;
			}

			book->release();
		}
		else
			isSucceed = false;
	}
	delete dbgo;
	delete cdh;
	dbgo = NULL;
	cdh = NULL;
	return isSucceed;
}


// 导入用户数据
bool CIOuserFrameWnd::OnIn()
{
	bool isSucceed = false;
	//创建链表头
	struct CMS_DB_HEADER * cdh = new struct CMS_DB_HEADER;
	if (cdh == NULL)
		return false;
	InitCDH(cdh);
	//打开并从xls读入用户数据
	Book* book = xlCreateBook();
	if (book)
	{
		if (book->load(L"D:/用户数据.xls"))
		{
			Sheet* sheet = book->getSheet(0);

			if (sheet)
			{
				//const wchar_t* s = sheet->readStr(2, 1);
				//if (s) std::wcout << s << std::endl << std::endl;

				//std::cout << sheet->readNum(4, 1) << std::endl;
				//std::cout << sheet->readNum(5, 1) << std::endl;
				//const wchar_t* f = sheet->readFormula(6, 1);
				//if (f) std::wcout << f << std::endl << std::endl;
				const wchar_t* s = sheet->readStr(1, 0);
				struct CMS_DB_NODE * cdn = NULL;
				struct UserInfo * pUI = NULL;
				int i = 1;		//初始行为1行
				while (s != NULL)
				{
					//创建结点
					cdn = new struct CMS_DB_NODE;
					if (cdn == NULL)
						return false;
					memset(cdn, 0, sizeof(struct CMS_DB_NODE));
					pUI = (struct UserInfo*)cdn->content;
					//填写结点
					wcsncpy(pUI->MSISDN, s, wcsnlen_s(s, MSISDN_LEN));		//MSISDN
					pUI->balance = sheet->readNum(i, 1);									//余额
					pUI->PlanNo = (int)sheet->readNum(i, 2);							//套餐编号
					s = sheet->readStr(i, 3);
					if (s == NULL)
						return false;
					wcsncpy(pUI->status, s, wcsnlen_s(s, STATUS_LEN));				//状态
					s = sheet->readStr(i, 4);
					if (s == NULL)
						return false;
					wcsncpy(pUI->name, s, wcsnlen_s(s, NAME_LEN));				//姓名
					s = sheet->readStr(i, 5);
					if (s == NULL)
						return false;
					wcsncpy(pUI->gender, s, wcsnlen_s(s, GENDER_LEN));				//性别
					s = sheet->readStr(i++, 6);
					if (s == NULL)
						return false;
					wcsncpy(pUI->address, s, wcsnlen_s(s, ADDR_LEN));				//地址
					//将结点加入链表
					AddToCDH(cdh, pUI, sizeof(struct UserInfo));
					//指向下一行数据
					s = sheet->readStr(i, 0);
				}
			}
		}
		else
		{
			//std::cout << "At first run generate !" << std::endl;
			::MessageBox(m_hWnd, L"确认“用户数据.xls”存在？", L"提示", MB_OK | MB_ICONQUESTION);
			DeleteCDH(cdh);
			isSucceed = false;
		}

		book->release();
	}
	else
		return false;

	if (cdh->n == 0)
	{
		DeleteCDH(cdh);
		return false;
	}
	//连接数据库 删除原有数据
	DBgo * dbgo = new DBgo;
	if (dbgo == NULL)
	{
		DeleteCDH(cdh);
		return false;
	}
	if (dbgo->DropUserInfo(NULL, DUI_WAY_ALL) == false)
	{
		delete dbgo;
		dbgo = NULL;
		DeleteCDH(cdh);
		return false;
	}
	//添加新数据
	struct CMS_DB_NODE * cdn = cdh->next;
	struct UserInfo * ui = NULL;
	isSucceed = true;
	while (cdn != NULL)
	{
		ui = (struct UserInfo*)cdn->content;
		if (dbgo->AddUserInfo(ui) == false)
		{
			isSucceed = false;
			break;
		}
		cdn = cdn->next;
	}

	//结束
	delete dbgo;
	dbgo = NULL;
	DeleteCDH(cdh);
	return isSucceed;
}
