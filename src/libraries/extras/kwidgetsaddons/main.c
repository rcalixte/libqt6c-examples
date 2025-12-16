#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    KGuiItem* primaryAction = k_guiitem_new7("Hello", "view-filter", "This is a tooltip", "This is a WhatsThis help text.");

    KGuiItem* secondaryAction = k_guiitem_new2("Bye");

    int32_t res = k_messagebox_question_two_actions(NULL, "Description to tell you to click<br />on <b>either</b> button", "Qt 6 KMessageBox Example", primaryAction, secondaryAction, "", KMESSAGEBOX_OPTION_NOTIFY);

    if (res == KMESSAGEBOX_BUTTONCODE_PRIMARYACTION) {
        printf("You clicked Hello\n");
    } else {
        printf("You clicked Bye\n");
    }

    q_application_delete(qapp);

    return 0;
}
