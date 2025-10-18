#include <libqt6c.h>

static QLabel* status_label = NULL;

void transaction_finished(void* UNUSED self, int status, uint UNUSED runtime) {
    if (status == PACKAGEKIT_TRANSACTION_EXIT_EXITSUCCESS) {
        q_label_set_text(status_label, "✅ Update check successful!");
    } else {
        q_label_set_text(status_label, "❌ Update check failed!");
    }
}

void check_for_updates() {
    q_label_set_text(status_label, "Checking for updates...");

    PackageKit__Transaction* transaction = q_packagekit__daemon_get_updates();
    q_packagekit__transaction_on_finished(transaction, transaction_finished);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();
    q_widget_set_window_title(widget, "Qt 6 PackageKit Example");
    q_widget_resize(widget, 300, 200);

    QVBoxLayout* layout = q_vboxlayout_new2();
    QPushButton* button = q_pushbutton_new3("Check for updates");
    status_label = q_label_new2();
    q_label_set_alignment(status_label, QT_ALIGNMENTFLAG_ALIGNCENTER);

    q_vboxlayout_add_stretch(layout);
    q_vboxlayout_add_widget(layout, status_label);
    q_vboxlayout_add_stretch(layout);
    q_vboxlayout_add_widget(layout, button);
    q_vboxlayout_add_stretch(layout);
    q_widget_set_layout(widget, layout);

    q_pushbutton_on_clicked(button, check_for_updates);

    q_widget_show(widget);

    int result = q_application_exec();

    q_widget_delete(widget);
    q_application_delete(qapp);

    return result;
}
