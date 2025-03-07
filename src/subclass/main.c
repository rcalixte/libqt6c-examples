#include <libqt6c.h>
#include <stdbool.h>
#include <stdio.h>

#define BUFFER_SIZE 32

static size_t currentColor = 0;

static const Qt__GlobalColor useColors[] = {
    QT_GLOBALCOLOR_BLACK,
    QT_GLOBALCOLOR_RED,
    QT_GLOBALCOLOR_GREEN,
    QT_GLOBALCOLOR_BLUE,
};

#define COLOR_COUNT (sizeof(useColors) / sizeof(useColors[0]))

void onPaintEvent(void* self, void* ev) {
    // Call base class paint event
    q_groupbox_qbase_paint_event(self, ev);

    // Draw on top
    QPainter* painter = q_painter_new2(self);
    QBrush* brush = q_brush_new12(useColors[currentColor], QT_BRUSHSTYLE_SOLIDPATTERN);

    q_painter_set_brush(painter, brush);
    q_painter_draw_rect2(painter, 80, 60, 160, 120);

    q_brush_delete(brush);
    q_painter_delete(painter);
}

void onContextMenuEvent(void* self, void* ev) {
    q_groupbox_qbase_context_menu_event(self, ev);

    currentColor++;
    if (currentColor >= COLOR_COUNT) {
        currentColor = 0;
    }
    q_groupbox_update(self);
}

void onKeyPressEvent(void* self, void* ev) {
    q_groupbox_qbase_key_press_event(self, ev);

    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "Keypress %d", q_keyevent_key(ev));
    q_groupbox_set_title(self, buffer);
}

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);
    q_application_set_application_display_name("Right-click to change the color");

    QGroupBox* groupbox = q_groupbox_new2();
    if (!groupbox) {
        fprintf(stderr, "Failed to create groupbox\n");
        return 1;
    }

    q_groupbox_set_title(groupbox, "QGroupBox title");
    q_groupbox_set_fixed_width(groupbox, 320);
    q_groupbox_set_fixed_height(groupbox, 240);
    q_groupbox_set_minimum_height(groupbox, 100);
    q_groupbox_set_minimum_width(groupbox, 100);
    q_groupbox_set_attribute2(groupbox, QT_WIDGETATTRIBUTE_WA_PAINTONSCREEN, true);
    q_groupbox_set_attribute2(groupbox, QT_WIDGETATTRIBUTE_WA_NOSYSTEMBACKGROUND, true);

    q_groupbox_on_paint_event(groupbox, onPaintEvent);
    q_groupbox_on_context_menu_event(groupbox, onContextMenuEvent);
    q_groupbox_on_key_press_event(groupbox, onKeyPressEvent);

    q_groupbox_show(groupbox);

    int result = q_application_exec();

    q_groupbox_delete(groupbox);
    return result;
}
