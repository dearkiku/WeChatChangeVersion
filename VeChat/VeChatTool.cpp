#include "VeChatTool.h"
// 用于多开微信
const wchar_t* wechatMutexW = L"_WeChat_App_Instance_Identity_Mutex_Name";

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

// 读取注册表值
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

// WString转String
std::string WStringToString(const std::wstring& wstr) {
	// 计算需要的缓冲区大小
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	std::string str(bufferSize, 0);
	// 将宽字符串转换为窄字符串
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], bufferSize, NULL, NULL);
	return str;
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
	// 将十六进制数转换为文本形式
	std::stringstream ss;
	ss << std::hex << std::uppercase << hexVersion;
	std::string hexStr = ss.str();
	// 检查是否有8个字符
	if (hexStr.length() != 8) {
		return "";
	}
	// 提取各个部分并转换
	std::string temp[5];
	temp[4] = std::to_string(std::stoi(hexStr.substr(6, 2), nullptr, 16)); // 转换最后两个字符
	temp[3] = hexStr.substr(5, 1);  // 第6个字符
	temp[2] = hexStr.substr(3, 1);  // 第4个字符
	temp[1] = hexStr.substr(1, 1);  // 第2个字符

	// 生成最终结果，带小数点
	std::string result;
	for (int i = 1; i <= 4; ++i) {
		if (i != 4) {
			result += temp[i] + ".";
		}
		else {
			result += temp[i];
		}
	}

	return result;
}

// 获取模块基址和大小
bool GetModuleBaseAndSize(HANDLE hProcess, const wchar_t* moduleName, MODULEINFO& modInfo) {
	HMODULE hMods[1024];
	DWORD cbNeeded;
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
		for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); ++i) {
			wchar_t szModName[MAX_PATH];
			if (GetModuleBaseName(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(wchar_t))) {
				// std::cout << i << L"szmname " << szModName << std::endl;
				if (wcscmp(szModName, moduleName) == 0) {
					GetModuleInformation(hProcess, hMods[i], &modInfo, sizeof(modInfo));
					return true;
				}
			}
		}
	}
	return false;
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

// 搜索内存
std::vector<DWORD> SearchMemoryForHex(HANDLE hProcess, DWORD start, DWORD end, const std::string& searchPattern) {
	DWORD startAddress = start;
	std::vector<DWORD> foundOffsets; // 存储找到的偏移量
	std::vector<BYTE> byteArray = ConvertHexStringToBytes(searchPattern); // 将字符串转换为4字节小端序列
	SIZE_T bufferSize = 4096; // 缓冲区大小，读取 4KB 内存块
	std::vector<BYTE> buffer(bufferSize); // 缓冲区
	SIZE_T bytesRead;

	// 遍历指定内存范围
	while (start < end) {
		MEMORY_BASIC_INFORMATION mbi;

		// 查询当前内存区域的信息
		if (VirtualQueryEx(hProcess, reinterpret_cast<LPCVOID>(start), &mbi, sizeof(mbi)) == 0) {
			break; // 无法查询内存信息，退出循环
		}

		// 检查内存是否可写
		if (IsMemoryWritable(mbi)) {
			DWORD regionEnd = reinterpret_cast<DWORD>(mbi.BaseAddress) + mbi.RegionSize;
			regionEnd = min(regionEnd, end); // 确保不会超出结束地址

			// 遍历该内存区域
			while (start < regionEnd) {
				// 读取内存
				if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(start), buffer.data(), bufferSize, &bytesRead)) {
					// 遍历读取的内存块，查找匹配的4字节小端序列
					for (SIZE_T i = 0; i <= bytesRead - byteArray.size(); ++i) {
						if (memcmp(buffer.data() + i, byteArray.data(), byteArray.size()) == 0) {
							// 计算相对于起始地址的偏移量
							DWORD offset = (start + i) - startAddress;
							std::cout << "SearchMemoryFor offset: " << std::hex << offset << std::endl;
							foundOffsets.push_back(offset); // 存储偏移量
						}
					}
				}
				start += bytesRead; // 更新起始地址
			}
		}
		else {
			start += mbi.RegionSize; // 如果内存不可写，跳过整个区域
		}
	}

	return foundOffsets;
}