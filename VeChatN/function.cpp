#include "function.h"

// WString转String
std::string WStringToString(const std::wstring& wstr) {
	// 计算需要的缓冲区大小
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	std::string str(bufferSize, 0);
	// 将宽字符串转换为窄字符串
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], bufferSize, NULL, NULL);
	return str;
}

// string转Wstring
std::wstring stringToWstring(const std::string& str) {
	int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, L'\0');
	MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

// string转DWORD
DWORD StringToDWORD(const std::string& str) {
	return static_cast<DWORD>(std::stoul(str));  // 转换为无符号长整数
}

// 将十进制结果转换为16进制
std::string decimalToHex(DWORD decimalValue) {
	std::stringstream ss;
	ss << std::hex << std::uppercase << decimalValue;
	return ss.str();
}

// 将 DWORD 转换为十六进制字符串，并保证长度为 8
std::string ToHexString(DWORD value) {
	std::stringstream ss;
	ss << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << value;
	return ss.str();
}

// 分割字符串为指定长度的部分
std::vector<std::string> SplitHexString(const std::string& hexStr, size_t segmentLength = 2) {
	std::vector<std::string> segments;
	for (size_t i = 0; i < hexStr.size(); i += segmentLength) {
		segments.push_back(hexStr.substr(i, segmentLength));
	}
	return segments;
}

// 转为十六进制的小端序
std::string decimalToLittleEndianHex(DWORD decimalValue) {
	std::stringstream ss;
	// 按照小端序转换，并确保每个字节都是两位数
	ss << std::hex << std::setw(2) << std::setfill('0') << (decimalValue & 0xFF) << " ";
	ss << std::setw(2) << std::setfill('0') << ((decimalValue >> 8) & 0xFF) << " ";
	ss << std::setw(2) << std::setfill('0') << ((decimalValue >> 16) & 0xFF) << " ";
	ss << std::setw(2) << std::setfill('0') << ((decimalValue >> 24) & 0xFF);

	return ss.str();
}

// 十六进制转为小端序特征码
std::string reverseHexPairs(const std::string& hexStr) {
	// 确保输入字符串的长度是偶数
	if (hexStr.length() % 2 != 0) {
		return "Invalid input"; // 异常处理
	}

	std::vector<std::string> pairs;

	// 两两一对进行分割
	for (size_t i = 0; i < hexStr.length(); i += 2) {
		pairs.push_back(hexStr.substr(i, 2));
	}

	// 倒叙排序
	std::reverse(pairs.begin(), pairs.end());

	// 将结果拼接成一个字符串，带空格
	std::ostringstream oss;
	for (size_t i = 0; i < pairs.size(); ++i) {
		oss << pairs[i];
		if (i != pairs.size() - 1) {
			oss << " ";  // 在每对之间加上空格，最后一个不加
		}
	}

	return oss.str();
}

// 微信版本号加密计算 3.6.0.18 -> 63060012
DWORD VeChatVerDecode(const std::string& version) {
	// 分割版本号
	std::vector<int> versionParts;
	std::istringstream ss(version);
	std::string token;

	while (std::getline(ss, token, '.')) {
		versionParts.push_back(std::stoi(token)); // 转换为整数并存入vector
	}

	// 确保分割出的部分为4个
	if (versionParts.size() != 4) {
		return 0;
	}

	// 计算最终版本号
	int Dowd = (((96 + versionParts[0]) * 256 + versionParts[1]) * 256 + versionParts[2]) * 256 + versionParts[3];
	return Dowd;
}

