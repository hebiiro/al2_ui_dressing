#pragma once

namespace apn::dark
{
	//
	// このクラスはコンフィグダイアログです。
	//
	inline struct config_dialog_t : my::Dialog, my::lockable_t
	{
		virtual void on_update_controls() {}
		virtual void on_update_config() {}
		virtual void on_init_dialog() {}
		virtual void on_command(UINT code, UINT id, HWND control) {}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			{
				// 初期化中にエディットボックスがコマンドを発行してしまうので、
				// それを防ぐためにロックしておきます。
				my::locker_t locker(this);

				if (!__super::create(hive.instance, MAKEINTRESOURCE(idd_config), nullptr))
				{
					hive.message_box(L"コンフィグダイアログの作成に失敗しました");

					return FALSE;
				}
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return __super::destroy();
		}

		//
		// コンフィグでコントロールを更新します。
		//
		BOOL to_ui()
		{
			MY_TRACE_FUNC("");

			if (is_locked()) return FALSE;

			my::locker_t locker(this);

			set_text(idc_dark_style_file_name, hive.dark.style_file_name);
			set_check(idc_d2d_flag_figure, hive.d2d.flag_figure);
			set_check(idc_d2d_flag_text, hive.d2d.flag_text);
			set_check(idc_dark_flag_exclude_comdlg32, hive.dark.flag_exclude_comdlg32);

			set_check(idc_round_flag_use, hive.round.flag_use);
			set_int(idc_round_size, hive.round.size);

			set_check(idc_border_flag_3d_edge, hive.border.flag_3d_edge);
			set_int(idc_border_width, hive.border.width);

			set_check(idc_gradient_flag_use, hive.gradient.flag_use);
			set_check(idc_gradient_flag_end_alpha, hive.gradient.flag_end_alpha);
			set_int(idc_gradient_end_alpha, hive.gradient.end_alpha);

			set_check(idc_shadow_flag_use, hive.shadow.flag_use);
			set_check(idc_shadow_flag_blur, hive.shadow.flag_blur);
			set_int(idc_shadow_offset_x, hive.shadow.offset.x);
			set_int(idc_shadow_offset_y, hive.shadow.offset.y);
			set_int(idc_shadow_size, hive.shadow.size);

			set_check(idc_scrollbar_flag_arrow_as_button, hive.scrollbar.flag_arrow_as_button);
			set_check(idc_scrollbar_flag_has_gripper, hive.scrollbar.flag_has_gripper);
			set_int(idc_scrollbar_reduction, hive.scrollbar.reduction);

			return TRUE;
		}

		//
		// コントロールでコンフィグを更新します。
		//
		BOOL from_ui(BOOL redraw)
		{
			MY_TRACE_FUNC("{/}", redraw);

			if (is_locked()) return FALSE;

//			get_text(idc_dark_style_file_name, hive.dark.style_file_name);
			get_check(idc_d2d_flag_figure, hive.d2d.flag_figure);
			get_check(idc_d2d_flag_text, hive.d2d.flag_text);
			get_check(idc_dark_flag_exclude_comdlg32, hive.dark.flag_exclude_comdlg32);

			get_check(idc_round_flag_use, hive.round.flag_use);
			get_int(idc_round_size, hive.round.size);

			get_check(idc_border_flag_3d_edge, hive.border.flag_3d_edge);
			get_int(idc_border_width, hive.border.width);

			get_check(idc_gradient_flag_use, hive.gradient.flag_use);
			get_check(idc_gradient_flag_end_alpha, hive.gradient.flag_end_alpha);
			get_int(idc_gradient_end_alpha, hive.gradient.end_alpha);

			get_check(idc_shadow_flag_use, hive.shadow.flag_use);
			get_check(idc_shadow_flag_blur, hive.shadow.flag_blur);
			get_int(idc_shadow_offset_x, hive.shadow.offset.x);
			get_int(idc_shadow_offset_y, hive.shadow.offset.y);
			get_int(idc_shadow_size, hive.shadow.size);

			get_check(idc_scrollbar_flag_arrow_as_button, hive.scrollbar.flag_arrow_as_button);
			get_check(idc_scrollbar_flag_has_gripper, hive.scrollbar.flag_has_gripper);
			get_int(idc_scrollbar_reduction, hive.scrollbar.reduction);

			if (redraw) app->redraw();

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_COMMAND:
				{
					// ロックされている場合はWM_COMMANDを処理しません。
					if (is_locked()) break;

					auto control_id = LOWORD(w_param);
					auto code = HIWORD(w_param);
					auto control = (HWND)l_param;

					switch (control_id)
					{
					case idc_dark_style_file_name_ref:
						{
							// ユーザーが指定したスタイルファイルのパスを取得します。
							auto file_name = get_open_file_name(
								hwnd,
								L"スタイルファイルを選択",
								L"スタイルファイル (*.conf)\0*.conf\0"
								L"すべてのファイル (*.*)\0*.*\0",
								hive.dark.style_file_name.c_str());
							if (file_name.empty()) break;

							app->set_style_file_name(file_name);

							break;
						}
					case idc_dark_flag_exclude_comdlg32:
						{
							from_ui(FALSE);

							break;
						}
					case idc_d2d_flag_figure:
					case idc_d2d_flag_text:
					case idc_round_flag_use:
					case idc_border_flag_3d_edge:
					case idc_gradient_flag_use:
					case idc_gradient_flag_end_alpha:
					case idc_shadow_flag_use:
					case idc_shadow_flag_blur:
					case idc_scrollbar_flag_arrow_as_button:
					case idc_scrollbar_flag_has_gripper:
						{
							from_ui(TRUE);

							break;
						}
					case idc_round_size:
					case idc_border_width:
					case idc_gradient_end_alpha:
					case idc_shadow_offset_x:
					case idc_shadow_offset_y:
					case idc_shadow_size:
					case idc_scrollbar_reduction:
						{
							if (code == EN_UPDATE)
								from_ui(TRUE);

							break;
						}
					}

					break;
				}
			case WM_NOTIFY:
				{
					auto header = (NMHDR*)l_param;
					if (header->code == UDN_DELTAPOS)
					{
						auto nm = (NMUPDOWN*)header;
						auto edit_id = (UINT)header->idFrom - 1;

						switch (edit_id)
						{
						case idc_border_width:
						case idc_shadow_size:
							{
								auto value = get_int(edit_id);
								value += (nm->iDelta > 0) ? -1 : +1;
								value = std::clamp(value, 0, +100);
								set_int(edit_id, value);
								break;
							}
						case idc_round_size:
						case idc_gradient_end_alpha:
						case idc_scrollbar_reduction:
							{
								auto value = get_int(edit_id);
								value += (nm->iDelta > 0) ? -10 : +10;
								value = std::clamp(value, 0, +100);
								set_int(edit_id, value);
								break;
							}
						case idc_shadow_offset_x:
						case idc_shadow_offset_y:
							{
								auto value = get_int(edit_id);
								value += (nm->iDelta > 0) ? -10 : +10;
								value = std::clamp(value, -100, +100);
								set_int(edit_id, value);
								break;
							}
						}
					}

					return FALSE;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	} config_dialog;
}
