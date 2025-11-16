#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QPdfDocument* document = q_pdfdocument_new();
    q_pdfdocument_load(document, "assets/example.pdf");

    QPdfView* pdfview = q_pdfview_new2();

    q_pdfview_set_window_title(pdfview, "Qt 6 PDF Example");
    q_pdfview_set_minimum_size2(pdfview, 650, 600);
    q_pdfview_set_page_mode(pdfview, QPDFVIEW_PAGEMODE_MULTIPAGE);
    q_pdfview_set_zoom_mode(pdfview, QPDFVIEW_ZOOMMODE_FITINVIEW);
    q_pdfview_set_document(pdfview, document);

    q_pdfview_show(pdfview);

    int result = q_application_exec();

    q_pdfdocument_delete(document);
    q_application_delete(qapp);

    return result;
}
