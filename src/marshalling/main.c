#include <libqt6c.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    // Initialize Qt application
    q_application_new(&argc, argv);

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

    // QString
    QWidget* widget = q_widget_new2();
    q_widget_set_tool_tip(widget, "Sample text");
    const char* tip = q_widget_tool_tip(widget);
    printf("Widget tooltip: %s\n", tip);
    q_widget_delete(widget);

    // QList<int>
    int nums[] = {10, 20, 30, 40, 50};
    libqt_list numbers = {
	    .len = 5,
	    .data = {(int*)nums},
    };
    QVersionNumber* version = q_versionnumber_new2(numbers);
    libqt_list segs_list = q_versionnumber_segments(version);
    int* segs = (int*)segs_list.data.ints;
    for (int i = 0; i < segs_list.len; i++) {
        if (segs[i]) {
            printf("Segment %d: %d\n", i, segs[i]);
        }
    }
    free(segs_list.data.ints);
    q_versionnumber_delete(version);

    // QStringList
    QInputDialog* dialog = q_inputdialog_new2();
    const char* text[] = {"foo", "bar", "baz", "quux", NULL};
    q_inputdialog_set_combo_box_items(dialog, text);
    const char** items = q_inputdialog_combo_box_items(dialog);
    // hacking around UB
    int STRV_LEN = 4;
    for (int i = 0; i < STRV_LEN; i++) {
        if (items[i]) {
            printf("Combo box item %d: %s\n", i + 1, items[i]);
            free((void*)items[i]);
        }
    }
    free(items);
    q_inputdialog_delete(dialog);

    // QList<Qt type>
    void* keyData[] = {
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
    for (int i = 0; i < (int)shortcuts.len; i++) {
        QKeySequence* shortcut = shortcuts_list[i];
        if (shortcut) {
            printf("Shortcut %d: %s\n", i, q_keysequence_to_string(shortcut));
        }
        q_keysequence_delete(shortcut);
    }
    q_action_delete(action);

    // QByteArray
    const char* f_input = "foo bar baz";
    char* bat = q_file_encode_name(f_input);
    const char* f_output = q_file_decode_name(bat);
    printf("QByteArray: %s\n", f_output);
    free(bat);

    // QAnyStringView parameter
    QVariant* variant = q_variant_new14("QAnyStringView");
    const char* value = q_variant_to_string(variant);
    printf("Value: %s\n", value);
    q_variant_delete(variant);

    return 0;
}
