#include "txxdelta3stream.h"
#include <util/exception.h>
#include "installerapplication.h"
#include "fs.h"

static const qint64 bufferSize = 1024 * 1024;

TxXdelta3Stream::~TxXdelta3Stream() { doClose(); }

void TxXdelta3Stream::open() {
    if (_isOpen) {
        throw NgException(QString("Tried to open diff stream twice"));
    }
    _srcStream->open();
    _diffStream->open();

    // https://github.com/jmacd/xdelta/blob/release3_1_apl/xdelta3/examples/encode_decode_test.c

    memset(&_xd3stream, 0, sizeof(_xd3stream));
    memset(&_xd3source, 0, sizeof(_xd3source));
    xd3_init_config(&_xd3config, 0);
    _xd3config.winsize = bufferSize;
    xd3_config_stream(&_xd3stream, &_xd3config);

    _srcBuffer = malloc(bufferSize);
    _diffBuffer = malloc(bufferSize);

    _xd3source.blksize = bufferSize;
    _xd3source.curblk = (uint8_t*)_srcBuffer;
    _xd3source.curblkno = 0;
    _xd3source.onblk = _srcStream->read((char*)_srcBuffer, bufferSize);
    xd3_set_source(&_xd3stream, &_xd3source);

    // The correct sequence of steps after this in all cases requires some thought
    // Sorry for not commenting this better :'(
    _xd3NeedsInput = true;
    _inputAvailable = true;
    readUntilOutput();

    _isOpen = true;
}

void TxXdelta3Stream::close() {
    if (!_isOpen) {
        throw NgException(
            QString("Tried to close diff stream that wasn't open"));
    }
    doClose();
}

void TxXdelta3Stream::doClose() {
    if (_isOpen) {
        xd3_close_stream(&_xd3stream);
        xd3_free_stream(&_xd3stream);
    }
    if (_srcStream != nullptr) {
        delete _srcStream;
        _srcStream = nullptr;
    }
    if (_diffStream != nullptr) {
        delete _diffStream;
        _diffStream = nullptr;
    }
    if (_srcBuffer != nullptr) {
        free(_srcBuffer);
        _srcBuffer = nullptr;
    }
    if (_diffBuffer != nullptr) {
        free(_diffBuffer);
        _diffBuffer = nullptr;
    }
    _isOpen = false;
}

void TxXdelta3Stream::seek(qint64 count) {
    throw NgException("Tried to seek unseekable diff stream");
}
void TxXdelta3Stream::seekAbs(qint64 pos) {
    throw NgException("Tried to seek unseekable diff stream");
}

void TxXdelta3Stream::readUntilOutput() {
    while (true) {
        if (_xd3NeedsInput) {
            if (!_inputAvailable) {
                return;
            }
            qint64 diffBytesRead =
                _diffStream->read((char*)_diffBuffer, bufferSize);
            if (diffBytesRead < bufferSize) {
                xd3_set_flags(&_xd3stream, XD3_FLUSH | _xd3stream.flags);
                _inputAvailable = false;
            }
            xd3_avail_input(&_xd3stream, (uint8_t*)_diffBuffer, diffBytesRead);
            _xd3NeedsInput = false;
        }

        int code = xd3_decode_input(&_xd3stream);
        switch (code) {
            case XD3_INPUT: {
                _xd3NeedsInput = true;
                break;
            }
            case XD3_OUTPUT: {
                _outBuffer = _xd3stream.next_out;
                _outBytesAvailable = _xd3stream.avail_out;
                return;
            }
            case XD3_GETSRCBLK: {
                _srcStream->seekAbs(_xd3source.blksize * _xd3source.getblkno);
                _xd3source.onblk = _srcStream->read((char*)_xd3source.curblk,
                                                    _xd3source.blksize);
                _xd3source.curblkno = _xd3source.getblkno;
                break;
            }
            case XD3_GOTHEADER:
            case XD3_WINSTART:
            case XD3_WINFINISH: {
                break;
            }
            default: {
                throw NgException(QString("Error while reading diff stream"));
            }
        }
    }
}

qint64 TxXdelta3Stream::read(void* buffer, qint64 max) {
    if (!_isOpen) {
        throw NgException(
            QString("Tried to read diff stream that wasn't open"));
    }

    qint64 result = 0;
    while (max > 0 && _outBytesAvailable > 0) {
        qint64 flushed = qMin(max, _outBytesAvailable);
        result += flushed;
        memcpy(buffer, _outBuffer, flushed);
        buffer = (char*)buffer + flushed;
        _outBuffer = (char*)_outBuffer + flushed;
        max -= flushed;
        _outBytesAvailable -= flushed;
        if (_outBytesAvailable <= 0) {
            xd3_consume_output(&_xd3stream);
            readUntilOutput();
        }
    }
    return result;
}