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

int main(int argc, char* argv[]) {
    // Initialize Qt application
    q_application_new(&argc, argv);

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

    q_pushbutton_set_fixed_width(button, 320);
    q_pushbutton_on_clicked(button, button_callback);
    q_widget_show(widget);

    int result = q_application_exec();
    q_widget_delete(widget);

    printf("OK!\n");

    return result;
}
