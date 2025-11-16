#include <libqt6c.h>
#include <stdio.h>

#define BUS_NAME "org.freedesktop.Notifications"
#define BUS_PATH "/org/freedesktop/Notifications"

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QDBusConnection* session_bus = q_dbusconnection_session_bus();
    QDBusMessage* message = q_dbusmessage_create_method_call(BUS_NAME, BUS_PATH, BUS_NAME, "Notify");

    const char* actions[] = {NULL};
    libqt_map hints = {
        .len = 0,
        .keys = NULL,
        .values = NULL,
    };

    QVariant* arguments[] = {
        q_variant_new24("Qt 6 D-Bus Example"),
        q_variant_new5(0),
        q_variant_new24("dialog-information"),
        q_variant_new24("Qt 6 D-Bus Example"),
        q_variant_new24("This is a test notification sent via D-Bus."),
        q_variant_new25(actions),
        q_variant_new22(hints),
        q_variant_new4(-1),
        NULL,
    };
    libqt_list args = qlist(arguments, 8);

    q_dbusmessage_set_arguments(message, args);

    QDBusMessage* reply = q_dbusconnection_call(session_bus, message);

    if (q_dbusmessage_type(reply) != QDBUSMESSAGE_MESSAGETYPE_REPLYMESSAGE) {
        printf("Failed to send message\n");
        q_application_quit();
    }

    q_dbusmessage_delete(reply);
    q_dbusmessage_delete(message);
    q_dbusconnection_delete(session_bus);
    q_application_delete(qapp);
}
