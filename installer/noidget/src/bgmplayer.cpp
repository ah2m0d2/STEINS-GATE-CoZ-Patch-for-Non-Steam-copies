#include "vendor/dr_mp3.h"

#include "bgmplayer.h"

#include <QFile>

BgmPlayer::BgmPlayer(QObject* parent) : QObject(parent) {
    haveDecoder = false;
    haveDevice = false;
    playing = false;
}

BgmPlayer::~BgmPlayer() {
    if (haveDevice) ma_device_uninit(&device);
    if (haveDecoder) ma_decoder_uninit(&decoder);
}

void BgmPlayer::setBgm(const QString& path, uint32_t loopStart,
                       uint32_t loopEnd) {
    if (haveDevice) {
        ma_device_uninit(&device);
        haveDevice = false;
    }
    if (haveDecoder) {
        ma_decoder_uninit(&decoder);
        haveDecoder = false;
    }
    playing = false;

    emit bgmAvailabilityChanged(false);

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }
    file = f.readAll();

    ma_result result = ma_decoder_init_memory_mp3(file.constData(), file.size(),
                                                  NULL, &decoder);
    if (result != MA_SUCCESS) return;

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate = decoder.outputSampleRate;
    deviceConfig.dataCallback = onSend;
    deviceConfig.pUserData = this;

    loopPointStart = loopStart;
    loopPointEnd = loopEnd;
    cursor = 0;

    result = ma_device_init(NULL, &deviceConfig, &device);
    if (result != MA_SUCCESS) {
        ma_decoder_uninit(&decoder);
        return;
    }

    haveDecoder = true;

    result = ma_device_start(&device);
    if (result != MA_SUCCESS) {
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        haveDecoder = false;
        return;
    }

    haveDevice = true;
    playing = true;
    emit bgmAvailabilityChanged(true);
}

void BgmPlayer::onSend(ma_device* pDevice, void* pOutput, const void* pInput,
                       ma_uint32 frameCount) {
    BgmPlayer* player = (BgmPlayer*)pDevice->pUserData;
    if (!player->haveDecoder) return;

    uint32_t framesToRead = frameCount;
    if (player->loopPointEnd > 0) {
        framesToRead = qMin(frameCount, player->loopPointEnd - player->cursor);
    }

    ma_uint32 framesRead = (ma_uint32)ma_decoder_read_pcm_frames(
        &player->decoder, pOutput, framesToRead);
    player->cursor += framesRead;
    ma_uint32 framesLeft = frameCount - framesRead;

    if (framesLeft > 0) {
        ma_decoder_seek_to_pcm_frame(&player->decoder, player->loopPointStart);
        player->cursor = player->loopPointStart;
        player->cursor +=
            ma_decoder_read_pcm_frames(&player->decoder, pOutput, framesLeft);
    }
}

void BgmPlayer::togglePaused() {
    if (!haveDecoder || !haveDevice) return;

    if (playing) {
        ma_device_stop(&device);
        playing = false;
    } else {
        ma_device_start(&device);
        playing = true;
    }
}