// 微信版本号解密计算 63060012 -> 3.6.0.18
std::string VeChatVerEncode(DWORD hexVersion) {
	std::string hexStr = ToHexString(hexVersion);

	// 分割字符串为四个部分
	std::vector<std::string> segments = SplitHexString(hexStr);

	// 检查是否分割为 4 个部分
	if (segments.size() != 4) {
		return "Invalid Version";
	}

	// 将十六进制字符串转换为十进制，并按需求调整
	int major = std::stoi(segments[0], nullptr, 16) - 96; // 第一部分减去96
	int minor = std::stoi(segments[1], nullptr, 16);      // 第二部分
	int patch = std::stoi(segments[2], nullptr, 16);      // 第三部分
	int build = std::stoi(segments[3], nullptr, 16);      // 第四部分

	// 格式化输出版本号
	std::stringstream versionStream;
	versionStream << major << "." << minor << "." << patch << "." << build;

	return versionStream.str();
	// ↓废弃↓
	//// 将十六进制数转换为文本形式
	//std::stringstream ss;
	//ss << std::hex << std::uppercase << hexVersion;
	//
	//// 检查是否有8个字符
	//if (hexStr.length() != 8) {
	//	return "";
	//}
	//// 提取各个部分并转换
	//std::string temp[5];
	//temp[4] = std::to_string(std::stoi(hexStr.substr(6, 2), nullptr, 16)); // 转换最后两个字符
	//temp[3] = hexStr.substr(5, 1);  // 第6个字符
	//temp[2] = hexStr.substr(3, 1);  // 第4个字符
	//temp[1] = hexStr.substr(1, 1);  // 第2个字符
	//
	//// 生成最终结果，带小数点
	//std::string result;
	//for (int i = 1; i <= 4; ++i) {
	//	if (i != 4) {
	//		result += temp[i] + ".";
	//	}
	//	else {
	//		result += temp[i];
	//	}
	//}
	//
	//return result;
}

// 读取注册表值_wstring
std::wstring ReadRegistryValue(HKEY hKey, const std::wstring& subKey, const std::wstring& valueName) {
	HKEY hOpenedKey;

	if (RegOpenKeyEx(hKey, subKey.c_str(), 0, KEY_READ, &hOpenedKey) != ERROR_SUCCESS) {
		std::cerr << "Failed to open registry key." << std::endl;
		return L""; // 返回空字符串
	}

	wchar_t buffer[512]; // 使用宽字符缓冲区
	DWORD bufferSize = sizeof(buffer);
	DWORD type;

	// 查询值
	if (RegQueryValueEx(hOpenedKey, valueName.c_str(), NULL, &type, (LPBYTE)buffer, &bufferSize) != ERROR_SUCCESS) {
		std::cerr << "Failed to read registry value." << std::endl;
		RegCloseKey(hOpenedKey);
		return L""; // 返回空字符串
	}

	RegCloseKey(hOpenedKey);

	// 确保读取的值是字符串类型
	if (type == REG_SZ) {
		buffer[bufferSize / sizeof(wchar_t) - 1] = L'\0'; // 确保以 null 结尾
		return std::wstring(buffer); // 返回宽字符串
	}
	else if (type == REG_DWORD) {
		DWORD value;
		memcpy(&value, buffer, sizeof(DWORD));
		return std::to_wstring(value); // 将 DWORD 转换为宽字符串
	}

	std::cerr << "Value is not a recognized type." << std::endl;
	return L""; // 返回空字符串
}

// 读取注册表值_DWORD
DWORD ReadRegistryValueDWORD(HKEY hKey, const std::wstring& subKey, const std::wstring& valueName) {
	HKEY hOpenedKey;
	if (RegOpenKeyEx(hKey, subKey.c_str(), 0, KEY_READ, &hOpenedKey) != ERROR_SUCCESS) {
		return 0;
	}
	char buffer[512]; // 缓冲区
	DWORD bufferSize = sizeof(buffer);
	DWORD type;
	// 查询值
	if (RegQueryValueEx(hOpenedKey, valueName.c_str(), NULL, &type, (LPBYTE)buffer, &bufferSize) != ERROR_SUCCESS) {
		RegCloseKey(hOpenedKey);
		return 0;
	}
	RegCloseKey(hOpenedKey);
	DWORD value;
	memcpy(&value, buffer, sizeof(DWORD));
	return value;
}

