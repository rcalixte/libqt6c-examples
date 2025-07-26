#include <libqt6c.h>
#include <stdint.h>
#include <stdio.h>

#define BUFFER_SIZE 64

static long counter = 0;

void button_callback(void* self) {
    counter++;
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "You have clicked the button %ld time(s)", counter);
    q_pushbutton_set_text(self, buffer);
}

void splash_mouse_press_event() {
}

void timer_callback(void* self, void* event) {
    void* splash_qv = q_timer_property(self, "splash");
    uint64_t splash = q_variant_to_u_long_long(splash_qv);

    void* widget_qv = q_timer_property(self, "widget");
    uint64_t widget = q_variant_to_u_long_long(widget_qv);

    q_splashscreen_close((QSplashScreen*)splash);
    q_widget_show((QWidget*)widget);
    q_timer_stop(self);
}

int main(int argc, char* argv[]) {
    // Initialize Qt application
    q_application_new(&argc, argv);

    QPixmap* pixmap = q_pixmap_new4("assets/libqt6c-examples.png");
    if (!pixmap) {
        fprintf(stderr, "Failed to load pixmap\n");
        return 1;
    }

    QSplashScreen* splash = q_splashscreen_new4(pixmap, QT_WINDOWTYPE_WINDOWSTAYSONTOPHINT);
    if (!splash) {
        fprintf(stderr, "Failed to create splash screen\n");
        return 1;
    }

    q_splashscreen_on_mouse_press_event(splash, splash_mouse_press_event);

    QWidget* widget = q_widget_new2();
    if (!widget) {
        // we can use assert or check for null to simulate exception handling
        // assert(widget != NULL);
        fprintf(stderr, "Failed to create widget\n");
        return 1;
    }

    // We don't need to free the button, it's a child of the widget
    QPushButton* button = q_pushbutton_new5("Hello world!", widget);
    if (!button) {
        fprintf(stderr, "Failed to create button\n");
        return 1;
    }

    q_widget_set_window_title(widget, "Hello world");

    q_pushbutton_set_fixed_width(button, 320);
    q_pushbutton_on_clicked(button, button_callback);

    q_splashscreen_show(splash);

    QTimer* timer = q_timer_new();
    if (!timer) {
        fprintf(stderr, "Failed to create timer\n");
        return 1;
    }

    QVariant* splash_qv = q_variant_new7((intptr_t)splash);
    q_timer_set_property(timer, "splash", splash_qv);

    QVariant* widget_qv = q_variant_new7((intptr_t)widget);
    q_timer_set_property(timer, "widget", widget_qv);

    q_timer_start(timer, 3000);
    q_timer_on_timer_event(timer, timer_callback);

    int result = q_application_exec();

    q_variant_delete(widget_qv);
    q_variant_delete(splash_qv);
    q_timer_delete(timer);
    q_widget_delete(widget);
    q_splashscreen_delete(splash);
    q_pixmap_delete(pixmap);

    printf("OK!\n");

    return result;
}
