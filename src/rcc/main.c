#include <libqt6c.h>
#include "rcc.h"

int main(int argc, char* argv[]) {
    // Initialize Qt application
    QApplication* qapp = q_application_new(&argc, argv);

    bool ok = qrc_rcc_init();
    if (!ok)
        printf("Resource initialization failed!\n");

    QWidget* widget = q_widget_new2();
    q_widget_set_minimum_size2(widget, 650, 150);

    QHBoxLayout* hbox = q_hboxlayout_new(widget);

    QRadioButton* radio1 = q_radiobutton_new(widget);
    q_radiobutton_set_tool_tip(radio1, "Qt");
    QIcon* icon1 = q_icon_new4(":/images/qt.png");
    q_radiobutton_set_icon(radio1, icon1);
    QSize* size1 = q_size_new4(50, 50);
    q_radiobutton_set_icon_size(radio1, size1);

    QRadioButton* radio2 = q_radiobutton_new(widget);
    q_radiobutton_set_tool_tip(radio2, "C");
    QIcon* icon2 = q_icon_new4(":/images/c.png");
    q_radiobutton_set_icon(radio2, icon2);
    QSize* size2 = q_size_new4(50, 50);
    q_radiobutton_set_icon_size(radio2, size2);

    QRadioButton* radio3 = q_radiobutton_new(widget);
    q_radiobutton_set_tool_tip(radio3, "libqt6c");
    QIcon* icon3 = q_icon_new4(":/images/libqt6c.png");
    q_radiobutton_set_icon(radio3, icon3);
    QSize* size3 = q_size_new4(120, 40);
    q_radiobutton_set_icon_size(radio3, size3);

    q_hboxlayout_add_stretch(hbox);
    q_hboxlayout_add_widget(hbox, radio1);
    q_hboxlayout_add_stretch(hbox);
    q_hboxlayout_add_widget(hbox, radio2);
    q_hboxlayout_add_stretch(hbox);
    q_hboxlayout_add_widget(hbox, radio3);
    q_hboxlayout_add_stretch(hbox);

    q_widget_show(widget);
    int result = q_application_exec();

    ok = qrc_rcc_delete();
    if (!ok)
        printf("Resource deinitialization failed!\n");

    q_size_delete(size1);
    q_icon_delete(icon1);
    q_size_delete(size2);
    q_icon_delete(icon2);
    q_size_delete(size3);
    q_icon_delete(icon3);
    q_widget_delete(widget);
    q_application_delete(qapp);

    return result;
}
