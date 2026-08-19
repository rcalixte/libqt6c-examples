#include <libqt6c.h>

#define LINE_NUMBER_ROLE (QT_ITEMDATAROLE_USERROLE + 1)
#define INITIAL_MAP_CAPACITY 32
#define MAX_LINE_LENGTH 4096

typedef struct {
    QWidget* tab;
    QListWidget* outline;
    QTextEdit* text_area;
} AppTab;

typedef struct {
    QMainWindow* w;
    QTabWidget* tabs;
} AppWindow;

// Global state
static AppWindow app_window;
static struct {
    void** keys;
    AppTab** values;
    size_t capacity;
    size_t size;
} app_tab_map = {0};

// Map functions
static void map_init(size_t initial_capacity) {
    app_tab_map.keys = (void**)malloc(initial_capacity * sizeof(void*));
    if (!app_tab_map.keys) {
        fprintf(stderr, "Failed to allocate memory for app_tab_map.keys\n");
        abort();
    }
    app_tab_map.values = (AppTab**)malloc(initial_capacity * sizeof(AppTab*));
    if (!app_tab_map.values) {
        fprintf(stderr, "Failed to allocate memory for app_tab_map.values\n");
        abort();
    }
    app_tab_map.capacity = initial_capacity;
    app_tab_map.size = 0;
}

static void map_put(void* key, AppTab* value) {
    if (app_tab_map.size >= app_tab_map.capacity) {
        size_t new_capacity = app_tab_map.capacity * 2;
        app_tab_map.keys = (void**)realloc(app_tab_map.keys, new_capacity * sizeof(void*));
        app_tab_map.values = (AppTab**)realloc(app_tab_map.values, new_capacity * sizeof(AppTab*));
        app_tab_map.capacity = new_capacity;
    }
    app_tab_map.keys[app_tab_map.size] = key;
    app_tab_map.values[app_tab_map.size] = value;
    app_tab_map.size++;
}

static AppTab* map_get(void* key) {
    for (size_t i = 0; i < app_tab_map.size; i++)
        if (app_tab_map.keys[i] == key)
            return app_tab_map.values[i];
    return NULL;
}

static void map_remove(void* key) {
    for (size_t i = 0; i < app_tab_map.size; i++)
        if (app_tab_map.keys[i] == key) {
            app_tab_map.keys[i] = app_tab_map.keys[app_tab_map.size - 1];
            app_tab_map.values[i] = app_tab_map.values[app_tab_map.size - 1];
            app_tab_map.size--;
            return;
        }
}

static void map_cleanup() {
    free(app_tab_map.keys);
    free(app_tab_map.values);
    app_tab_map.keys = NULL;
    app_tab_map.values = NULL;
    app_tab_map.size = 0;
    app_tab_map.capacity = 0;
}

// AppTab methods
static void handle_jump_to_bookmark(void* self, void* current, void* previous UNUSED) {
    AppTab* tab = map_get(self);
    if (!tab || !current)
        return;

    QVariant* line_number_variant = q_listwidgetitem_data(current, LINE_NUMBER_ROLE);
    int line_number = q_variant_to_int(line_number_variant);
    q_variant_delete(line_number_variant);

    QTextDocument* doc = q_textedit_document(tab->text_area);
    QTextBlock* block = q_textdocument_find_block_by_line_number(doc, line_number);
    QTextCursor* cursor = q_textcursor_new4(block);

    q_textcursor_set_position(cursor, q_textblock_position(block));
    q_textedit_set_text_cursor(tab->text_area, cursor);
    q_textedit_set_focus(tab->text_area);

    q_textblock_delete(block);
    q_textcursor_delete(cursor);
}

