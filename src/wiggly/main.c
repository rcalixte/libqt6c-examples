#include <libqt6c.h>

const char* wiggly_text = "Hello Wiggly Text";
#define MAX_LENGTH 32
static int sine_values[] = {
    0, 38, 71, 92,
    100, 92, 71, 38,
    0, -38, -71, -92,
    -100, -92, -71, -38};

static libqt_list sine_table = {
    .len = 16,
    .data.ints = sine_values,
};

typedef struct {
    QBasicTimer* timer;
    char text[MAX_LENGTH];
    size_t step;
    QWidget* widget;
    QFontMetrics* font_metrics;
    QColor* color;
} WigglyWidget;

static WigglyWidget wiggly;

void on_paint_event(void* self, void* event UNUSED) {
    int32_t x = (q_widget_width(self) - q_fontmetrics_horizontal_advance(wiggly.font_metrics, wiggly.text)) / 4;
    int32_t y = (q_widget_height(self) + q_fontmetrics_ascent(wiggly.font_metrics) - q_fontmetrics_descent(wiggly.font_metrics)) / 2;

    QPainter* painter = q_painter_new2(q_widget_as_q_paint_device(self));

    for (size_t i = 0; i < strlen(wiggly.text); i++) {
        int32_t index = (wiggly.step + i) % sine_table.len;
        q_color_set_hsv(wiggly.color, (63 - index) * (sine_table.len / 4), 255, 191);
        q_painter_set_pen(painter, wiggly.color);
        char ch_str[2] = {wiggly.text[i], '\0'};
        q_painter_draw_text3(painter,
                             x,
                             y - (sine_table.data.ints[index] * q_fontmetrics_height(wiggly.font_metrics) * 2) / 300,
                             ch_str);
        x += q_fontmetrics_horizontal_advance(wiggly.font_metrics, ch_str) * 3;
    }

    q_painter_delete(painter);
}

void on_timer_event(void* self, void* event) {
    if (q_timerevent_timer_id(event) == q_basictimer_timer_id(wiggly.timer)) {
        wiggly.step += 1;
        q_widget_update(self);
    } else {
        q_widget_super_timer_event(self, event);
    }
}

void initialize_wiggly_widget(const char* text) {
    wiggly.step = 0;
    snprintf(wiggly.text, MAX_LENGTH, "%s", text);

    wiggly.widget = q_widget_new2();
    q_widget_set_background_role(wiggly.widget, QPALETTE_COLORROLE_MIDLIGHT);
    q_widget_set_auto_fill_background(wiggly.widget, true);

    wiggly.timer = q_basictimer_new();
    q_basictimer_start3(wiggly.timer, 60, wiggly.widget);

    QFont* font = q_font_new();
    q_font_set_point_size(font, q_font_point_size(font) + 25);
    q_widget_set_font(wiggly.widget, font);
    q_font_delete(font);

    q_widget_on_paint_event(wiggly.widget, on_paint_event);
    q_widget_on_timer_event(wiggly.widget, on_timer_event);

    QFont* wiggly_font = q_font_new();
    wiggly.font_metrics = q_fontmetrics_new(wiggly_font);
    wiggly.color = q_color_new3();
    q_font_delete(wiggly_font);
}

void cleanup_wiggly_widget() {
    q_basictimer_delete(wiggly.timer);
    q_color_delete(wiggly.color);
    q_fontmetrics_delete(wiggly.font_metrics);
    q_widget_delete_later(wiggly.widget);
}

void on_text_changed(void* self UNUSED, const char* text) {
    snprintf(wiggly.text, MAX_LENGTH, "%s", text);
    q_widget_update(wiggly.widget);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QDialog* dialog = q_dialog_new2();
    q_dialog_set_window_title(dialog, "Qt 6 Wiggly Text Example");
    q_dialog_resize(dialog, 500, 180);

    initialize_wiggly_widget(wiggly_text);
    QLineEdit* line_edit = q_lineedit_new2();
    q_lineedit_set_text(line_edit, wiggly_text);
    q_lineedit_set_max_length(line_edit, MAX_LENGTH - 1);
    q_lineedit_on_text_changed(line_edit, on_text_changed);

    QVBoxLayout* layout = q_vboxlayout_new(dialog);
    q_vboxlayout_add_widget(layout, wiggly.widget);
    q_vboxlayout_add_widget(layout, line_edit);

    q_dialog_show(dialog);

    int result = q_application_exec();

    cleanup_wiggly_widget();
    q_dialog_delete(dialog);
    q_application_delete(qapp);

    return result;
}
