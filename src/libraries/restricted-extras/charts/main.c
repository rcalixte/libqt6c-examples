#include <libqt6c.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);

    QLineSeries* series = q_lineseries_new();
    if (!series) {
        fprintf(stderr, "Failed to create line series\n");
        return 1;
    }

    q_lineseries_set_name(series, "Sine Wave");

    double x = 0;
    double y = 0;

    for (float i = -500; i <= 500; i++) {
        x = i / 10000;
        y = sin(1 / x) * x;
        if (isnan(y)) y = 0;
        q_lineseries_append(series, x, y);
    }

    QChart* chart = q_chart_new();
    q_chart_add_series(chart, series);
    q_chart_create_default_axes(chart);

    QChartView* chart_view = q_chartview_new3(chart);
    q_chartview_set_window_title(chart_view, "Qt 6 Charts Example");
    q_chartview_resize(chart_view, 650, 400);
    q_chartview_set_render_hint(chart_view, QPAINTER_RENDERHINT_ANTIALIASING);
    q_chartview_show(chart_view);

    int result = q_application_exec();

    q_lineseries_delete(series);
    return result;
}