static void update_outline_for_content(const AppTab* tab, const char* content) {
    q_listwidget_clear(tab->outline);

    char line[MAX_LINE_LENGTH];
    char prev_line[MAX_LINE_LENGTH];
    char tooltip[32];
    bool in_code_block = false;
    int line_number = 0;

    while (*content) {
        size_t i = 0;
        while (*content && *content != '\n' && i < MAX_LINE_LENGTH - 1)
            line[i++] = *content++;
        line[i] = '\0';
        if (*content == '\n')
            content++;

        if (!in_code_block) {
            if (line[0] == '#') {
                QListWidgetItem* bookmark = q_listwidgetitem_new7(line, tab->outline);
                snprintf(tooltip, sizeof(tooltip), "Line %d", line_number + 1);
                q_listwidgetitem_set_tool_tip(bookmark, tooltip);

                QVariant* line_num = q_variant_new4(line_number);
                q_listwidgetitem_set_data(bookmark, LINE_NUMBER_ROLE, line_num);
                q_variant_delete(line_num);
            } else if (((strncmp(line, "---", 3) == 0) ||
                        (strncmp(line, "===", 3) == 0)) &&
                       prev_line[0] != '\0') {
                QListWidgetItem* bookmark = q_listwidgetitem_new7(prev_line, tab->outline);
                snprintf(tooltip, sizeof(tooltip), "Line %d", line_number);
                q_listwidgetitem_set_tool_tip(bookmark, tooltip);

                QVariant* line_num = q_variant_new4(line_number - 1);
                q_listwidgetitem_set_data(bookmark, LINE_NUMBER_ROLE, line_num);
                q_variant_delete(line_num);
            }
        }

        if (strncmp(line, "```", 3) == 0)
            in_code_block = !in_code_block;

        strncpy(prev_line, line, sizeof(prev_line));
        line_number++;
    }
}

static void handle_text_changed(void* self) {
    AppTab* tab = map_get(self);
    if (!tab)
        return;

    const char* content = q_textedit_to_plain_text(self);
    if (!content)
        return;

    update_outline_for_content(tab, content);
    libqt_free(content);
}

void initialize_app_tab(AppTab* tab) {
    tab->tab = q_widget_new2();
    QHBoxLayout* layout = q_hboxlayout_new(tab->tab);

    QSplitter* panes = q_splitter_new2();
    q_hboxlayout_add_widget(layout, panes);

    tab->outline = q_listwidget_new(tab->tab);
    q_splitter_add_widget(panes, tab->outline);
    q_listwidget_on_current_item_changed(tab->outline, handle_jump_to_bookmark);

    tab->text_area = q_textedit_new(tab->tab);
    map_put(tab->text_area, tab);
    map_put(tab->outline, tab);

    q_textedit_on_text_changed(tab->text_area, handle_text_changed);
    q_splitter_add_widget(panes, tab->text_area);

    int size_list[] = {250, 550};
    libqt_list sizes = {
        .len = 2,
        .data.ints = size_list,
    };
    q_splitter_set_sizes(panes, sizes);
}

// AppWindow methods
static void handle_tab_close(void* self, int index) {
    // Get the widget at this index before removing it
    QWidget* widget = q_tabwidget_widget(self, index);
    if (!widget)
        return;

    // Remove the tab from the tab widget
    q_tabwidget_remove_tab(self, index);

    // Find and remove the AppTab instance
    for (size_t i = 0; i < app_tab_map.size; i++) {
        AppTab* tab = app_tab_map.values[i];
        if (tab && tab->tab == widget) {
            // Remove from map before freeing
            if (tab->text_area)
                map_remove(tab->text_area);
            if (tab->outline)
                map_remove(tab->outline);
            q_widget_delete(tab->tab);
            free(tab);
            break;
        }
    }
}

static void handle_close_current_tab(void* self UNUSED) {
    int current_index = q_tabwidget_current_index(app_window.tabs);
    if (current_index >= 0)
        handle_tab_close(app_window.tabs, current_index);
}

static void create_tab_with_contents(const char* title, const char* content) {
    AppTab tab;
    initialize_app_tab(&tab);
    // the new tab is cleaned up during handle_tab_close

    q_textedit_set_text(tab.text_area, content);

    QIcon* icon = q_icon_from_theme("text-markdown");
    int idx = q_tabwidget_add_tab2(app_window.tabs, tab.tab, icon, title);
    q_icon_delete(icon);

    q_tabwidget_set_current_index(app_window.tabs, idx);
}

static void handle_new_tab(void* self UNUSED) {
    create_tab_with_contents("New Document", "");
}

