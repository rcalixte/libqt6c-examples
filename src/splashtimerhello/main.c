#include <libqt6c.h>

#define BUFFER_SIZE 64
static size_t counter = 0;
static char buffer[BUFFER_SIZE];

void on_clicked(void* self) {
    counter++;
    snprintf(buffer, BUFFER_SIZE, "You have clicked the button %ld time(s)", counter);
    q_pushbutton_set_text(self, buffer);
}

void splash_mouse_press_event() {
}

void timer_callback(void* self) {
    QVariant* splash_qv = q_timer_property(self, "splash");
    uint64_t splash = q_variant_to_u_long_long(splash_qv);

    QVariant* widget_qv = q_timer_property(self, "widget");
    uint64_t widget = q_variant_to_u_long_long(widget_qv);

    q_splashscreen_close((QSplashScreen*)splash);
    q_widget_show((QWidget*)widget);
    q_timer_stop(self);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QPixmap* pixmap = q_pixmap_new4("assets/libqt6c-examples.png");

    QSplashScreen* splash = q_splashscreen_new4(pixmap, QT_WINDOWTYPE_WINDOWSTAYSONTOPHINT);
    q_splashscreen_on_mouse_press_event(splash, splash_mouse_press_event);

    QWidget* widget = q_widget_new2();
    q_widget_set_window_title(widget, "Hello world");

    QPushButton* button = q_pushbutton_new5("Hello world!", widget);
    q_pushbutton_set_fixed_width(button, 320);
    q_pushbutton_on_clicked(button, on_clicked);

    q_splashscreen_show(splash);

    QTimer* timer = q_timer_new();

    QVariant* splash_qv = q_variant_new7((intptr_t)splash);
    q_timer_set_property(timer, "splash", splash_qv);

    QVariant* widget_qv = q_variant_new7((intptr_t)widget);
    q_timer_set_property(timer, "widget", widget_qv);

    q_timer_start(timer, 3000);
    q_timer_on_timeout(timer, timer_callback);

    int result = q_application_exec();

    q_variant_delete(widget_qv);
    q_variant_delete(splash_qv);
    q_timer_delete(timer);
    q_widget_delete(widget);
    q_splashscreen_delete(splash);
    q_pixmap_delete(pixmap);
    q_application_delete(qapp);

    printf("OK!\n");

    return result;
}
