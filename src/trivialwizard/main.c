#include <libqt6c.h>
#include <stdint.h>

QWizardPage* createIntroPage() {
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

QWizardPage* createRegistrationPage() {
    QWizardPage* page = q_wizardpage_new2();

    const char* title = "Registration";
    q_wizardpage_set_title(page, title);

    const char* subtitle = "Please fill both fields";
    q_wizardpage_set_sub_title(page, subtitle);

    const char* name = "Name:";
    QLabel* nameLabel = q_label_new5(name, page);
    QLineEdit* nameLineEdit = q_lineedit_new(page);

    const char* email = "Email address:";
    QLabel* emailLabel = q_label_new5(email, page);
    QLineEdit* emailLineEdit = q_lineedit_new(page);

    QGridLayout* layout = q_gridlayout_new(page);
    q_gridlayout_add_widget2(layout, nameLabel, 0, 0);
    q_gridlayout_add_widget2(layout, nameLineEdit, 0, 1);
    q_gridlayout_add_widget2(layout, emailLabel, 1, 0);
    q_gridlayout_add_widget2(layout, emailLineEdit, 1, 1);
    q_wizardpage_set_layout(page, layout);

    return page;
}

QWizardPage* createConclusionPage() {
    QWizardPage* page = q_wizardpage_new2();

    const char* title = "Conclusion";
    q_wizardpage_set_title(page, title);

    const char* success = "You are now successfully registered. Have a nice day!";
    QLabel* label = q_label_new5(success, page);
    q_label_set_word_wrap(label, true);

    QVBoxLayout* layout = q_vboxlayout_new2();
    q_vboxlayout_add_widget(layout, label);
    q_wizardpage_set_layout(page, layout);

    return page;
}

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);

    QWizard* wizard = q_wizard_new2();

    QWizardPage* intro_page = createIntroPage();
    q_wizard_add_page(wizard, intro_page);

    QWizardPage* registration_page = createRegistrationPage();
    q_wizard_add_page(wizard, registration_page);

    QWizardPage* conclusion_page = createConclusionPage();
    q_wizard_add_page(wizard, conclusion_page);

    const char* title = "Trivial Wizard";
    q_wizard_set_window_title(wizard, title);
    q_wizard_show(wizard);

    int ret = q_application_exec();

    q_wizard_delete_later(wizard);
    return ret;
}
