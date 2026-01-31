#pragma once

namespace apn::dark
{
	//
	// このクラスはaviutl2ウィンドウを管理します。
	//
	inline struct aviutl2_window_t
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return catch_aviutl2_window();
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// aviutl2ウィンドウを捕捉します。
		//
		BOOL catch_aviutl2_window()
		{
			MY_TRACE_FUNC("");
#if 0
			hive.aviutl2_window = ::GetParent(hive.plugin_window);

			if (auto owner = ::GetWindow(hive.aviutl2_window, GW_OWNER))
				hive.aviutl2_window = owner;
#else
			// カレントスレッドのウィンドウを列挙します。
			return ::EnumThreadWindows(::GetCurrentThreadId(), [](HWND hwnd, LPARAM l_param)
			{
				// クラス名を取得します。
				auto class_name = my::get_class_name(hwnd);
				MY_TRACE_STR(class_name);

				// クラス名が一致しない場合は除外します。
				if (class_name != L"aviutl2Manager") return TRUE;

				// トップレベルウィンドウではない場合は除外します。
				if (::GetWindow(hwnd, GW_OWNER)) return TRUE;

				// aviutl2ウィンドウをセットします。
				hive.aviutl2_window = hwnd;

				// 列挙を終了します。
				return FALSE;
			}, 0);
#endif
		}
	} aviutl2_window;
}
