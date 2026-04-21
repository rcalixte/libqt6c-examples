#include <libqt6c.h>

static KStatusNotifierItem* status_notifier_item = NULL;
static QTextEdit* text_edit = NULL;
static const char* title = "Qt 6 KStatusNotifierItem Example";

QIcon* create_icon(int color) {
    QPixmap* pixmap = q_pixmap_new2(16, 16);
    QColor* fill_color = q_color_new4(color);

    q_pixmap_fill1(pixmap, fill_color);

    QIcon* icon = q_icon_new2(pixmap);

    q_color_delete(fill_color);
    q_pixmap_delete(pixmap);

    return icon;
}

void on_activate_requested(void* self UNUSED, bool active, void* pos) {
    const char* active_str = active ? "true" : "false";
    int x = q_point_x(pos);
    int y = q_point_x(pos);
    const char* fmt = "Activated: active = %s, pos = (%d, %d)";
    size_t len = snprintf(NULL, 0, fmt, active_str, x, y);

    char* text = malloc(len + 1);
    if (!text) {
        fprintf(stderr, "Failed to allocate memory for text\n");
        abort();
    }

    snprintf(text, len + 1, fmt, active_str, x, y);
    q_textedit_append(text_edit, text);
    libqt_free(text);
}

void on_secondary_activate_requested(void* self UNUSED, void* pos) {
    int x = q_point_x(pos);
    int y = q_point_y(pos);
    const char* fmt = "Secondary Activated: pos = (%d, %d)";
    size_t len = snprintf(NULL, 0, fmt, x, y);

    char* text = malloc(len + 1);
    if (!text) {
        fprintf(stderr, "Failed to allocate memory for text\n");
        abort();
    }

    snprintf(text, len + 1, fmt, x, y);
    q_textedit_append(text_edit, text);
    libqt_free(text);
}

void on_scroll_requested(void* self UNUSED, int delta, int32_t orientation) {
    const char* orientation_str = orientation == QT_ORIENTATION_HORIZONTAL ? "Horizontally" : "Vertically";
    const char* fmt = "Scrolled %s: delta = %d";
    size_t len = snprintf(NULL, 0, fmt, orientation_str, delta);

    char* text = malloc(len + 1);
    if (!text) {
        fprintf(stderr, "Failed to allocate memory for text\n");
        abort();
    }

    snprintf(text, len + 1, fmt, orientation_str, delta);
    q_textedit_append(text_edit, text);
    libqt_free(text);
}

void on_close_event(void* self UNUSED, void* event UNUSED) {
    q_application_quit();
}

void on_triggered(void* self) {
    const char* text = q_action_text(self);
    if (strcmp(text, "NeedsAttention") == 0) {
        QIcon* icon = create_icon(QT_GLOBALCOLOR_BLUE);
        k_statusnotifieritem_set_icon_by_pixmap(status_notifier_item, icon);
        k_statusnotifieritem_set_status(status_notifier_item, KSTATUSNOTIFIERITEM_ITEMSTATUS_NEEDSATTENTION);
        q_icon_delete(icon);
    } else if (strcmp(text, "Active") == 0) {
        QIcon* icon = create_icon(QT_GLOBALCOLOR_RED);
        k_statusnotifieritem_set_icon_by_pixmap(status_notifier_item, icon);
        k_statusnotifieritem_set_status(status_notifier_item, KSTATUSNOTIFIERITEM_ITEMSTATUS_ACTIVE);
        q_icon_delete(icon);
    } else if (strcmp(text, "Passive") == 0)
        k_statusnotifieritem_set_status(status_notifier_item, KSTATUSNOTIFIERITEM_ITEMSTATUS_PASSIVE);

    libqt_free(text);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    status_notifier_item = k_statusnotifieritem_new2("org.libqt6c.kstatusnotifieritemexample");

    QIcon* red_icon = create_icon(QT_GLOBALCOLOR_RED);
    k_statusnotifieritem_set_icon_by_pixmap(status_notifier_item, red_icon);
    q_icon_delete(red_icon);

    k_statusnotifieritem_set_category(status_notifier_item, KSTATUSNOTIFIERITEM_ITEMCATEGORY_COMMUNICATIONS);
    k_statusnotifieritem_set_status(status_notifier_item, KSTATUSNOTIFIERITEM_ITEMSTATUS_ACTIVE);
    k_statusnotifieritem_set_tool_tip_title(status_notifier_item, title);

    QIcon* green_icon = create_icon(QT_GLOBALCOLOR_GREEN);
    QMenu* menu = q_menu_new2();
    QAction* attention_action = q_menu_add_action3(menu, green_icon, "NeedsAttention");
    q_action_on_triggered(attention_action, on_triggered);
    q_icon_delete(green_icon);

    QIcon* magenta_icon = create_icon(QT_GLOBALCOLOR_MAGENTA);
    QAction* active_action = q_menu_add_action3(menu, magenta_icon, "Active");
    q_action_on_triggered(active_action, on_triggered);
    q_icon_delete(magenta_icon);

    QMenu* sub_menu = q_menu_new3("Sub Menu");
    QIcon* menu_icon = create_icon(QT_GLOBALCOLOR_DARKBLUE);
    q_menu_set_icon(sub_menu, menu_icon);
    q_icon_delete(menu_icon);

    QIcon* yellow_icon = create_icon(QT_GLOBALCOLOR_YELLOW);
    QAction* sub_action = q_menu_add_action3(sub_menu, yellow_icon, "Passive");
    q_action_on_triggered(sub_action, on_triggered);
    q_icon_delete(yellow_icon);

    q_menu_add_menu(menu, sub_menu);
    k_statusnotifieritem_set_context_menu(status_notifier_item, menu);
    k_statusnotifieritem_on_activate_requested(status_notifier_item, on_activate_requested);
    k_statusnotifieritem_on_secondary_activate_requested(status_notifier_item, on_secondary_activate_requested);
    k_statusnotifieritem_on_scroll_requested(status_notifier_item, on_scroll_requested);

    q_messagebox_information(NULL, title,
                             "Check your system tray for the status notifier item icon.\n\n"
                             "In order to quit the example, close the text edit window or quit via the system tray menu.");

    text_edit = q_textedit_new3("Logged activity:");
    q_textedit_set_read_only(text_edit, true);
    q_textedit_set_minimum_size2(text_edit, 400, 300);
    q_textedit_set_window_title(text_edit, title);
    q_textedit_on_close_event(text_edit, on_close_event);

    q_textedit_show(text_edit);

    int result = q_application_exec();

    q_textedit_delete(text_edit);
    k_statusnotifieritem_delete(status_notifier_item);
    q_application_delete(qapp);

    return result;
}
