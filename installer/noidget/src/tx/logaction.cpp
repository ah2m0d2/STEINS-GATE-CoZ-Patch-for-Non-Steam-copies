#include "logaction.h"

void LogAction::run() { emit log(_text, _fileOnly); }