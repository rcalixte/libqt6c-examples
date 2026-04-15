#include <libqt6c.h>
#include "design.h"

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    MainWindowUi* uic = new_main_window_ui();

    q_mainwindow_show(uic->MainWindow);

    int result = q_application_exec();

    cleanup_main_window_ui(uic);
    q_application_delete(qapp);

    return result;
}
