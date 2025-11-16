#include <libqt6c.h>

static QLabel* label;
#define BUFFER_SIZE 64
static char buffer[BUFFER_SIZE];

void mouse_press_event(void* UNUSED self, void* event) {
    int button = q_mouseevent_button(event);
    switch (button) {
    case QT_MOUSEBUTTON_LEFTBUTTON:
        snprintf(buffer, BUFFER_SIZE, "## Left mouse button pressed!");
        q_label_set_text(label, buffer);
        break;
    case QT_MOUSEBUTTON_RIGHTBUTTON:
        snprintf(buffer, BUFFER_SIZE, "## Right mouse button pressed!");
        q_label_set_text(label, buffer);
        break;
    default:
        snprintf(buffer, BUFFER_SIZE, "## Mouse button keycode: %d", button);
        q_label_set_text(label, buffer);
    }
}

void key_press_event(void* UNUSED self, void* event) {
    int key = q_keyevent_key(event);
    snprintf(buffer, BUFFER_SIZE, "## You pressed key code: %d", key);
    q_label_set_text(label, buffer);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();

    q_widget_set_fixed_width(widget, 400);
    q_widget_set_fixed_height(widget, 100);
    q_widget_on_mouse_press_event(widget, mouse_press_event);
    q_widget_on_key_press_event(widget, key_press_event);

    label = q_label_new3("### Press any key or click the mouse here!");

    q_label_set_focus_policy(label, QT_FOCUSPOLICY_STRONGFOCUS);
    q_label_set_text_format(label, QT_TEXTFORMAT_MARKDOWNTEXT);
    q_label_set_alignment(label, QT_ALIGNMENTFLAG_ALIGNCENTER);
    q_label_on_mouse_press_event(label, mouse_press_event);
    q_label_on_key_press_event(label, key_press_event);

    QVBoxLayout* layout = q_vboxlayout_new2();

    q_vboxlayout_add_stretch(layout);
    q_vboxlayout_add_widget(layout, label);
    q_vboxlayout_add_stretch(layout);
    q_widget_set_layout(widget, layout);

    q_widget_show(widget);

    int result = q_application_exec();

    q_widget_delete(widget);
    q_application_delete(qapp);

    return result;
}
