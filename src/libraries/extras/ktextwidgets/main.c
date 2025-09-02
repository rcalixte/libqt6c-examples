#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    KRichTextEdit* textedit = k_richtextedit_new3();
    k_richtextedit_set_window_title(textedit, "Qt 6 KTextWidgets Example");
    k_richtextedit_set_minimum_size2(textedit, 950, 650);
    k_richtextedit_set_font_family(textedit, "DejaVu Sans Mono");
    k_richtextedit_set_font_size(textedit, 13);

    QFile* file = q_file_new4("src/libraries/extras/ktextwidgets/main.c", textedit);

    // Use Ctrl+F to search the file or right-click for a rich menu
    if (q_file_open(file, QIODEVICEBASE_OPENMODEFLAG_READONLY)) {
        const char* text = q_file_read_all(file);
        k_richtextedit_set_text_or_html(textedit, text);
        libqt_free(text);
        q_file_close(file);
    }

    k_richtextedit_show(textedit);

    int result = q_application_exec();

    k_richtextedit_delete(textedit);
    q_application_delete(qapp);

    return result;
}
