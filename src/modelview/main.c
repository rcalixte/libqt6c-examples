#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QSplitter* splitter = q_splitter_new2();

    const char* dir = q_dir_current_path();
    if (!dir) {
        fprintf(stderr, "Failed to get current path\n");
        q_splitter_delete(splitter);
        return 1;
    }

    QFileSystemModel* model = q_filesystemmodel_new();
    QModelIndex* modelindex = q_filesystemmodel_set_root_path(model, dir);
    libqt_free(dir);

    QTreeView* tree = q_treeview_new(splitter);
    q_treeview_set_model(tree, model);
    q_treeview_set_root_index(tree, modelindex);

    QListView* list = q_listview_new(splitter);
    q_listview_set_model(list, model);
    q_listview_set_root_index(list, modelindex);

    QItemSelectionModel* tree_model = q_treeview_selection_model(tree);
    q_listview_set_selection_model(list, tree_model);

    q_splitter_set_window_title(splitter, "Folder Model Views");
    q_splitter_show(splitter);

    int result = q_application_exec();

    q_splitter_delete(splitter);
    q_modelindex_delete(modelindex);
    q_filesystemmodel_delete(model);
    q_application_delete(qapp);

    return result;
}
