#include <libqt6c.h>

static QOpenGLExtraFunctions* glfuncs;

void initialize_g_l() {
    glfuncs = q_openglcontext_extra_functions(q_openglcontext_current_context());

    q_openglextrafunctions_initialize_open_g_l_functions(glfuncs);
    q_openglextrafunctions_gl_clear_color(glfuncs, 0.33, 0.33, 0.33, 1.0);
}

void resize_g_l(void* UNUSED self, int width, int height) {
    q_openglextrafunctions_gl_viewport(glfuncs, 0, 0, width, height);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QMainWindow* window = q_mainwindow_new2();

    q_mainwindow_set_window_title(window, "Qt 6 OpenGL Example");
    q_mainwindow_set_minimum_size2(window, 400, 400);

    QOpenGLWidget* glwidget = q_openglwidget_new2();

    q_openglwidget_on_initialize_g_l(glwidget, initialize_g_l);
    q_openglwidget_on_resize_g_l(glwidget, resize_g_l);

    q_mainwindow_set_central_widget(window, glwidget);

    q_mainwindow_show(window);

    int result = q_application_exec();

    q_mainwindow_delete(window);
    q_application_delete(qapp);

    return result;
}
