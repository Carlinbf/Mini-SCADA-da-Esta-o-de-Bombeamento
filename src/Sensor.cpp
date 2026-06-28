#include "../include/Sensor.hpp"
#include <sstream>

Sensor::Sensor(std::string tag, std::string unidade)
    : tag(tag), unidade(unidade), valor(0.0), status("OK") {}

Sensor::~Sensor() {}

std::string Sensor::paraJson(std::string timestamp) {
    std::ostringstream ss;

    ss << "{\"estacao\":\"EB-59\","
       << "\"tag\":\"" << tag << "\","
       << "\"valor\":" << valor << ","
       << "\"unidade\":\"" << unidade << "\","
       << "\"timestamp\":\"" << timestamp << "\","
       << "\"status\":\"" << status << "\"}";
       
    return ss.str();
}