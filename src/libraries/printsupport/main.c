#include <libqt6c.h>

static QPrintDialog* dialog = NULL;

void on_pressed(void* self UNUSED) {
    q_printdialog_show(dialog);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QPushButton* button = q_pushbutton_new3("QPrintSupport sample");
    dialog = q_printdialog_new(button);

    q_pushbutton_set_fixed_width(button, 320);
    q_pushbutton_on_pressed(button, on_pressed);
    q_pushbutton_show(button);

    int result = q_application_exec();

    q_pushbutton_delete(button);
    q_application_delete(qapp);

    return result;
}
