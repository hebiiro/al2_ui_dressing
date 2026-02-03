#pragma once

namespace apn::dark
{
	namespace
	{
		//
		// このクラスは::PolyPatBlt()の引数です。
		//
		struct PATRECT {
			int x, y, w, h; HBRUSH brush;
		};
	}

	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct hive_t : hive_base_t
	{
		inline static constexpr struct message_t {
			//
			// 初期化後のウィンドウメッセージです。
			//
			inline static const auto c_post_init = ::RegisterWindowMessageW(L"al2_ui_dressing::post_init");
		} c_message;

		//
		// 現在表示されているコモンダイアログの数です。
		//
		ULONG comdlg32_visible_count = {};

		//
		// このクラスはダークモード化の設定です。
		//
		struct dark_t
		{
			//
			// スタイルのファイル名です。
			//
			std::wstring style_file_name;

			//
			// カスタムカラーのファイル名です。
			//
			std::wstring custom_color_file_name;

			//
			// TRUEの場合はコモンダイアログをダークモード化から除外します。
			//
			BOOL flag_exclude_comdlg32 = FALSE;
		} dark;

		//
		// このクラスはDirect2Dの設定です。
		//
		struct d2d_t
		{
			//
			// TRUEの場合はDirect2Dを使用して図形を描画します。
			//
			BOOL flag_figure = FALSE;

			//
			// TRUEの場合はDirect2Dを使用して文字列を描画します。
			//
			BOOL flag_text = FALSE;
		} d2d;

		//
		// このクラスはスクロールバーの設定です。
		//
		struct scrollbar_t
		{
			//
			// TRUEの場合は矢印をボタンのように描画します。
			//
			BOOL flag_arrow_as_button = FALSE;

			//
			// TRUEの場合はつまみのグリッパーを描画します。
			//
			BOOL flag_has_gripper = FALSE;

			//
			// スクロールバーの縮小率です。%単位です。
			//
			int32_t reduction = 50;
		} scrollbar;

		//
		// このクラスは丸みの設定です。
		//
		struct round_t
		{
			//
			// TRUEの場合は矩形の角を丸めます。
			//
			BOOL flag_use = TRUE;

			//
			// 丸角のサイズです。%単位です。
			//
			int32_t size = 20;
		} round;

		//
		// このクラスは縁の設定です。
		//
		struct border_t
		{
			//
			// TRUEの場合は縁を3Dのように描画します。
			//
			BOOL flag_3d_edge = FALSE;

			//
			// 縁の幅です。1/10px単位です。
			//
			int32_t width = 10;
		} border;

		//
		// このクラスはグラデーションの設定です。
		//
		struct gradient_t
		{
			//
			// TRUEの場合は背景をグラデーションで描画します。
			//
			BOOL flag_use = TRUE;

			//
			// TRUEの場合はグラデーション終了色にアルファを適用します。
			//
			BOOL flag_end_alpha = TRUE;

			//
			// グラデーション終了色のアルファです。%単位です。
			//
			int32_t end_alpha = 60;
		} gradient;

		//
		// このクラスは影の設定です。
		//
		struct shadow_t
		{
			//
			// TRUEの場合はテキストの影を描画します。
			//
			BOOL flag_use = TRUE;

			//
			// TRUEの場合は影をぼかします。
			//
			BOOL flag_blur = TRUE;

			//
			// 影のオフセット(位置)です。1/10px単位です。
			//
			POINT offset = { 10, 30 };

			//
			// ぼかしの範囲です。1/10px単位です。
			//
			int32_t size = 15;
		} shadow;

		//
		// レンダラを使用するかどうかのフラグです。
		// レンダラの使用を停止したい場合はTRUEに設定します。
		//
		thread_local inline static BOOL is_renderer_locked = FALSE;

		//
		// メインスレッドのIDです。
		//
		const DWORD main_thread_id = ::GetCurrentThreadId();

		//
		// 現在のスレッドが有効の場合はTRUEを返します。
		//
		BOOL is_valid_thread() const
		{
			// レンダラーがロックされている場合は無効です。
			if (is_renderer_locked) return FALSE;

			// メインスレッドではない場合は無効です。
			if (main_thread_id != ::GetCurrentThreadId()) return FALSE;

			// コモンダイアログを除外する場合は
			if (dark.flag_exclude_comdlg32)
			{
				// コモンダイアログが表示されていない場合は有効です。
				return comdlg32_visible_count == 0;
			}

			// このスレッドは有効です。
			return TRUE;
		}

		//
		// このクラスはフックする前のオリジナルのAPIを保持します。
		//
		struct orig_t
		{
			LRESULT (WINAPI *CallWindowProcWInternal)(void* u1, WNDPROC wnd_proc, HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
			decltype(&::Rectangle) Rectangle;
			decltype(&::FillRect) FillRect;
			BOOL (WINAPI *DrawFrame)(HDC dc, LPRECT rc, UINT width, UINT type);
			decltype(&::DrawFrameControl) DrawFrameControl;
			decltype(&::FrameRect) FrameRect;
			decltype(&::DrawEdge) DrawEdge;
			decltype(&::DrawFocusRect) DrawFocusRect;
			decltype(&::DrawStateW) DrawStateW;
			decltype(&::GrayStringW) GrayStringW;
			decltype(&::DrawTextW) DrawTextW;
			decltype(&::DrawTextExW) DrawTextExW;
			decltype(&::DrawShadowText) DrawShadowText;
			decltype(&::DrawMenuBar) DrawMenuBar;
			decltype(&::ExtTextOutW) ExtTextOutW;
			decltype(&::PatBlt) PatBlt;
			BOOL (WINAPI *PolyPatBlt)(HDC dc, DWORD rop, const PATRECT* rects, int nb_rects, ULONG reserved);
			decltype(&::GetSysColor) GetSysColor;
			decltype(&::GetSysColorBrush) GetSysColorBrush;

			decltype(&::GetThemeColor) GetThemeColor;
			decltype(&::DrawThemeParentBackground) DrawThemeParentBackground;
			decltype(&::DrawThemeBackground) DrawThemeBackground;
			decltype(&::DrawThemeBackgroundEx) DrawThemeBackgroundEx;
			decltype(&::DrawThemeText) DrawThemeText;
			decltype(&::DrawThemeTextEx) DrawThemeTextEx;
			decltype(&::DrawThemeIcon) DrawThemeIcon;
			decltype(&::DrawThemeEdge) DrawThemeEdge;
		} orig = {};
	} hive;
}
