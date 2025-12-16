#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QWebEngineView* webengine = q_webengineview_new2();
    if (!webengine) {
        fprintf(stderr, "Failed to create web engine view\n");
        return 1;
    }

    QUrl* url = q_url_new3("https://github.com/rcalixte/libqt6c-examples");
    if (!url) {
        fprintf(stderr, "Failed to create URL\n");
        q_webengineview_delete(webengine);
        return 1;
    }

    q_webengineview_set_url(webengine, url);
    q_webengineview_set_geometry(webengine, 100, 100, 640, 400);
    q_webengineview_set_visible(webengine, true);

    int result = q_application_exec();

    q_webengineview_delete(webengine);
    q_url_delete(url);
    q_application_delete(qapp);

    return result;
}
