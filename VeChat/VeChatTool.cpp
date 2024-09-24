#include "VeChatTool.h"
// ���ڶ࿪΢��
const wchar_t* wechatMutexW = L"_WeChat_App_Instance_Identity_Mutex_Name";

// ��������Ȩ��
bool ElevatePrivileges() {
	HANDLE hToken = NULL;
	//�򿪵�ǰ���̵ķ�������
	int hRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
	if (hRet)
	{
		TOKEN_PRIVILEGES tp{};
		tp.PrivilegeCount = 1;
		//ȡ������Ȩ�޵�LUID
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		//�����������Ƶ�Ȩ��
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		CloseHandle(hToken);
	}
	return TRUE;
}

// ��ȡע���ֵ
std::wstring ReadRegistryValue(HKEY hKey, const std::wstring& subKey, const std::wstring& valueName) {
	HKEY hOpenedKey;

	if (RegOpenKeyEx(hKey, subKey.c_str(), 0, KEY_READ, &hOpenedKey) != ERROR_SUCCESS) {
		std::cerr << "Failed to open registry key." << std::endl;
		return L""; // ���ؿ��ַ���
	}

	wchar_t buffer[512]; // ʹ�ÿ��ַ�������
	DWORD bufferSize = sizeof(buffer);
	DWORD type;

	// ��ѯֵ
	if (RegQueryValueEx(hOpenedKey, valueName.c_str(), NULL, &type, (LPBYTE)buffer, &bufferSize) != ERROR_SUCCESS) {
		std::cerr << "Failed to read registry value." << std::endl;
		RegCloseKey(hOpenedKey);
		return L""; // ���ؿ��ַ���
	}

	RegCloseKey(hOpenedKey);

	// ȷ����ȡ��ֵ���ַ�������
	if (type == REG_SZ) {
		buffer[bufferSize / sizeof(wchar_t) - 1] = L'\0'; // ȷ���� null ��β
		return std::wstring(buffer); // ���ؿ��ַ���
	}
	else if (type == REG_DWORD) {
		DWORD value;
		memcpy(&value, buffer, sizeof(DWORD));
		return std::to_wstring(value); // �� DWORD ת��Ϊ���ַ���
	}

	std::cerr << "Value is not a recognized type." << std::endl;
	return L""; // ���ؿ��ַ���
}

// ��ȡע���ֵ_DWORD
DWORD ReadRegistryValueDWORD(HKEY hKey, const std::wstring& subKey, const std::wstring& valueName) {
	HKEY hOpenedKey;
	if (RegOpenKeyEx(hKey, subKey.c_str(), 0, KEY_READ, &hOpenedKey) != ERROR_SUCCESS) {
		return 0;
	}
	char buffer[512]; // ������
	DWORD bufferSize = sizeof(buffer);
	DWORD type;
	// ��ѯֵ
	if (RegQueryValueEx(hOpenedKey, valueName.c_str(), NULL, &type, (LPBYTE)buffer, &bufferSize) != ERROR_SUCCESS) {
		RegCloseKey(hOpenedKey);
		return 0;
	}
	RegCloseKey(hOpenedKey);
	DWORD value;
	memcpy(&value, buffer, sizeof(DWORD));
	return value; 
}

// WStringתString
std::string WStringToString(const std::wstring& wstr) {
	// ������Ҫ�Ļ�������С
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	std::string str(bufferSize, 0);
	// �����ַ���ת��Ϊխ�ַ���
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], bufferSize, NULL, NULL);
	return str;
}

// stringתDWORD
DWORD StringToDWORD(const std::string& str) {
	return static_cast<DWORD>(std::stoul(str));  // ת��Ϊ�޷��ų�����
}

// ��ʮ���ƽ��ת��Ϊ16����
std::string decimalToHex(DWORD decimalValue) {
	std::stringstream ss;
	ss << std::hex << std::uppercase << decimalValue;
	return ss.str();
}

// תΪʮ�����Ƶ�С����
std::string decimalToLittleEndianHex(DWORD decimalValue) {
	std::stringstream ss;
	// ����С����ת������ȷ��ÿ���ֽڶ�����λ��
	ss << std::hex << std::setw(2) << std::setfill('0') << (decimalValue & 0xFF) << " ";
	ss << std::setw(2) << std::setfill('0') << ((decimalValue >> 8) & 0xFF) << " ";
	ss << std::setw(2) << std::setfill('0') << ((decimalValue >> 16) & 0xFF) << " ";
	ss << std::setw(2) << std::setfill('0') << ((decimalValue >> 24) & 0xFF);

	return ss.str();
}

// ʮ������תΪС����������
std::string reverseHexPairs(const std::string& hexStr) {
	// ȷ�������ַ����ĳ�����ż��
	if (hexStr.length() % 2 != 0) {
		return "Invalid input"; // �쳣����
	}

	std::vector<std::string> pairs;

	// ����һ�Խ��зָ�
	for (size_t i = 0; i < hexStr.length(); i += 2) {
		pairs.push_back(hexStr.substr(i, 2));
	}

	// ��������
	std::reverse(pairs.begin(), pairs.end());

	// �����ƴ�ӳ�һ���ַ��������ո�
	std::ostringstream oss;
	for (size_t i = 0; i < pairs.size(); ++i) {
		oss << pairs[i];
		if (i != pairs.size() - 1) {
			oss << " ";  // ��ÿ��֮����Ͽո����һ������
		}
	}

	return oss.str();
}