// -------------------------------------------------------------------------------------------------------------------

// 提升进程权限
bool ElevatePrivileges() {
	HANDLE hToken = NULL;
	//打开当前进程的访问令牌
	int hRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
	if (hRet)
	{
		TOKEN_PRIVILEGES tp{};
		tp.PrivilegeCount = 1;
		//取得描述权限的LUID
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		//调整访问令牌的权限
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		CloseHandle(hToken);
	}
	return TRUE;
}

// 获取模块基址和大小
bool GetModuleBaseAndSize(HANDLE hProcess, MODULEINFO& modInfo) {
	BOOL Bit;
	// 判断进程位数
	IsWow64Process(hProcess, &Bit);
	// 选择获取32位模块或64位模块
	DWORD dwFilterFlag = Bit ? LIST_MODULES_32BIT : LIST_MODULES_64BIT;
	// 模块句柄
	HMODULE hMods[1024];
	// lphModule 数组的字节大小
	DWORD cbNeeded;
	if (EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, dwFilterFlag)) {
		// 遍历模块
		for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
			TCHAR szModName[MAX_PATH];
			// 模块名获取
			if (GetModuleBaseName(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
				// 模块名对比
				if (wcscmp(szModName, L"WeChatWin.dll") == 0) {
					if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
						std::wcout << "模块名：" << szModName << std::endl;
					}
					else {
						std::wcout << "获取模块名错误：" << GetLastError() << std::endl;

					}
					// std::cout << L"模块名称：" << szModName << L" - ERROR:" << GetLastError() << std::endl;
					return	GetModuleInformation(hProcess, hMods[i], &modInfo, sizeof(modInfo));
				}
			}
		}
	}
	else {
		std::cerr << "无法获取模块，错误代码：" << GetLastError() << std::endl;
	}
	return false;
}

// 将搜索目标从字符串转换为 4 字节小端序列
std::vector<BYTE> ConvertHexStringToBytes(const std::string& hexStr) {
	std::vector<BYTE> byteArray;
	std::stringstream ss(hexStr);
	std::string byteStr;

	// 按空格分割输入的十六进制字符串，并将其转换为 BYTE
	while (std::getline(ss, byteStr, ' ')) {
		byteArray.push_back(static_cast<BYTE>(std::stoul(byteStr, nullptr, 16)));
	}

	// 确保是4字节数据
	if (byteArray.size() != 4) {
		std::cout << "输入的十六进制字符串格式不正确，必须是4字节的格式，如：\"12 00 06 63\"" << std::endl;
		exit(1);
	}
	return byteArray;
}

// 检查内存页是否可写
bool IsMemoryWritable(MEMORY_BASIC_INFORMATION& mbi) {
	return (mbi.State == MEM_COMMIT) && (mbi.Protect & (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY));
}

