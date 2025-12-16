#include <libqt6c.h>

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QSplitter* splitter = q_splitter_new2();
    if (!splitter) {
        fprintf(stderr, "Failed to create splitter\n");
        return 1;
    }

    const char* dir = q_dir_current_path();
    if (!dir) {
        fprintf(stderr, "Failed to get current path\n");
        q_splitter_delete(splitter);
        return 1;
    }

    QFileSystemModel* model = q_filesystemmodel_new();
    if (!model) {
        fprintf(stderr, "Failed to create filesystem model\n");
        libqt_free(dir);
        q_splitter_delete(splitter);
        return 1;
    }

    QModelIndex* modelindex = q_filesystemmodel_set_root_path(model, dir);
    if (!modelindex) {
        fprintf(stderr, "Failed to create model index\n");
        q_filesystemmodel_delete(model);
        libqt_free(dir);
        q_splitter_delete(splitter);
        return 1;
    }

    libqt_free(dir);

    // Tree view
    QTreeView* tree = q_treeview_new(splitter);
    if (!tree) {
        fprintf(stderr, "Failed to create tree view\n");
        q_modelindex_delete(modelindex);
        q_filesystemmodel_delete(model);
        q_splitter_delete(splitter);
        return 1;
    }
    q_treeview_set_model(tree, model);
    q_treeview_set_root_index(tree, modelindex);

    // List view
    QListView* list = q_listview_new(splitter);
    if (!list) {
        fprintf(stderr, "Failed to create list view\n");
        // tree will be deleted with splitter
        q_modelindex_delete(modelindex);
        q_filesystemmodel_delete(model);
        q_splitter_delete(splitter);
        return 1;
    }
    q_listview_set_model(list, model);
    q_listview_set_root_index(list, modelindex);

    QItemSelectionModel* tree_model = q_treeview_selection_model(tree);
    q_listview_set_selection_model(list, tree_model);

    q_splitter_set_window_title(splitter, "Folder Model Views");
    q_splitter_show(splitter);

    int result = q_application_exec();

    // Cleanup
    q_modelindex_delete(modelindex);
    q_filesystemmodel_delete(model);
    q_application_delete(qapp);

    return result;
}
