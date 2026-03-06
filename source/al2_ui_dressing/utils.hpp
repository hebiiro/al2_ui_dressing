#pragma once

namespace apn::dark
{
	// リターン先のアドレスを返します。
	//
	inline LRESULT CALLBACK get_ret_addr(HWND hwnd, UINT message, WPARAM wp, LPARAM lp)
	{
		return (LRESULT)(&hwnd)[-1];
	}

	//
	// リターンアドレスを返します。
	//
	inline constexpr auto ret_addr(auto* arg1)
	{
		return reinterpret_cast<my::addr_t*>(arg1)[-1];
	}

	inline std::string safe_string(LPCSTR name)
	{
		if ((ULONG_PTR)name > 0x0000FFFF)
			return name;
		else
			return my::format("{/hex}", (my::addr_t)name);
	}

	inline std::wstring safe_string(LPCWSTR name)
	{
		if ((ULONG_PTR)name > 0x0000FFFF)
			return name;
		else
			return my::format(L"{/hex}", (my::addr_t)name);
	}

	inline std::wstring safe_string(LPCRECT rc)
	{
		if (rc)
			return my::format(L"{/}, {/}, {/}, {/}", rc->left, rc->top, rc->right, rc->bottom);
		else
			return L"null";
	}

	inline std::wstring safe_string(LPCWSTR str, int c)
	{
		if (c < 0)
			return str;
		else
			return std::wstring(str, c);
	}

	inline std::wstring safe_string(LPCWSTR str, int c, UINT options)
	{
		if (options & ETO_GLYPH_INDEX)
			return L"ETO_GLYPH_INDEX";
		else
			return safe_string(str, c);
	}

	//
	// このクラスはGDI+を管理します。
	//
	struct gdiplus_manager_t {
		Gdiplus::GdiplusStartupInput si;
		Gdiplus::GdiplusStartupOutput so;
		ULONG_PTR token;
		ULONG_PTR hook_token;
		gdiplus_manager_t() {
			si.SuppressBackgroundThread = TRUE;
			Gdiplus::GdiplusStartup(&token, &si, &so);
			so.NotificationHook(&hook_token);
		}
		~gdiplus_manager_t() {
			so.NotificationUnhook(hook_token);
			Gdiplus::GdiplusShutdown(token);
		}
	};

	//
	// アイコンをビットマップに変換して返します。
	//
	HBITMAP to_bitmap(HICON icon)
	{
		gdiplus_manager_t manager;

		auto bitmap = HBITMAP {};
		Gdiplus::Bitmap(icon).GetHBITMAP(Gdiplus::Color(), &bitmap);
		return bitmap;
	}

	//
	// 指定されたファイルのアイコンを取得します。
	//
	HICON get_icon(const std::wstring& path, int icon_index)
	{
		auto icon = HICON {};
		::ExtractIconExW(path.c_str(), icon_index, nullptr, &icon, 1);
		return icon;
	}

	//
	// 指定されたファイルパスのアイコンを取得します。
	//
	HICON get_shell_icon(const std::wstring& path)
	{
		// シェルからファイル情報を取得します。
		SHFILEINFO sfi = {};
		::SHGetFileInfoW(path.c_str(), 0,
			&sfi, sizeof(sfi), SHGFI_ICON | SHGFI_SMALLICON);

		// アイコンを返します。
		return sfi.hIcon;
	}

	//
	// このクラスは与えられた矩形をクリッピングします。
	//
	struct clipper_t
	{
		HDC dc = {};
		HRGN rgn = {};

		clipper_t(HDC dc, LPCRECT rc, LPCRECT rc_clip)
			: dc(dc)
		{
			if (rc && rc_clip && !::IsRectEmpty(rc_clip) && !::EqualRect(rc, rc_clip))
			{
				::CreateRectRgn(0, 0, 0, 0);
				::GetClipRgn(dc, rgn);

				auto rc = *rc_clip;
				::LPtoDP(dc, (LPPOINT)&rc, 2);

				my::gdi::unique_ptr<HRGN> rgn(
					::CreateRectRgnIndirect(&rc));

				::ExtSelectClipRgn(dc, rgn.get(), RGN_COPY);
			}
		}

		~clipper_t()
		{
			if (rgn)
			{
				::SelectClipRgn(dc, rgn);
				::DeleteObject(rgn);
			}
		}
	};

	//
	// ::ExtTextOut()のフックをロックします。
	//
	thread_local inline my::lockable_t ext_text_out_lock;
}