// 搜索内存函数
std::vector<DWORD64> SearchMemoryForHex(HANDLE hProcess, DWORD64 start, DWORD64 end, const std::string& searchPattern) {
	DWORD64 startAddress = start;
	std::vector<DWORD64> foundOffsets;  // 存储找到的偏移量
	std::vector<BYTE> byteArray = ConvertHexStringToBytes(searchPattern);  // 将字符串转换为字节数组
	SIZE_T bufferSize = 4096;  // 缓冲区大小，读取 4KB 内存块
	std::vector<BYTE> buffer(bufferSize);  // 缓冲区
	SIZE_T bytesRead;

	// 遍历指定内存范围
	while (start < end) {
		MEMORY_BASIC_INFORMATION mbi;

		// 查询当前内存区域的信息
		if (VirtualQueryEx(hProcess, reinterpret_cast<LPCVOID>(start), &mbi, sizeof(mbi)) == 0) {
			break;  // 无法查询内存信息，退出循环
		}

		// 检查内存是否可写
		if (IsMemoryWritable(mbi)) {
			DWORD64 regionEnd = reinterpret_cast<DWORD64>(mbi.BaseAddress) + mbi.RegionSize;
			if (regionEnd > end) {
				regionEnd = end;
			}
			// 遍历该内存区域
			while (start < regionEnd) {
				// 读取内存
				if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(start), buffer.data(), bufferSize, &bytesRead)) {
					// 遍历读取的内存块，查找匹配的字节序列
					for (SIZE_T i = 0; i <= bytesRead - byteArray.size(); ++i) {
						if (memcmp(buffer.data() + i, byteArray.data(), byteArray.size()) == 0) {
							// 计算相对于起始地址的偏移量
							DWORD64 offset = (start + i) - startAddress;
							std::cout << "SearchMemoryFor offset: " << std::hex << offset << std::endl;
							foundOffsets.push_back(offset);  // 存储偏移量
						}
					}
				}
				start += bytesRead;  // 更新起始地址
			}
		}
		else {
			start += mbi.RegionSize;  // 如果内存不可写，跳过整个区域
		}
	}
	return foundOffsets;
}

// 获取注册表配置
bool GetRegConfig() {
	// 注册表路径
	std::wstring subKey = L"SOFTWARE\\Tencent\\WeChat";
	// 读取微信安装路径
	vxInfo.installPath = ReadRegistryValue(HKEY_CURRENT_USER, subKey, L"InstallPath");
	if (vxInfo.installPath.empty()) {
		return false;
	}
	// 拼接INI文件路径
	vxInfo.iniFilePath = WStringToString(vxInfo.installPath + L"\\vconf.ini");
	// 读取微信对应版本
	// vxInfo.OldVersion = VeChatVerEncode(ReadRegistryValueDWORD(HKEY_CURRENT_USER, subKey, L"Version"));
	vxInfo.OldVersion = ReadIniValue("r", "o", "", vxInfo.iniFilePath);
	if (vxInfo.OldVersion.empty()) {
		vxInfo.OldVersion = VeChatVerEncode(ReadRegistryValueDWORD(HKEY_CURRENT_USER, subKey, L"Version"));
	}
	// std::to_string(VeChatVerDecode("3.9.12.17")); // 成功还原
	vxInfo.NewVersion = ReadIniValue("r", "n", "4.6.1.8", vxInfo.iniFilePath);
	return true;
}

// 微信多开限制解除
bool EnableMultiWeChat() {
	// 创建一个微信互斥体
	HANDLE hMutex = CreateMutex(NULL, FALSE, wechatMutexW);

	SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
	PSID pEveryoneSID = NULL;
	char szBuffer[4096] = { 0 };
	PACL pAcl = (PACL)szBuffer;

	// 初始化权限分配
	AllocateAndInitializeSid(
		&SIDAuthWorld,
		1,
		SECURITY_WORLD_RID,
		0, 0, 0, 0, 0, 0, 0,
		&pEveryoneSID);

	InitializeAcl(pAcl, sizeof(szBuffer), ACL_REVISION);
	// 设置ACL，禁止访问互斥体，这样微信的实例无法检测到已有的互斥体
	AddAccessDeniedAce(pAcl, ACL_REVISION, MUTEX_ALL_ACCESS, pEveryoneSID);
	if (hMutex == NULL) {
		return false;
	}
	// 更新互斥体的安全信息，完成多开设置
	DWORD dsRes = SetSecurityInfo(hMutex, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pAcl, NULL);
	if (dsRes == ERROR_SUCCESS) {
		return true;
	}
	return false;
}

// 检查是否可以多开
bool IsMultiWeChatEnabled() {
	// 尝试打开互斥体
	HANDLE hMutex = CreateMutex(NULL, FALSE, wechatMutexW);
	if (hMutex == NULL) {
		OutputDebugString(L"\n不能创建互斥体 可以多开微信\n");
		return true;
	}
	OutputDebugString(L"\n可以创建互斥体 不能多开微信\n");
	CloseHandle(hMutex);
	return false;
}


