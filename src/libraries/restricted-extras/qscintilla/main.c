#include <libqt6c.h>

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);

    QsciScintilla* area = q_sciscintilla_new2();

    q_sciscintilla_set_fixed_size2(area, 640, 480);
    q_sciscintilla_show(area);

    int result = q_application_exec();

    q_sciscintilla_delete(area);
    return result;
}
