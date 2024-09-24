#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <psapi.h>
#include <tchar.h>
#include <iostream>
#include <tlhelp32.h>

#pragma comment(lib, "Psapi.lib")

// ��������Ȩ��
bool ElevatePrivileges();

// ΢�Ű汾�Ž��ܼ��� 63060012 -> 3.6.0.18
std::string VeChatVerEncode(DWORD hexVersion);

// ΢�Ű汾�ż��ܼ��� 3.6.0.18 -> 63060012
DWORD VeChatVerDecode(const std::string& version);

// WStringתString
std::string WStringToString(const std::wstring& wstr);

// �� string ת��Ϊ DWORD
DWORD StringToDWORD(const std::string& str);

// ��ȡע���ֵ
std::wstring ReadRegistryValue(HKEY hKey, const std::wstring& subKey, const std::wstring& valueName);

// ��ȡע���ֵ
DWORD ReadRegistryValueDWORD(HKEY hKey, const std::wstring& subKey, const std::wstring& valueName);

// ��ʮ���ƽ��ת��Ϊ16����
std::string decimalToHex(DWORD decimalValue);

// תΪʮ�����Ƶ�С����
std::string decimalToLittleEndianHex(DWORD decimalValue);

// ʮ������תΪС����������
std::string reverseHexPairs(const std::string& hexStr);

// ��ȡģ���ַ�ʹ�С
bool GetModuleBaseAndSize(HANDLE hProcess, const wchar_t* moduleName, MODULEINFO& modInfo);

// ��INI�ļ��ж�ȡֵ
std::string ReadIniValue(const std::string& section, const std::string& key, const std::string& defaultValue, const std::string& filePath);

// ��ֵд�� INI �ļ�
bool WriteIniValue(const std::string& section, const std::string& key, const std::string& value, const std::string& filePath);

// �ڴ���������������4�ֽڵ�ʮ����������
std::vector<DWORD> SearchMemoryForHex(HANDLE hProcess, DWORD start, DWORD end, const std::string& searchPattern);