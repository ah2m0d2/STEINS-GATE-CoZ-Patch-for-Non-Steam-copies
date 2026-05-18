#include "streamcloseaction.h"

void StreamCloseAction::run() {
    emit log("Closing stream", true);
    _stream->close();
}