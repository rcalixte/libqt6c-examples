#include <libqt6c.h>

static float zoom_in_scale = 1.25;
static float zoom_out_scale = 0.8;
static int dX = 32;
static int dY = 64;
static int replacement_r = 255;
static int replacement_g = 255;
static int replacement_b = 255;

#define MAX_BUFFER_SIZE 64
static char buffer[MAX_BUFFER_SIZE];

static QStatusBar* status_bar = NULL;
static QGraphicsScene* scene = NULL;
static QGraphicsView* view = NULL;

void scene_key_press_event(void* self UNUSED, void* event) {
    int key = q_keyevent_key(event);
    switch (key) {
    case QT_KEY_Key_0:
        q_graphicsview_scale(view, zoom_in_scale, zoom_in_scale);
        break;
    case QT_KEY_Key_9:
        q_graphicsview_scale(view, zoom_out_scale, zoom_out_scale);
        break;
    }
}

void scene_wheel_event(void* self UNUSED, void* event) {
    if ((q_application_query_keyboard_modifiers() & QT_KEYBOARDMODIFIER_SHIFTMODIFIER) != 0) {
        if (q_graphicsscenewheelevent_delta(event) > 0)
            q_graphicsview_scale(view, zoom_in_scale, zoom_in_scale);
        else
            q_graphicsview_scale(view, zoom_out_scale, zoom_out_scale);
    }
}

void view_resize_event(void* self, void* event UNUSED) {
    QRectF* rect = q_graphicsscene_items_bounding_rect(scene);
    q_graphicsview_fit_in_view22(self, rect, QT_ASPECTRATIOMODE_KEEPASPECTRATIO);

    q_rectf_delete(rect);
}

void draw_pixel(QGraphicsPixmapItem* item, QPointF* pos) {
    int x = (int)q_pointf_x(pos);
    int y = (int)q_pointf_y(pos);

    QPixmap* pm = q_graphicspixmapitem_pixmap(item);
    QImage* img = q_pixmap_to_image(pm);
    QColor* color = q_color_new13(replacement_r, replacement_g, replacement_b, 255);

    int height = q_image_height(img);
    int width = q_image_width(img);

    if (x < 0 || y < 0 || x >= width || y >= height)
        return;

    snprintf(buffer, MAX_BUFFER_SIZE, "x: %d, y: %d, r: %d, g: %d, b: %d", x, y, replacement_r, replacement_g, replacement_b);
    q_statusbar_show_message(status_bar, buffer);

    q_image_set_pixel_color(img, x, y, color);

    QPixmap* pm2 = q_pixmap_from_image(img);
    q_graphicspixmapitem_set_pixmap(item, pm2);

    q_pixmap_delete(pm2);
    q_color_delete(color);
    q_image_delete(img);
    q_pixmap_delete(pm);
}

void item_mouse_event(void* self, void* event) {
    QPointF* pos = q_graphicsscenemouseevent_pos(event);

    draw_pixel(self, pos);

    q_pointf_delete(pos);
}

void item_hover_move_event(void* self, void* event) {
    QPointF* pos = q_graphicsscenehoverevent_pos(event);

    int x = (int)q_pointf_x(pos);
    int y = (int)q_pointf_y(pos);

    QPixmap* pm = q_graphicspixmapitem_pixmap(self);
    QImage* img = q_pixmap_to_image(pm);

    int height = q_image_height(img);
    int width = q_image_width(img);

    if (x < 0 || y < 0 || x >= width || y >= height)
        return;

    QColor* color_value = q_image_pixel_color(img, x, y);

    int r = q_color_red(color_value);
    int g = q_color_green(color_value);
    int b = q_color_blue(color_value);

    snprintf(buffer, MAX_BUFFER_SIZE, "x: %d, y: %d, r: %d, g: %d, b: %d", x, y, r, g, b);
    q_statusbar_show_message(status_bar, buffer);

    q_color_delete(color_value);
    q_image_delete(img);
    q_pixmap_delete(pm);
    q_pointf_delete(pos);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QMainWindow* main_window = q_mainwindow_new2();
    q_mainwindow_set_window_title(main_window, "Qt 6 Pixel Editor Example");
    q_mainwindow_resize(main_window, 490, 520);
    q_mainwindow_set_minimum_size2(main_window, 360, 450);

    status_bar = q_statusbar_new(main_window);
    q_mainwindow_set_status_bar(main_window, status_bar);

    scene = q_graphicsscene_new();
    view = q_graphicsview_new2();

    q_graphicsscene_on_key_press_event(scene, scene_key_press_event);
    q_graphicsscene_on_wheel_event(scene, scene_wheel_event);
    q_graphicsview_on_resize_event(view, view_resize_event);

    QImage* image = q_image_new3(dX, dY, QIMAGE_FORMAT_FORMAT_ARGB32);

    for (int x = 0; x < dX; x++)
        for (int y = 0; y < dY; y++) {
            QColor* color = q_color_new13(x, y * 3, x * 4, 255);
            q_image_set_pixel_color(image, x, y, color);
            q_color_delete(color);
        }

    QPixmap* pixmap = q_pixmap_from_image(image);
    QGraphicsPixmapItem* item = q_graphicspixmapitem_new2(pixmap);

    q_graphicspixmapitem_set_accept_hover_events(item, true);

    q_graphicspixmapitem_on_mouse_move_event(item, item_mouse_event);
    q_graphicspixmapitem_on_mouse_press_event(item, item_mouse_event);
    q_graphicspixmapitem_on_hover_move_event(item, item_hover_move_event);

    q_graphicsscene_add_item(scene, item);
    q_graphicsview_set_scene(view, scene);
    q_graphicsview_show(view);

    q_statusbar_show_message(status_bar, "Click and drag to draw a pixel."
                                         " Use Shift+scroll or keys 0 or 9 to zoom in or out.");

    q_mainwindow_set_central_widget(main_window, view);
    q_mainwindow_show(main_window);

    int result = q_application_exec();

    q_graphicspixmapitem_delete(item);
    q_pixmap_delete(pixmap);
    q_image_delete(image);
    q_graphicsview_delete(view);
    q_graphicsscene_delete(scene);
    q_mainwindow_delete(main_window);
    q_application_delete(qapp);

    return result;
}
