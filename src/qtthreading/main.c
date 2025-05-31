#include <libqt6c.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

typedef struct Counter {
    int32_t counter;
    void* object; // QLabel
    bool running;
    pthread_t thread;
} Counter;

typedef struct ButtonData {
    Counter** counters;
    size_t counter_count;
    void* button; // QPushButton
} ButtonData;

void update_label_text(void* ctx) {
    Counter* counter = (Counter*)ctx;
    char text[64];
    snprintf(text, sizeof(text), "%d %ld", counter->counter, time(NULL));
    q_label_set_text(counter->object, text);
}

void* run_counter(void* arg) {
    Counter* counter = (Counter*)arg;
    while (counter->running) {
        char text[64];
        snprintf(text, sizeof(text), "%d %ld", counter->counter, time(NULL));
        q_threading_async(counter, update_label_text);

        counter->counter++;
        usleep(1 * 1000);
    }
    return NULL;
}

void button_clicked(void* button, bool checked) {
    void* variant = q_pushbutton_property(button, "buttonData");
    if (!variant)
        return;

    uint64_t ptr_val = q_variant_to_u_long_long(variant);
    ButtonData* data = (ButtonData*)ptr_val;

    bool is_running = false;
    for (size_t i = 0; i < data->counter_count; i++) {
        if (data->counters[i]->running) {
            is_running = true;
            break;
        }
    }

    if (is_running) {
        // Stop all counters
        for (size_t i = 0; i < data->counter_count; i++) {
            Counter* counter = data->counters[i];
            counter->running = false;
            pthread_join(counter->thread, NULL);
        }
        q_pushbutton_set_text(data->button, "Start!");
    } else {
        // Start all counters
        for (size_t i = 0; i < data->counter_count; i++) {
            Counter* counter = data->counters[i];
            counter->running = true;
            pthread_create(&counter->thread, NULL, run_counter, counter);
        }
        q_pushbutton_set_text(data->button, "Stop!");
    }
}

int main(int argc, char* argv[]) {
    // Initialize Qt application
    q_application_new(&argc, argv);

    // Get CPU count for thread count
    long thread_count = sysconf(_SC_NPROCESSORS_ONLN);
    if (thread_count < 2)
        thread_count = 2;

    // Create main window
    void* window = q_mainwindow_new2();
    if (!window) {
        fprintf(stderr, "Failed to create main window\n");
        return 1;
    }

    q_mainwindow_set_fixed_size2(window, 250, 50 * thread_count + 1);
    q_mainwindow_set_window_title(window, "Qt 6 Threading Example");

    // Create central widget and layout
    void* widget = q_widget_new(window);
    void* vlayout = q_vboxlayout_new2();
    q_widget_set_layout(widget, vlayout);
    q_mainwindow_set_central_widget(window, widget);

    // Create counters array
    Counter** counters = calloc(thread_count, sizeof(Counter*));
    if (!counters) {
        fprintf(stderr, "Failed to allocate counters array\n");
        return 1;
    }

    // Initialize counters
    for (int i = 0; i < thread_count; i++) {
        counters[i] = calloc(1, sizeof(Counter));
        if (!counters[i]) {
            fprintf(stderr, "Failed to allocate counter %d\n", i);
            // Cleanup previously allocated counters
            for (int j = 0; j < i; j++) {
                free(counters[j]);
            }
            free(counters);
            return 1;
        }

        void* label = q_label_new(window);
        if (!label) {
            fprintf(stderr, "Failed to create label %d\n", i);
            // Cleanup
            for (int j = 0; j <= i; j++) {
                free(counters[j]);
            }
            free(counters);
            return 1;
        }

        q_label_set_alignment(label, QT_ALIGNMENTFLAG_ALIGNCENTER);
        q_label_set_text(label, "0 0");

        void* wlayout = q_widget_layout(widget);
        q_layout_add_widget(wlayout, label);

        counters[i]->object = label;
        counters[i]->counter = 0;
        counters[i]->running = false;
    }

    // Create start button
    void* button = q_pushbutton_new5("Start!", window);
    if (!button) {
        fprintf(stderr, "Failed to create button\n");
        // Cleanup
        for (int i = 0; i < thread_count; i++) {
            free(counters[i]);
        }
        free(counters);
        return 1;
    }

    // Setup button data
    ButtonData* button_data = calloc(1, sizeof(ButtonData));
    if (!button_data) {
        fprintf(stderr, "Failed to allocate button data\n");
        // Cleanup
        for (int i = 0; i < thread_count; i++) {
            free(counters[i]);
        }
        free(counters);
        return 1;
    }

    button_data->counters = counters;
    button_data->counter_count = thread_count;
    button_data->button = button;

    // Store button data in Qt property system
    void* variant = q_variant_new7((intptr_t)button_data);
    q_pushbutton_set_property(button, "buttonData", variant);

    // Connect button click handler
    q_pushbutton_on_clicked1(button, button_clicked);

    // Add button to layout
    void* qwlayout = q_widget_layout(widget);
    q_layout_add_widget(qwlayout, button);

    // Show window and start event loop
    q_mainwindow_show(window);
    int result = q_application_exec();

    // Cleanup
    // First stop all running threads
    for (int i = 0; i < thread_count; i++) {
        if (counters[i]->running) {
            counters[i]->running = false;
            int ret = pthread_join(counters[i]->thread, NULL);
            if (ret != 0) {
                fprintf(stderr, "Failed to join thread %d: %s\n", i, strerror(ret));
            }
        }
        free(counters[i]);
    }
    free(counters);
    free(button_data);
    q_variant_delete(variant);

    return result;
}
