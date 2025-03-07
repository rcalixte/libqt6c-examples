#include <libqt6c.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);

    QSvgWidget* svg = q_svgwidget_new3("assets/libqt6c-examples.svg");
    if (!svg) {
        fprintf(stderr, "Failed to create SVG widget\n");
        return 1;
    }

    q_svgwidget_show(svg);

    int result = q_application_exec();

    q_svgwidget_delete(svg);
    return result;
}
