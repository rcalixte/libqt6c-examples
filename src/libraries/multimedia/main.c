#include <libqt6c.h>

static const char* MP3_PATH = "src/libraries/multimedia/pixabay-public-domain-strong-hit-36455.mp3";

void onPlaybackStateChanged(void* UNUSED player, int32_t state) {
    printf("Playback state: %d\n", state);

    if (state == QMEDIAPLAYER_PLAYBACKSTATE_STOPPEDSTATE) {
        printf("Playback complete.\n");
        q_application_exit();
    }
}

int main(int argc, char* argv[]) {
    QApplication* qapp = q_application_new(&argc, argv);

    QMediaPlayer* player = q_mediaplayer_new();
    if (!player) {
        fprintf(stderr, "Failed to create media player\n");
        return 1;
    }

    if (q_mediaplayer_error(player) != QMEDIAPLAYER_ERROR_NOERROR) {
        fprintf(stderr, "Failed to create player.\n");
        q_mediaplayer_delete(player);
        return 1;
    }

    QAudioOutput* output = q_audiooutput_new();
    if (!output) {
        fprintf(stderr, "Failed to create audio output\n");
        q_mediaplayer_delete(player);
        return 1;
    }

    q_mediaplayer_set_audio_output(player, output);
    QUrl* url = q_url_new3(MP3_PATH);
    q_mediaplayer_set_source(player, url);
    q_audiooutput_set_volume(output, 50);

    q_mediaplayer_on_playback_state_changed(player, onPlaybackStateChanged);

    fprintf(stdout, "Playing %s\n", MP3_PATH);
    q_mediaplayer_play(player);

    int result = q_application_exec();

    q_url_delete(url);
    q_audiooutput_delete(output);
    q_mediaplayer_delete(player);
    q_application_delete(qapp);

    return result;
}
