#include <libqt6c.h>
#include <stdbool.h>

const char** on_mime_types() {
    const char** ret = (const char**)malloc(4 * sizeof(char*));
    if (ret == NULL) {
        return NULL;
    }

    ret[0] = "image/gif";
    ret[1] = "image/jpeg";
    ret[2] = "image/png";
    ret[3] = NULL;

    return ret;
}

int main(int argc, char* argv[]) {
    // Initialize Qt application
    QApplication* qapp = q_application_new(&argc, argv);

    // Bool
    QCheckBox* checkBox = q_checkbox_new2();
    q_checkbox_set_checked(checkBox, true);
    printf("Checkbox is checked: %d\n", q_checkbox_is_checked(checkBox));
    q_checkbox_delete(checkBox);

    // Int
    QSize* size = q_size_new3();
    q_size_set_width(size, 128);
    printf("Width: %d\n", q_size_width(size));
    q_size_delete(size);

    // Int by reference
    QSize* size_ref = q_size_new4(32, 32);
    int* ref = q_size_rheight(size_ref);
    *ref = 64;
    printf("Height: %d\n", q_size_height(size_ref));
    q_size_delete(size_ref);

    // QString
    QWidget* widget = q_widget_new2();
    q_widget_set_tool_tip(widget, "Sample text");
    const char* tip = q_widget_tool_tip(widget);
    printf("Widget tooltip: %s\n", tip);
    libqt_free(tip);
    q_widget_delete(widget);

    // QList<int>
    int nums[] = {10, 20, 30, 40, 50};
    libqt_list numbers = {
        .len = 5,
        .data.ints = nums,
    };
    QVersionNumber* version = q_versionnumber_new2(numbers);
    libqt_list segs_list = q_versionnumber_segments(version);
    int* segs = (int*)segs_list.data.ints;
    for (size_t i = 0; i < segs_list.len; i++) {
        if (segs[i]) {
            printf("Segment %zu: %d\n", i, segs[i]);
        }
    }
    free(segs_list.data.ints);
    q_versionnumber_delete(version);

    // QStringList
    QInputDialog* dialog = q_inputdialog_new2();
    const char* text[] = {"foo", "bar", "baz", "quux", NULL};
    q_inputdialog_set_combo_box_items(dialog, text);
    const char** items = q_inputdialog_combo_box_items(dialog);
    size_t items_len = libqt_strv_length(items);
    for (size_t i = 0; i < items_len; i++) {
        if (items[i]) {
            printf("Combo box item %zu: %s\n", i + 1, items[i]);
            libqt_free(items[i]);
        }
    }
    free(items);
    q_inputdialog_delete(dialog);

    // QStringList callback
    QTableWidget* table = q_tablewidget_new2();
    q_tablewidget_on_mime_types(table, on_mime_types);
    const char** table_mime_types = q_tablewidget_mime_types(table);
    size_t table_mime_len = libqt_strv_length(table_mime_types);
    for (size_t i = 0; i < table_mime_len; i++) {
        printf("MimeTypes[%zu]: %s\n", i, table_mime_types[i]);
        libqt_free(table_mime_types[i]);
    }
    free(table_mime_types);
    q_tablewidget_delete(table);

    // QList<Qt type>
    QKeySequence* keyData[] = {
        q_keysequence_from_string("F1"),
        q_keysequence_from_string("F2"),
        q_keysequence_from_string("F3"),
        NULL,
    };
    libqt_list keyList = qlist(keyData, 3);
    QAction* action = q_action_new();
    q_action_set_shortcuts(action, keyList);
    libqt_list shortcuts = q_action_shortcuts(action);
    QKeySequence** shortcuts_list = (QKeySequence**)shortcuts.data.ptr;
    for (size_t i = 0; i < shortcuts.len; i++) {
        QKeySequence* shortcut = shortcuts_list[i];
        if (shortcut) {
            printf("Shortcut %zu: %s\n", i, q_keysequence_to_string(shortcut));
        }
        q_keysequence_delete(shortcut);
    }
    q_action_delete(action);

    // QByteArray
    const char* f_input = "foo bar baz";
    char* bat = q_file_encode_name(f_input);
    const char* f_output = q_file_decode_name(bat);
    printf("QByteArray: %s\n", f_output);
    libqt_free(f_output);
    free(bat);

    // QAnyStringView parameter
    QObject* object = q_object_new();
    q_object_set_object_name(object, "QAnyStringView Name");
    const char* value = q_object_object_name(object);
    printf("Value: %s\n", value);
    libqt_free(value);
    q_object_delete(object);

    // QMap<QString, QVariant>
    libqt_map input_map;
    input_map.len = 3;
    const char** map_key = (const char**)malloc(input_map.len * sizeof(char*));
    if (map_key == NULL) {
        fprintf(stderr, "Failed to allocate memory for map key\n");
        exit(1);
    }
    map_key[0] = "foo";
    map_key[1] = "bar";
    map_key[2] = "baz";
    QVariant** map_value = (QVariant**)malloc(input_map.len * sizeof(QVariant*));
    if (map_value == NULL) {
        fprintf(stderr, "Failed to allocate memory for map value\n");
        free(map_key);
        exit(1);
    }
    map_value[0] = q_variant_new24("FOO");
    map_value[1] = q_variant_new24("BAR");
    map_value[2] = q_variant_new24("BAZ");
    input_map.keys = (void*)map_key;
    input_map.values = (void*)map_value;
    QJsonObject* qtobj = q_jsonobject_from_variant_map(input_map);
    libqt_map output_map = q_jsonobject_to_variant_map(qtobj);
    const char** keys = (const char**)output_map.keys;
    QVariant** values = (QVariant**)output_map.values;
    for (size_t i = 0; i < output_map.len; i++) {
        const char* key = keys[i];
        QVariant* value = values[i];
        const char* value_str = q_variant_to_string(value);
        printf("QMap[%s]: %s\n", key, value_str);
        libqt_free(value_str);
        libqt_free(key);
        free(value);
    }
    free(values);
    free(keys);
    q_jsonobject_delete(qtobj);
    free(map_value);
    free(map_key);

    // QMultiMap<QString, QString>
    char** map_keys = (char**)malloc(2 * sizeof(char*));
    if (map_keys == NULL) {
        fprintf(stderr, "Failed to allocate memory for keys\n");
        abort();
    }
    map_keys[0] = "Accept";
    map_keys[1] = NULL;
    char*** map_values = (char***)malloc(1 * sizeof(const char**));
    if (map_values == NULL) {
        free(map_keys);
        fprintf(stderr, "Failed to allocate memory for values\n");
        abort();
    }
    map_values[0] = (char**)malloc(4 * sizeof(char*));
    if (map_values[0] == NULL) {
        free(map_keys);
        free(map_values);
        fprintf(stderr, "Failed to allocate memory for values[0]\n");
        abort();
    }
    map_values[0][0] = "text/html";
    map_values[0][1] = "application/xhtml+xml";
    map_values[0][2] = "application/xml;";
    map_values[0][3] = NULL;
    libqt_map map = {
        .len = 1,
        .keys = map_keys,
        .values = map_values,
    };
    QHttpHeaders* qheaders = q_httpheaders_from_multi_map(map);
    for (size_t i = 0; i < map.len; i++) {
        free(map_values[i]);
    }
    free(map_keys);
    free(map_values);
    libqt_map header_map = q_httpheaders_to_multi_map(qheaders);
    char** header_keys = header_map.keys;
    char*** header_values = header_map.values;
    for (size_t i = 0; i < header_map.len; i++) {
        printf("HTTP Header: %s: ", header_keys[i]);
        size_t count = 0;
        while (header_values[i][count] != NULL) {
            count++;
        }
        for (size_t j = 0; j < count; j++) {
            printf("%s", header_values[i][j]);
            libqt_free(header_values[i][j]);
            if (j < count - 1) {
                printf(",");
            }
        }
        printf("\n");
        free(header_values[i]);
        libqt_free(header_keys[i]);
    }
    free(header_values);
    free(header_keys);
    q_httpheaders_delete(qheaders);

    q_application_delete(qapp);

    return 0;
}
