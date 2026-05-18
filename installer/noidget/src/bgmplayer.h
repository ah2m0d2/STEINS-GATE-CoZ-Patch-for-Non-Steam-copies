#pragma once

#include "vendor/miniaudio.h"

#include <QObject>
#include <QString>
#include <QByteArray>

class BgmPlayer : public QObject {
    Q_OBJECT

   public:
    explicit BgmPlayer(QObject* parent = 0);
    ~BgmPlayer();

    void setBgm(const QString& path, uint32_t loopStart, uint32_t loopEnd);
    void togglePaused();

   signals:
    void bgmAvailabilityChanged(bool available);

   private:
    static void onSend(ma_device* pDevice, void* pOutput, const void* pInput,
                       ma_uint32 frameCount);

    // Careful with accessing these - onSend runs on a background thread

    QByteArray file;
    bool haveDecoder;
    bool haveDevice;
    bool playing;
    ma_decoder decoder;
    ma_device device;
    ma_device_config deviceConfig;
    uint32_t loopPointStart;
    uint32_t loopPointEnd;
    uint32_t cursor;
};