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

// 提升进程权限
bool ElevatePrivileges();

// 微信版本号解密计算 63060012 -> 3.6.0.18
std::string VeChatVerEncode(DWORD hexVersion);

// 微信版本号加密计算 3.6.0.18 -> 63060012
DWORD VeChatVerDecode(const std::string& version);

// WString转String
std::string WStringToString(const std::wstring& wstr);

// 将 string 转换为 DWORD
DWORD StringToDWORD(const std::string& str);

// 读取注册表值
std::wstring ReadRegistryValue(HKEY hKey, const std::wstring& subKey, const std::wstring& valueName);

// 读取注册表值
DWORD ReadRegistryValueDWORD(HKEY hKey, const std::wstring& subKey, const std::wstring& valueName);

// 将十进制结果转换为16进制
std::string decimalToHex(DWORD decimalValue);

// 转为十六进制的小端序
std::string decimalToLittleEndianHex(DWORD decimalValue);

// 十六进制转为小端序特征码
std::string reverseHexPairs(const std::string& hexStr);

// 获取模块基址和大小
bool GetModuleBaseAndSize(HANDLE hProcess, const wchar_t* moduleName, MODULEINFO& modInfo);

// 从INI文件中读取值
std::string ReadIniValue(const std::string& section, const std::string& key, const std::string& defaultValue, const std::string& filePath);

// 将值写入 INI 文件
bool WriteIniValue(const std::string& section, const std::string& key, const std::string& value, const std::string& filePath);

// 内存搜索函数，搜索4字节的十六进制数据
std::vector<DWORD> SearchMemoryForHex(HANDLE hProcess, DWORD start, DWORD end, const std::string& searchPattern);