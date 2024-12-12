#pragma once
#include <string>

// 定义32位和64位模块标识
#define LIST_MODULES_32BIT 0x01
#define LIST_MODULES_64BIT 0x02
// 基础客户端数据结构
struct clientInfo {
	std::wstring installPath;   // 读取安装路径
	std::string OldVersion;		// 版本号 1661337618 -> 3.6.0.18
	std::string NewVersion;		// 版本号 1661337618 -> 3.6.0.18
	std::string iniFilePath;	// 配置项路径
};
// 客户端
extern clientInfo vxInfo;
// 微信多开互斥体名
extern const wchar_t* wechatMutexW;
