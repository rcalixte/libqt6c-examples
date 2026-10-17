#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QMainWindow* window = q_mainwindow_new2();
    Q3DBars* bars = q_3dbars_new();
    QCategory3DAxis* column_axis = q_category3daxis_new();
    q_category3daxis_set_title(column_axis, "Columns");
    q_category3daxis_set_title_visible(column_axis, true);
    const char** col_labels = (const char**)malloc(2 * sizeof *col_labels);
    if (col_labels == NULL) {
        fprintf(stderr, "Failed to allocate memory for column labels\n");
        q_mainwindow_delete(window);
        q_application_delete(qapp);
        return 1;
    }
    col_labels[0] = "Column 1";
    col_labels[1] = "Column 2";
    q_category3daxis_set_labels(column_axis, col_labels);
    q_category3daxis_set_label_auto_rotation(column_axis, 30);
    free(col_labels);

    QCategory3DAxis* row_axis = q_category3daxis_new();
    q_category3daxis_set_title(row_axis, "Rows");
    q_category3daxis_set_title_visible(row_axis, true);
    const char** row_labels = (const char**)malloc(2 * sizeof *row_labels);
    if (row_labels == NULL) {
        fprintf(stderr, "Failed to allocate memory for row labels\n");
        q_mainwindow_delete(window);
        q_application_delete(qapp);
        return 1;
    }
    row_labels[0] = "Row 1";
    row_labels[1] = "Row 2";
    q_category3daxis_set_labels(row_axis, row_labels);
    q_category3daxis_set_label_auto_rotation(row_axis, 30);
    free(row_labels);

    QValue3DAxis* value_axis = q_value3daxis_new();
    q_value3daxis_set_title(value_axis, "Values");
    q_value3daxis_set_title_visible(value_axis, true);
    q_value3daxis_set_range(value_axis, 0, 5);

    q_3dbars_set_column_axis(bars, column_axis);
    q_3dbars_set_row_axis(bars, row_axis);
    q_3dbars_set_value_axis(bars, value_axis);

    QBar3DSeries* series = q_bar3dseries_new();
    QBarDataItem* row0_data[] = {
        q_bardataitem_new2(1),
        q_bardataitem_new2(2),
    };
    libqt_list row0 = qlist(row0_data, sizeof row0_data / sizeof row0_data[0]);
    QBarDataItem* row1_data[] = {
        q_bardataitem_new2(3),
        q_bardataitem_new2(4),
    };
    libqt_list row1 = qlist(row1_data, sizeof row1_data / sizeof row1_data[0]);
    q_bardataproxy_add_row(q_bar3dseries_data_proxy(series), row0);
    q_bardataproxy_add_row(q_bar3dseries_data_proxy(series), row1);
    q_3dbars_set_primary_series(bars, series);
    for (size_t i = 0; i < row0.len; i++)
        q_bardataitem_delete(row0_data[i]);
    for (size_t i = 0; i < row1.len; i++)
        q_bardataitem_delete(row1_data[i]);

    QWidget* container = q_mainwindow_create_window_container(bars);
    if (!q_3dbars_has_context(bars)) {
        fprintf(stderr, "Failed to initialize the OpenGL context\n");
        q_mainwindow_delete(window);
        q_application_delete(qapp);
        return 1;
    }

    q_3dcamera_set_y_rotation(q_3dscene_active_camera(q_3dbars_scene(bars)), 22.5);
    QRect* geometry = q_screen_geometry(q_application_primary_screen());
    int size = q_rect_height(geometry) * 3 / 4;
    q_widget_set_minimum_size2(container, size, size);
    q_widget_set_size_policy2(container, QSIZEPOLICY_POLICY_EXPANDING, QSIZEPOLICY_POLICY_EXPANDING);
    q_widget_set_focus_policy(container, QT_FOCUSPOLICY_STRONGFOCUS);
    q_rect_delete(geometry);

    q_mainwindow_set_central_widget(window, container);
    q_mainwindow_set_window_title(window, "Qt 6 Data Visualization Example");
    q_mainwindow_resize(window, 800, 600);

    q_mainwindow_show(window);

    int result = q_application_exec();

    q_mainwindow_delete(window);
    q_application_delete(qapp);

    return result;
}
