#include <libqt6c.h>
#include <stdio.h>

int on_row_count(void* self, void* index) {
    return 1000;
}

int on_column_count(void* self, void* index) {
    return 1;
}

QVariant* on_data(void* self, void* index, int role) {
    switch (role) {
    case QT_ITEMDATAROLE_FOREGROUNDROLE:
        if (q_modelindex_row(index) % 2 == 0) {
            QColor* color = q_color_new5(0, 0, 0);
            QVariant* ret = q_color_to_q_variant(color);
            q_color_delete(color);
            return ret;
        } else {
            QColor* color = q_color_new5(255, 0, 0);
            QVariant* ret = q_color_to_q_variant(color);
            q_color_delete(color);
            return ret;
        }
    case QT_ITEMDATAROLE_BACKGROUNDROLE:
        if (q_modelindex_row(index) % 2 == 0) {
            QColor* color = q_color_new5(255, 255, 255);
            QVariant* ret = q_color_to_q_variant(color);
            q_color_delete(color);
            return ret;
        } else {
            QColor* color = q_color_new5(80, 80, 80);
            QVariant* ret = q_color_to_q_variant(color);
            q_color_delete(color);
            return ret;
        }
    case QT_ITEMDATAROLE_DISPLAYROLE: {
        char buf[16];
        snprintf(buf, sizeof(buf), "this is row %d", q_modelindex_row(index));
        return q_variant_new11(buf);
    }
    default:
        return q_variant_new();
    }
}

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);

    QAbstractListModel* model = q_abstractlistmodel_new();

    q_abstractlistmodel_on_column_count(model, on_column_count);
    q_abstractlistmodel_on_row_count(model, on_row_count);
    q_abstractlistmodel_on_data(model, on_data);

    QListView* listview = q_listview_new2();
    q_listview_set_model(listview, model);

    q_listview_show(listview);

    int result = q_application_exec();

    q_listview_delete(listview);

    return result;
}
