//
// Created by nvt on 8/4/16.
//

#include "Noto.h"

#include <core/Predef.h>

int Noto::logLevel = Noto::INFO_LEVEL;

std::shared_ptr<spdlog::logger> Noto::getConsole() {
    static std::shared_ptr<spdlog::logger> log  = spdlog::stdout_logger_mt("console",true);
    return log;
}
