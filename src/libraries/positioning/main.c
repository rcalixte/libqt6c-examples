#include <libqt6c.h>

static QGeoCoordinate* coord;
static QLabel* label;

static char buffer[64];

void on_value_changed(void* self, double value) {
    const char* name = q_doublespinbox_object_name(self);

    if (strcmp(name, "lat") == 0) {
        q_geocoordinate_set_latitude(coord, value);
    } else if (strcmp(name, "lon") == 0) {
        q_geocoordinate_set_longitude(coord, value);
    }

    const char* geotext = q_geocoordinate_to_string1(coord,
                                                     QGEOCOORDINATE_COORDINATEFORMAT_DEGREESWITHHEMISPHERE);

    snprintf(buffer, sizeof(buffer), "### %s", geotext);

    q_label_set_text(label, buffer);

    libqt_free(geotext);
    libqt_free(name);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QMainWindow* window = q_mainwindow_new2();
    q_mainwindow_set_window_title(window, "Qt 6 Positioning Example");
    q_mainwindow_resize(window, 300, 120);

    QWidget* widget = q_widget_new2();

    QDoubleSpinBox* lat = q_doublespinbox_new2();
    q_doublespinbox_set_object_name(lat, "lat");
    q_doublespinbox_set_range(lat, -90.0, 90.0);
    q_doublespinbox_set_decimals(lat, 5);
    q_doublespinbox_set_value(lat, 0.0);
    q_doublespinbox_on_value_changed(lat, on_value_changed);

    QDoubleSpinBox* lon = q_doublespinbox_new2();
    q_doublespinbox_set_object_name(lon, "lon");
    q_doublespinbox_set_range(lon, -180.0, 180.0);
    q_doublespinbox_set_decimals(lon, 5);
    q_doublespinbox_set_value(lon, 0.0);
    q_doublespinbox_on_value_changed(lon, on_value_changed);

    coord = q_geocoordinate_new2(q_doublespinbox_value(lat), q_doublespinbox_value(lon));

    const char* geotext = q_geocoordinate_to_string1(coord,
                                                     QGEOCOORDINATE_COORDINATEFORMAT_DEGREESWITHHEMISPHERE);

    snprintf(buffer, sizeof(buffer), "### %s", geotext);

    label = q_label_new3(buffer);
    q_label_set_text_format(label, QT_TEXTFORMAT_MARKDOWNTEXT);

    QFormLayout* layout = q_formlayout_new2();
    q_formlayout_set_form_alignment(layout, QT_ALIGNMENTFLAG_ALIGNHCENTER);
    q_formlayout_set_spacing(layout, 10);
    q_formlayout_add_row3(layout, "Latitude:", lat);
    q_formlayout_add_row3(layout, "Longitude:", lon);
    q_formlayout_add_widget(layout, label);

    q_widget_set_layout(widget, layout);
    q_mainwindow_set_central_widget(window, widget);

    q_mainwindow_show(window);

    int result = q_application_exec();

    libqt_free(geotext);
    q_geocoordinate_delete(coord);
    q_mainwindow_delete(window);
    q_application_delete(qapp);

    return result;
}
