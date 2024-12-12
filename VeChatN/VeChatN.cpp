// VeChatN.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "VeChatN.h"
#include "function.h"
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HWND hEdit_Version[2], hButton[2], hBitMap, hCheckBox[2], hStatic[2], hAccept;
// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
// INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。
	ElevatePrivileges();
	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_VECHATN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VECHATN));

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VECHATN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VECHATN);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // 将实例句柄存储在全局变量中

	//HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	//	CW_USEDEFAULT, 0, 303, 194, nullptr, nullptr, hInstance, nullptr);
	HWND hWnd = CreateWindowEx(
		WS_EX_TOPMOST, szWindowClass, szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, 0, 303, 194,
		NULL, NULL, hInstance, NULL);
	if (!hWnd) {
		return FALSE;
	}
	// 修改窗口风格以隐藏最小化和最大化按钮
	LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
	style &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX); // 清除WS_MINIMIZEBOX和WS_MAXIMIZEBOX风格
	SetWindowLongPtr(hWnd, GWL_STYLE, style);

	// 设置窗口为始终置顶
	// SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

// 获取进程路径
bool GetProcessPath(DWORD processId, HANDLE hProcess, std::wstring& processPath) {
	// 获取路径缓冲区
	WCHAR pathBuffer[MAX_PATH];
	DWORD pathLength = MAX_PATH;

	// 优先使用 QueryFullProcessImageName
	if (QueryFullProcessImageName(hProcess, 0, pathBuffer, &pathLength)) {
		// processPath.assign(pathBuffer);
		std::filesystem::path processPathF(pathBuffer);
		processPath = processPathF.parent_path().wstring();
	}
	else {
		// 如果 QueryFullProcessImageName 不可用，则尝试使用 GetModuleFileNameEx
		if (!GetModuleFileNameEx(hProcess, NULL, pathBuffer, MAX_PATH)) {
			CloseHandle(hProcess);
			// MessageBox(NULL, L"无法获取进程路径", L"错误", MB_OK | MB_ICONERROR);
			return false;
		}
		// processPath.assign(pathBuffer);
		std::filesystem::path processPathF(pathBuffer);
		processPath = processPathF.parent_path().wstring();
	}
	// 如果末尾没有 '\\'，手动添加
	if (!processPath.empty() && processPath.back() != L'\\') {
		processPath += L'\\';
	}
	CloseHandle(hProcess);
	return true;
}

