#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    KFileCustomDialog* dialog = k_filecustomdialog_new2();
    k_filecustomdialog_set_window_title(dialog, "Qt 6 KIO Example");

    QLabel* label = q_label_new3("Select a file or directory");

    k_filecustomdialog_set_custom_widget(dialog, label);

    int result = k_filecustomdialog_exec(dialog);

    k_filecustomdialog_delete(dialog);
    q_application_delete(qapp);

    return result;
}
