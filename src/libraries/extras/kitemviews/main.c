#include <libqt6c.h>

static QDialog* dialog;
static QTreeWidget* treewidget;
static KTreeWidgetSearchLine* m_searchline;

static void switch_case_sensitivity(void* UNUSED self, bool checked) {
    k_treewidgetsearchline_set_case_sensitivity(m_searchline, checked ? QT_CASESENSITIVITY_CASESENSITIVE : QT_CASESENSITIVITY_CASEINSENSITIVE);
}

static void switch_keep_parents_visible(void* UNUSED self, bool checked) {
    k_treewidgetsearchline_set_keep_parents_visible(m_searchline, checked);
}

static void on_accepted() {
    q_dialog_accept(dialog);
}

static void on_rejected() {
    q_dialog_reject(dialog);
}

static void show_event(void* self, void* event) {
    q_dialog_qbase_show_event(self, event);

    QHeaderView* headerview = q_treewidget_header(treewidget);
    for (int i = 0; i < q_headerview_count(headerview); i++) {
        if (!q_headerview_is_section_hidden(headerview, i)) {
            q_treewidget_resize_column_to_contents(treewidget, i);
        }
    }
}

static void create3rdLevel(QTreeWidgetItem* item) {
    const char* growing[] = {"Growing", "$2.00", "", "Farmer", NULL};
    q_treewidgetitem_new7(item, growing);
    const char* ripe[] = {"Ripe", "$8.00", "", "Market", NULL};
    q_treewidgetitem_new7(item, ripe);
    const char* decaying[] = {"Decaying", "$0.50", "", "Ground", NULL};
    q_treewidgetitem_new7(item, decaying);
    const char* pickled[] = {"Pickled", "$4.00", "", "Shop", NULL};
    q_treewidgetitem_new7(item, pickled);
}

static void create2ndLevel(QTreeWidgetItem* item) {
    const char* beans_s[] = {"Beans", NULL};
    QTreeWidgetItem* beans = q_treewidgetitem_new7(item, beans_s);
    q_treewidget_expand_item(treewidget, beans);
    create3rdLevel(beans);

    const char* grapes_s[] = {"Grapes", NULL};
    QTreeWidgetItem* grapes = q_treewidgetitem_new7(item, grapes_s);
    q_treewidget_expand_item(treewidget, grapes);
    create3rdLevel(grapes);

    const char* plums_s[] = {"Plums", NULL};
    QTreeWidgetItem* plums = q_treewidgetitem_new7(item, plums_s);
    q_treewidget_expand_item(treewidget, plums);
    create3rdLevel(plums);

    const char* bananas_s[] = {"Bananas", NULL};
    QTreeWidgetItem* bananas = q_treewidgetitem_new7(item, bananas_s);
    q_treewidget_expand_item(treewidget, bananas);
    create3rdLevel(bananas);
}

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);

    dialog = q_dialog_new2();

    q_dialog_set_window_title(dialog, "Qt 6 KItemViews");
    q_dialog_set_whats_this(dialog, "This is a test dialog for KTreeWidgetSearchLineTest");

    treewidget = q_treewidget_new(dialog);
    q_treewidget_set_column_count(treewidget, 4);
    const char* labels[] = {"Item", "Price", "HIDDEN COLUMN", "Source", NULL};
    q_treewidget_set_header_labels(treewidget, labels);
    q_treewidget_hide_column(treewidget, 2);

    KTreeWidgetSearchLineWidget* searchwidget = k_treewidgetsearchlinewidget_new3(dialog, treewidget);
    m_searchline = k_treewidgetsearchlinewidget_search_line(searchwidget);

    const char* red_s[] = {"Red", NULL};
    QTreeWidgetItem* red = q_treewidgetitem_new4(treewidget, red_s);
    q_treewidgetitem_set_whats_this(red, 0, "This item is red");
    q_treewidgetitem_set_whats_this(red, 1, "This item is pricey");
    q_treewidget_expand_item(treewidget, red);

    const char* blue_s[] = {"Blue", NULL};
    QTreeWidgetItem* blue = q_treewidgetitem_new4(treewidget, blue_s);
    q_treewidget_expand_item(treewidget, blue);

    const char* green_s[] = {"Green", NULL};
    QTreeWidgetItem* green = q_treewidgetitem_new4(treewidget, green_s);
    q_treewidget_expand_item(treewidget, green);

    const char* yellow_s[] = {"Yellow", NULL};
    QTreeWidgetItem* yellow = q_treewidgetitem_new4(treewidget, yellow_s);
    q_treewidget_expand_item(treewidget, yellow);

    create2ndLevel(red);
    create2ndLevel(blue);
    create2ndLevel(green);
    create2ndLevel(yellow);

    QVBoxLayout* vboxlayout = q_vboxlayout_new(dialog);
    QHBoxLayout* hboxlayout = q_hboxlayout_new2();

    QPushButton* case_sensitive = q_pushbutton_new5("&Case Sensitive", dialog);
    q_hboxlayout_add_widget(hboxlayout, case_sensitive);

    q_pushbutton_set_checkable(case_sensitive, true);
    q_pushbutton_on_toggled(case_sensitive, switch_case_sensitivity);

    QPushButton* keep_parents_visible = q_pushbutton_new5("Keep &Parents Visible", dialog);
    q_hboxlayout_add_widget(hboxlayout, keep_parents_visible);

    q_pushbutton_set_checkable(keep_parents_visible, true);
    q_pushbutton_set_checked(keep_parents_visible, true);
    q_pushbutton_on_toggled(keep_parents_visible, switch_keep_parents_visible);

    QDialogButtonBox* buttonbox = q_dialogbuttonbox_new(dialog);
    q_dialogbuttonbox_set_standard_buttons(buttonbox, QDIALOGBUTTONBOX_STANDARDBUTTON_OK | QDIALOGBUTTONBOX_STANDARDBUTTON_CANCEL);

    q_dialogbuttonbox_on_accepted(buttonbox, on_accepted);
    q_dialogbuttonbox_on_rejected(buttonbox, on_rejected);

    q_vboxlayout_add_widget(vboxlayout, searchwidget);
    q_vboxlayout_add_widget(vboxlayout, treewidget);
    q_vboxlayout_add_layout(vboxlayout, hboxlayout);
    q_vboxlayout_add_widget(vboxlayout, buttonbox);

    k_treewidgetsearchline_set_focus(m_searchline);
    q_dialog_resize(dialog, 350, 600);
    q_dialog_on_show_event(dialog, show_event);

    q_dialog_exec(dialog);
}