// 获取进程名
std::wstring GetProcessName(HWND hWnd, std::wstring& processPath) {
	DWORD processId = 0;
	GetWindowThreadProcessId(hWnd, &processId); // 获取进程ID
	if (processId == 0) return L"";

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
	if (hProcess == NULL) return L"";

	WCHAR processName[MAX_PATH] = { 0 };
	GetModuleBaseName(hProcess, NULL, processName, MAX_PATH);
	GetProcessPath(processId, hProcess, processPath);
	return std::wstring(processName);
}
//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static bool isDragging = false; // 是否拖动
	static HCURSOR hCursor = LoadCursor(NULL, IDC_CROSS); // 十字鼠标指针
	static HBRUSH hbrBackground;  // 用于保存背景色的画刷
	switch (message) {
	case WM_CREATE: {
		// hbrBackground = CreateSolidBrush(RGB(255, 255, 255));  // 设置白色背景
		hbrBackground = CreateSolidBrush(GetSysColor(COLOR_WINDOW));  // 获取系统背景颜色

		// ------------------------------------------------------------------------------
		hStatic[0] = CreateWindowEx(
			0,
			WC_STATIC, L"旧版本号：",
			WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER | ES_AUTOHSCROLL,
			15, 9, 80, 27,
			hWnd, 0, hInst, 0);
		// ------------------------------------------------------------------------------
		hEdit_Version[0] = CreateWindowEx(
			0,
			WC_EDIT, L"0.0.0.0",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_CENTER | SS_CENTERIMAGE,
			100, 9, 173, 27,
			hWnd, 0, hInst, 0);
		// ------------------------------------------------------------------------------
		hStatic[1] = CreateWindowEx(
			0,
			WC_STATIC, L"新版本号：",
			WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER | ES_AUTOHSCROLL,
			15, 54, 80, 27,
			hWnd, 0, hInst, 0);
		// ------------------------------------------------------------------------------
		hEdit_Version[1] = CreateWindowEx(
			0,
			WC_EDIT, L"4.6.1.8",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_CENTER | SS_CENTERIMAGE,
			100, 54, 173, 27,
			hWnd, 0, hInst, 0);
		// ------------------------------------------------------------------------------
		hCheckBox[0] = CreateWindowEx(
			0,
			WC_BUTTON, L"解除多开限制",
			WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BST_CHECKED,
			130, 88, 125, 27,
			hWnd, (HMENU)IDC_CHECKBOX_MUEX, hInst, NULL);

		hCheckBox[1] = CreateWindowEx(
			0,
			WC_BUTTON, L"成功后关闭",
			WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BST_CHECKED,
			15, 88, 110, 27,
			hWnd, (HMENU)IDC_CHECKBOX_CLOSE, hInst, NULL);
		SetCheckBoxState(hCheckBox[1], true);
		// ------------------------------------------------------------------------------
		hBitMap = CreateWindowEx(
			0,
			WC_STATIC, NULL,
			WS_CHILD | WS_VISIBLE | SS_BITMAP,
			15, 119, 32, 32,
			hWnd, 0, hInst, 0);
		SendMessage(hBitMap, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_POS)));
		// ------------------------------------------------------------------------------
		hButton[0] = CreateWindowEx(
			0,
			WC_BUTTON, L"运行并修改",
			WS_CHILD | WS_VISIBLE,
			72, 119, 117, 32,
			hWnd, (HMENU)IDC_BUTTON_RUNM, hInst, 0);
		// ------------------------------------------------------------------------------
		hButton[1] = CreateWindowEx(
			0,
			WC_BUTTON, L"运行",
			WS_CHILD | WS_VISIBLE,
			215, 119, 58, 32,
			hWnd, (HMENU)IDC_BUTTON_RUN, hInst, 0);
		// ------------------------------------------------------------------------------
		if (GetRegConfig()) {
			std::wstring oldVer = stringToWstring(vxInfo.OldVersion);
			std::wstring newVer = stringToWstring(vxInfo.NewVersion);
			SetWindowText(hEdit_Version[0], oldVer.c_str());
			SetWindowText(hEdit_Version[1], newVer.c_str());
		}
		if (IsMultiWeChatEnabled()) {
			SetCheckBoxState(hCheckBox[0], TRUE);
			EnableWindow(hCheckBox[0], FALSE);
		}
		break;
	}
	case WM_CTLCOLORSTATIC: {
		HDC hdcStatic = (HDC)wParam;
		SetBkMode(hdcStatic, TRANSPARENT);  // 透明背景
		SetTextColor(hdcStatic, RGB(0, 0, 0));  // 黑色文本
		return (LRESULT)hbrBackground;  // 设置标签控件的背景色
	}
	case WM_LBUTTONDOWN: {
		// 获取鼠标点击位置
		POINT pt{};
		pt.x = LOWORD(lParam); // 从 lParam 提取 X 坐标
		pt.y = HIWORD(lParam); // 从 lParam 提取 Y 坐标

		// 将鼠标坐标转换为屏幕坐标
		ClientToScreen(hWnd, &pt);

		// 获取图片控件的位置
		RECT bitmapRect;
		GetWindowRect(hBitMap, &bitmapRect);
		if (PtInRect(&bitmapRect, pt)) {
			isDragging = true;
			SetCapture(hWnd);  // 捕获鼠标输入
			SetCursor(hCursor);  // 更改鼠标指针样式
		}
		break;
	}
	case WM_LBUTTONUP: {
		if (isDragging) {
			isDragging = false;
			ReleaseCapture();  // 释放鼠标捕获
			// 获取鼠标位置
			POINT pt;
			GetCursorPos(&pt);

			// 获取鼠标位置对应的窗口句柄
			HWND targetHwnd = WindowFromPoint(pt);
			if (targetHwnd != NULL) {
				std::wstring processName = GetProcessName(targetHwnd, vxInfo.installPath);
				if (processName == L"WeChat.exe") {
					if (vxInfo.installPath.empty()) {
						MessageBox(hWnd, L"无法获取目标程序运行路径", L"提示", MB_OK);
						break;
					}
					if (MessageBox(hWnd, L"目标是 WeChat，是否修改", L"提示", MB_OKCANCEL) == IDOK) {
						OutputDebugString(vxInfo.installPath.c_str());
						DWORD hProcessId = 0;
						GetWindowThreadProcessId(targetHwnd, &hProcessId); // 获取进程ID
						if (hProcessId == NULL) {

							break;
						}
						OutputDebugStringA("\n获取到的进程ID: ");
						OutputDebugStringA(std::to_string(hProcessId).c_str());
						// 进程句柄
						HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, hProcessId);
						if (hProcess == NULL) {
							MessageBox(hWnd, L"无法打开进程", 0, 0);
							std::cout << "OpenProcess Error: " << GetLastError() << std::endl;
							return false;
						}
						BOOL revise = ReviseWeChatMemory(hProcess);
						if (revise) {
							if (GetCheckBoxState(hCheckBox[1])) {
								SendMessage(hWnd, WM_DESTROY, 0, 0);
							}
							MessageBox(hWnd, L"修改成功", L"提示", MB_OK);
						}
						else {
							MessageBox(hWnd, L"修改失败", L"提示", MB_OK);
						}
					}
				}
			}
		}
		break;
	}
	case WM_MOUSEMOVE: {
		if (isDragging) {
			// 在拖拽状态下动态更新鼠标位置
			SetCursor(hCursor);
		}
		break;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDC_CHECKBOX_MUEX: {
			EnableMultiWeChat();
			BOOL enabled = IsMultiWeChatEnabled();
			SetCheckBoxState(hCheckBox[0], enabled);
			EnableWindow(hCheckBox[0], !enabled);
			break;
		}
		case IDC_BUTTON_RUNM: {
			// 运行并修改
			if (RunWeChat(true)) {
				if (GetCheckBoxState(hCheckBox[1])) {
					SendMessage(hWnd, WM_DESTROY, 0, 0);
				}
				MessageBox(hWnd, L"修改成功", L"提示", MB_OK);
			}
			else {
				MessageBox(hWnd, L"修改失败", L"提示", MB_OK);
			}
			break;
		}
		case IDC_BUTTON_RUN: {
			// 仅运行
			RunWeChat(false);
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_CTLCOLORBTN: {
		HDC hdcButton = (HDC)wParam;
		SetBkMode(hdcButton, TRANSPARENT);  // 透明背景
		SetTextColor(hdcButton, RGB(0, 0, 0));  // 黑色文本
		return (LRESULT)hbrBackground;  // 设置按钮控件的背景色
		break;
	}
	case WM_DESTROY:
		DeleteObject(hbrBackground);  // 清理画刷
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}