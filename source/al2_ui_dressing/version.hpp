#pragma once

namespace apn::dark
{
	//
	// このクラスはこのプログラムのバージョンを管理します。
	//
	inline struct version_t {
		//
		// プラグイン名とプラグイン情報です。
		//
		const std::wstring name = L"UIドレッシングMOD";
		const std::wstring information = L"🐍" + name + L"🔖";
		const std::wstring revision = L"r1";
	} version;
}
