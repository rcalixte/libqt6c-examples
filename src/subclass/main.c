#include <libqt6c.h>

#define BUFFER_SIZE 32
static char buffer[BUFFER_SIZE];
static size_t current_color = 0;

static const Qt__GlobalColor colors_list[] = {
    QT_GLOBALCOLOR_BLACK,
    QT_GLOBALCOLOR_RED,
    QT_GLOBALCOLOR_GREEN,
    QT_GLOBALCOLOR_BLUE,
};

#define COLOR_COUNT (sizeof(colors_list) / sizeof(colors_list[0]))

void on_paint_event(void* self, void* ev) {
    // Call the base class's PaintEvent to get initial content
    // (Comment this out to see the QGroupBox disappear)
    q_groupbox_super_paint_event(self, ev);

    // Then, draw on top of it
    QPainter* painter = q_painter_new2(q_groupbox_as_q_paint_device(self));
    QBrush* brush = q_brush_new12(colors_list[current_color], QT_BRUSHSTYLE_SOLIDPATTERN);

    q_painter_set_brush(painter, brush);
    q_painter_draw_rect2(painter, 80, 60, 160, 120);

    q_brush_delete(brush);
    q_painter_delete(painter);
}

void on_context_menu_event(void* self, void* ev) {
    q_groupbox_super_context_menu_event(self, ev);

    current_color++;
    if (current_color >= COLOR_COUNT)
        current_color = 0;
    q_groupbox_update(self);
}

void on_key_press_event(void* self, void* ev) {
    q_groupbox_super_key_press_event(self, ev);

    snprintf(buffer, BUFFER_SIZE, "Keypress %d", q_keyevent_key(ev));
    q_groupbox_set_title(self, buffer);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);
    q_application_set_application_display_name("Right-click to change the color");

    QGroupBox* groupbox = q_groupbox_new2();

    q_groupbox_set_title(groupbox, "QGroupBox title");
    q_groupbox_set_fixed_size2(groupbox, 320, 240);
    q_groupbox_set_minimum_size2(groupbox, 100, 100);
    q_groupbox_on_paint_event(groupbox, on_paint_event);
    q_groupbox_on_context_menu_event(groupbox, on_context_menu_event);
    q_groupbox_on_key_press_event(groupbox, on_key_press_event);

    q_groupbox_show(groupbox);

    int result = q_application_exec();

    q_groupbox_delete(groupbox);
    q_application_delete(qapp);

    return result;
}
