#ifndef SENSOR_HPP
#define SENSOR_HPP

#include <string>

class Sensor {
protected:
    std::string tag;
    std::string unidade;
    double valor;
    std::string status;

public:

    Sensor(std::string tag, std::string unidade);

    virtual ~Sensor();

    virtual double ler() = 0;

    std::string paraJson(std::string timestamp);

    std::string getTag() const { return tag; }
    std::string getUnidade() const { return unidade; }
    double getValor() const { return valor; }
    std::string getStatus() const { return status; }
    
    void setValor(double novoValor) { valor = novoValor; }
    void setStatus(std::string novoStatus) { status = novoStatus; }
};

#endif