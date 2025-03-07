#include <libqt6c.h>
#include <stdio.h>

#define BUFFER_SIZE 64

void mouse_press_event(void* self, void* event) {
    int button = q_mouseevent_button(event);
    char buffer[BUFFER_SIZE];
    switch (q_mouseevent_button(event)) {
    case QT_MOUSEBUTTON_LEFTBUTTON:
        snprintf(buffer, BUFFER_SIZE, "Left mouse button pressed!");
        q_label_set_text(self, buffer);
        break;
    case QT_MOUSEBUTTON_RIGHTBUTTON:
        snprintf(buffer, BUFFER_SIZE, "Right mouse button pressed!");
        q_label_set_text(self, buffer);
        break;
    default:
        snprintf(buffer, BUFFER_SIZE, "Mouse button keycode: %d", button);
        q_label_set_text(self, buffer);
    }
}

void key_press_event(void* self, void* event) {
    int key = q_keyevent_key(event);
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "You pressed key code: %d", key);
    q_label_set_text(self, buffer);
}

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();
    if (!widget) {
        fprintf(stderr, "Failed to create widget\n");
        return 1;
    }

    q_widget_set_fixed_width(widget, 400);
    q_widget_set_fixed_height(widget, 100);

    QLabel* label = q_label_new5("Press any key or click the mouse here!", widget);
    if (!label) {
        fprintf(stderr, "Failed to create label\n");
        q_widget_delete(widget);
        return 1;
    }

    q_label_set_focus_policy(label, QT_FOCUSPOLICY_STRONGFOCUS);
    q_label_on_key_press_event(label, key_press_event);
    q_label_on_mouse_press_event(label, mouse_press_event);

    q_widget_show(widget);

    int result = q_application_exec();

    q_widget_delete(widget);
    return result;
}
