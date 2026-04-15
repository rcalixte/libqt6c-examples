#include <libqt6c.h>
// Either the single global header or individual library headers can be used
#include <libqapplication.h>
#include <libqwidget.h>
#include <libqpushbutton.h>

#define BUFFER_SIZE 64
static size_t counter = 0;
static char buffer[BUFFER_SIZE];

void on_clicked(void* self) {
    counter++;
    snprintf(buffer, BUFFER_SIZE, "You have clicked the button %ld time(s)", counter);
    q_pushbutton_set_text(self, buffer);
}

int main(int argc, char* argv[]) {
    // Initialize the Qt application
    QApplication* qapp = q_application_new(&argc, argv);

    // Create a new widget
    QWidget* widget = q_widget_new2();

    // We don't need to free/delete the button, it's a child of the widget
    QPushButton* button = q_pushbutton_new5("Hello world!", widget);
    q_pushbutton_set_fixed_width(button, 320);
    // Connect the button to the callback function
    q_pushbutton_on_clicked(button, on_clicked);

    // Display the widget
    q_widget_show(widget);

    // Start the event loop
    int result = q_application_exec();

    // Cleanup the widget and the application objects
    q_widget_delete(widget);
    q_application_delete(qapp);

    printf("OK!\n");

    return result;
}
