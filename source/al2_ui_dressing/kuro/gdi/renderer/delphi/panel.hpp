#pragma once

namespace apn::dark::kuro::gdi::delphi
{
	struct panel_renderer_t : renderer_t
	{
		virtual COLORREF on_get_sys_color(int color_id) override
		{
			MY_TRACE_FUNC("{/}", color_id);

			constexpr auto get_color = [](auto id) {
				return style.get_color_entry(id).parts[0].win32;
			};

			switch (color_id)
			{
			case COLOR_BTNHIGHLIGHT:
			case COLOR_BTNSHADOW:
				return get_color(style_t::color_e::WindowSeparator);
			}

			return __super::on_get_sys_color(color_id);
		}
	};
}
