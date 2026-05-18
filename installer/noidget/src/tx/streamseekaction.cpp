#include "streamseekaction.h"

void StreamSeekAction::run() {
    emit log(QString("Seeking stream (%1)").arg(_count), true);
    _stream->seek(_count);
}