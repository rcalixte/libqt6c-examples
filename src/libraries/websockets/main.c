#include <libqt6c.h>
#include <ctype.h>

static const char WS_URL[] = "ws://localhost:";
static const char CLIENT_STR[] = "Qt 6 WebSockets Example Client #";
static uint16_t LOCAL_PORT = 12345;
enum {
    MAX_CLIENTS = 10,
    NAME_LEN = 4,
    NUM_CLIENTS = 3,
};
static int OFFSET_X = 200;

typedef struct {
    char name[NAME_LEN];
    QDialog* dialog;
    QWebSocket* socket;
    QTextEdit* messages;
    QLineEdit* input;
    QPushButton* button;
} ClientDialog;

static QWebSocket* clients[MAX_CLIENTS];
static size_t client_num = 0;
static ClientDialog client_dialogs[NUM_CLIENTS];

void on_client_connected(void* self) {
    for (size_t i = 0; i < NUM_CLIENTS; i++)
        if (client_dialogs[i].socket == self) {
            q_textedit_append(client_dialogs[i].messages, "Connected!");
            q_lineedit_set_enabled(client_dialogs[i].input, true);
            q_pushbutton_set_enabled(client_dialogs[i].button, true);
            q_lineedit_set_focus(client_dialogs[i].input);
            return;
        }
}

void on_client_message_received(void* self, const char* message) {
    for (size_t i = 0; i < NUM_CLIENTS; i++)
        if (client_dialogs[i].socket == self) {
            q_textedit_append(client_dialogs[i].messages, message);
            return;
        }
}

void on_client_error_occurred(void* self, int32_t error_val UNUSED) {
    const char* err_str = q_websocket_error_string(self);
    for (size_t i = 0; i < NUM_CLIENTS; i++)
        if (client_dialogs[i].socket == self) {
            q_textedit_append(client_dialogs[i].messages, "= Error =");
            q_textedit_append(client_dialogs[i].messages, err_str);
            libqt_free(err_str);
            return;
        }
    libqt_free(err_str);
}

void on_client_close_event(void* self UNUSED, void* event) {
    for (size_t i = 0; i < NUM_CLIENTS; i++) {
        q_websocket_close(client_dialogs[i].socket);
        q_websocket_delete(client_dialogs[i].socket);
        q_dialog_super_close_event(client_dialogs[i].dialog, event);
    }
}

const char* trim_whitespace(const char* str) {
    if (str == NULL)
        return NULL;

    const char *start, *end;

    for (start = str; *start && isspace((unsigned char)*start); start++);

    if (*start == '\0')
        return start;

    for (end = start + strlen(start); end > start && isspace((unsigned char)*(end - 1)); end--);

    return start;
}

void send_message(const ClientDialog* self) {
    const char* message = q_lineedit_text(self->input);
    if (message == NULL)
        return;

    const char* trimmed_text = trim_whitespace(message);
    if (strlen(trimmed_text) == 0)
        return;

    size_t out_message_len = strlen(self->name) + strlen(trimmed_text) + 5;
    char* out_message = (char*)malloc(out_message_len);
    if (out_message == NULL) {
        fprintf(stderr, "Failed to allocate memory for out_message\n");
        abort();
    }
    snprintf(out_message, out_message_len, "(%s): %s", self->name, trimmed_text);

    q_websocket_send_text_message(self->socket, out_message);
    free(out_message);

    size_t self_entry_len = strlen(trimmed_text) + 4;
    char* self_entry = (char*)malloc(self_entry_len);
    if (self_entry == NULL) {
        fprintf(stderr, "Failed to allocate memory for self_entry\n");
        abort();
    }
    snprintf(self_entry, self_entry_len, ">> %s", trimmed_text);

    q_textedit_append(self->messages, self_entry);
    q_lineedit_clear(self->input);

    free(self_entry);
    libqt_free(message);
}

void on_send_clicked(void* self) {
    for (size_t i = 0; i < NUM_CLIENTS; i++)
        if (client_dialogs[i].button == self) {
            send_message(&client_dialogs[i]);
            return;
        }
}

void connect_to_server(const ClientDialog* self) {
    q_textedit_append(self->messages, "Connecting...");
    char* port_str = (char*)malloc(6);
    if (port_str == NULL) {
        fprintf(stderr, "Failed to allocate memory for port_str\n");
        abort();
    }
    snprintf(port_str, 6, "%d", LOCAL_PORT);
    size_t ws_len = sizeof WS_URL + strlen(port_str) + 1;
    char* ws = (char*)malloc(ws_len);
    if (ws == NULL) {
        free(port_str);
        fprintf(stderr, "Failed to allocate memory for ws\n");
        abort();
    }
    snprintf(ws, ws_len, "%s%s", WS_URL, port_str);

    QUrl* url = q_url_new3(ws);
    q_websocket_open(self->socket, url);

    free(ws);
    free(port_str);
    q_url_delete(url);
}

