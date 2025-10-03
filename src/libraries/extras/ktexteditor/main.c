#include <libqt6c.h>

static KTextEditor__Editor* editor;

void toolbar_triggered(void* self, void* UNUSED action) {
    k_texteditor__editor_config_dialog(editor, self);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QMainWindow* window = q_mainwindow_new2();
    q_mainwindow_set_window_title(window, "Qt 6 KTextEditor Example");
    q_mainwindow_set_minimum_size2(window, 1100, 900);

    editor = k_texteditor__editor_instance();
    KTextEditor__Document* doc = k_texteditor__editor_create_document(editor, window);

    const char* dir = q_dir_current_path();
    char file[512];
    snprintf(file, sizeof(file), "file://%s/src/libraries/extras/ktexteditor/main.c", dir);
    libqt_free(dir);

    QUrl* url = q_url_new3(file);
    k_texteditor__document_open_url(doc, url);
    q_url_delete(url);

    k_texteditor__document_set_modified_on_disk_warning(doc, true);

    KTextEditor__View* view = k_texteditor__document_create_view(doc, window, window);

    QToolBar* toolbar = q_toolbar_new3();
    q_toolbar_add_action2(toolbar, "Configure");
    q_toolbar_on_action_triggered(toolbar, toolbar_triggered);

    q_mainwindow_add_tool_bar2(window, toolbar);
    q_mainwindow_set_central_widget(window, view);
    q_mainwindow_show(window);

    int result = q_application_exec();

    q_mainwindow_delete(window);
    q_application_delete(qapp);

    return result;
}
