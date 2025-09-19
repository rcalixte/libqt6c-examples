#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    Attica__Project* project = k_attica__project_new();

    k_attica__project_set_description(project, "Qt 6 for C");
    k_attica__project_set_name(project, "libqt6c");
    k_attica__project_set_version(project, "6.8.2");
    k_attica__project_set_url(project, "https://github.com/rcalixte/libqt6c");
    k_attica__project_set_license(project, "MIT");

    QWidget* widget = q_widget_new2();
    q_widget_set_window_title(widget, "Qt 6 Example for Attica");
    q_widget_set_minimum_size2(widget, 350, 250);

    QLabel* desc = q_label_new3("Description:");
    q_label_set_text_interaction_flags(desc, QT_TEXTINTERACTIONFLAG_NOTEXTINTERACTION);
    const char* desc_text = k_attica__project_description(project);
    QLineEdit* desc_edit = q_lineedit_new3(desc_text);
    libqt_free(desc_text);
    q_lineedit_set_read_only(desc_edit, true);

    QLabel* name = q_label_new3("Name:");
    q_label_set_text_interaction_flags(name, QT_TEXTINTERACTIONFLAG_NOTEXTINTERACTION);
    const char* name_text = k_attica__project_name(project);
    QLineEdit* name_edit = q_lineedit_new3(name_text);
    libqt_free(name_text);
    q_lineedit_set_read_only(name_edit, true);

    QLabel* version = q_label_new3("Version:");
    q_label_set_text_interaction_flags(version, QT_TEXTINTERACTIONFLAG_NOTEXTINTERACTION);
    const char* version_text = k_attica__project_version(project);
    QLineEdit* version_edit = q_lineedit_new3(version_text);
    libqt_free(version_text);
    q_lineedit_set_read_only(version_edit, true);

    QLabel* url = q_label_new3("URL:");
    q_label_set_text_interaction_flags(url, QT_TEXTINTERACTIONFLAG_NOTEXTINTERACTION);
    const char* url_text = k_attica__project_url(project);
    QLineEdit* url_edit = q_lineedit_new3(url_text);
    libqt_free(url_text);
    q_lineedit_set_read_only(url_edit, true);

    QLabel* lic = q_label_new3("License:");
    q_label_set_text_interaction_flags(lic, QT_TEXTINTERACTIONFLAG_NOTEXTINTERACTION);
    const char* lic_text = k_attica__project_license(project);
    QLineEdit* lic_edit = q_lineedit_new3(lic_text);
    libqt_free(lic_text);
    q_lineedit_set_read_only(lic_edit, true);

    QGridLayout* layout = q_gridlayout_new2();

    q_gridlayout_add_widget2(layout, desc, 0, 0);
    q_gridlayout_add_widget2(layout, desc_edit, 0, 1);
    q_gridlayout_add_widget2(layout, name, 1, 0);
    q_gridlayout_add_widget2(layout, name_edit, 1, 1);
    q_gridlayout_add_widget2(layout, version, 2, 0);
    q_gridlayout_add_widget2(layout, version_edit, 2, 1);
    q_gridlayout_add_widget2(layout, url, 3, 0);
    q_gridlayout_add_widget2(layout, url_edit, 3, 1);
    q_gridlayout_add_widget2(layout, lic, 4, 0);
    q_gridlayout_add_widget2(layout, lic_edit, 4, 1);

    q_widget_set_layout(widget, layout);
    q_widget_show(widget);

    int result = q_application_exec();

    q_widget_delete(widget);
    k_attica__project_delete(project);
    q_application_delete(qapp);

    return result;
}
