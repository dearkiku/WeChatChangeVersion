#include "VeChatTool.h" // 所有调用的方法都在这里面
//int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevinstance, _In_  LPSTR lpCmdLine, _In_ int nShowCmd)
int main()
{
	ElevatePrivileges();
	// EnumerateWeChatProcesses();

	//std::string version = "3.6.0.18"; // 输入的版本号
	//int result = VeChatVerDecode(version); // 调用函数计算
	//
	//std::cout << "VeChatVerDecode: " << result << std::endl; // 输出结果
	//std::cout << "decimalToLittleEndianHex: " << decimalToLittleEndianHex(result) << std::endl; // 输出结果
	//std::cout << "decimalToHex VeChatVerDecode: " << decimalToHex(result) << std::endl; // 输出结果
	//std::cout << "reverseHexPairs: " << reverseHexPairs(decimalToHex(result)) << std::endl; // 输出结果
	// 
	//int hexVersion = 0x63060012; // 输入十六进制的版本号
	//std::string restored = VeChatVerEncode(hexVersion);
	//std::cout << "Restored version: " << restored << std::endl; // 应该输出 3.6.0.18

	// 注册表子项的名称
	std::wstring subKey = L"SOFTWARE\\Tencent\\WeChat";

	// 读取安装路径
	std::wstring installPath = ReadRegistryValue(HKEY_CURRENT_USER, subKey, L"InstallPath");
	std::cout << "InstallPath: " << installPath.c_str() << std::endl;
	// 读取版本号 1661337618 -> 3.6.0.18
	std::string oldVersion = VeChatVerEncode(ReadRegistryValueDWORD(HKEY_CURRENT_USER, subKey, L"Version"));

	// 3.6.0.18 -> 63060012 -> 12 00 06 63
	// std::cout << "Version1: " << decimalToLittleEndianHex(VeChatVerDecode(oldVersion)) << std::endl;

	// 定义 STARTUPINFO 和 PROCESS_INFORMATION 结构体
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	// si.dwFlags = STARTF_USESHOWWINDOW;  // 使用 wShowWindow 的值
	// si.wShowWindow = SW_HIDE;           // 设置进程隐藏
	ZeroMemory(&pi, sizeof(pi));
	// 拼接程序路径
	std::wstring weChatPath = installPath + L"\\WeChat.exe";
	// 启动 WeChat.exe
	// https://learn.microsoft.com/zh-cn/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw
	if (!CreateProcess(weChatPath.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		std::cout << "进程创建错误: " << GetLastError() << std::endl;
		return 1;
	}
	DWORD hProcessId = pi.dwProcessId;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, hProcessId);
	if (hProcess == NULL) {
		std::cout << "OpenProcess Error: " << GetLastError() << std::endl;
		return 1;
	}
	// 拼接INI文件路径
	const std::string iniFilePath = WStringToString(installPath + L"\\vconf.ini");
	// std::cout << "iniFilePath: " << iniFilePath << std::endl;
	// std::cout << "进程ID: " << hProcessId << " 进程句柄: " << hProcess << std::endl;
	Sleep(2618);
	// 旧的版本号
	// std::string oldVersion = "3.6.0.18"; // 当前的版本号
	// 版本号转特征码
	// std::string oldVersionDecode = reverseHexPairs(decimalToHex(VeChatVerDecode(oldVersion))); // 调用函数计算
	std::string oldVersionDecode = decimalToLittleEndianHex(VeChatVerDecode(oldVersion)); // 调用函数计算

	// 新的版本号
	std::string newVersion = ReadIniValue("r", "v", "4.6.1.8", iniFilePath);
	// 版本号编码
	DWORD newVersionDecode = VeChatVerDecode(newVersion);

	// 获取模块句柄
	MODULEINFO modInfo;
	short i = 0;
	while (!GetModuleBaseAndSize(hProcess, L"WeChatWin.dll", modInfo)) {
		if (i >= 15) {
			CloseHandle(hProcess);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			return 1;
		}
		Sleep(2618);
		i++;
	}
	std::cout << "WeChatWin.dll Base Address: " << modInfo.lpBaseOfDll << ", Size: " << modInfo.SizeOfImage << std::endl;
	DWORD startAddress = (DWORD)modInfo.lpBaseOfDll;  // 起始地址
	DWORD endAddress = startAddress + modInfo.SizeOfImage; // 结束地址
	std::cout
		<< "startAddress: " << std::hex << startAddress << " endAddress：" << std::hex << endAddress
		<< "\noldVersion：" << oldVersion << " newVersion：" << newVersion
		<< "\noldVersionDecode：" << oldVersionDecode << " newVersionDecode：" << newVersionDecode
		<< std::endl;
	// 获取偏移总数
	int offsetCount = std::stoi(ReadIniValue("r", "c", "0", iniFilePath));
	// std::cout << "offsetCount: " << offsetCount << std::endl;
	if (offsetCount == 0)
	{
		// 搜索偏移列表
		std::vector<DWORD> offsetList = SearchMemoryForHex(hProcess, startAddress, endAddress, oldVersionDecode);
		for (unsigned int i = 0; i < offsetList.size(); i++)
		{
			// std::cout << "偏移值: " << std::hex << offsetList[i] << std::endl;
			// std::cout << "计算的地址: " << std::hex << startAddress + offsetList[i] << std::endl;
			if (!WriteProcessMemory(hProcess, LPVOID(startAddress + offsetList[i]), &newVersionDecode, 4, 0))
			{
				std::cout << "WriteProcessMemory Error : " << GetLastError() << std::endl;
			}
			else if (!WriteIniValue("l", std::to_string(i), std::to_string(offsetList[i]), iniFilePath))
			{
				std::cout << "WriteIniValue Error : " << GetLastError() << std::endl;
			}
		}
		// 写出偏移总数
		WriteIniValue("r", "c", std::to_string(offsetList.size()), iniFilePath);
	}
	else
	{
		// 从配置项中获取偏移
		for (int i = 0; i < offsetCount; i++)
		{
			std::string offsetKey = std::to_string(i);
			DWORD offset = StringToDWORD(ReadIniValue("l", offsetKey, "", iniFilePath));
			// std::cout << "偏移值: " << std::hex << offset << std::endl;
			// std::cout << "计算的地址: " << std::hex << startAddress + offset << std::endl;
			if (!WriteProcessMemory(hProcess, LPVOID(startAddress + offset), &newVersionDecode, 4, 0))
			{
				std::cout << "WriteProcessMemory Error : " << GetLastError() << std::endl;
			}
		}
	}
	// 储存版本号
	// WriteIniValue("r", "v", newVersion, iniFilePath);
	CloseHandle(hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return 0;
}
