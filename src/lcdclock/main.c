#include <libqt6c.h>

static void* lcd;
static void* time;

void show_time(void* self) {
    time = q_time_current_time();
    const char* text = q_time_to_string_with_format(time, "hh:mm");
    char lcd_text[6];

    strncpy(lcd_text, text, 5);
    lcd_text[5] = '\0';

    if (q_time_second(time) % 2 == 0) {
        lcd_text[2] = ' ';
    }

    q_lcdnumber_display(lcd, lcd_text);

    q_time_delete(time);
    free(text);
}

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();

    q_widget_set_window_title(widget, "Qt 6 LCD Clock Example");
    q_widget_resize(widget, 360, 240);

    QHBoxLayout* hbox = q_hboxlayout_new(widget);
    lcd = q_lcdnumber_new(widget);

    q_lcdnumber_set_style_sheet(lcd, "background-color: #555555; color: white;");

    show_time(NULL);

    q_hboxlayout_add_widget(hbox, lcd);

    QTimer* timer = q_timer_new2(widget);
    q_timer_start(timer, 1000);
    q_timer_on_timeout(timer, show_time);

    q_widget_show(widget);

    int result = q_application_exec();

    q_widget_delete(widget);

    return result;
}
