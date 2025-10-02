#pragma once
#include <string>
using namespace std;

class Capacitor {
    double capacitance;
    double lastVoltage;
    float r, g, b;
    string name;
public:
    Capacitor(double val, float R, float G, float B, string n)
        : capacitance(val), lastVoltage(0), r(R), g(G), b(B), name(n) {}
    double GetVoltage(double I, double T) {
        lastVoltage += (I / capacitance) * T;
        return lastVoltage;
    }
    void Update() { lastVoltage = 0; }
    void Display();
    string GetName() const { return name; }
};
