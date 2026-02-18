#include <libqt6c.h>

typedef struct {
    int32_t color;
    bool on;
    QWidget* widget;
} LightWidget;

typedef struct {
    LightWidget* red;
    LightWidget* yellow;
    LightWidget* green;
    QWidget* widget;
} TrafficWidget;

void on_paint_event(void* self, void* UNUSED event) {
    QVariant* on_variant = q_widget_property(self, "on");
    uint64_t on_value = q_variant_to_long_long(on_variant);
    bool* on = (bool*)on_value;
    if (!*on)
        return;

    QVariant* color_variant = q_widget_property(self, "color");
    int32_t color = q_variant_to_int(color_variant);

    QStylePainter* painter = q_stylepainter_new(self);
    QBrush* brush = q_brush_new4(color);

    q_stylepainter_set_render_hint(painter, QPAINTER_RENDERHINT_ANTIALIASING);
    q_stylepainter_set_brush(painter, brush);

    int32_t height = q_widget_height(self);
    int32_t width = q_widget_width(self);
    int32_t min = height < width ? height : width;
    int32_t size = (min * 2) / 3;
    int32_t x = (width - size) / 2;
    int32_t y = (height - size) / 2;
    q_stylepainter_draw_ellipse3(painter, x, y, size, size);

    q_brush_delete(brush);
    q_stylepainter_delete(painter);
}

LightWidget* initialize_light_widget(int32_t color) {
    LightWidget* light = malloc(sizeof(LightWidget));
    if (light == NULL) {
        fprintf(stderr, "Failed to allocate memory for light widget\n");
        abort();
    }

    light->color = color;
    light->on = false;
    light->widget = q_widget_new2();
    q_widget_on_paint_event(light->widget, on_paint_event);

    QVariant* on_variant = q_variant_new7((intptr_t)&light->on);
    q_widget_set_property(light->widget, "on", on_variant);

    QVariant* color_variant = q_variant_new4(light->color);
    q_widget_set_property(light->widget, "color", color_variant);

    q_variant_delete(color_variant);
    q_variant_delete(on_variant);

    return light;
}

bool is_on(LightWidget* light) {
    return light->on;
}

void set_on(LightWidget* light, bool on) {
    if (on == light->on)
        return;
    light->on = on;
    q_widget_update(light->widget);
}

void turn_off(LightWidget* light) {
    set_on(light, false);
}

void turn_on(LightWidget* light) {
    set_on(light, true);
}

void cleanup_light_widget(LightWidget* light) {
    q_widget_delete(light->widget);
    free(light);
}

TrafficWidget* initialize_traffic_widget() {
    TrafficWidget* traffic = malloc(sizeof(TrafficWidget));
    if (traffic == NULL) {
        fprintf(stderr, "Failed to allocate memory for traffic widget\n");
        abort();
    }

    traffic->widget = q_widget_new2();
    QVBoxLayout* layout = q_vboxlayout_new(traffic->widget);

    traffic->red = initialize_light_widget(QT_GLOBALCOLOR_RED);
    traffic->yellow = initialize_light_widget(QT_GLOBALCOLOR_YELLOW);
    traffic->green = initialize_light_widget(QT_GLOBALCOLOR_GREEN);

    q_vboxlayout_add_widget(layout, traffic->red->widget);
    q_vboxlayout_add_widget(layout, traffic->yellow->widget);
    q_vboxlayout_add_widget(layout, traffic->green->widget);

    QPalette* palette = q_palette_new();
    QColor* color = q_color_new4(QT_GLOBALCOLOR_BLACK);

    q_palette_set_color2(palette, QPALETTE_COLORROLE_WINDOW, color);
    q_widget_set_palette(traffic->widget, palette);
    q_widget_set_auto_fill_background(traffic->widget, true);

    q_color_delete(color);
    q_palette_delete(palette);

    return traffic;
}

