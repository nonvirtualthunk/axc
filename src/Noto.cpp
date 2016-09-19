//
// Created by nvt on 8/4/16.
//

#include "Noto.h"

#include <core/Predef.h>

int Noto::logLevel = Noto::INFO_LEVEL;
int Noto::historySize = 500;
Arx::Sequence<LogStatement> Noto::history;

std::shared_ptr<spdlog::logger> Noto::getConsole() {
    static std::shared_ptr<spdlog::logger> log  = spdlog::stdout_logger_mt("console",true);
    return log;
}

void Noto::recordLogStatement(int level, const Arx::String &str) {
    LogStatement stmt(level,str);

    static std::mutex historyMutex;

    {
        std::lock_guard<std::mutex> guard(historyMutex);
        Noto::history.pushFront(stmt);
        Noto::history.head().statement.append('\n');
        while (Noto::history.size() > historySize) {
            Noto::history.popBack();
        }
    }
}
