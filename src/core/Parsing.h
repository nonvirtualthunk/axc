//
// Created by Sam Bock on 9/6/16.
//

#ifndef TEST2_PARSING_H
#define TEST2_PARSING_H

#include <Noto.h>
#include "ArxString.h"
#include "ArxFile.h"

using namespace Arx;

struct ParsingContext {
    File referenceDirectory = File(".");
    File rootDirectory = File(".");
};

struct ParseError {
    String message;
    int cursor;
    int line;

    ParseError(const String &message, int cursor, int line) : message(message), cursor(cursor), line(line) {}
};

struct ParseStateBase {
    ParseStateBase(String &raw,ParsingContext& context) : raw(raw), context(context) {}

    int cursor = 0;
    int currentLine = 1;
    String& raw;
    String buffer;
    ParsingContext& context;

    Sequence<ParseError> errors;

    bool hasNext() {
        return cursor < raw.size();
    }
    char next() {
        char c = raw[cursor++];
        if (c == '\n') {
            currentLine += 1;
        }
        return c;
    }
    void reverse(int n) {
        for (int i = 0; i < n; ++i) {
            cursor--;
            if (raw[cursor] == '\n') {
                currentLine -= 1;
            }
        }
    }
    void advance(int n) {
        for (int i = 0; i < n; ++i) {
            next();
        }
    }
    void startBuffering() {
        buffer.clear();
        buffer.append(raw[cursor-1]);
    }
    String takeUntil(char target, int startFromDelta) {
        String tmp;
        if (startFromDelta == -1) {
            tmp.append(raw[cursor + startFromDelta]);
        }
        while (hasNext()) {
            char c = next();
            if (c == target) {
                return tmp;
            } else {
                tmp.append(c);
            }
        }
        return tmp;
    }
    String takeUntil(char target, char target2, int startFromDelta) {
        String tmp;
        if (startFromDelta == -1) {
            tmp.append(raw[cursor + startFromDelta]);
        }
        while (hasNext()) {
            char c = next();
            if (c == target || c == target2) {
                return tmp;
            } else {
                tmp.append(c);
            }
        }
        return tmp;
    }
    String takeUntilWhitespace(int startFromDelta) {
        String tmp;
        if (startFromDelta == -1) {
            tmp.append(raw[cursor + startFromDelta]);
        }
        while (hasNext()) {
            char c = next();
            if (c == ' ' || c == '\n' || c == '\t' || c == '\r' ) {
                return tmp;
            } else {
                tmp.append(c);
            }
        }
        return tmp;
    }
    void advanceUntil(char target) {
        while (hasNext() && next() != target) {}
    }
    void advanceUntil(char target, char target2) {
        while (hasNext()) {
            char c = next();
            if (c == target || c == target2) {
                break;
            }
        }
    }

    void addError(const String& message) {
        Noto::error("Syntax error: {}", message);
        Noto::error("\tLine: {}, absolute char position: {}", currentLine, cursor);
        errors.add(ParseError(message, cursor, currentLine));
    }
};

#endif //TEST2_PARSING_H
