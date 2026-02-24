#include <libqt6c.h>

static QLCDNumber* lcd;
static QTime* lcd_time;

void show_time() {
    lcd_time = q_time_current_time();
    char lcd_format[6] = "hh:mm";

    if (q_time_second(lcd_time) % 2 == 0) {
        lcd_format[2] = ' ';
    }

    const char* text = q_time_to_string2(lcd_time, lcd_format);
    q_lcdnumber_display(lcd, text);

    q_time_delete(lcd_time);
    libqt_free(text);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();

    q_widget_set_window_title(widget, "Qt 6 LCD Clock Example");
    q_widget_resize(widget, 360, 240);

    QHBoxLayout* hbox = q_hboxlayout_new(widget);
    lcd = q_lcdnumber_new(widget);

    q_lcdnumber_set_style_sheet(lcd, "background-color: #555555; color: white;");

    show_time();

    q_hboxlayout_add_widget(hbox, lcd);

    QTimer* timer = q_timer_new2(widget);
    q_timer_start(timer, 1000);
    q_timer_on_timeout(timer, show_time);

    q_widget_show(widget);

    int result = q_application_exec();

    q_widget_delete(widget);
    q_application_delete(qapp);

    return result;
}
