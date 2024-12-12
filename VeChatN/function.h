#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <iomanip>  
#include <winnt.h>
#include <memoryapi.h>
#include <handleapi.h>
#include <iostream>
#include <WinBase.h>
#include <sstream>
#include <psapi.h>
#include <aclapi.h>
#include <algorithm>

#include "global.h"


// WString转String
std::string WStringToString(const std::wstring& wstr);

// string转Wstring
std::wstring stringToWstring(const std::string& str);

// ---------------------------------------------------------

// 提升进程权限
bool ElevatePrivileges();

// 微信多开限制解除
bool EnableMultiWeChat();

// 检查是否可以多开
bool IsMultiWeChatEnabled();

// 获取注册表配置
bool GetRegConfig();

// ---------------------------------------------------------

// 运行WeChat
bool RunWeChat(bool memory);

// 修改微信内存中的版本号
bool ReviseWeChatMemory(HANDLE hProcess);

// ---------------------------------------------------------

// 获取复选框的选中状态
BOOL GetCheckBoxState(HWND hCheckBox);

// 设置复选框的选中状态
void SetCheckBoxState(HWND hCheckBox, BOOL Checked);

// ---------------------------------------------------------

// 搜索内存函数
std::vector<DWORD64> SearchMemoryForHex(HANDLE hProcess, DWORD64 start, DWORD64 end, const std::string& searchPattern);

// 检查内存页是否可写
bool IsMemoryWritable(MEMORY_BASIC_INFORMATION& mbi);

// 将搜索目标从字符串转换为 4 字节小端序列
std::vector<BYTE> ConvertHexStringToBytes(const std::string& hexStr);

// 获取模块基址和大小
bool GetModuleBaseAndSize(HANDLE hProcess, MODULEINFO& modInfo);

// ---------------------------------------------------------

// 从INI文件中读取值
std::string ReadIniValue(const std::string& section, const std::string& key, const std::string& defaultValue, const std::string& filePath);

// 将值写入INI文件
bool WriteIniValue(const std::string& section, const std::string& key, const std::string& value, const std::string& filePath);