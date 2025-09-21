#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();
    q_widget_set_window_title(widget, "Qt 6 KNewStuff Example");
    q_widget_set_minimum_size2(widget, 300, 100);

    KNSWidgets__Button* button = k_nswidgets__button_new(widget);
    k_nswidgets__button_set_text(button, "Click me!");
    k_nswidgets__button_set_minimum_width(button, 100);

    QVBoxLayout* layout = q_vboxlayout_new2();
    q_vboxlayout_add_widget(layout, button);
    q_widget_set_layout(widget, layout);

    q_widget_show(widget);

    int result = q_application_exec();

    q_widget_delete(widget);
    q_application_delete(qapp);

    return result;
}
