#include <libqt6c.h>

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);

    KPlotWidget* kplot = k_plotwidget_new2();

    k_plotwidget_set_window_title(kplot, "Qt 6 KPlotting Example");
    k_plotwidget_set_minimum_size2(kplot, 400, 400);

    QColor* color1 = q_color_new4(QT_GLOBALCOLOR_RED);
    KPlotObject* plotobject1 = k_plotobject_new3(color1, KPLOTOBJECT_PLOTTYPE_BARS);
    k_plotobject_add_point3(plotobject1, 0.1, 0.1);
    k_plotobject_add_point3(plotobject1, 0.3, 0.3);
    k_plotobject_add_point3(plotobject1, 0.5, 0.5);

    QColor* color2 = q_color_new4(QT_GLOBALCOLOR_BLUE);
    KPlotObject* plotobject2 = k_plotobject_new3(color2, KPLOTOBJECT_PLOTTYPE_BARS);
    k_plotobject_add_point3(plotobject2, 0.6, 0.8);
    k_plotobject_add_point3(plotobject2, 0.7, 0.7);
    k_plotobject_add_point3(plotobject2, 0.8, 0.6);

    k_plotwidget_add_plot_object(kplot, plotobject1);
    k_plotwidget_add_plot_object(kplot, plotobject2);

    k_plotwidget_show(kplot);

    int result = q_application_exec();

    q_color_delete(color1);
    q_color_delete(color2);
    k_plotwidget_delete(kplot);

    return result;
}
