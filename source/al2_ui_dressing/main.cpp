#include "pch.h"
#include "resource.h"
#include "version.hpp"
#include "app_interface.hpp"
#include "hive.hpp"
#include "utils.hpp"
#include "debug.hpp"
#include "kuro/color.hpp"
#include "kuro/style.hpp"
#include "kuro/custom_style.hpp"
#include "kuro/paint/utils.hpp"
#include "kuro/paint/pigment.hpp"
#include "kuro/paint/palette.hpp"
#include "kuro/paint/material.hpp"
#include "kuro/paint/d2d/core.hpp"
#include "kuro/paint/d2d/utils.hpp"
#include "kuro/paint/d2d/recter.hpp"
#include "kuro/paint/d2d/texter.hpp"
#include "kuro/paint/stylus.hpp"
#include "kuro/paint/material/sys_color.hpp"
#include "kuro/paint/material/window.hpp"
#include "kuro/paint/material/menu.hpp"
#include "kuro/paint/material/scrollbar.hpp"
#include "kuro/paint/material/dialog.hpp"
#include "kuro/paint/material/button.hpp"
#include "kuro/paint/material/editbox.hpp"
#include "kuro/paint/material/combobox.hpp"
#include "kuro/paint/material/listbox.hpp"
#include "kuro/paint/material/tooltip.hpp"
#include "kuro/paint/material/trackbar.hpp"
#include "kuro/paint/material/spin.hpp"
#include "kuro/paint/material/tab.hpp"
#include "kuro/paint/material/toolbar.hpp"
#include "kuro/paint/material/header.hpp"
#include "kuro/paint/material/listview.hpp"
#include "kuro/paint/material/treeview.hpp"
#include "kuro/paint/material/command_module.hpp"
#include "kuro/paint/material/address_band.hpp"
#include "kuro/paint/manager.hpp"
#include "kuro/gdi/renderer.hpp"
#include "kuro/gdi/renderer_nc.hpp"
#include "kuro/gdi/renderer/aviutl2.hpp"
#include "kuro/gdi/renderer/menu.hpp"
#include "kuro/gdi/renderer/dialog.hpp"
#include "kuro/gdi/renderer/static.hpp"
#include "kuro/gdi/renderer/button.hpp"
#include "kuro/gdi/renderer/editbox.hpp"
#include "kuro/gdi/renderer/richedit.hpp"
#include "kuro/gdi/renderer/combobox.hpp"
#include "kuro/gdi/renderer/comboboxex.hpp"
#include "kuro/gdi/renderer/listbox.hpp"
#include "kuro/gdi/renderer/tooltip.hpp"
#include "kuro/gdi/renderer/trackbar.hpp"
#include "kuro/gdi/renderer/spin.hpp"
#include "kuro/gdi/renderer/tab.hpp"
#include "kuro/gdi/renderer/toolbar.hpp"
#include "kuro/gdi/renderer/header.hpp"
#include "kuro/gdi/renderer/listview.hpp"
#include "kuro/gdi/renderer/treeview.hpp"
#include "kuro/gdi/renderer/comdlg32/dialog.hpp"
#include "kuro/gdi/renderer/comdlg32/direct_ui_hwnd.hpp"
#include "kuro/gdi/renderer/comdlg32/dui_view.hpp"
#include "kuro/gdi/renderer/rigaya/dialog.hpp"
#include "kuro/gdi/manager.hpp"
#include "kuro/theme/name.hpp"
#include "kuro/theme/renderer.hpp"
#include "kuro/theme/renderer/base/menu.hpp"
#include "kuro/theme/renderer/menu.hpp"
#include "kuro/theme/renderer/scrollbar.hpp"
#include "kuro/theme/renderer/static.hpp"
#include "kuro/theme/renderer/button.hpp"
#include "kuro/theme/renderer/editbox.hpp"
#include "kuro/theme/renderer/combobox.hpp"
#include "kuro/theme/renderer/listbox.hpp"
#include "kuro/theme/renderer/tooltip.hpp"
#include "kuro/theme/renderer/trackbar.hpp"
#include "kuro/theme/renderer/spin.hpp"
#include "kuro/theme/renderer/tab.hpp"
#include "kuro/theme/renderer/toolbar.hpp"
#include "kuro/theme/renderer/header.hpp"
#include "kuro/theme/renderer/listview.hpp"
#include "kuro/theme/renderer/treeview.hpp"
#include "kuro/theme/renderer/rebar.hpp"
#include "kuro/theme/renderer/itemsview.hpp"
#include "kuro/theme/renderer/command_module.hpp"
#include "kuro/theme/renderer/common_items_dialog.hpp"
#include "kuro/theme/renderer/preview_pane.hpp"
#include "kuro/theme/renderer/explorer_nav_pane.hpp"
#include "kuro/theme/renderer/address_band.hpp"
#include "kuro/theme/renderer/search_band/search_box.hpp"
#include "kuro/theme/renderer/search_band/search_editbox.hpp"
#include "kuro/theme/renderer/immersive_start/menu.hpp"
#include "kuro/theme/from_handle.hpp"
#include "kuro/theme/from_vsclass.hpp"
#include "kuro/theme/manager.hpp"
#include "kuro/hook/entry.hpp"
#include "kuro/hook/manager.hpp"
#include "kuro/hook/entry/call_wnd_proc_ret.hpp"
#include "kuro/hook/entry/gdi.hpp"
#include "kuro/hook/entry/theme.hpp"
#include "kuro/hook/entry/darken_window.hpp"
#include "config_dialog.hpp"
#include "config_io.hpp"
#include "app.hpp"

namespace apn::dark
{
	//
	// エントリポイントです。
	//
	EXTERN_C BOOL APIENTRY DllMain(HMODULE instance, DWORD reason, LPVOID reserved)
	{
		switch (reason)
		{
		case DLL_PROCESS_ATTACH:
			{
				::DisableThreadLibraryCalls(hive.instance = instance);

				// このdllがアンロードされないようにします。
				::LoadLibrary(my::get_module_file_name(instance).c_str());

				break;
			}
		case DLL_PROCESS_DETACH:
			{
				break;
			}
		}

		return TRUE;
	}

	//
	// aviutl2のコンフィグを取得します。
	//
	EXTERN_C void InitializeConfig(CONFIG_HANDLE* config)
	{
		hive.aviutl2.config = config;
	}

	//
	// プラグインDLL初期化関数です。
	//
	EXTERN_C bool InitializePlugin(DWORD version)
	{
		if (version < 2003000) return false;

		app->dll_init();

		return true;
	}

	//
	// プラグインDLL終了関数です。
	//
	EXTERN_C void UninitializePlugin()
	{
		app->dll_exit();

		return;
	}

	//
	// 入力プラグインの構造体を返します。
	//
	EXTERN_C INPUT_PLUGIN_TABLE* GetInputPluginTable()
	{
		// 入力プラグインの構造体を自動的に構築します。
		static input_plugin_table_t input_plugin_table;

		return &input_plugin_table;
	}
}
