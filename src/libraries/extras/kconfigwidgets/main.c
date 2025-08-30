#include <libqt6c.h>

struct KColorSchemeManager* manager;

void on_clicked(void* UNUSED self, void* index) {
    k_colorschememanager_activate_scheme(manager, index);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QMainWindow* window = q_mainwindow_new2();
    q_mainwindow_set_window_title(window, "Qt 6 KConfigWidgets");
    q_mainwindow_set_minimum_size2(window, 400, 450);
    manager = k_colorschememanager_instance();
    QListView* listview = q_listview_new(window);
    QAbstractItemModel* manager_model = k_colorschememanager_model(manager);
    q_listview_set_model(listview, manager_model);
    q_listview_on_clicked(listview, on_clicked);

    QDialogButtonBox* box = q_dialogbuttonbox_new7(QDIALOGBUTTONBOX_STANDARDBUTTON_CLOSE, window);
    q_dialogbuttonbox_on_rejected(box, q_application_quit);

    QWidget* widget = q_widget_new2();
    QVBoxLayout* layout = q_vboxlayout_new(widget);
    q_vboxlayout_add_widget(layout, listview);
    q_vboxlayout_add_widget(layout, box);

    q_mainwindow_set_central_widget(window, widget);

    QMenu* menu = q_menu_new4("Menu", window);
    KActionMenu* manager_menu = k_colorschememenu_create_menu(manager, window);
    q_menu_add_action(menu, manager_menu);
    q_menubar_add_menu(q_mainwindow_menu_bar(window), menu);

    q_mainwindow_show(window);

    int ret = q_application_exec();

    q_mainwindow_delete(window);
    q_application_delete(qapp);

    return ret;
}