void initialize_dialog(ClientDialog* self, const char* name, const char* num_str) {
    strncpy(self->name, num_str, strlen(num_str));
    self->name[strlen(num_str)] = '\0';

    self->dialog = q_dialog_new2();
    q_dialog_set_window_title(self->dialog, name);
    q_dialog_set_minimum_size2(self->dialog, 400, 300);

    self->socket = q_websocket_new();
    q_websocket_set_parent(self->socket, self->dialog);

    self->messages = q_textedit_new(self->dialog);
    q_textedit_set_read_only(self->messages, true);

    self->input = q_lineedit_new(self->dialog);
    q_lineedit_set_placeholder_text(self->input, "Enter your message here");
    q_lineedit_set_enabled(self->input, false);

    self->button = q_pushbutton_new5("Send", self->dialog);
    q_pushbutton_set_enabled(self->button, false);

    QVBoxLayout* layout = q_vboxlayout_new2();
    QHBoxLayout* input_layout = q_hboxlayout_new2();

    q_vboxlayout_add_widget(layout, self->messages);
    q_hboxlayout_add_widget(input_layout, self->input);
    q_hboxlayout_add_widget(input_layout, self->button);
    q_vboxlayout_add_layout(layout, input_layout);
    q_dialog_set_layout(self->dialog, layout);

    q_websocket_on_connected(self->socket, on_client_connected);
    q_websocket_on_text_message_received(self->socket, on_client_message_received);
    q_websocket_on_error_occurred(self->socket, on_client_error_occurred);
    q_dialog_on_close_event(self->dialog, on_client_close_event);
    q_pushbutton_on_clicked(self->button, on_send_clicked);
}

void on_server_message_received(void* self, const char* message) {
    for (size_t i = 0; i < NUM_CLIENTS; i++) {
        if (clients[i] == NULL || clients[i] == self)
            continue;

        q_websocket_send_text_message(clients[i], message);
    }
}

void on_server_disconnected(void* self) {
    q_websocketserver_delete_later(self);
}

void on_new_connection(void* self) {
    QWebSocket* client = q_websocketserver_next_pending_connection(self);
    if (client_num >= MAX_CLIENTS) {
        q_websocket_close(client);
        return;
    }

    clients[client_num] = client;
    client_num += 1;
    q_websocket_on_text_message_received(client, on_server_message_received);
    q_websocket_on_disconnected(client, on_server_disconnected);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWebSocketServer* server = q_websocketserver_new("Example Qt WebSockets Server",
                                                     QWEBSOCKETSERVER_SSLMODE_NONSECUREMODE);
    QHostAddress* localhost = q_hostaddress_new7(QHOSTADDRESS_SPECIALADDRESS_LOCALHOSTIPV6);

    if (!q_websocketserver_listen2(server, localhost, LOCAL_PORT)) {
        fprintf(stderr, "Failed to listen on port %d\n", LOCAL_PORT);
        const char* err_str = q_websocketserver_error_string(server);
        fprintf(stderr, "Error: %s\n", err_str);
        libqt_free(err_str);
        abort();
    }

    for (size_t i = 0; i < NUM_CLIENTS; i++) {
        char num_str[8];
        snprintf(num_str, sizeof num_str, "%zu", i + 1);
        char client_name[sizeof CLIENT_STR + NAME_LEN];
        snprintf(client_name, sizeof client_name, "%s%s", CLIENT_STR, num_str);
        initialize_dialog(&client_dialogs[i], client_name, num_str);

        connect_to_server(&client_dialogs[i]);

        q_dialog_show(client_dialogs[i].dialog);
        int width = q_dialog_width(client_dialogs[i].dialog);
        int y = q_dialog_y(client_dialogs[i].dialog);
        q_dialog_move(client_dialogs[i].dialog, OFFSET_X + (width + 10) * (size_t)i, y);
    }

    q_websocketserver_on_new_connection(server, on_new_connection);

    int result = q_application_exec();

    for (size_t i = 0; i < NUM_CLIENTS; i++)
        q_dialog_delete_later(client_dialogs[i].dialog);
    q_hostaddress_delete(localhost);
    q_websocketserver_delete(server);
    q_application_delete(qapp);

    return result;
}
