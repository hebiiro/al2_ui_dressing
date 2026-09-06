#pragma once

namespace apn::dark::kuro::gdi::delphi
{
	struct combobox_renderer_t : renderer_t
	{
		virtual BOOL on_fill_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			// ドロップダウンボタンを描画します。
			{
				// コンボボックスのパレットを使用します。
				const auto& palette = paint::combobox_material.palette;

				auto part_id = CP_DROPDOWNBUTTONRIGHT;
				auto state_id = CBXSR_NORMAL;

				if (auto pigment = palette.get(part_id, state_id))
					brush = pigment->background.get_brush();
			}

			return hive.orig.FillRect(dc, rc, brush);
		}

		virtual BOOL on_draw_edge(message_state_t* current_state, HDC dc, LPRECT rc, UINT edge, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}, {/hex}", dc, safe_string(rc), edge, flags);

			return TRUE; // 縁の描画を省略します。
		}

		virtual BOOL on_ext_text_out_w(message_state_t* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, options = {/hex}, {/}, {/}, {/}, {/hex}, bk_color = {/hex}, text_color = {/hex}",
				dc, x, y, options, safe_string(rc), safe_string(text, c, options), c, dx, ::GetBkColor(dc), ::GetTextColor(dc));

//			if (!(options & (ETO_GLYPH_INDEX | ETO_IGNORELANGUAGE)))
			{
				// リストボックス用のパレットを使用します。
				const auto& palette = paint::listbox_material.palette;

				auto part_id = EP_EDITTEXT;
				auto state_id = ::IsWindowEnabled(current_state->hwnd) ? ETS_NORMAL : ETS_DISABLED;

				// 選択カラーの場合は
				if (::GetBkColor(dc) == hive.orig.GetSysColor(COLOR_HIGHLIGHT))
					state_id = ETS_SELECTED; // 選択状態として描画します。

				if (auto pigment = palette.get(part_id, state_id))
					return paint::stylus.ext_text_out(dc, x, y, options, rc, text, c, dx, pigment);
			}

			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}
	};
}
