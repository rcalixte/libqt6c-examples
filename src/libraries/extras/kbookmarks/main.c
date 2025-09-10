#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    KBookmarkManager* manager = k_bookmarkmanager_new("assets/example.xml");
    KBookmarkDialog* dialog = k_bookmarkdialog_new(manager);

    QUrl* url = q_url_new3("https://github.com/rcalixte/libqt6c-examples");
    k_bookmarkdialog_add_bookmark(dialog, "Qt 6 examples for C", url, "www");

    q_url_delete(url);
    k_bookmarkdialog_delete(dialog);
    k_bookmarkmanager_delete(manager);
    q_application_delete(qapp);
}
