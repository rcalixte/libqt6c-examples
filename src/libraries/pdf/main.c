#include <libqt6c.h>

static const char* PDF_FILE = "assets/example.pdf";

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QPdfDocument* document = q_pdfdocument_new();

    int err = q_pdfdocument_load(document, PDF_FILE);
    if (err != QPDFDOCUMENT_ERROR_NONE) {
        q_pdfdocument_delete(document);
        q_application_delete(qapp);
        fprintf(stderr, "Failed to load document: %s\n", PDF_FILE);
        return 1;
    }

    QPdfView* pdfview = q_pdfview_new2();

    q_pdfview_set_window_title(pdfview, "Qt 6 PDF Example");
    q_pdfview_set_minimum_size2(pdfview, 650, 600);
    q_pdfview_set_page_mode(pdfview, QPDFVIEW_PAGEMODE_MULTIPAGE);
    q_pdfview_set_zoom_mode(pdfview, QPDFVIEW_ZOOMMODE_FITINVIEW);
    q_pdfview_set_document(pdfview, document);

    q_pdfview_show(pdfview);

    int result = q_application_exec();

    q_pdfview_delete(pdfview);
    q_pdfdocument_delete(document);
    q_application_delete(qapp);

    return result;
}
