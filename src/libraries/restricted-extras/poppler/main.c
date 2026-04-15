#include <libqt6c.h>

static const char* FILENAME = "assets/example.pdf";
static double DPI = 150.0;

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    Poppler__Document* document = q_poppler__document_load(FILENAME);
    if (document == NULL || q_poppler__document_is_locked(document)) {
        if (document != NULL)
            q_poppler__document_delete(document);
        fprintf(stderr, "Failed to load document: %s\n", FILENAME);
        q_application_quit();
        return 1;
    }

    int num_pages = q_poppler__document_num_pages(document);

    QWidget* widget = q_widget_new2();
    q_widget_set_window_title(widget, "Qt 6 Poppler Example");
    q_widget_resize(widget, 1200, 700);

    QVBoxLayout* layout = q_vboxlayout_new(widget);

    QScrollArea* scroll_area = q_scrollarea_new(widget);
    q_scrollarea_set_widget_resizable(scroll_area, true);

    QWidget* container = q_widget_new2();

    QVBoxLayout* page_layout = q_vboxlayout_new(container);
    q_vboxlayout_set_alignment(page_layout, container, QT_ALIGNMENTFLAG_ALIGNHCENTER);

    q_scrollarea_set_widget(scroll_area, container);
    q_vboxlayout_add_widget(layout, scroll_area);

    for (int i = 0; i < num_pages; i++) {
        Poppler__Page* page = q_poppler__document_page(document, i);
        if (page == NULL) {
            q_poppler__document_delete(document);
            fprintf(stderr, "Failed to load page: %d\n", i);
            q_application_quit();
            return 1;
        }

        QImage* image = q_poppler__page_render_to_image22(page, DPI, DPI);

        if (q_image_has_alpha_channel(image)) {
            QSize* size = q_image_size(image);
            QImage* background = q_image_new2(size, QIMAGE_FORMAT_FORMAT_RGB32);

            q_image_fill3(background, QT_GLOBALCOLOR_WHITE);

            QPainter* painter = q_painter_new2(background);
            q_painter_draw_image9(painter, 0, 0, image);

            q_image_delete(image);
            image = background;

            q_painter_delete(painter);
            q_size_delete(size);
        }

        QLabel* label = q_label_new2();

        QPixmap* pixmap = q_pixmap_from_image(image);

        q_label_set_pixmap(label, pixmap);
        q_label_set_alignment(label, QT_ALIGNMENTFLAG_ALIGNCENTER);
        q_label_set_style_sheet(label, "border: 1px solid #ccc; background-color: white;");

        q_vboxlayout_add_widget(page_layout, label);

        q_pixmap_delete(pixmap);
        q_image_delete(image);
        q_poppler__page_delete(page);
    }

    q_widget_show(widget);

    int result = q_application_exec();

    q_widget_delete(widget);
    q_poppler__document_delete(document);
    q_application_delete(qapp);

    return result;
}
