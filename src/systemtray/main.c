#include <libqt6c.h>

static QComboBox* type_combo = NULL;
static QDialog* dialog = NULL;
static QSpinBox* duration_spinbox = NULL;
static QLineEdit* title_edit = NULL;
static QTextEdit* body_edit = NULL;
static QSystemTrayIcon* tray_icon = NULL;

static const char* WINDOW_TITLE = "Qt 6 System Tray Example";

void on_quit(void* self UNUSED) {
    q_application_quit();
}

void on_show_message(void* self UNUSED) {
    QVariant* type_variant = q_combobox_item_data(type_combo, q_combobox_current_index(type_combo));
    int selected_icon = q_variant_to_int(type_variant);
    const char* title_text = q_lineedit_text(title_edit);
    const char* msg_text = q_textedit_to_plain_text(body_edit);

    q_systemtrayicon_show_message42(
        tray_icon,
        title_text,
        msg_text,
        selected_icon,
        q_spinbox_value(duration_spinbox) * 1000);

    libqt_free(msg_text);
    libqt_free(title_text);
    q_variant_delete(type_variant);
}

void on_minimize(void* self UNUSED) {
    q_dialog_hide(dialog);
}

void on_maximize(void* self UNUSED) {
    q_dialog_show_maximized(dialog);
}

void on_restore(void* self UNUSED) {
    q_dialog_show_normal(dialog);
}

