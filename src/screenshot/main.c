#include <libqt6c.h>

const char* format = "png";
const char* mid_path = "/untitled.";

static QWidget* screenshot = NULL;
static QLabel* screenshot_label = NULL;
static QSpinBox* delay_spinbox = NULL;
static QCheckBox* hide_checkbox = NULL;
static QPushButton* new_button = NULL;
static QPixmap* original_pixmap = NULL;

void update_screenshot_label() {
    QSize* size = q_label_size(screenshot_label);
    QPixmap* pixmap = q_pixmap_scaled32(original_pixmap,
                                        size,
                                        QT_ASPECTRATIOMODE_KEEPASPECTRATIO,
                                        QT_TRANSFORMATIONMODE_SMOOTHTRANSFORMATION);

    q_label_set_pixmap(screenshot_label, pixmap);

    q_pixmap_delete(pixmap);
    q_size_delete(size);
}

void on_resize_event(void* self UNUSED, void* event UNUSED) {
    QSize* scaled_size = q_pixmap_size(original_pixmap);
    QSize* label_size = q_label_size(screenshot_label);

    q_size_scale2(scaled_size, label_size, QT_ASPECTRATIOMODE_KEEPASPECTRATIO);

    QPixmap* pixmap = q_label_pixmap2(screenshot_label);
    QSize* size = q_pixmap_size(pixmap);

    if (scaled_size != size)
        update_screenshot_label();

    q_size_delete(size);
    q_pixmap_delete(pixmap);
    q_size_delete(label_size);
    q_size_delete(scaled_size);
}

void shoot_screenshot() {
    QScreen* screen = q_application_primary_screen();
    QWindow* window = q_widget_window_handle(screenshot);

    if (window != NULL)
        screen = q_window_screen(window);

    if (screen == NULL)
        return;

    if (q_spinbox_value(delay_spinbox) != 0)
        q_application_beep();

    original_pixmap = q_screen_grab_window1(screen, 0);
    update_screenshot_label();

    q_pushbutton_set_disabled(new_button, false);
    if (q_checkbox_is_checked(hide_checkbox))
        q_widget_show(screenshot);
}

void on_quit(void* self UNUSED) {
    q_application_quit();
}

void save_screenshot(void* self UNUSED) {
    const char* initial_path = q_standardpaths_writable_location(QSTANDARDPATHS_STANDARDLOCATION_PICTURESLOCATION);

    if (initial_path == NULL) {
        libqt_free(initial_path);
        initial_path = q_dir_current_path();
    }

    char* out_path = (char*)malloc(strlen(initial_path) + strlen(mid_path) + strlen(format) + 1);
    sprintf(out_path, "%s%s%s", initial_path, mid_path, format);
    libqt_free(initial_path);

    QFileDialog* file_dialog = q_filedialog_new5(screenshot, "Save As", out_path);
    q_filedialog_set_accept_mode(file_dialog, QFILEDIALOG_ACCEPTMODE_ACCEPTSAVE);
    q_filedialog_set_file_mode(file_dialog, QFILEDIALOG_FILEMODE_ANYFILE);
    q_filedialog_set_directory(file_dialog, out_path);

    const char** mimetypes = q_imagewriter_supported_mime_types();
    q_filedialog_set_mime_type_filters(file_dialog, mimetypes);
    char filter_path[16];
    snprintf(filter_path, 16, "image/%s", format);
    q_filedialog_select_mime_type_filter(file_dialog, filter_path);
    q_filedialog_set_default_suffix(file_dialog, format);

    for (int i = 0; mimetypes[i] != NULL; i++)
        libqt_free(mimetypes[i]);
    free(mimetypes);
    free(out_path);

    if (q_filedialog_exec(file_dialog) != QDIALOG_DIALOGCODE_ACCEPTED)
        return;

    const char** selected_files = q_filedialog_selected_files(file_dialog);
    if (selected_files == NULL) {
        free(selected_files);
        return;
    }

    if (!q_pixmap_save(original_pixmap, selected_files[0])) {
        const char* save_path = q_dir_to_native_separators(selected_files[0]);
        char* error_str = "Failed to save screenshot to ";
        char* error_message = (char*)malloc(strlen(save_path) + strlen(error_str) + 1);
        sprintf(error_message, "%s%s", error_str, save_path);
        libqt_free(save_path);

        q_messagebox_warning(screenshot, "Save Error", error_message);
        free(error_message);
    }

    for (int i = 0; selected_files[i] != NULL; i++)
        libqt_free(selected_files[i]);
    free(selected_files);
}

void on_timeout(void* self UNUSED) {
    shoot_screenshot();
}

