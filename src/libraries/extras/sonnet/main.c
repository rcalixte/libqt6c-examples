#include <libqt6c.h>

static Sonnet__Highlighter* highlighter1;
static Sonnet__Highlighter* highlighter2;

static void on_dictionary_changed(void* UNUSED self, const char* dictionary) {
    k_sonnet__highlighter_set_current_language(highlighter1, dictionary);
    k_sonnet__highlighter_set_current_language(highlighter2, dictionary);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();
    q_widget_set_window_title(widget, "Qt 6 Sonnet Example");

    Sonnet__DictionaryComboBox* comboBox = k_sonnet__dictionarycombobox_new2();
    QTextEdit* textedit1 = q_textedit_new2();
    q_textedit_set_text(textedit1, "This is a sample buffer. Whih this thingg will be checkin for misstakes. Whih, Enviroment, covermant. Whih.");

    Sonnet__SpellCheckDecorator* installer1 = k_sonnet__spellcheckdecorator_new(textedit1);
    highlighter1 = k_sonnet__spellcheckdecorator_highlighter(installer1);

    k_sonnet__highlighter_set_current_language(highlighter1, "en_US");

    QTextEdit* textedit2 = q_textedit_new2();
    q_textedit_set_text(textedit2, "John Doe said:\n> Hello how aree you?\nI am ffine thanks");

    Sonnet__SpellCheckDecorator* installer2 = k_sonnet__spellcheckdecorator_new(textedit2);
    highlighter2 = k_sonnet__spellcheckdecorator_highlighter(installer2);

    k_sonnet__highlighter_set_current_language(highlighter2, "en_US");

    k_sonnet__dictionarycombobox_on_dictionary_changed(comboBox, on_dictionary_changed);

    QVBoxLayout* layout = q_vboxlayout_new(widget);
    q_vboxlayout_add_widget(layout, comboBox);
    q_vboxlayout_add_widget(layout, textedit1);
    q_vboxlayout_add_widget(layout, textedit2);

    q_widget_show(widget);

    int ret = q_application_exec();

    q_widget_delete(widget);
    q_application_delete(qapp);

    return ret;
}
