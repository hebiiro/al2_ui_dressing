#pragma once

namespace apn::dark
{
	//
	// このクラスはこのプログラムのバージョンを管理します。
	//
	inline struct version_t : version_base_t {
		//
		// コンストラクタです。
		//
		version_t() : version_base_t(L"UIドレッシングMOD", L"🐍UIドレッシングMOD🔖", L"r2") {}
	} version;
}
