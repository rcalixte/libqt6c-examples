#include <libqt6c.h>

#define BUFFER_SIZE 256
static char buffer[BUFFER_SIZE];

static QCheckBox* toggle = NULL;
static QPushButton* button = NULL;
static QListWidget* list = NULL;
static QLabel* status = NULL;
static QBluetoothDeviceDiscoveryAgent* agent = NULL;

void on_toggled(void* self UNUSED, bool checked) {
    q_pushbutton_set_enabled(button, checked);
    q_listwidget_set_enabled(list, checked);

    if (!checked && q_bluetoothdevicediscoveryagent_is_active(agent)) {
        q_bluetoothdevicediscoveryagent_stop(agent);
    }

    const char* text = checked ? "Bluetooth enabled." : "Bluetooth disabled.";
    q_label_set_text(status, text);
}

void on_clicked(void* self) {
    if (q_bluetoothdevicediscoveryagent_is_active(agent)) {
        return;
    }

    q_listwidget_clear(list);
    q_label_set_text(status, "Scanning...");
    q_pushbutton_set_enabled(self, false);
    q_bluetoothdevicediscoveryagent_start2(
        agent,
        QBLUETOOTHDEVICEDISCOVERYAGENT_DISCOVERYMETHOD_CLASSICMETHOD | QBLUETOOTHDEVICEDISCOVERYAGENT_DISCOVERYMETHOD_LOWENERGYMETHOD);
}

void on_device_discovered(void* self UNUSED, void* info) {
    const char* name = q_bluetoothdeviceinfo_name(info);

    QBluetoothAddress* address = q_bluetoothdeviceinfo_address(info);

    const char* address_str = q_bluetoothaddress_to_string(address);

    snprintf(buffer, BUFFER_SIZE, "%s (%s)", name, address_str);
    const char* title = strlen(name) > 0 ? buffer : "Unknown";

    q_listwidget_add_item(list, title);

    libqt_free(address_str);
    libqt_free(name);
}

void on_finished(void* self UNUSED) {
    q_pushbutton_set_enabled(button, q_checkbox_is_checked(toggle));

    snprintf(buffer, BUFFER_SIZE, "Scan complete - %d device(s) found.", q_listwidget_count(list));
    q_label_set_text(status, buffer);
}

void on_error_occurred(void* self, int32_t error_val UNUSED) {
    q_pushbutton_set_enabled(button, q_checkbox_is_checked(toggle));

    const char* err_str = q_bluetoothdevicediscoveryagent_error_string(self);
    q_label_set_text(status, err_str);
    libqt_free(err_str);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();

    q_widget_set_window_title(widget, "Qt 6 Bluetooth Example");
    q_widget_set_minimum_size2(widget, 400, 400);

    QBluetoothLocalDevice* local_device = q_bluetoothlocaldevice_new();
    QVBoxLayout* layout = q_vboxlayout_new(widget);

    if (q_bluetoothlocaldevice_is_valid(local_device)) {
        toggle = q_checkbox_new3("Bluetooth enabled");
        q_checkbox_set_checked(toggle, true);

        button = q_pushbutton_new3("Scan for devices");
        list = q_listwidget_new2();
        status = q_label_new3("Ready.");

        q_vboxlayout_add_widget(layout, toggle);
        q_vboxlayout_add_widget(layout, button);
        q_vboxlayout_add_widget(layout, list);
        q_vboxlayout_add_widget(layout, status);

        agent = q_bluetoothdevicediscoveryagent_new3(widget);
        q_bluetoothdevicediscoveryagent_set_low_energy_discovery_timeout(agent, 3000);

        q_checkbox_on_toggled(toggle, on_toggled);
        q_pushbutton_on_clicked(button, on_clicked);
        q_bluetoothdevicediscoveryagent_on_device_discovered(agent, on_device_discovered);
        q_bluetoothdevicediscoveryagent_on_finished(agent, on_finished);
        q_bluetoothdevicediscoveryagent_on_error_occurred(agent, on_error_occurred);
    } else {
        QLabel* label = q_label_new3("## No Bluetooth adapter detected."
                                     "\n### Please ensure that your device has a Bluetooth adapter.");
        q_label_set_text_format(label, QT_TEXTFORMAT_MARKDOWNTEXT);
        q_label_set_alignment(label, QT_ALIGNMENTFLAG_ALIGNCENTER);
        q_label_set_word_wrap(label, true);
        q_vboxlayout_add_widget(layout, label);
    }

    q_widget_show(widget);

    int result = q_application_exec();

    q_bluetoothlocaldevice_delete(local_device);
    q_widget_delete(widget);
    q_application_delete(qapp);

    return result;
}
