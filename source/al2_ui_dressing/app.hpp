#pragma once

namespace apn::dark
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct app_t : app_interface_t
	{
		//
		// スタイルファイルを監視します。
		//
		std::unique_ptr<my::FileWatcherBase> style_file_watcher;

		//
		// カスタムカラーファイルを監視します。
		//
		std::unique_ptr<my::FileWatcherBase> custom_color_file_watcher;

		//
		// dllの初期化処理を実行します。
		//
		virtual BOOL dll_init() override
		{
			MY_TRACE_FUNC("");

			// 各種開始処理を実行します。
			aviutl2_window.init();
			config_io.init();
			kuro::hook::manager.init();
			kuro::style.init();
			kuro::custom_style.init();
			kuro::paint::manager.init();
			kuro::gdi::manager.init(hive.aviutl2_window);
			kuro::theme::manager.init(hive.aviutl2_window);
			config_dialog.init();

			// コンフィグダイアログをプラグインウィンドウに設定します。
			hive.plugin_window = config_dialog;

			// コンフィグをファイルから読み込みます。
			// ※コンフィグファイルへの書き込みは
			// aviutl2ウィンドウが終了するときに実行されます。
			read_config();

			// スタイルファイルの監視をリセットします。
			reset_style_file_watcher();

			// カスタムカラーファイルの監視をリセットします。
			reset_custom_color_file_watcher();

			// コンフィグをaviutl2ウィンドウに適用します。
//			aviutl2_window.apply_config();

			// 初期化後メッセージをポストします。
			::PostMessage(hive.aviutl2_window, hive.c_message.c_post_init, 0, 0);

#ifdef _DEBUG // テスト用コードです。
			{
				for (int i = 0; i < 50; i++)
					MY_TRACE("{/} => {/hex}\n", i, hive.orig.GetSysColor(i));

				auto window_color = hive.orig.GetSysColor(COLOR_WINDOW);
				auto button_color = hive.orig.GetSysColor(COLOR_BTNFACE);
				auto background_color = hive.orig.GetSysColor(COLOR_BACKGROUND);
				auto menu_color = hive.orig.GetSysColor(COLOR_MENU);
				auto menu_hilight_color = hive.orig.GetSysColor(COLOR_MENUHILIGHT);
				auto menu_bar_color = hive.orig.GetSysColor(COLOR_MENUBAR);
				auto hilight_color = hive.orig.GetSysColor(COLOR_HIGHLIGHT);
				auto button_hilight_color = hive.orig.GetSysColor(COLOR_BTNHIGHLIGHT);
				auto hot_light_color = hive.orig.GetSysColor(COLOR_HOTLIGHT);

				int break_point = 0; // ここでシステムカラーを確認します。
			}
#endif
			return TRUE;
		}

		//
		// dllの後始末処理を実行します。
		//
		virtual BOOL dll_exit() override
		{
			MY_TRACE_FUNC("");

			// 各種終了処理を実行します。
			config_dialog.exit();
			kuro::theme::manager.exit();
			kuro::gdi::manager.exit();
			kuro::paint::manager.exit();
			kuro::custom_style.exit();
			kuro::style.exit();
			kuro::hook::manager.exit();
			config_io.exit();
			aviutl2_window.exit();

			return TRUE;
		}

		//
		// 初期化後処理を実行します。
		//
		virtual BOOL post_init() override
		{
			MY_TRACE_FUNC("");

			::DrawMenuBar(hive.aviutl2_window);
#if 0
			// テスト用ダイアログを表示します。
			test_dialog_t dialog; dialog.do_modal();
#endif
			return TRUE;
		}

		//
		// コンフィグファイルを読み込みます。
		//
		virtual BOOL read_config() override
		{
			MY_TRACE_FUNC("");

			return config_io.read();
		}

		//
		// コンフィグファイルに書き込みます。
		//
		virtual BOOL write_config() override
		{
			MY_TRACE_FUNC("");

			return config_io.write();
		}

		//
		// プロセス内のすべてのウィンドウを再描画します。
		//
		virtual BOOL redraw() override
		{
			MY_TRACE_FUNC("");

			return ::EnumWindows(
				[](HWND hwnd, LPARAM l_param)
			{
				auto pid = DWORD {};
				auto tid = ::GetWindowThreadProcessId(hwnd, &pid);

				if (pid == ::GetCurrentProcessId())
					redraw_window(hwnd);

				return TRUE;
			}, 0);
		}

		//
		// 指定されたウィンドウとその子ウィンドウを再描画します。
		//
		inline static BOOL redraw_window(HWND hwnd)
		{
//			MY_TRACE_FUNC("{/hex}", hwnd);

			if (::GetWindowLong(hwnd, GWL_STYLE) & WS_CAPTION)
				::SendMessage(hwnd, WM_ACTIVATE, hwnd == ::GetActiveWindow(), 0);

			::RedrawWindow(hwnd, 0, 0,
				RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT |
				RDW_INVALIDATE | RDW_ALLCHILDREN);

			return ::EnumChildWindows(hwnd,
				[](HWND hwnd, LPARAM l_param)
			{
#if 0 // aviutl2では必要ない処理だと思われます。
				auto class_name = my::get_class_name(hwnd);

				if (class_name == TRACKBAR_CLASS)
				{
					// トラックバー用の処理です。
					::SendMessage(hwnd, WM_SETFOCUS, 0, 0);
				}
				else if (class_name == WC_BUTTON)
				{
					// ボタン用の処理です。
					auto icon = (HICON)::SendMessage(hwnd, BM_GETIMAGE, IMAGE_ICON, 0);
					::SendMessage(hwnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
				}
				else
#endif
				{
					redraw_window(hwnd);
				}

				return TRUE;
			}, 0);
		}

		//
		// スタイルファイルを再読み込みします。
		//
		BOOL reload_style_file()
		{
			// スタイルファイルを読み込みます。
			if (!kuro::style.read_file(hive.dark.style_file_name.c_str())) return FALSE;

			// マテリアルをリロードします。
			if (!kuro::paint::manager.reload()) return FALSE;

			// すべてのウィンドウを再描画します。
			return redraw();
		}

		//
		// スタイルファイルの監視をリセットします。
		//
		void reset_style_file_watcher()
		{
			style_file_watcher.reset(new my::FileWatcher(
				hive.dark.style_file_name.c_str(), config_dialog, [&]() { reload_style_file(); }));
		}

		//
		// スタイルファイルのパスをセットします。
		//
		virtual BOOL set_style_file_name(const std::wstring& style_file_name) override
		{
			// スタイルファイルのパスをセットします。
			hive.dark.style_file_name = style_file_name;

			// ダイアログコントロールを更新します。
			config_dialog.set_text(idc_dark_style_file_name, hive.dark.style_file_name);

			// スタイルファイルの監視をリセットします。
			reset_style_file_watcher();

			// スタイルファイルを再読み込みします。
			return reload_style_file();
		}

		//
		// カスタムカラーファイルを再読み込みします。
		//
		BOOL reload_custom_color_file()
		{
			// カスタムカラーファイルを読み込みます。
			if (!kuro::custom_style.read_custom_color_file(hive.dark.custom_color_file_name.c_str())) return FALSE;

			// マテリアルをリロードします。
			if (!kuro::paint::manager.reload()) return FALSE;

			// すべてのウィンドウを再描画します。
			return redraw();
		}

		//
		// カスタムカラーファイルの監視をリセットします。
		//
		void reset_custom_color_file_watcher()
		{
			custom_color_file_watcher.reset(new my::FileWatcher(
				hive.dark.custom_color_file_name.c_str(), config_dialog, [&]() { reload_custom_color_file(); }));
		}

		//
		// カスタムカラーファイルのパスをセットします。
		//
		virtual BOOL set_custom_color_file_name(const std::wstring& custom_color_file_name) override
		{
			// カスタムカラーファイルのパスをセットします。
			hive.dark.custom_color_file_name = custom_color_file_name;

			// ダイアログコントロールを更新します。
//			config_dialog.set_text(IDC_JD_CUSTOM_COLOR_FILE_NAME, hive.dark.custom_color_file_name);

			// カスタムカラーファイルの監視をリセットします。
			reset_custom_color_file_watcher();

			// カスタムカラーファイルを再読み込みします。
			return reload_custom_color_file();
		}
	} app_impl;
}
