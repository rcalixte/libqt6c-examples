#include <libqt6c.h>

static QLabel* label;

void on_resuming_from_idle(void* self) {
    k_idletime_remove_all_idle_timeouts(self);
    k_idletime_add_idle_timeout(self, 5000);
}

void on_timeout_reached() {
    q_label_set_text(label, "## Timeout reached!");
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();
    q_widget_set_window_title(widget, "Qt 6 KIdleTime Example");
    q_widget_set_fixed_size2(widget, 550, 150);

    QVBoxLayout* layout = q_vboxlayout_new2();
    label = q_label_new2();
    q_label_set_alignment(label, QT_ALIGNMENTFLAG_ALIGNCENTER);
    q_label_set_text_format(label, QT_TEXTFORMAT_MARKDOWNTEXT);
    q_label_set_text(label, "### This text will stay here until you have been idle for 5 seconds.");

    KIdleTime* idle_time = k_idletime_instance();
    k_idletime_catch_next_resume_event(idle_time);
    k_idletime_simulate_user_activity(idle_time);
    k_idletime_on_resuming_from_idle(idle_time, on_resuming_from_idle);
    k_idletime_on_timeout_reached(idle_time, on_timeout_reached);

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
