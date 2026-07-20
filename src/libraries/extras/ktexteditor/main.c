#include <libqt6c.h>

static KTextEditor__Editor* editor;
static const char* SRC_FILE = "src/libraries/extras/ktexteditor/main.c";

void toolbar_triggered(void* self, void* action UNUSED) {
    k_texteditor__editor_config_dialog(editor, self);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QMainWindow* window = q_mainwindow_new2();
    q_mainwindow_set_window_title(window, "Qt 6 KTextEditor Example");
    q_mainwindow_set_minimum_size2(window, 1100, 880);

    editor = k_texteditor__editor_instance();
    KTextEditor__Document* doc = k_texteditor__editor_create_document(editor, window);

    QUrl* url = q_url_from_local_file(SRC_FILE);
    int result = 0;

    if (k_texteditor__document_open_url(doc, url)) {
        k_texteditor__document_set_modified_on_disk_warning(doc, true);

        KTextEditor__View* view = k_texteditor__document_create_view(doc, window, NULL);
        QToolBar* toolbar = q_toolbar_new3();
        q_toolbar_add_action2(toolbar, "Configure");
        q_toolbar_on_action_triggered(toolbar, toolbar_triggered);
        q_mainwindow_add_tool_bar2(window, toolbar);
        q_mainwindow_set_central_widget(window, view);

        q_mainwindow_show(window);

        result = q_application_exec();
    }

    k_texteditor__document_delete(doc);
    q_url_delete(url);
    q_mainwindow_delete(window);
    q_application_delete(qapp);

    return result;
}
