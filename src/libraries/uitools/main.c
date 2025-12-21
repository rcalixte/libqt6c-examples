#include <libqt6c.h>

static const char* FORMFILE = "src/libraries/uitools/design.ui";

int main(int argc, char* argv[]) {
    q_application_set_attribute(QT_APPLICATIONATTRIBUTE_AA_SHAREOPENGLCONTEXTS);
    QApplication* qapp = q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();
    q_widget_set_window_title(widget, "Qt 6 UI Tools Example");

    QVBoxLayout* layout = q_vboxlayout_new(widget);

    QUiLoader* loader = q_uiloader_new();

    QFile* file = q_file_new2(FORMFILE);

    if (q_file_open(file, QIODEVICEBASE_OPENMODEFLAG_READONLY)) {
        QWidget* parent = q_widget_new2();
        QWidget* form = q_uiloader_load2(loader, file, parent);
        q_vboxlayout_add_widget(layout, form);
        q_widget_resize(widget, 1000, 550);

        q_file_close(file);
    } else {
        char buffer[256] = "### Failed to open form file: ";
        QLabel* label = q_label_new5(strncat(buffer, FORMFILE, strlen(FORMFILE)), widget);
        q_label_set_text_format(label, QT_TEXTFORMAT_MARKDOWNTEXT);
        q_label_set_alignment(label, QT_ALIGNMENTFLAG_ALIGNCENTER);
        q_vboxlayout_add_widget(layout, label);
        q_widget_resize(widget, 550, 100);
    }

    q_widget_show(widget);

    int result = q_application_exec();

    q_file_delete(file);
    q_uiloader_delete(loader);
    q_widget_delete(widget);
    q_application_delete(qapp);

    return result;
}
