#include <libqt6c.h>

#define BUS_NAME "org.freedesktop.Notifications"
#define BUS_PATH "/org/freedesktop/Notifications"

int main(int argc, char* argv[]) {
    QCoreApplication* qapp = q_coreapplication_new(&argc, argv);

    QDBusConnection* session_bus = q_dbusconnection_session_bus();
    QDBusMessage* message = q_dbusmessage_create_method_call(BUS_NAME, BUS_PATH, BUS_NAME, "Notify");

    const char* actions[] = {NULL};
    libqt_map hints = {
        .len = 0,
        .keys = NULL,
        .values = NULL,
    };

    QVariant* variant_name = q_variant_new24("Qt 6 D-Bus Example");
    QVariant* variant_id = q_variant_new5(0);
    QVariant* variant_icon = q_variant_new24("dialog-information");
    QVariant* variant_body = q_variant_new24("This is a test notification sent via D-Bus.");
    QVariant* variant_actions = q_variant_new25(actions);
    QVariant* variant_hints = q_variant_new22(hints);
    QVariant* variant_timeout = q_variant_new4(-1);

    QVariant* arguments[] = {
        variant_name,
        variant_id,
        variant_icon,
        variant_name,
        variant_body,
        variant_actions,
        variant_hints,
        variant_timeout,
        NULL,
    };
    libqt_list args = qlist(arguments, 8);

    q_dbusmessage_set_arguments(message, args);

    QDBusMessage* reply = q_dbusconnection_call(session_bus, message);

    if (q_dbusmessage_type(reply) != QDBUSMESSAGE_MESSAGETYPE_REPLYMESSAGE) {
        printf("Failed to send message\n");
        q_coreapplication_quit();
    }

    q_variant_delete(variant_name);
    q_variant_delete(variant_id);
    q_variant_delete(variant_icon);
    q_variant_delete(variant_body);
    q_variant_delete(variant_actions);
    q_variant_delete(variant_hints);
    q_variant_delete(variant_timeout);
    q_dbusmessage_delete(reply);
    q_dbusmessage_delete(message);
    q_dbusconnection_delete(session_bus);
    q_coreapplication_delete(qapp);
}
