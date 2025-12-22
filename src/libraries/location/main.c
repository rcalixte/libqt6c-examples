#include <libqt6c.h>

static QListWidget* listwidget = NULL;

void add_place(QPlace* place) {
    const char* name = q_place_name(place);
    const char* placeid = q_place_place_id(place);
    char* text = malloc(strlen(name) + strlen(placeid) + 2);
    snprintf(text, strlen(name) + strlen(placeid) + 1, "%s\n%s", name, placeid);

    q_listwidget_add_item(listwidget, text);

    free(text);
    libqt_free(placeid);
    libqt_free(name);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    listwidget = q_listwidget_new2();
    q_listwidget_set_window_title(listwidget, "Qt 6 Location Example");
    q_listwidget_resize(listwidget, 400, 250);
    q_listwidget_set_spacing(listwidget, 5);

    QPlace* place1 = q_place_new();
    q_place_set_name(place1, "Eiffel Tower");
    q_place_set_place_id(place1, "Champ de Mars, Paris, France");

    QPlace* place2 = q_place_new();
    q_place_set_name(place2, "Space Needle");
    q_place_set_place_id(place2, "Seattle, Washington, USA");

    QPlace* place3 = q_place_new();
    q_place_set_name(place3, "Statue of Liberty");
    q_place_set_place_id(place3, "New York, USA");

    add_place(place1);
    add_place(place2);
    add_place(place3);

    q_listwidget_show(listwidget);

    int result = q_application_exec();

    q_place_delete(place3);
    q_place_delete(place2);
    q_place_delete(place1);
    q_listwidget_delete(listwidget);
    q_application_delete(qapp);

    return result;
}
