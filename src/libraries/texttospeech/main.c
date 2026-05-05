#include <libqt6c.h>
#include "mainwindow.h"

static MainWindowUi* ui = NULL;
static QTextToSpeech* speech = NULL;
static libqt_list voices;

void reset() {
    q_pushbutton_set_enabled(ui->pauseButton, false);
    q_pushbutton_set_enabled(ui->resumeButton, false);
    q_texttospeech_stop(speech);
}

void on_pitch_changed(void* self UNUSED, int value) {
    q_texttospeech_set_pitch(speech, value);
    reset();
}

void on_rate_changed(void* self UNUSED, int value) {
    q_texttospeech_set_rate(speech, value);
    reset();
}

void on_volume_changed(void* self UNUSED, int value) {
    q_texttospeech_set_volume(speech, value);
    reset();
}

void on_language_selected(void* self, int index) {
    QVariant* variant = q_combobox_item_data(self, index);
    QLocale* locale = q_variant_to_locale(variant);
    q_texttospeech_set_locale(speech, locale);
    q_locale_delete(locale);
    q_variant_delete(variant);
    reset();
}

void on_voice_selected(void* self UNUSED, int index) {
    if (voices.len <= (size_t)index)
        return;

    QVoice** voice_data = voices.data.ptr;
    q_texttospeech_set_voice(speech, voice_data[index]);
    reset();
}

void on_speak_clicked(void* self UNUSED) {
    const char* text = q_plaintextedit_to_plain_text(ui->plainTextEdit);
    q_texttospeech_say(speech, text);
    libqt_free(text);
}

void on_stop_clicked(void* self UNUSED) {
    q_texttospeech_stop(speech);
}

void on_pause_clicked(void* self UNUSED) {
    q_texttospeech_pause(speech);
}

void on_resume_clicked(void* self UNUSED) {
    q_texttospeech_resume(speech);
}

void on_state_changed(void* self UNUSED, int32_t state) {
    switch (state) {
    case QTEXTTOSPEECH_STATE_SPEAKING:
        q_statusbar_show_message(ui->statusbar, "Speech started...");
        break;
    case QTEXTTOSPEECH_STATE_READY:
        q_statusbar_show_message2(ui->statusbar, "Speech stopped...", 2000);
        break;
    case QTEXTTOSPEECH_STATE_PAUSED:
        q_statusbar_show_message(ui->statusbar, "Speech paused...");
        break;
    default:
        q_statusbar_show_message(ui->statusbar, "Speech error!");
    }

    q_pushbutton_set_enabled(ui->pauseButton, state == QTEXTTOSPEECH_STATE_SPEAKING);
    q_pushbutton_set_enabled(ui->resumeButton, state == QTEXTTOSPEECH_STATE_PAUSED);
    q_pushbutton_set_enabled(ui->stopButton, state == QTEXTTOSPEECH_STATE_SPEAKING || state == QTEXTTOSPEECH_STATE_PAUSED);
}

void on_locale_changed(void* self UNUSED, void* locale) {
    QVariant* variant = q_variant_new21(locale);
    q_combobox_set_current_index(ui->language, q_combobox_find_data(ui->language, variant));
    q_variant_delete(variant);

    QSignalBlocker* blocker = q_signalblocker_new(ui->voice);
    reset();
    q_combobox_clear(ui->voice);

    if (voices.len > 0) {
        QVoice** voice_data = voices.data.ptr;
        for (size_t i = 0; i < voices.len; i++)
            free(voices.data.ptr);
    }

    voices = q_texttospeech_available_voices(speech);
    QVoice** voice_data = voices.data.ptr;
    QVoice* current = q_texttospeech_voice(speech);
    const char* current_name = q_voice_name(current);

    for (size_t i = 0; i < voices.len; ++i) {
        const char* name = q_voice_name(voice_data[i]);
        const char* gender_name = q_voice_gender_name(q_voice_gender(voice_data[i]));
        const char* age_name = q_voice_age_name(q_voice_age(voice_data[i]));
        const char* fmt = "%s - %s - %s";
        size_t len = snprintf(NULL, 0, fmt, name, gender_name, age_name);

        char* item = malloc(len + 1);
        if (!item) {
            fprintf(stderr, "Failed to allocate memory for text\n");
            abort();
        }

        snprintf(item, len + 1, fmt, name, gender_name, age_name);
        q_combobox_add_item(ui->voice, item);
        if (strcmp(name, current_name) == 0)
            q_combobox_set_current_index(ui->voice, q_combobox_count(ui->voice) - 1);

        libqt_free(item);
        libqt_free(age_name);
        libqt_free(gender_name);
        libqt_free(name);
    }

    libqt_free(current_name);
    q_voice_delete(current);
    q_signalblocker_delete(blocker);
}