// ΢�Ű汾�ż��ܼ��� 3.6.0.18 -> 63060012
DWORD VeChatVerDecode(const std::string& version) {
	// �ָ�汾��
	std::vector<int> versionParts;
	std::istringstream ss(version);
	std::string token;

	while (std::getline(ss, token, '.')) {
		versionParts.push_back(std::stoi(token)); // ת��Ϊ����������vector
	}

	// ȷ���ָ���Ĳ���Ϊ4��
	if (versionParts.size() != 4) {
		return 0;
	}

	// �������հ汾��
	int Dowd = (((96 + versionParts[0]) * 256 + versionParts[1]) * 256 + versionParts[2]) * 256 + versionParts[3];
	return Dowd;
}

// ΢�Ű汾�Ž��ܼ��� 63060012 -> 3.6.0.18
std::string VeChatVerEncode(DWORD hexVersion) {
	// ��ʮ��������ת��Ϊ�ı���ʽ
	std::stringstream ss;
	ss << std::hex << std::uppercase << hexVersion;
	std::string hexStr = ss.str();
	// ����Ƿ���8���ַ�
	if (hexStr.length() != 8) {
		return "";
	}
	// ��ȡ�������ֲ�ת��
	std::string temp[5];
	temp[4] = std::to_string(std::stoi(hexStr.substr(6, 2), nullptr, 16)); // ת����������ַ�
	temp[3] = hexStr.substr(5, 1);  // ��6���ַ�
	temp[2] = hexStr.substr(3, 1);  // ��4���ַ�
	temp[1] = hexStr.substr(1, 1);  // ��2���ַ�

	// �������ս������С����
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

// ��ȡģ���ַ�ʹ�С
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

// ��INI�ļ��ж�ȡֵ
std::string ReadIniValue(const std::string& section, const std::string& key, const std::string& defaultValue, const std::string& filePath) {
	char result[256]; // ANSI �ַ�����洢���
	GetPrivateProfileStringA(section.c_str(), key.c_str(), defaultValue.c_str(), result, sizeof(result), filePath.c_str());
	return std::string(result);  // ת��Ϊ std::string ����
}

// ��ֵд��INI�ļ�
bool WriteIniValue(const std::string& section, const std::string& key, const std::string& value, const std::string& filePath) {
	return WritePrivateProfileStringA(section.c_str(), key.c_str(), value.c_str(), filePath.c_str());
}

// ������Ŀ����ַ���ת��Ϊ 4 �ֽ�С������
std::vector<BYTE> ConvertHexStringToBytes(const std::string& hexStr) {
	std::vector<BYTE> byteArray;
	std::stringstream ss(hexStr);
	std::string byteStr;

	// ���ո�ָ������ʮ�������ַ�����������ת��Ϊ BYTE
	while (std::getline(ss, byteStr, ' ')) {
		byteArray.push_back(static_cast<BYTE>(std::stoul(byteStr, nullptr, 16)));
	}

	// ȷ����4�ֽ�����
	if (byteArray.size() != 4) {
		std::cout << "�����ʮ�������ַ�����ʽ����ȷ��������4�ֽڵĸ�ʽ���磺\"12 00 06 63\"" << std::endl;
		exit(1);
	}
	return byteArray;
}

// ����ڴ�ҳ�Ƿ��д
bool IsMemoryWritable(MEMORY_BASIC_INFORMATION& mbi) {
	return (mbi.State == MEM_COMMIT) && (mbi.Protect & (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY));
}

// �����ڴ�
std::vector<DWORD> SearchMemoryForHex(HANDLE hProcess, DWORD start, DWORD end, const std::string& searchPattern) {
	DWORD startAddress = start;
	std::vector<DWORD> foundOffsets; // �洢�ҵ���ƫ����
	std::vector<BYTE> byteArray = ConvertHexStringToBytes(searchPattern); // ���ַ���ת��Ϊ4�ֽ�С������
	SIZE_T bufferSize = 4096; // ��������С����ȡ 4KB �ڴ��
	std::vector<BYTE> buffer(bufferSize); // ������
	SIZE_T bytesRead;

	// ����ָ���ڴ淶Χ
	while (start < end) {
		MEMORY_BASIC_INFORMATION mbi;

		// ��ѯ��ǰ�ڴ��������Ϣ
		if (VirtualQueryEx(hProcess, reinterpret_cast<LPCVOID>(start), &mbi, sizeof(mbi)) == 0) {
			break; // �޷���ѯ�ڴ���Ϣ���˳�ѭ��
		}

		// ����ڴ��Ƿ��д
		if (IsMemoryWritable(mbi)) {
			DWORD regionEnd = reinterpret_cast<DWORD>(mbi.BaseAddress) + mbi.RegionSize;
			regionEnd = min(regionEnd, end); // ȷ�����ᳬ��������ַ

			// �������ڴ�����
			while (start < regionEnd) {
				// ��ȡ�ڴ�
				if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(start), buffer.data(), bufferSize, &bytesRead)) {
					// ������ȡ���ڴ�飬����ƥ���4�ֽ�С������
					for (SIZE_T i = 0; i <= bytesRead - byteArray.size(); ++i) {
						if (memcmp(buffer.data() + i, byteArray.data(), byteArray.size()) == 0) {
							// �����������ʼ��ַ��ƫ����
							DWORD offset = (start + i) - startAddress;
							std::cout << "SearchMemoryFor offset: " << std::hex << offset << std::endl;
							foundOffsets.push_back(offset); // �洢ƫ����
						}
					}
				}
				start += bytesRead; // ������ʼ��ַ
			}
		}
		else {
			start += mbi.RegionSize; // ����ڴ治��д��������������
		}
	}

	return foundOffsets;
}