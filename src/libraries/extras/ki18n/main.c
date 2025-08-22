#include <libqt6c.h>

static libqt_list all_countries;
static KCountry** countries;
static QLabel* emoji_flag_label;
static QLabel* currency_label;
static char emoji_buffer[24];
static char currency_buffer[24];

void on_current_index_changed(void* UNUSED self, int index) {
    KCountry* country = countries[index];
    const char* emoji_flag = k_country_emoji_flag(country);
    snprintf(emoji_buffer, sizeof(emoji_buffer), "Emoji flag: %s", emoji_flag);
    q_label_set_text(emoji_flag_label, emoji_buffer);
    libqt_free(emoji_flag);

    const char* currency = k_country_currency_code(country);
    if (currency == NULL || currency[0] == '\0') {
        q_label_set_text(currency_label, "Currency code: ");
    } else {
        snprintf(currency_buffer, sizeof(currency_buffer), "Currency code: %s", currency);
        q_label_set_text(currency_label, currency_buffer);
    }
    libqt_free(currency);
}

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);

    all_countries = k_country_all_countries();
    countries = (KCountry**)all_countries.data.ptr;

    QWidget* widget = q_widget_new2();
    q_widget_set_window_title(widget, "Qt 6 KCountry Example");
    q_widget_set_fixed_size2(widget, 400, 250);

    // Ownership of the created widgets will be transferred to the widget via the layout
    QVBoxLayout* vboxlayout = q_vboxlayout_new2();
    QLabel* label = q_label_new3("Select a country:");
    QComboBox* country_combo = q_combobox_new2();
    emoji_flag_label = q_label_new2();
    QFont* font = q_font_new2("Noto Color Emoji");
    const char* style_sheet = "font-size: 28px;";
    q_label_set_font(emoji_flag_label, font);
    q_label_set_style_sheet(emoji_flag_label, style_sheet);
    q_label_set_alignment(emoji_flag_label, QT_ALIGNMENTFLAG_ALIGNCENTER);
    currency_label = q_label_new2();
    q_label_set_font(currency_label, font);
    q_label_set_style_sheet(currency_label, style_sheet);
    q_label_set_alignment(currency_label, QT_ALIGNMENTFLAG_ALIGNCENTER);
    q_font_delete(font);

    for (size_t i = 0; i < all_countries.len; ++i) {
        const char* name = k_country_name(countries[i]);
        q_combobox_add_item(country_combo, name);
        libqt_free(name);
    }

    q_combobox_on_current_index_changed(country_combo, on_current_index_changed);

    q_vboxlayout_add_widget(vboxlayout, label);
    q_vboxlayout_add_widget(vboxlayout, country_combo);
    q_vboxlayout_add_stretch(vboxlayout);
    q_vboxlayout_add_widget(vboxlayout, emoji_flag_label);
    q_vboxlayout_add_widget(vboxlayout, currency_label);
    q_vboxlayout_add_stretch(vboxlayout);
    q_widget_set_layout(widget, vboxlayout);

    q_widget_show(widget);

    int result = q_application_exec();

    q_widget_delete(widget);

    for (size_t i = 0; i < all_countries.len; ++i) {
        k_country_delete(countries[i]);
    }
    libqt_free(all_countries.data.ptr);

    return result;
}
