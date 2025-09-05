#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    KShortcutsDialog* dialog = k_shortcutsdialog_new2();
    k_shortcutsdialog_set_window_title(dialog, "Qt 6 KXmlGui Example");
    k_shortcutsdialog_set_minimum_size2(dialog, 400, 450);
    k_shortcutsdialog_set_attribute(dialog, QT_WIDGETATTRIBUTE_WA_DELETEONCLOSE);

    // Empty shortcut dialog
    k_shortcutsdialog_configure(dialog);

    q_application_delete(qapp);

    return 0;
}