LightWidget* red_light(TrafficWidget* traffic) {
    return traffic->red;
}

LightWidget* yellow_light(TrafficWidget* traffic) {
    return traffic->yellow;
}

LightWidget* green_light(TrafficWidget* traffic) {
    return traffic->green;
}

void cleanup_traffic_widget(TrafficWidget* traffic) {
    cleanup_light_widget(traffic->red);
    cleanup_light_widget(traffic->yellow);
    cleanup_light_widget(traffic->green);
    q_widget_delete(traffic->widget);
    free(traffic);
}

void on_entered(void* self) {
    QVariant* light_variant = q_state_property(self, "light");
    uint64_t light_value = q_variant_to_u_long_long(light_variant);
    LightWidget* light = (LightWidget*)light_value;

    QVariant* timer_variant = q_state_property(self, "timer");
    uint64_t timer_value = q_variant_to_u_long_long(timer_variant);
    QTimer* timer = (QTimer*)timer_value;

    turn_on(light);
    q_timer_start2(timer);
}

void on_exited(void* self) {
    QVariant* light_variant = q_state_property(self, "light");
    uint64_t light_value = q_variant_to_u_long_long(light_variant);
    LightWidget* light = (LightWidget*)light_value;

    turn_off(light);
}

QState* create_light_state(LightWidget* light, int32_t duration) {
    QState* light_state = q_state_new();
    QState* timing = q_state_new3(light_state);

    QTimer* timer = q_timer_new2(light_state);
    q_timer_set_interval(timer, duration);
    q_timer_set_single_shot(timer, true);

    QVariant* light_variant = q_variant_new7((intptr_t)light);
    QVariant* timer_variant = q_variant_new7((intptr_t)timer);

    q_state_set_property(timing, "light", light_variant);
    q_state_set_property(timing, "timer", timer_variant);
    q_state_on_entered(timing, on_entered);
    q_state_on_exited(timing, on_exited);

    QFinalState* done = q_finalstate_new2(light_state);
    q_state_add_transition2(timing, timer, "timeout()", done);

    q_state_set_initial_state(light_state, timing);

    q_variant_delete(timer_variant);
    q_variant_delete(light_variant);

    return light_state;
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWidget* traffic_light = q_widget_new2();
    q_widget_set_window_title(traffic_light, "Qt 6 State Machine Example");
    q_widget_resize(traffic_light, 380, 800);
    q_widget_set_minimum_height(traffic_light, 450);
    q_widget_set_minimum_width(traffic_light, 200);

    QVBoxLayout* layout = q_vboxlayout_new(traffic_light);
    TrafficWidget* traffic_widget = initialize_traffic_widget();

    q_vboxlayout_add_widget(layout, traffic_widget->widget);
    q_vboxlayout_set_contents_margins(layout, 0, 0, 0, 0);

    QState* red_going_green = create_light_state(red_light(traffic_widget), 3000);
    QState* green_going_yellow = create_light_state(green_light(traffic_widget), 3000);
    QState* yellow_going_red = create_light_state(yellow_light(traffic_widget), 1000);

    q_state_add_transition2(red_going_green, red_going_green, "finished()", green_going_yellow);
    q_state_add_transition2(green_going_yellow, green_going_yellow, "finished()", yellow_going_red);
    q_state_add_transition2(yellow_going_red, yellow_going_red, "finished()", red_going_green);

    QStateMachine* machine = q_statemachine_new3(traffic_light);
    q_statemachine_add_state(machine, red_going_green);
    q_statemachine_add_state(machine, green_going_yellow);
    q_statemachine_add_state(machine, yellow_going_red);
    q_statemachine_set_initial_state(machine, red_going_green);
    q_statemachine_start(machine);

    q_widget_show(traffic_light);

    int result = q_application_exec();

    cleanup_traffic_widget(traffic_widget);
    q_widget_delete(traffic_light);
    q_application_delete(qapp);

    return result;
}
