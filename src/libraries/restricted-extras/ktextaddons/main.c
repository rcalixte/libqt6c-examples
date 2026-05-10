#include <libqt6c.h>

static QTextEdit* textedit = NULL;
static TextTranslator__TranslatorWidget* translator = NULL;

void on_triggered(void* self UNUSED) {
    const char* text = q_textedit_to_plain_text(textedit);
    if (text == NULL || strlen(text) == 0) {
        libqt_free(text);
        return;
    }

    k_texttranslator__translatorwidget_set_text_to_translate(translator, text);
    k_texttranslator__translatorwidget_show(translator);
    libqt_free(text);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QMainWindow* window = q_mainwindow_new2();
    q_mainwindow_set_window_title(window, "Qt 6 KTextAddons Example");
    q_mainwindow_resize(window, 800, 600);

    QWidget* widget = q_widget_new(window);
    QVBoxLayout* layout = q_vboxlayout_new(widget);
    textedit = q_textedit_new(widget);
    q_textedit_set_placeholder_text(textedit, "Type or paste text here and use the toolbar button to translate");
    q_vboxlayout_add_widget2(layout, textedit, 1);

    translator = k_texttranslator__translatorwidget_new(widget);
    k_texttranslator__translatorwidget_hide(translator);
    q_vboxlayout_add_widget(layout, translator);

    QToolBar* toolbar = q_mainwindow_add_tool_bar3(window, "Tools");
    QAction* action = q_toolbar_add_action2(toolbar, "Translate");
    q_action_on_triggered(action, on_triggered);

    q_mainwindow_set_central_widget(window, widget);
    q_mainwindow_show(window);

    int result = q_application_exec();

    q_mainwindow_delete(window);
    q_application_delete(qapp);

    return result;
}