void on_message_clicked(void* self UNUSED) {
    q_messagebox_information(dialog, WINDOW_TITLE,
                             "Sorry, I already gave what help I could.\n"
                             "Maybe you should try asking a human?");
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    if (!q_systemtrayicon_is_system_tray_available()) {
        q_messagebox_critical(NULL, WINDOW_TITLE, "No system tray available.");
        return -1;
    }

    q_application_set_quit_on_last_window_closed(false);

    QGroupBox* message_box = q_groupbox_new3("Balloon Message");
    type_combo = q_combobox_new2();

    QVariant* no_variant = q_variant_new4(QSYSTEMTRAYICON_MESSAGEICON_NOICON);
    QVariant* info_variant = q_variant_new4(QSYSTEMTRAYICON_MESSAGEICON_INFORMATION);
    QVariant* warn_variant = q_variant_new4(QSYSTEMTRAYICON_MESSAGEICON_WARNING);
    QVariant* crit_variant = q_variant_new4(QSYSTEMTRAYICON_MESSAGEICON_CRITICAL);

    dialog = q_dialog_new2();
    q_dialog_set_window_title(dialog, WINDOW_TITLE);
    q_dialog_resize(dialog, 400, 300);
    QStyle* style = q_dialog_style(dialog);

    QIcon* info_icon = q_style_standard_icon(
        style,
        QSTYLE_STANDARDPIXMAP_SP_MESSAGEBOXINFORMATION,
        NULL,
        NULL);
    QIcon* warn_icon = q_style_standard_icon(
        style,
        QSTYLE_STANDARDPIXMAP_SP_MESSAGEBOXWARNING,
        NULL,
        NULL);
    QIcon* crit_icon = q_style_standard_icon(
        style,
        QSTYLE_STANDARDPIXMAP_SP_MESSAGEBOXCRITICAL,
        NULL,
        NULL);

    q_combobox_add_item22(type_combo, "None", no_variant);
    q_combobox_add_item3(type_combo, info_icon, "Information", info_variant);
    q_combobox_add_item3(type_combo, warn_icon, "Warning", warn_variant);
    q_combobox_add_item3(type_combo, crit_icon, "Critical", crit_variant);
    q_combobox_set_current_index(type_combo, 1);

    QLabel* duration_label = q_label_new3("Duration:");
    duration_spinbox = q_spinbox_new2();
    q_spinbox_set_range(duration_spinbox, 5, 60);
    q_spinbox_set_suffix(duration_spinbox, " s");
    q_spinbox_set_value(duration_spinbox, 15);

    QLabel* warning_label = q_label_new3("(some systems might ignore this hint)");
    q_label_set_indent(warning_label, 10);

    title_edit = q_lineedit_new3("Cannot connect to network");
    body_edit = q_textedit_new2();
    q_textedit_set_plain_text(body_edit,
                              "Don't believe me. Honestly, I don't have a clue.\n"
                              "Click this balloon for details.");

    QPushButton* quit_button = q_pushbutton_new3("&Quit");
    q_pushbutton_set_fixed_width(quit_button, 100);
    q_pushbutton_on_clicked(quit_button, on_quit);

    QPushButton* show_button = q_pushbutton_new3("Show Message");
    q_pushbutton_set_default(show_button, true);
    q_pushbutton_on_clicked(show_button, on_show_message);

    QGridLayout* message_layout = q_gridlayout_new2();
    q_gridlayout_add_widget2(message_layout, q_label_new3("Type:"), 0, 0);
    q_gridlayout_add_widget3(message_layout, type_combo, 0, 1, 1, 2);
    q_gridlayout_add_widget2(message_layout, duration_label, 1, 0);
    q_gridlayout_add_widget2(message_layout, duration_spinbox, 1, 1);
    q_gridlayout_add_widget3(message_layout, warning_label, 1, 2, 1, 3);
    q_gridlayout_add_widget2(message_layout, q_label_new3("Title"), 2, 0);
    q_gridlayout_add_widget3(message_layout, title_edit, 2, 1, 1, 4);
    q_gridlayout_add_widget2(message_layout, q_label_new3("Body:"), 3, 0);
    q_gridlayout_add_widget3(message_layout, body_edit, 3, 1, 2, 4);
    q_gridlayout_add_widget2(message_layout, show_button, 5, 4);
    q_gridlayout_set_column_stretch(message_layout, 3, 1);
    q_gridlayout_set_row_stretch(message_layout, 4, 1);
    q_groupbox_set_layout(message_box, message_layout);

    QAction* minimize_action = q_action_new5("Mi&nimize", dialog);
    q_action_on_triggered(minimize_action, on_minimize);

    QAction* maximize_action = q_action_new5("Ma&ximize", dialog);
    q_action_on_triggered(maximize_action, on_maximize);

    QAction* restore_action = q_action_new5("&Restore", dialog);
    q_action_on_triggered(restore_action, on_restore);

    QAction* quit_action = q_action_new5("&Quit", dialog);
    q_action_on_triggered(quit_action, on_quit);

    QMenu* tray_menu = q_menu_new(dialog);
    q_menu_add_action(tray_menu, minimize_action);
    q_menu_add_action(tray_menu, maximize_action);
    q_menu_add_action(tray_menu, restore_action);
    q_menu_add_separator(tray_menu);
    q_menu_add_action(tray_menu, quit_action);

    tray_icon = q_systemtrayicon_new3(dialog);
    q_systemtrayicon_set_context_menu(tray_icon, tray_menu);
    q_systemtrayicon_on_message_clicked(tray_icon, on_message_clicked);

    QVBoxLayout* layout = q_vboxlayout_new2();
    q_vboxlayout_add_widget(layout, message_box);
    q_vboxlayout_add_widget(layout, quit_button);
    q_dialog_set_layout(dialog, layout);
    q_dialog_set_window_icon(dialog, info_icon);
    q_systemtrayicon_set_icon(tray_icon, info_icon);

    q_systemtrayicon_show(tray_icon);
    q_dialog_show(dialog);

    int result = q_application_exec();

    q_icon_delete(info_icon);
    q_icon_delete(warn_icon);
    q_icon_delete(crit_icon);
    q_variant_delete(crit_variant);
    q_variant_delete(warn_variant);
    q_variant_delete(info_variant);
    q_variant_delete(no_variant);
    q_application_delete(qapp);

    return result;
}
