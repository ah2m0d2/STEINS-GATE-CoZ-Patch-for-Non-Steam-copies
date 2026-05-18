#include <QtCore>
#ifdef Q_OS_WIN32
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#endif

#define DR_MP3_IMPLEMENTATION
#include "vendor/dr_mp3.h"

#define MINIAUDIO_IMPLEMENTATION
#include "vendor/miniaudio.h"