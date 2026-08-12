#include <libqt6c.h>

QWizardPage* create_intro_page() {
    QWizardPage* page = q_wizardpage_new2();
    q_wizardpage_set_title(page, "Introduction");

    const char* text = "This wizard will help you register your copy of Super Product Two";
    QLabel* label = q_label_new5(text, page);
    q_label_set_word_wrap(label, true);

    QVBoxLayout* layout = q_vboxlayout_new2();
    q_vboxlayout_add_widget(layout, label);
    q_wizardpage_set_layout(page, layout);

    return page;
}

QWizardPage* create_registration_page() {
    QWizardPage* page = q_wizardpage_new2();
    q_wizardpage_set_title(page, "Registration");
    q_wizardpage_set_sub_title(page, "Please fill both fields");

    QGridLayout* layout = q_gridlayout_new(page);
    q_gridlayout_add_widget2(layout, q_label_new5("Name:", page), 0, 0);
    q_gridlayout_add_widget2(layout, q_lineedit_new(page), 0, 1);
    q_gridlayout_add_widget2(layout, q_label_new5("Email address:", page), 1, 0);
    q_gridlayout_add_widget2(layout, q_lineedit_new(page), 1, 1);
    q_wizardpage_set_layout(page, layout);

    return page;
}

QWizardPage* create_conclusion_page() {
    QWizardPage* page = q_wizardpage_new2();
    q_wizardpage_set_title(page, "Conclusion");

    const char* success = "You are now successfully registered. Have a nice day!";
    QLabel* label = q_label_new5(success, page);
    q_label_set_word_wrap(label, true);

    QVBoxLayout* layout = q_vboxlayout_new2();
    q_vboxlayout_add_widget(layout, label);
    q_wizardpage_set_layout(page, layout);

    return page;
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWizard* wizard = q_wizard_new2();

    QWizardPage* intro_page = create_intro_page();
    q_wizard_add_page(wizard, intro_page);

    QWizardPage* registration_page = create_registration_page();
    q_wizard_add_page(wizard, registration_page);

    QWizardPage* conclusion_page = create_conclusion_page();
    q_wizard_add_page(wizard, conclusion_page);

    q_wizard_set_window_title(wizard, "Qt 6 Trivial Wizard Example");
    q_wizard_show(wizard);

    int ret = q_application_exec();

    q_wizard_delete(wizard);
    q_application_delete(qapp);

    return ret;
}
