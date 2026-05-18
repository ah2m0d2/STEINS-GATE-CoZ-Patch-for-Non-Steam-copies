#include "streamopenaction.h"

void StreamOpenAction::run() {
    emit log("Opening stream", true);
    _stream->open();
}