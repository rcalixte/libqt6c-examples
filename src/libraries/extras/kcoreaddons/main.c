#include <libqt6c.h>

static QTextEdit* plainTextEditor;
static QTextBrowser* htmlview;
static QTimer* timer;

static void on_timeout() {
    const char* plaintext = q_textedit_to_plain_text(plainTextEditor);
    int64_t options = 0;
    const char* html = k_texttohtml_convert_to_html(plaintext, &options, 4096, 255);
    q_textbrowser_set_html(htmlview, html);
    libqt_free(plaintext);
    libqt_free(html);
}

static void on_text_changed() {
    q_timer_start2(timer);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QMainWindow* window = q_mainwindow_new2();
    QWidget* widget = q_widget_new2();
    QHBoxLayout* layout = q_hboxlayout_new2();

    q_mainwindow_set_window_title(window, "Qt 6 KCoreAddons Example");
    q_mainwindow_set_central_widget(window, widget);
    q_widget_set_layout(widget, layout);

    plainTextEditor = q_textedit_new2();
    q_textedit_set_accept_rich_text(plainTextEditor, false);

    q_hboxlayout_add_widget(layout, plainTextEditor);

    htmlview = q_textbrowser_new2();
    q_hboxlayout_add_widget(layout, htmlview);

    timer = q_timer_new2(qapp);
    q_timer_set_single_shot(timer, true);
    q_timer_set_interval(timer, 1000);
    q_timer_on_timeout(timer, on_timeout);
    q_textedit_on_text_changed(plainTextEditor, on_text_changed);

    q_mainwindow_show(window);

    int ret = q_application_exec();

    q_mainwindow_delete(window);
    q_application_delete(qapp);
    return ret;
}
