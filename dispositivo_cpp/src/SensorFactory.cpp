#include "../include/SensorFactory.hpp"
#include "../include/SensorNivel.hpp"
#include "../include/SensorPressao.hpp"
#include "../include/SensorTemperatura.hpp"
#include "../include/SensorVazao.hpp"
  
Sensor* SensorFactory::criarSensor(const std::string& tipo) {
    if (tipo == "NIVEL" || tipo == "nivel") {
        return new SensorNivel();
    } 
    else if (tipo == "PRESSAO" || tipo == "pressao") {
        return new SensorPressao();
    } 
    else if (tipo == "TEMPERATURA" || tipo == "temperatura") {
        return new SensorTemperatura();
    } 
    else if (tipo == "VAZAO" || tipo == "vazao") {
        return new SensorVazao();
    }
    
    return nullptr;
}