#include <libqt6c.h>

static char buf[16];
static QVariant* variant_black = NULL;
static QVariant* variant_grey = NULL;
static QVariant* variant_red = NULL;
static QVariant* variant_white = NULL;
static QVariant* variant_ret = NULL;

int on_row_count(void* self UNUSED, void* index UNUSED) {
    return 1000;
}

int on_column_count(void* self UNUSED, void* index UNUSED) {
    return 1;
}

QVariant* on_data(void* self UNUSED, void* index, int role) {
    switch (role) {
    case QT_ITEMDATAROLE_FOREGROUNDROLE:
        if (q_modelindex_row(index) % 2 == 0)
            return variant_black;
        else
            return variant_red;

    case QT_ITEMDATAROLE_BACKGROUNDROLE:
        if (q_modelindex_row(index) % 2 == 0)
            return variant_white;
        else
            return variant_grey;

    case QT_ITEMDATAROLE_DISPLAYROLE: {
        snprintf(buf, sizeof(buf), "this is row %d", q_modelindex_row(index));
        QVariant* variant = q_variant_new24(buf);
        q_variant_set_value(variant_ret, variant);
        q_variant_delete(variant);
        return variant_ret;
    }
    default:
        q_variant_clear(variant_ret);
        return variant_ret;
    }
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QAbstractListModel* model = q_abstractlistmodel_new();

    QColor* color0 = q_color_new5(0, 0, 0);
    variant_black = q_color_to_q_variant(color0);
    q_color_delete(color0);

    QColor* color1 = q_color_new5(255, 0, 0);
    variant_red = q_color_to_q_variant(color1);
    q_color_delete(color1);

    QColor* color2 = q_color_new5(255, 255, 255);
    variant_white = q_color_to_q_variant(color2);
    q_color_delete(color2);

    QColor* color3 = q_color_new5(80, 80, 80);
    variant_grey = q_color_to_q_variant(color3);
    q_color_delete(color3);

    variant_ret = q_variant_new();

    q_abstractlistmodel_on_column_count(model, on_column_count);
    q_abstractlistmodel_on_row_count(model, on_row_count);
    q_abstractlistmodel_on_data(model, on_data);

    QListView* listview = q_listview_new2();
    q_listview_set_model(listview, model);

    q_listview_show(listview);

    int result = q_application_exec();

    q_listview_delete(listview);
    q_variant_delete(variant_ret);
    q_variant_delete(variant_grey);
    q_variant_delete(variant_white);
    q_variant_delete(variant_red);
    q_variant_delete(variant_black);
    q_abstractlistmodel_delete(model);
    q_application_delete(qapp);

    return result;
}
