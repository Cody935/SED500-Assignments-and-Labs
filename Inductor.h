#pragma once
#include <string>
using namespace std;

class Inductor {
    double inductance;
    double lastCurrent;
    float r, g, b;
    string name;
public:
    Inductor(double val, float R, float G, float B, string n)
        : inductance(val), lastCurrent(0), r(R), g(G), b(B), name(n) {}
    double GetVoltage(double I, double T) {
        double voltage = inductance * (I - lastCurrent) / T;
        lastCurrent = I;
        return voltage;
    }
    void Update() { lastCurrent = 0; }
    void Display();
    string GetName() const { return name; }
};
