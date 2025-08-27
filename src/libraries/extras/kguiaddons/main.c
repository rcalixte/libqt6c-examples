#include <libqt6c.h>

static QLabel* label;

static void on_text_changed(void* self, const char* text) {
    int pos = q_lineedit_cursor_position(self);
    int ret = k_datevalidator_validate((void*)q_lineedit_validator(self), text, &pos);

    switch (ret) {
    case QVALIDATOR_STATE_ACCEPTABLE:
        q_label_set_text(label, "Validation result: Acceptable");
        break;
    case QVALIDATOR_STATE_INTERMEDIATE:
        q_label_set_text(label, "Validation result: Intermediate");
        break;
    case QVALIDATOR_STATE_INVALID:
        q_label_set_text(label, "Validation result: Invalid");
        break;
    }
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();
    q_widget_set_window_title(widget, "Qt 6 KGuiAddons Example");
    q_widget_set_minimum_size2(widget, 380, 180);

    QLabel* titlelabel = q_label_new3("Enter a date:");

    QLineEdit* input = q_lineedit_new2();
    q_lineedit_on_text_changed(input, on_text_changed);
    label = q_label_new2();
    q_label_set_alignment(label, QT_ALIGNMENTFLAG_ALIGNCENTER);
    q_label_set_style_sheet(label, "font: bold;");

    KDateValidator* validator = k_datevalidator_new();
    q_lineedit_set_validator(input, validator);

    QVBoxLayout* layout = q_vboxlayout_new(widget);
    q_vboxlayout_add_widget(layout, titlelabel);
    q_vboxlayout_add_widget(layout, input);
    q_vboxlayout_add_widget(layout, label);

    q_widget_show(widget);

    int ret = q_application_exec();

    q_widget_delete(widget);
    q_application_delete(qapp);

    return ret;
}
