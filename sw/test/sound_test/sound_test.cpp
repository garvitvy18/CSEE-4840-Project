// file: play_mp3_usb.cpp
#include <iostream>
#include <mpg123.h>
#include <alsa/asoundlib.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " /path/to/file.mp3\n";
        return 1;
    }
    const char* mp3_path = argv[1];
    const char* alsa_dev  = "plughw:1,0";  // your USB audio device

    // --- 1) Initialize mpg123 ---
    mpg123_init();
    int err;
    mpg123_handle *mh = mpg123_new(nullptr, &err);
    if (!mh) {
        std::cerr << "mpg123_new() failed: " << mpg123_plain_strerror(err) << "\n";
        return 1;
    }
    if (mpg123_open(mh, mp3_path) != MPG123_OK) {
        std::cerr << "Unable to open MP3: " << mpg123_plain_strerror(err) << "\n";
        return 1;
    }

    // Get audio format from file
    long rate;
    int channels, encoding;
    mpg123_getformat(mh, &rate, &channels, &encoding);

    // Force 16‑bit signed little‑endian (most USB DACs support it)
    mpg123_format_none(mh);
    mpg123_format(mh, rate, channels, MPG123_ENC_SIGNED_16);

    // --- 2) Open ALSA device ---
    snd_pcm_t *pcm = nullptr;
    if ((err = snd_pcm_open(&pcm, alsa_dev, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        std::cerr << "snd_pcm_open: " << snd_strerror(err) << "\n";
        return 1;
    }
    // Set hardware params: S16_LE, interleaved, same rate & channels as MP3
    if ((err = snd_pcm_set_params(
            pcm,
            SND_PCM_FORMAT_S16_LE,
            SND_PCM_ACCESS_RW_INTERLEAVED,
            channels,
            rate,
            /*allow resample*/ 1,
            /*latency*/ 500000  // 0.5s
        )) < 0)
    {
        std::cerr << "snd_pcm_set_params: " << snd_strerror(err) << "\n";
        return 1;
    }

    // --- 3) Decode & play loop ---
    const size_t buffer_size = 8192;
    unsigned char* buffer = new unsigned char[buffer_size];
    size_t bytes_done = 0;

    while (mpg123_read(mh, buffer, buffer_size, &bytes_done) == MPG123_OK) {
        // writei wants number of frames; each frame = channels * 2 bytes
        snd_pcm_sframes_t frames = snd_pcm_writei(pcm, buffer, bytes_done / (channels * 2));
        if (frames < 0) {
            frames = snd_pcm_recover(pcm, frames, 0);
        }
        if (frames < 0) {
            std::cerr << "snd_pcm_writei failed: " << snd_strerror(frames) << "\n";
            break;
        }
    }

    // --- 4) Cleanup ---
    delete[] buffer;
    snd_pcm_drain(pcm);
    snd_pcm_close(pcm);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();

    return 0;
}