#include <libqt6c.h>

static const char* SRCFILE = "src/libraries/extras/ksyntaxhighlighting/main.c";

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QMainWindow* window = q_mainwindow_new2();
    q_mainwindow_set_window_title(window, "Qt 6 KSyntaxHighlighting Example");
    q_mainwindow_set_minimum_size2(window, 1600, 750);

    QFile* file = q_file_new4(SRCFILE, window);

    if (!q_file_open(file, QIODEVICEBASE_OPENMODEFLAG_READONLY)) {
        fprintf(stderr, "Failed to open file: \t%s\n", q_file_file_name(file));
        return 1;
    }

    QPlainTextEdit* plain_text_edit = q_plaintextedit_new2();

    QFont* font = q_font_new6("DejaVu Sans Mono", 13);

    q_plaintextedit_set_font(plain_text_edit, font);
    q_font_delete(font);

    q_mainwindow_set_central_widget(window, plain_text_edit);

    const char* text = q_file_read_all(file);

    q_plaintextedit_set_plain_text(plain_text_edit, text);

    QTextDocument* document = q_plaintextedit_document(plain_text_edit);
    KSyntaxHighlighting__SyntaxHighlighter* highlighter = k_syntaxhighlighting__syntaxhighlighter_new2(document);
    KSyntaxHighlighting__Repository* repository = k_syntaxhighlighting__repository_new();

    int lightness = q_color_lightness((void*)q_palette_color2((void*)q_plaintextedit_palette(plain_text_edit), QPALETTE_COLORROLE_BASE));

    KSyntaxHighlighting__Theme* theme;

    if (lightness < 128) {
        theme = k_syntaxhighlighting__repository_default_theme1(repository, KSYNTAXHIGHLIGHTING_REPOSITORY_DEFAULTTHEME_DARKTHEME);
    } else {
        theme = k_syntaxhighlighting__repository_default_theme1(repository, KSYNTAXHIGHLIGHTING_REPOSITORY_DEFAULTTHEME_LIGHTTHEME);
    }

    k_syntaxhighlighting__syntaxhighlighter_set_theme(highlighter, theme);
    k_syntaxhighlighting__syntaxhighlighter_set_definition(highlighter, k_syntaxhighlighting__repository_definition_for_file_name(repository, SRCFILE));

    q_mainwindow_show(window);

    int result = q_application_exec();

    k_syntaxhighlighting__syntaxhighlighter_delete(highlighter);
    libqt_free(text);
    q_file_close(file);
    q_mainwindow_delete(window);
    q_application_delete(qapp);

    return result;
}
