#include <libqt6c.h>

static const char* ZIP_FILE = "zig-out/hello.zip";

int main(int argc, char* argv[]) {
    QCoreApplication* qapp = q_coreapplication_new(&argc, argv);

    KZip* archive = k_zip_new(ZIP_FILE);

    if (k_zip_open(archive, QIODEVICEBASE_OPENMODEFLAG_WRITEONLY)) {
        k_zip_write_file(archive, "world", "The whole world inside a hello");
        fprintf(stdout, "Successfully wrote to '%s'\n", ZIP_FILE);
        k_zip_close(archive);
    } else {
        fprintf(stdout, "Failed to open '%s' for writing\n", ZIP_FILE);
    }

    k_zip_delete(archive);
    q_coreapplication_delete(qapp);

    return 0;
}
