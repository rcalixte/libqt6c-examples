#include <libqt6c.h>

static QDataWidgetMapper* mapper;
static QPushButton* next_button;
static QPushButton* previous_button;
static QSqlRelationalTableModel* model;

void to_previous() {
    q_datawidgetmapper_to_previous(mapper);
}

void to_next() {
    q_datawidgetmapper_to_next(mapper);
}

void update_buttons(void* UNUSED self, int index) {
    q_pushbutton_set_enabled(previous_button, index > 0);
    QModelIndex* model_index = q_modelindex_new3();
    q_pushbutton_set_enabled(next_button, index < q_sqlrelationaltablemodel_row_count(model, model_index) - 1);
    q_modelindex_delete(model_index);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWidget* widget = q_widget_new2();

    QSqlDatabase* db = q_sqldatabase_add_database("QSQLITE");
    q_sqldatabase_set_database_name(db, ":memory:");
    if (!q_sqldatabase_open(db)) {
        q_messagebox_critical42(
            widget,
            "Cannot open database",
            "Unable to establish a database connection.\nThis example needs SQLite support. Please read the Qt SQL driver documentation for information on how to build it.",
            QMESSAGEBOX_STANDARDBUTTON_CANCEL);

        q_widget_delete(widget);
        q_sqldatabase_delete(db);
        exit(1);
    }

    // Setup the main table
    QSqlQuery* query = q_sqlquery_new2();
    q_sqlquery_exec(query, "create table person (id int primary key, name varchar(20), address varchar(200), typeid int)");
    q_sqlquery_exec(query, "insert into person values(1, 'Alice', '<qt>123 Main Street<br/>Market Town</qt>', 101)");
    q_sqlquery_exec(query, "insert into person values(2, 'Bob', '<qt>PO Box 32<br/>Mail Handling Service<br/>Service City</qt>', 102)");
    q_sqlquery_exec(query, "insert into person values(3, 'Carol', '<qt>The Lighthouse<br/>Remote Island</qt>', 103)");
    q_sqlquery_exec(query, "insert into person values(4, 'Donald', '<qt>47338 Park Avenue<br/>Big City</qt>', 101)");
    q_sqlquery_exec(query, "insert into person values(5, 'Emma', '<qt>Research Station<br/>Base Camp<br/>Big Mountain</qt>', 103)");

    // Setup the address table
    q_sqlquery_exec(query, "create table addresstype (id int, description varchar(20))");
    q_sqlquery_exec(query, "insert into addresstype values(101, 'Home')");
    q_sqlquery_exec(query, "insert into addresstype values(102, 'Work')");
    q_sqlquery_exec(query, "insert into addresstype values(103, 'Other')");

    model = q_sqlrelationaltablemodel_new2(widget);
    q_sqlrelationaltablemodel_set_table(model, "person");
    q_sqlrelationaltablemodel_set_edit_strategy(model, QSQLTABLEMODEL_EDITSTRATEGY_ONMANUALSUBMIT);

    int type_index = q_sqlrelationaltablemodel_field_index(model, "typeid");
    QSqlRelation* relation = q_sqlrelation_new2("addresstype", "id", "description");
    q_sqlrelationaltablemodel_set_relation(model, type_index, relation);

    q_sqlrelationaltablemodel_select(model);

    // Ownership of these widgets will be transferred to the widget via the layout
    QLabel* name_label = q_label_new3("Na&me:");
    QLineEdit* name_edit = q_lineedit_new2();
    QLabel* address_label = q_label_new3("&Address:");
    QTextEdit* address_edit = q_textedit_new2();
    QLabel* type_label = q_label_new3("&Type:");
    QComboBox* type_combobox = q_combobox_new2();
    next_button = q_pushbutton_new3("&Next");
    previous_button = q_pushbutton_new3("&Previous");

    q_label_set_buddy(name_label, name_edit);
    q_label_set_buddy(address_label, address_edit);
    q_label_set_buddy(type_label, type_combobox);

    QSqlTableModel* rel_model = q_sqlrelationaltablemodel_relation_model(model, type_index);
    q_combobox_set_model(type_combobox, rel_model);
    q_combobox_set_model_column(type_combobox, q_sqltablemodel_field_index(rel_model, "description"));

    mapper = q_datawidgetmapper_new2(widget);
    q_datawidgetmapper_set_model(mapper, model);
    QStyledItemDelegate* relational_delegate = q_styleditemdelegate_new2(mapper);
    q_datawidgetmapper_set_item_delegate(mapper, relational_delegate);
    q_datawidgetmapper_add_mapping(mapper, name_edit, q_sqlrelationaltablemodel_field_index(model, "name"));
    q_datawidgetmapper_add_mapping(mapper, address_edit, q_sqlrelationaltablemodel_field_index(model, "address"));
    q_datawidgetmapper_add_mapping(mapper, type_combobox, type_index);

    q_pushbutton_on_clicked(previous_button, to_previous);
    q_pushbutton_on_clicked(next_button, to_next);
    q_datawidgetmapper_on_current_index_changed(mapper, update_buttons);

    QGridLayout* layout = q_gridlayout_new2();
    q_gridlayout_add_widget2(layout, name_label, 0, 0);
    q_gridlayout_add_widget2(layout, name_edit, 0, 1);
    q_gridlayout_add_widget2(layout, previous_button, 0, 2);
    q_gridlayout_add_widget2(layout, address_label, 1, 0);
    q_gridlayout_add_widget3(layout, address_edit, 1, 1, 2, 1);
    q_gridlayout_add_widget2(layout, next_button, 1, 2);
    q_gridlayout_add_widget2(layout, type_label, 3, 0);
    q_gridlayout_add_widget2(layout, type_combobox, 3, 1);
    q_widget_set_layout(widget, layout);

    q_widget_set_window_title(widget, "Qt 6 SQL Widget Mapper");
    q_datawidgetmapper_to_first(mapper);

    q_widget_show(widget);

    int result = q_application_exec();

    q_sqlrelation_delete(relation);
    q_sqlquery_delete(query);
    q_sqldatabase_delete(db);
    q_widget_delete(widget);
    q_application_delete(qapp);

    return result;
}
