#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    KCharsets* charsets = k_charsets_charsets();

    const char** names = k_charsets_available_encoding_names(charsets);

    QWidget* widget = q_widget_new2();

    q_widget_set_window_title(widget, "Qt 6 KCharsets");
    q_widget_set_minimum_size2(widget, 300, 400);

    // Ownership of these widgets will be transferred to the widget via the layout
    QVBoxLayout* vboxlayout = q_vboxlayout_new2();
    QLabel* label = q_label_new3("Available Encodings:");
    QListWidget* listwidget = q_listwidget_new2();

    q_listwidget_add_items(listwidget, names);

    q_vboxlayout_add_widget(vboxlayout, label);
    q_vboxlayout_add_widget(vboxlayout, listwidget);
    q_widget_set_layout(widget, vboxlayout);

    q_widget_show(widget);

    int result = q_application_exec();

    q_widget_delete(widget);

    for (size_t i = 0; names[i] != NULL; i++) {
        free((char*)names[i]);
    }
    free(names);

    q_application_delete(qapp);

    return result;
}
