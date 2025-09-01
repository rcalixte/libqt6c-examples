#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();
    q_widget_set_window_title(widget, "Qt 6 KCompletion Example");
    q_widget_set_minimum_size2(widget, 300, 200);

    QLabel* label = q_label_new3("Enter the letter 'H':");

    QVBoxLayout* vboxlayout = q_vboxlayout_new2();

    KLineEdit* lineedit = k_lineedit_new3();
    // Try different completion modes!
    k_lineedit_set_completion_mode(lineedit, KCOMPLETION_COMPLETIONMODE_COMPLETIONPOPUPAUTO);

    KCompletion* completion = k_completion_new();
    k_completion_set_sounds_enabled(completion, false);
    k_lineedit_set_completion_object(lineedit, completion, true);

    const char* items[] = {"Hello Qt", "Hello C", "Hello libqt6c", "Hello you", "Hello world", NULL};
    k_completion_set_items(completion, items);

    q_vboxlayout_add_stretch(vboxlayout);
    q_vboxlayout_add_widget(vboxlayout, label);
    q_vboxlayout_add_widget(vboxlayout, lineedit);
    q_vboxlayout_add_stretch(vboxlayout);
    q_widget_set_layout(widget, vboxlayout);

    q_widget_show(widget);

    int result = q_application_exec();

    q_widget_delete(widget);
    q_application_delete(qapp);

    return result;
}
