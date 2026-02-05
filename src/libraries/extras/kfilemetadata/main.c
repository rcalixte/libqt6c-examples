#include <libqt6c.h>

static const char* FILENAME = "assets/Qt.png";

typedef struct {
    const char* key;
    int32_t property;
} TextMapping;

static TextMapping text_mapping[] = {
    {.key = "Comment", .property = KFILEMETADATA_PROPERTY_PROPERTY_COMMENT},
};

const char** on_mimetypes() {
    const char** ret = (const char**)malloc(2 * sizeof(char*));
    if (ret == NULL) {
        return NULL;
    }

    ret[0] = "image/png";
    ret[1] = NULL;

    return ret;
}

void on_extract(void* UNUSED self, void* result) {
    QImageReader* reader = q_imagereader_new5(FILENAME, "png");

    if (!q_imagereader_can_read(reader)) {
        fprintf(stderr, "Unable to read input image: '%s'\n", FILENAME);
        q_imagereader_delete(reader);
        return;
    }

    k_filemetadata__extractionresult_add_type(result, KFILEMETADATA_TYPE_TYPE_IMAGE);

    if ((k_filemetadata__extractionresult_input_flags(result) & KFILEMETADATA_EXTRACTIONRESULT_FLAG_EXTRACTMETADATA) == 0) {
        fprintf(stderr, "Unable to extract metadata from image: '%s'\n", FILENAME);
        q_imagereader_delete(reader);
        return;
    }

    for (size_t i = 0; i < sizeof(text_mapping) / sizeof(TextMapping); i++) {
        const char* value = q_imagereader_text(reader, text_mapping[i].key);
        if (value != NULL) {
            QVariant* variant = q_variant_new24(value);
            k_filemetadata__extractionresult_add(result, text_mapping[i].property, variant);
            q_variant_delete(variant);
        }
        libqt_free(value);
    }

    q_imagereader_delete(reader);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QListWidget* listwidget = q_listwidget_new2();
    q_listwidget_set_window_title(listwidget, "Qt 6 KFileMetaData Example");
    q_listwidget_resize(listwidget, 500, 250);
    q_listwidget_set_spacing(listwidget, 5);

    QSize* size = q_size_new4(200, 200);
    q_listwidget_set_icon_size(listwidget, size);
    q_listwidget_set_view_mode(listwidget, QLISTVIEW_VIEWMODE_ICONMODE);

    QIcon* icon = q_icon_new4(FILENAME);
    QListWidgetItem* item = q_listwidgetitem_new3(icon, "Image Properties");

    q_listwidget_add_item2(listwidget, item);

    QObject* object = q_object_new();

    KFileMetaData__ExtractorPlugin* png_extractor = k_filemetadata__extractorplugin_new(object);
    k_filemetadata__extractorplugin_on_mimetypes(png_extractor, on_mimetypes);
    k_filemetadata__extractorplugin_on_extract(png_extractor, on_extract);

    KFileMetaData__SimpleExtractionResult* result = k_filemetadata__simpleextractionresult_new(FILENAME);
    k_filemetadata__extractorplugin_extract(png_extractor, result);

    libqt_map properties = k_filemetadata__simpleextractionresult_properties(result);

    int32_t* keys = properties.keys;
    QVariant*** values = properties.values;

    for (size_t i = 0; i < properties.len; i++) {
        KFileMetaData__PropertyInfo* info = k_filemetadata__propertyinfo_new2(keys[i]);
        const char* name = k_filemetadata__propertyinfo_display_name(info);
        for (size_t j = 0; values[i][j] != NULL; j++) {
            const char* value_str = q_variant_to_string(values[i][j]);
            char* text = malloc(strlen(name) + strlen(value_str) + 3);
            snprintf(text, strlen(name) + strlen(value_str) + 3, "%s: %s", name, value_str);
            q_listwidget_add_item(listwidget, text);
            free(text);
            libqt_free(value_str);
            libqt_free(name);
            free(values[i][j]);
        }
        k_filemetadata__propertyinfo_delete(info);
    }

    q_listwidget_show(listwidget);

    int ret = q_application_exec();

    free(keys);
    free(values);
    k_filemetadata__simpleextractionresult_delete(result);
    q_object_delete(object);
    q_listwidgetitem_delete(item);
    q_icon_delete(icon);
    q_size_delete(size);
    q_listwidget_delete(listwidget);
    q_application_delete(qapp);

    return ret;
}
