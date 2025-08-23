#include <libqt6c.h>

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);

    QTermWidget* term = q_termwidget_new3();
    q_termwidget_set_window_title(term, "Qt 6 QTermWidget Example");
    q_termwidget_set_minimum_size2(term, 640, 480);
    q_termwidget_set_color_scheme(term, "WhiteOnBlack");
    q_termwidget_on_finished(term, q_application_quit);

    q_termwidget_show(term);

    int result = q_application_exec();

    q_termwidget_delete(term);
    return result;
}