// ---------------------------------------------------------

// 获取复选框的选中状态
BOOL GetCheckBoxState(HWND hCheckBox) {
	// 通过 SendMessage 获取复选框的状态
	return (SendMessage(hCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED);
}

// 设置复选框的选中状态
void SetCheckBoxState(HWND hCheckBox, BOOL Checked) {
	SendMessage(hCheckBox, BM_SETCHECK, Checked ? BST_CHECKED : BST_UNCHECKED, 0);
}

// 从INI文件中读取值
std::string ReadIniValue(const std::string& section, const std::string& key, const std::string& defaultValue, const std::string& filePath) {
	char result[256]; // ANSI 字符数组存储结果
	GetPrivateProfileStringA(section.c_str(), key.c_str(), defaultValue.c_str(), result, sizeof(result), filePath.c_str());
	return std::string(result);  // 转换为 std::string 返回
}

// 将值写入INI文件
bool WriteIniValue(const std::string& section, const std::string& key, const std::string& value, const std::string& filePath) {
	return WritePrivateProfileStringA(section.c_str(), key.c_str(), value.c_str(), filePath.c_str());
}

// 运行WeChat
bool RunWeChat(bool memory) {
	if (vxInfo.installPath.empty()) {
		// 路径为空
		return false;
	}

	STARTUPINFOW si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	// si.dwFlags = STARTF_USESHOWWINDOW;  // 使用 wShowWindow 的值
	// si.wShowWindow = SW_HIDE;           // 设置进程隐藏

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	// 拼接程序路径
	std::wstring weChatPath = vxInfo.installPath + L"\\WeChat.exe";

	if (!CreateProcess(weChatPath.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		MessageBox(NULL, L"进程创建错误: ", 0, 0);
		std::cout << "CreateProcess Error: " << GetLastError() << std::endl;
		return false;
	}

	if (!memory) {
		// CloseHandle(hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return true;
	}

	// ----------需要修改版本号----------

	// 进程ID
	DWORD hProcessId = pi.dwProcessId;

	OutputDebugStringA("\n进程ID：");
	OutputDebugStringA(std::to_string(hProcessId).c_str());

	// 进程句柄
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, hProcessId);

	if (hProcess == NULL) {
		MessageBox(NULL, L"无法打开进程", 0, 0);
		std::cout << "OpenProcess Error: " << GetLastError() << std::endl;
		return false;
	}

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return ReviseWeChatMemory(hProcess);
}

// 修改微信内存中的版本号
bool ReviseWeChatMemory(HANDLE hProcess) {
	// 是否成功获取到模块信息
	DWORD exitCode;
	// 模块信息
	MODULEINFO modInfo;
	while (true) {
		if (GetModuleBaseAndSize(hProcess, modInfo)) {
			// 获取成功跳出循环
			break;
		}
		else {
			if (GetExitCodeProcess(hProcess, &exitCode)) {
				if (exitCode == STILL_ACTIVE) {
					OutputDebugString(L"\n进程仍在运行 没有获取到模块信息");
					Sleep(618);
				}
				else {
					OutputDebugString(L"\n进程已退出，退出码：");
					OutputDebugStringA(std::to_string(exitCode).c_str());
					CloseHandle(hProcess);
					return false; // 进程已结束
				}
			}
			else {
				CloseHandle(hProcess);
				return false; // 进程已结束
			}
		}
	}

	// 模块地址计算
	DWORD64 startAddress = (DWORD64)modInfo.lpBaseOfDll;  // 起始地址
	DWORD64 endAddress = startAddress + modInfo.SizeOfImage; // 结束地址

	OutputDebugStringA("\n起始地址：");	OutputDebugStringA(std::to_string(startAddress).c_str());

	OutputDebugStringA("\n模块大小：");	OutputDebugStringA(std::to_string(modInfo.SizeOfImage).c_str());

	OutputDebugStringA("\n结束地址：");	OutputDebugStringA(std::to_string(endAddress).c_str());

	// 版本号计算
	std::string oldVersionDecode = decimalToLittleEndianHex(VeChatVerDecode(vxInfo.OldVersion));
	DWORD newVersionDecode = VeChatVerDecode(vxInfo.NewVersion);

	OutputDebugStringA("\n小端序旧版本号： ");	OutputDebugStringA(oldVersionDecode.c_str());
	OutputDebugStringA("\n可视化旧版本号： ");	OutputDebugStringA(vxInfo.OldVersion.c_str());
	OutputDebugStringA("\n十进制新版本号： ");	OutputDebugStringA(std::to_string(newVersionDecode).c_str());

	// 获取偏移量总数
	int offsetCount = std::stoi(ReadIniValue("r", "c", "0", vxInfo.iniFilePath));
	std::vector<DWORD64> offsetList;
	if (offsetCount == 0) { // 没有偏移量 需要从进程内存中读取
		// 从内存读取偏移列表
		offsetList = SearchMemoryForHex(hProcess, startAddress, endAddress, oldVersionDecode);
	}
	else { // 有偏移量 遍历修改
		// 从配置项读取偏移列表
		for (int i = 0; i < offsetCount; i++) {
			DWORD offset = StringToDWORD(ReadIniValue("l", std::to_string(i), "", vxInfo.iniFilePath));
			offsetList.push_back(offset);
			// std::cout << "偏移值: " << std::hex << offset << std::endl;
			// std::cout << "计算的地址: " << std::hex << startAddress + offset << std::endl;
			//OutputDebugStringA("\n正在修改的内存基地址(配置项)[");
			//OutputDebugStringA(std::to_string(i).c_str());
			//OutputDebugStringA("] : ");
			//OutputDebugStringA(std::to_string(startAddress + offset).c_str());
			// 
			//if (!WriteProcessMemory(hProcess, LPVOID(startAddress + offset), &newVersionDecode, 4, 0)) {
			//	// std::cout << "WriteProcessMemory Error : " << GetLastError() << std::endl;
			//	OutputDebugStringA("\n内存修改错误： ");
			//	OutputDebugStringA(std::to_string(GetLastError()).c_str());
			//}
		}
	}

	// 将offsetList中的数据遍历修改
	for (unsigned int i = 0; i < offsetList.size(); i++) {
		OutputDebugStringA("\n正在修改的内存基地址[");
		OutputDebugStringA(std::to_string(i).c_str());
		OutputDebugStringA("] : ");
		OutputDebugStringA(std::to_string(startAddress + offsetList[i]).c_str());
		if (!WriteProcessMemory(hProcess, LPVOID(startAddress + offsetList[i]), &newVersionDecode, 4, 0)) {
			// std::cout << "WriteProcessMemory Error : " << GetLastError() << std::endl;
			OutputDebugStringA("\n内存[");
			OutputDebugStringA(std::to_string(i).c_str());
			OutputDebugStringA("]修改错误： ");
			OutputDebugStringA(std::to_string(GetLastError()).c_str());
		}
		//  将找到的偏移写到配置项 下次直接读取无需搜索内存
		if (!WriteIniValue("l", std::to_string(i), std::to_string(offsetList[i]), vxInfo.iniFilePath)) {
			// std::cout << "WriteIniValue Error : " << GetLastError() << std::endl;
			OutputDebugStringA("\n配置项[");
			OutputDebugStringA(std::to_string(i).c_str());
			OutputDebugStringA("]写出错误： ");
			OutputDebugStringA(std::to_string(GetLastError()).c_str());
		}
	}
	// 写出偏移总数
	WriteIniValue("r", "c", std::to_string(offsetList.size()), vxInfo.iniFilePath);
	CloseHandle(hProcess);
	return true;
}