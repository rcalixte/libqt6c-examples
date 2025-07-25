#include <libqt6c.h>
#include <stdio.h>

void onFinished(void* self, int UNUSED result) {
    q_printdialog_delete_later(self);
}

void onPressed() {
    QPrintDialog* dialog = q_printdialog_new3();
    if (!dialog) {
        fprintf(stderr, "Failed to create print dialog\n");
        return;
    }

    q_printdialog_on_finished(dialog, onFinished);
    q_printdialog_show(dialog);
}

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);

    QPushButton* button = q_pushbutton_new3("QPrintSupport sample");
    if (!button) {
        fprintf(stderr, "Failed to create button\n");
        return 1;
    }

    q_pushbutton_set_fixed_width(button, 320);
    q_pushbutton_on_pressed(button, onPressed);
    q_pushbutton_show(button);

    int result = q_application_exec();

    q_pushbutton_delete(button);
    return result;
}
