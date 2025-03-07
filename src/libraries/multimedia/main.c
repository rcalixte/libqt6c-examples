#include <libqt6c.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

void onPlaybackStateChanged(void* player, int64_t state) {
    printf("Playback state: %ld\n", state);

    if (state == QMEDIAPLAYER_PLAYBACKSTATE_STOPPEDSTATE) {
        printf("Playback complete.\n");
        q_coreapplication_exit();
    }
}

int main(int argc, char* argv[]) {
    q_application_new(&argc, argv);

    // Get absolute path to MP3
    char mp3_path[PATH_MAX];
    const char* res = realpath("src/libraries/multimedia/pixabay-public-domain-strong-hit-36455.mp3", mp3_path);
    if (!res) {
        fprintf(stderr, "Failed to resolve MP3 path\n");
        return 1;
    }

    QMediaPlayer* player = q_mediaplayer_new();
    if (!player) {
        fprintf(stderr, "Failed to create media player\n");
        return 1;
    }

    QAudioOutput* output = q_audiooutput_new();
    if (!output) {
        fprintf(stderr, "Failed to create audio output\n");
        q_mediaplayer_delete(player);
        return 1;
    }

    q_mediaplayer_set_audio_output(player, output);
    QUrl* url = q_url_new3(mp3_path);
    q_mediaplayer_set_source(player, url);
    q_audiooutput_set_volume(output, 50);

    q_mediaplayer_on_playback_state_changed(player, onPlaybackStateChanged);

    fprintf(stdout, "Playing %s\n", mp3_path);
    q_mediaplayer_play(player);

    int result = q_application_exec();

    q_url_delete(url);
    q_audiooutput_delete(output);
    q_mediaplayer_delete(player);

    return result;
}
