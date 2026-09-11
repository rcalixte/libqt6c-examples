#include <libqt6c.h>

#define MODEL_CAPACITY 4
static QStandardItem* row_data[MODEL_CAPACITY];

static KRearrangeColumnsProxyModel* proxy;
static libqt_list row = {
    .len = 0,
    .data.ptr = (void*)row_data,
};

static void timer_callback() {
    int headers[] = {2, 1, 0, 3};
    libqt_list columns = {
        .len = sizeof headers / sizeof headers[0],
        .data.ints = headers,
    };

    k_rearrangecolumnsproxymodel_set_source_columns(proxy, columns);
}

static libqt_list make_standard_items_list(const char* labels[]) {
    size_t i = 0;
    for (i = 0; labels[i] != NULL; i++)
        row_data[i] = q_standarditem_new2(labels[i]);
    row.len = i;
    return row;
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    const char* row_0[] = {"A0", "B0", "C0", "D0", NULL};
    const char* row_1[] = {"A1", "B1", "C1", "D1", NULL};
    const char* row_2[] = {"A2", "B2", "C2", "D2", NULL};
    const char* labels[] = {"H1", "H2", "H3", "H4", NULL};

    QStandardItemModel* source = q_standarditemmodel_new();
    q_standarditemmodel_append_row(source, make_standard_items_list(row_0));
    q_standarditemmodel_append_row(source, make_standard_items_list(row_1));
    q_standarditemmodel_append_row(source, make_standard_items_list(row_2));
    q_standarditemmodel_set_horizontal_header_labels(source, labels);

    proxy = k_rearrangecolumnsproxymodel_new();

    int headers[] = {2, 3, 1, 0};
    libqt_list columns = {
        .len = sizeof headers / sizeof headers[0],
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
    q_timer_start(timer, 3000);
    q_timer_on_timeout(timer, timer_callback);

    int result = q_application_exec();

    q_timer_delete(timer);
    q_treeview_delete(treeview);
    k_rearrangecolumnsproxymodel_delete(proxy);
    q_standarditemmodel_delete(source);
    q_application_delete(qapp);

    return result;
}
