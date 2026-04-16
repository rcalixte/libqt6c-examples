#include <libqt6c.h>

static QComboBox* from = NULL;
static QComboBox* to = NULL;
static QLineEdit* input = NULL;
static QLabel* result = NULL;

#define BUFFER_SIZE 128
static char buffer[BUFFER_SIZE];

void on_text_changed(void* self UNUSED, const char* param1 UNUSED) {
    const char* text = q_lineedit_text(input);
    if (text == NULL || strcmp(text, "") == 0) {
        q_label_set_text(result, "### Result:");
        libqt_free(text);
        return;
    }

    double value = atof(text);
    if (value == 0 && strcmp(text, "0") != 0) {
        q_label_set_text(result, "### Invalid input");
        libqt_free(text);
        return;
    }

    QVariant* from_data = q_combobox_current_data(from);
    int32_t from_id = q_variant_to_int(from_data);

    QVariant* to_data = q_combobox_current_data(to);
    int32_t to_id = q_variant_to_int(to_data);

    KUnitConversion__Value* converted_obj = k_unitconversion__value_new4(value, from_id);
    KUnitConversion__Value* converted_value = k_unitconversion__value_convert_to2(converted_obj, to_id);

    const char* converted_text = k_unitconversion__value_to_string(converted_value);
    snprintf(buffer, BUFFER_SIZE, "### Result: %s", converted_text);
    q_label_set_text(result, buffer);

    k_unitconversion__value_delete(converted_value);
    k_unitconversion__value_delete(converted_obj);
    q_variant_delete(to_data);
    q_variant_delete(from_data);
    libqt_free(converted_text);
    libqt_free(text);
}

void on_combo_changed(void* self UNUSED, int32_t index UNUSED) {
    on_text_changed(input, "");
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();
    q_widget_set_window_title(widget, "Qt 6 KUnitConversion Example");
    q_widget_set_fixed_size2(widget, 450, 300);

    QVBoxLayout* layout = q_vboxlayout_new(widget);
    KUnitConversion__Converter* converter = k_unitconversion__converter_new();

    // Update the category type to change the units!
    KUnitConversion__UnitCategory* category = k_unitconversion__converter_category2(converter, KUNITCONVERSION_CATEGORYID_LENGTHCATEGORY);

    from = q_combobox_new2();
    to = q_combobox_new2();

    libqt_list units = k_unitconversion__unitcategory_units(category);
    KUnitConversion__Unit** unit_data = units.data.ptr;

    for (size_t i = 0; i < units.len; i++) {
        const char* description = k_unitconversion__unit_description(unit_data[i]);

        int32_t id = k_unitconversion__unit_id(unit_data[i]);
        QVariant* data = q_variant_new4(id);

        q_combobox_add_item22(from, description, data);
        q_combobox_add_item22(to, description, data);

        q_variant_delete(data);
        libqt_free(description);
    }

    input = q_lineedit_new2();
    q_lineedit_set_placeholder_text(input, "Enter a value");

    result = q_label_new3("### Result:");
    q_label_set_text_format(result, QT_TEXTFORMAT_MARKDOWNTEXT);
    q_label_set_alignment(result, QT_ALIGNMENTFLAG_ALIGNCENTER);

    q_vboxlayout_add_widget(layout, q_label_new3("From:"));
    q_vboxlayout_add_widget(layout, from);
    q_vboxlayout_add_widget(layout, input);
    q_vboxlayout_add_stretch(layout);
    q_vboxlayout_add_widget(layout, q_label_new3("To:"));
    q_vboxlayout_add_widget(layout, to);
    q_vboxlayout_add_stretch(layout);
    q_vboxlayout_add_widget(layout, result);

    q_combobox_on_current_index_changed(from, on_combo_changed);
    q_combobox_on_current_index_changed(to, on_combo_changed);
    q_lineedit_on_text_changed(input, on_text_changed);

    q_lineedit_set_focus(input);

    q_widget_show(widget);

    int result = q_application_exec();

    k_unitconversion__unitcategory_delete(category);
    k_unitconversion__converter_delete(converter);
    q_widget_delete(widget);
    q_application_delete(qapp);

    return result;
}
