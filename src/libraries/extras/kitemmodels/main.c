#include <libqt6c.h>

static KRearrangeColumnsProxyModel* proxy;

static void timer_callback() {
    int headers[] = {2, 1, 0, 3};
    libqt_list columns = {
        .len = 4,
        .data.ints = headers,
    };

    k_rearrangecolumnsproxymodel_set_source_columns(proxy, columns);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    void* row_0_items[] = {
        q_standarditem_new2("A0"),
        q_standarditem_new2("B0"),
        q_standarditem_new2("C0"),
        q_standarditem_new2("D0"),
        NULL,
    };
    libqt_list row_0 = qlist(row_0_items, 5);

    void* row_1_items[] = {
        q_standarditem_new2("A1"),
        q_standarditem_new2("B1"),
        q_standarditem_new2("C1"),
        q_standarditem_new2("D1"),
        NULL,
    };
    libqt_list row_1 = qlist(row_1_items, 5);

    void* row_2_items[] = {
        q_standarditem_new2("A2"),
        q_standarditem_new2("B2"),
        q_standarditem_new2("C2"),
        q_standarditem_new2("D2"),
        NULL,
    };
    libqt_list row_2 = qlist(row_2_items, 5);

    const char* labels[] = {"H1", "H2", "H3", "H4", NULL};

    QStandardItemModel* source = q_standarditemmodel_new();
    q_standarditemmodel_insert_row(source, 0, row_0);
    q_standarditemmodel_insert_row(source, 1, row_1);
    q_standarditemmodel_insert_row(source, 2, row_2);
    q_standarditemmodel_set_horizontal_header_labels(source, labels);

    proxy = k_rearrangecolumnsproxymodel_new();

    int headers[] = {2, 3, 1, 0};
    libqt_list columns = {
        .len = 4,
        .data.ints = headers,
    };

    k_rearrangecolumnsproxymodel_set_source_columns(proxy, columns);
    k_rearrangecolumnsproxymodel_set_source_model(proxy, source);

    QTreeView* treeview = q_treeview_new2();
    q_treeview_set_model(treeview, proxy);
    q_treeview_set_window_title(treeview, "Qt 6 KItemModels Example");
    q_treeview_set_minimum_size2(treeview, 410, 100);

    q_treeview_show(treeview);

    QTimer* timer = q_timer_new();
    q_timer_start(timer, 1000);
    q_timer_on_timeout(timer, timer_callback);

    int result = q_application_exec();

    q_timer_delete(timer);
    q_treeview_delete(treeview);
    k_rearrangecolumnsproxymodel_delete(proxy);
    q_standarditemmodel_delete(source);
    q_application_delete(qapp);

    return result;
}
