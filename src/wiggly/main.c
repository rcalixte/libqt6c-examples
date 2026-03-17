#include <libqt6c.h>

const char* wiggly_text = "Hello Wiggly Text";
#define MAX_LENGTH 32
static int sineValues[] = {
    0, 38, 71, 92,
    100, 92, 71, 38,
    0, -38, -71, -92,
    -100, -92, -71, -38};

static libqt_list sineTable = {
    .len = 16,
    .data.ints = sineValues,
};

typedef struct {
    QBasicTimer* timer;
    char text[MAX_LENGTH];
    size_t step;
    QWidget* widget;
} WigglyWidget;

static WigglyWidget* wiggly = NULL;

void on_paint_event(void* self, void* event UNUSED) {
    QFont* font = q_font_new();
    QFontMetrics* fontMetrics = q_fontmetrics_new(font);

    int32_t x = (q_widget_width(self) - q_fontmetrics_horizontal_advance(fontMetrics, wiggly->text)) / 4;
    int32_t y = (q_widget_height(self) + q_fontmetrics_ascent(fontMetrics) - q_fontmetrics_descent(fontMetrics)) / 2;

    QColor* color = q_color_new3();
    QStylePainter* painter = q_stylepainter_new(self);

    for (size_t i = 0; i < strlen(wiggly->text); i++) {
        int32_t index = (wiggly->step + i) % sineTable.len;
        q_color_set_hsv(color, (63 - index) * (sineTable.len / 4), 255, 191);
        q_stylepainter_set_pen(painter, color);
        char ch_str[2] = {wiggly->text[i], '\0'};
        q_stylepainter_draw_text3(painter,
                                  x,
                                  y - (sineTable.data.ints[index] * q_fontmetrics_height(fontMetrics) * 2) / 300,
                                  ch_str);
        x += q_fontmetrics_horizontal_advance(fontMetrics, ch_str) * 3;
    }

    q_stylepainter_delete(painter);
    q_color_delete(color);
    q_fontmetrics_delete(fontMetrics);
    q_font_delete(font);
}

void on_timer_event(void* self, void* event) {
    if (q_timerevent_timer_id(event) == q_basictimer_timer_id(wiggly->timer)) {
        wiggly->step += 1;
        q_widget_update(self);
    } else {
        q_widget_super_timer_event(self, event);
    }
}

WigglyWidget* initialize_wiggly_widget(const char* text) {
    WigglyWidget* wiggly = malloc(sizeof(WigglyWidget));
    wiggly->step = 0;
    snprintf(wiggly->text, MAX_LENGTH, "%s", text);

    wiggly->widget = q_widget_new2();
    q_widget_set_background_role(wiggly->widget, QPALETTE_COLORROLE_MIDLIGHT);
    q_widget_set_auto_fill_background(wiggly->widget, true);

    wiggly->timer = q_basictimer_new();
    q_basictimer_start3(wiggly->timer, 60, wiggly->widget);

    QFont* font = q_font_new();
    q_font_set_point_size(font, q_font_point_size(font) + 25);
    q_widget_set_font(wiggly->widget, font);
    q_font_delete(font);

    q_widget_on_paint_event(wiggly->widget, on_paint_event);
    q_widget_on_timer_event(wiggly->widget, on_timer_event);

    return wiggly;
}

void cleanup_wiggly_widget(WigglyWidget* wiggly) {
    q_basictimer_delete(wiggly->timer);
    q_widget_delete_later(wiggly->widget);
    free(wiggly);
    wiggly = NULL;
}

void on_text_changed(void* self UNUSED, const char* text) {
    snprintf(wiggly->text, MAX_LENGTH, "%s", text);
    q_widget_update(wiggly->widget);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QDialog* dialog = q_dialog_new2();
    q_dialog_set_window_title(dialog, "Qt 6 Wiggly Text Example");
    q_dialog_resize(dialog, 500, 180);

    wiggly = initialize_wiggly_widget(wiggly_text);
    QLineEdit* line_edit = q_lineedit_new2();
    q_lineedit_set_text(line_edit, wiggly_text);
    q_lineedit_set_max_length(line_edit, MAX_LENGTH - 1);
    q_lineedit_on_text_changed(line_edit, on_text_changed);

    QVBoxLayout* layout = q_vboxlayout_new(dialog);
    q_vboxlayout_add_widget(layout, wiggly->widget);
    q_vboxlayout_add_widget(layout, line_edit);

    q_dialog_show(dialog);

    int result = q_application_exec();

    cleanup_wiggly_widget(wiggly);
    q_dialog_delete(dialog);
    q_application_delete(qapp);

    return result;
}
