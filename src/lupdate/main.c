#include <libqt6c.h>

static QLabel* label;
static QMainWindow* window;
static QPushButton* up_button;
static QPushButton* down_button;
static QPushButton* left_button;
static QPushButton* right_button;
static QAction* exit_action;
static QMenu* file_menu;

void on_triggered(void* UNUSED self) {
    q_mainwindow_close(window);
}

void retranslate() {
    const char* label_text = q_application_translate("Main", "L&anguage:");
    q_label_set_text(label, label_text);
    libqt_free(label_text);

    const char* up_text = q_application_translate("Main", "&Up");
    q_pushbutton_set_text(up_button, up_text);
    libqt_free(up_text);

    const char* down_text = q_application_translate("Main", "&Down");
    q_pushbutton_set_text(down_button, down_text);
    libqt_free(down_text);

    const char* left_text = q_application_translate("Main", "&Left");
    q_pushbutton_set_text(left_button, left_text);
    libqt_free(left_text);

    const char* right_text = q_application_translate("Main", "&Right");
    q_pushbutton_set_text(right_button, right_text);
    libqt_free(right_text);

    const char* exit_text = q_application_translate("Main", "E&xit");
    q_action_set_text(exit_action, exit_text);
    libqt_free(exit_text);

    const char* file_text = q_application_translate("Main", "&File");
    q_menu_set_title(file_menu, file_text);
    libqt_free(file_text);

    const char* quit_bind = q_application_translate3("Main", "Ctrl+Q", "Quit");
    QKeySequence* exit_key = q_keysequence_new2(quit_bind);
    q_action_set_shortcut(exit_action, exit_key);

    q_keysequence_delete(exit_key);
    libqt_free(quit_bind);
}

void on_current_text_changed(void* UNUSED self, const char* text) {
    QLocale* locale = q_locale_new2(text);
    QTranslator* translator = q_translator_new();
    if (q_translator_load42(translator, locale, "lupdate", "_", "src/lupdate")) {
        q_application_install_translator(translator);
        retranslate();
    }

    q_locale_delete(locale);
    q_translator_delete(translator);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QComboBox* combo = q_combobox_new2();
    const char* texts[] = {"en", "es", "fr", NULL};

    q_combobox_add_items(combo, texts);
    q_combobox_on_current_text_changed(combo, on_current_text_changed);

    label = q_label_new3("L&anguage:");
    q_label_set_buddy(label, combo);

    window = q_mainwindow_new2();
    q_mainwindow_set_window_title(window, "Qt 6 Translation Example");
    q_mainwindow_set_minimum_size2(window, 460, 270);

    QWidget* widget = q_widget_new2();

    up_button = q_pushbutton_new3("&Up");
    down_button = q_pushbutton_new3("&Down");
    left_button = q_pushbutton_new3("&Left");
    right_button = q_pushbutton_new3("&Right");

    QGridLayout* grid_layout = q_gridlayout_new2();

    q_gridlayout_add_widget2(grid_layout, up_button, 0, 1);
    q_gridlayout_add_widget2(grid_layout, down_button, 2, 1);
    q_gridlayout_add_widget2(grid_layout, left_button, 1, 0);
    q_gridlayout_add_widget2(grid_layout, right_button, 1, 2);

    QVBoxLayout* vbox_layout = q_vboxlayout_new2();

    q_vboxlayout_add_stretch(vbox_layout);
    q_vboxlayout_add_widget(vbox_layout, label);
    q_vboxlayout_add_widget(vbox_layout, combo);

    q_gridlayout_add_layout(grid_layout, vbox_layout, 3, 0);

    q_widget_set_layout(widget, grid_layout);
    q_mainwindow_set_central_widget(window, widget);

    exit_action = q_action_new5("E&xit", window);

    QKeySequence* exit_key = q_keysequence_new2("Ctrl+Q");
    q_action_set_shortcut(exit_action, exit_key);
    q_action_on_triggered(exit_action, on_triggered);

    file_menu = q_menubar_add_menu2(q_mainwindow_menu_bar(window), "&File");
    q_menu_add_action(file_menu, exit_action);

    q_mainwindow_show(window);

    int result = q_application_exec();

    q_keysequence_delete(exit_key);
    q_mainwindow_delete(window);
    q_application_delete(qapp);

    return result;
}