void on_engine_ready() {
    if (q_texttospeech_state(speech) != QTEXTTOSPEECH_STATE_READY) {
        on_state_changed(speech, q_texttospeech_state(speech));
        return;
    }

    q_pushbutton_set_enabled(ui->pauseButton, false);
    q_pushbutton_set_enabled(ui->resumeButton, false);

    QSignalBlocker* blocker = q_signalblocker_new(ui->language);

    q_combobox_clear(ui->language);
    libqt_list locales = q_texttospeech_available_locales(speech);
    QLocale** locale_data = locales.data.ptr;
    QLocale* current = q_texttospeech_locale(speech);
    const char* current_name = q_locale_name(current);

    for (size_t i = 0; i < locales.len; ++i) {
        const char* language = q_locale_language_to_string(q_locale_language(locale_data[i]));
        const char* territory = q_locale_territory_to_string(q_locale_territory(locale_data[i]));
        const char* fmt = "%s (%s)";
        size_t len = snprintf(NULL, 0, fmt, language, territory);

        char* name = malloc(len + 1);
        if (!name) {
            fprintf(stderr, "Failed to allocate memory for text\n");
            free(locale_data);
            abort();
        }

        snprintf(name, len + 1, fmt, language, territory);
        QVariant* variant = q_variant_new21(locale_data[i]);
        q_combobox_add_item22(ui->language, name, variant);
        const char* locale_name = q_locale_name(locale_data[i]);
        if (strcmp(locale_name, current_name) == 0)
            q_locale_operator_assign(current, locale_data[i]);

        q_locale_delete(locale_data[i]);
        libqt_free(locale_name);
        q_variant_delete(variant);
        libqt_free(name);
        libqt_free(territory);
        libqt_free(language);
    }
    free(locale_data);

    on_rate_changed(ui->rate, q_slider_value(ui->rate));
    on_pitch_changed(ui->pitch, q_slider_value(ui->pitch));
    on_volume_changed(ui->volume, q_slider_value(ui->volume));

    q_pushbutton_on_clicked(ui->speakButton, on_speak_clicked);
    q_pushbutton_on_clicked(ui->stopButton, on_stop_clicked);
    q_pushbutton_on_clicked(ui->pauseButton, on_pause_clicked);
    q_pushbutton_on_clicked(ui->resumeButton, on_resume_clicked);

    q_texttospeech_on_state_changed(speech, on_state_changed);
    q_texttospeech_on_locale_changed(speech, on_locale_changed);

    q_signalblocker_unblock(blocker);
    on_locale_changed(speech, current);

    libqt_free(current_name);
    q_locale_delete(current);
    q_signalblocker_delete(blocker);
}

void on_engine_selected(void* self, int index) {
    QVariant* variant = q_combobox_item_data(self, index);
    const char* engine_name = q_variant_to_string(variant);
    if (speech != NULL)
        q_texttospeech_delete(speech);

    speech = q_texttospeech_new5(engine_name, ui->MainWindow);

    libqt_free(engine_name);
    q_variant_delete(variant);

    if (q_texttospeech_state(speech) == QTEXTTOSPEECH_STATE_READY)
        on_engine_ready();
    else
        q_texttospeech_on_state_changed(speech, on_state_changed);
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    ui = new_main_window_ui();

    const char** engines = q_texttospeech_available_engines();
    for (size_t i = 0; engines[i] != NULL; i++) {
        QVariant* engine_variant = q_variant_new24(engines[i]);
        q_combobox_add_item22(ui->engine, engines[i], engine_variant);
        q_variant_delete(engine_variant);
        libqt_free(engines[i]);
    }
    free(engines);

    q_combobox_set_current_index(ui->engine, 0);
    on_engine_selected(ui->engine, 0);

    q_slider_on_value_changed(ui->pitch, on_pitch_changed);
    q_slider_on_value_changed(ui->rate, on_rate_changed);
    q_slider_on_value_changed(ui->volume, on_volume_changed);
    q_combobox_on_current_index_changed(ui->engine, on_engine_selected);
    q_combobox_on_current_index_changed(ui->language, on_language_selected);
    q_combobox_on_current_index_changed(ui->voice, on_voice_selected);

    q_mainwindow_show(ui->MainWindow);

    int result = q_application_exec();

    if (voices.len > 0) {
        QVoice** voice_data = voices.data.ptr;
        for (size_t i = 0; i < voices.len; i++)
            q_voice_delete(voice_data[i]);
        free(voices.data.ptr);
    }

    if (speech != NULL)
        q_texttospeech_delete(speech);
    cleanup_main_window_ui(ui);
    q_application_delete(qapp);

    return result;
}
