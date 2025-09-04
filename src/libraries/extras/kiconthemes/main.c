#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    KIconButton* button = k_iconbutton_new2();
    k_iconbutton_set_window_title(button, "Qt 6 KIconThemes Example");
    k_iconbutton_set_text(button, "Click to open the chooser dialog");
    k_iconbutton_set_minimum_size2(button, 320, 70);
    k_iconbutton_set_icon_size(button, 64);

    k_iconbutton_show(button);

    int result = q_application_exec();

    k_iconbutton_delete(button);
    q_application_delete(qapp);

    return result;
}