static void handle_file_open(void* self UNUSED) {
    const char* fname = q_filedialog_get_open_file_name4(
        app_window.w,
        "Open markdown file...",
        "",
        "Markdown files (*.md *.txt);;All Files (*)");
    if (!fname)
        return;

    FILE* file = fopen(fname, "r");
    if (!file) {
        libqt_free(fname);
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* content = (char*)malloc(size + 1);
    if (!content) {
        fclose(file);
        libqt_free(fname);
        return;
    }

    size_t read = fread(content, 1, size, file);
    content[read] = '\0';
    fclose(file);

    // Get basename of file
    const char* basename = strrchr(fname, '/');
    basename = basename ? basename + 1 : fname;

    create_tab_with_contents(basename, content);

    free(content);
    libqt_free(fname);
}

static void handle_exit(void* self UNUSED) {
    q_application_quit();
}

static void handle_about(void* self UNUSED) {
    q_application_about_qt();
}

void initialize_app_window() {
    app_window.w = q_mainwindow_new2();
    q_mainwindow_set_window_title(app_window.w, "Markdown Outliner");

    // Menu setup
    QMenuBar* menubar = q_menubar_new2();

    // File menu
    QMenu* file_menu = q_menubar_add_menu2(menubar, "&File");

    QAction* new_action = q_menubar_add_action2(file_menu, "New Tab");
    QKeySequence* new_shortcut = q_keysequence_new2("Ctrl+N");
    q_action_set_shortcut(new_action, new_shortcut);
    q_keysequence_delete(new_shortcut);
    QIcon* new_icon = q_icon_from_theme("document-new");
    q_action_set_icon(new_action, new_icon);
    q_icon_delete(new_icon);
    q_action_on_triggered(new_action, handle_new_tab);

    QAction* open_action = q_menubar_add_action2(file_menu, "Open...");
    QKeySequence* open_shortcut = q_keysequence_new2("Ctrl+O");
    q_action_set_shortcut(open_action, open_shortcut);
    q_keysequence_delete(open_shortcut);
    QIcon* open_icon = q_icon_from_theme("document-open");
    q_action_set_icon(open_action, open_icon);
    q_icon_delete(open_icon);
    q_action_on_triggered(open_action, handle_file_open);

    q_menubar_add_separator(file_menu);

    QAction* exit_action = q_menubar_add_action2(file_menu, "Exit");
    QKeySequence* exit_shortcut = q_keysequence_new2("Ctrl+Q");
    q_action_set_shortcut(exit_action, exit_shortcut);
    q_keysequence_delete(exit_shortcut);
    QIcon* exit_icon = q_icon_from_theme("application-exit");
    q_action_set_icon(exit_action, exit_icon);
    q_icon_delete(exit_icon);
    q_action_on_triggered(exit_action, handle_exit);

    // Help menu
    QMenu* help_menu = q_menubar_add_menu2(menubar, "&Help");
    QAction* about_action = q_menubar_add_action2(help_menu, "About Qt");
    QIcon* about_icon = q_icon_from_theme("help-about");
    q_action_set_icon(about_action, about_icon);
    q_icon_delete(about_icon);
    QKeySequence* about_shortcut = q_keysequence_new2("F1");
    q_action_set_shortcut(about_action, about_shortcut);
    q_keysequence_delete(about_shortcut);
    q_action_on_triggered(about_action, handle_about);

    q_mainwindow_set_menu_bar(app_window.w, menubar);

    // Ctrl+W shortcut
    QKeySequence* close_shortcut = q_keysequence_new2("Ctrl+W");
    QAction* close_action = q_mainwindow_add_action4(app_window.w, "Ctrl+W", close_shortcut);
    q_action_set_shortcut(close_action, close_shortcut);
    q_action_on_triggered(close_action, handle_close_current_tab);
    q_keysequence_delete(close_shortcut);

    // Main widgets
    app_window.tabs = q_tabwidget_new(app_window.w);
    q_tabwidget_set_tabs_closable(app_window.tabs, true);
    q_tabwidget_set_movable(app_window.tabs, true);
    q_tabwidget_on_tab_close_requested(app_window.tabs, handle_tab_close);
    q_mainwindow_set_central_widget(app_window.w, app_window.tabs);

    // Add initial tab
    const char* readme = "# Welcome\n\n## This is a basic markdown editor.\n\n"
                         "- You can use the outline on the left to jump to parts of the document\n"
                         "- Demonstrates use of the Qt library bindings for C\n";
    create_tab_with_contents("README.md", readme);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    map_init(INITIAL_MAP_CAPACITY);

    initialize_app_window();

    q_mainwindow_show(app_window.w);

    int result = q_application_exec();

    // Cleanup
    while (q_tabwidget_count(app_window.tabs) > 0)
        handle_tab_close(app_window.tabs, 0);
    map_cleanup();
    q_mainwindow_delete(app_window.w);

    q_application_delete(qapp);

    return result;
}