void new_screenshot(void* self UNUSED) {
    if (q_checkbox_is_checked(hide_checkbox))
        q_widget_hide(screenshot);

    QTimer* timer = q_timer_new2(screenshot);
    q_timer_set_single_shot(timer, true);
    q_timer_on_timeout(timer, on_timeout);
    q_timer_start3(timer, q_spinbox_value(delay_spinbox) * 1000);
}

void on_delay_changed(void* self UNUSED, int value) {
    if (value == 0) {
        q_checkbox_set_disabled(hide_checkbox, true);
        q_checkbox_set_checked(hide_checkbox, false);
    } else {
        q_checkbox_set_disabled(hide_checkbox, false);
    }
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    screenshot = q_widget_new2();
    q_widget_set_window_title(screenshot, "Qt 6 Screenshot Example");
    q_widget_set_minimum_size2(screenshot, 400, 300);

    screenshot_label = q_label_new5("Take a screenshot", screenshot);
    q_label_set_size_policy2(screenshot_label,
                             QSIZEPOLICY_POLICY_EXPANDING,
                             QSIZEPOLICY_POLICY_EXPANDING);
    q_label_set_alignment(screenshot_label, QT_ALIGNMENTFLAG_ALIGNCENTER);

    QScreen* screen = q_widget_screen(screenshot);
    QRect* rect = q_screen_geometry(screen);

    q_label_set_minimum_size2(screenshot_label,
                              q_rect_width(rect) / 8,
                              q_rect_height(rect) / 8);

    QVBoxLayout* main_layout = q_vboxlayout_new(screenshot);
    q_vboxlayout_add_widget(main_layout, screenshot_label);

    QGroupBox* options_groupbox = q_groupbox_new4("Options", screenshot);

    delay_spinbox = q_spinbox_new(options_groupbox);
    q_spinbox_set_suffix(delay_spinbox, " s");
    q_spinbox_set_maximum(delay_spinbox, 60);
    q_spinbox_set_minimum(delay_spinbox, 0);
    q_spinbox_on_value_changed(delay_spinbox, on_delay_changed);

    hide_checkbox = q_checkbox_new4("Hide This Window", options_groupbox);

    QGridLayout* options_layout = q_gridlayout_new(options_groupbox);
    q_gridlayout_add_widget2(options_layout,
                             q_label_new5("Screenshot Delay:", screenshot),
                             0, 0);
    q_gridlayout_add_widget2(options_layout, delay_spinbox, 0, 1);
    q_gridlayout_add_widget3(options_layout, hide_checkbox, 1, 0, 1, 2);

    q_vboxlayout_add_widget(main_layout, options_groupbox);

    new_button = q_pushbutton_new5("New Screenshot", screenshot);
    q_pushbutton_on_clicked(new_button, new_screenshot);

    QPushButton* save_button = q_pushbutton_new5("Save Screenshot", screenshot);
    q_pushbutton_on_clicked(save_button, save_screenshot);

    QKeySequence* quit_shortcut = q_keysequence_new3(QT_KEYBOARDMODIFIER_CONTROLMODIFIER | QT_KEY_Key_Q);

    QPushButton* quit_button = q_pushbutton_new5("Quit", screenshot);
    q_pushbutton_on_clicked(quit_button, on_quit);
    q_pushbutton_set_shortcut(quit_button, quit_shortcut);

    QHBoxLayout* buttons_layout = q_hboxlayout_new2();
    q_hboxlayout_add_widget(buttons_layout, new_button);
    q_hboxlayout_add_widget(buttons_layout, save_button);
    q_hboxlayout_add_widget(buttons_layout, quit_button);
    q_hboxlayout_add_stretch(buttons_layout);

    q_vboxlayout_add_layout(main_layout, buttons_layout);

    shoot_screenshot();

    q_spinbox_set_value(delay_spinbox, 5);

    QRect* available_geometry = q_screen_available_geometry(screen);
    QPoint* top_left_point = q_rect_top_left(available_geometry);
    QPoint* offset = q_point_new4(50, 50);
    q_point_operator_plus_assign(top_left_point, offset);

    q_widget_move2(screenshot, top_left_point);
    q_widget_on_resize_event(screenshot, on_resize_event);

    q_widget_show(screenshot);

    int result = q_application_exec();

    q_point_delete(offset);
    q_point_delete(top_left_point);
    q_rect_delete(available_geometry);
    q_keysequence_delete(quit_shortcut);
    q_rect_delete(rect);
    q_widget_delete(screenshot);
    q_application_delete(qapp);

    return result;
}
