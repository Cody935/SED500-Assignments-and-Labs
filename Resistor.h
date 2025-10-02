#pragma once
#include <string>
using namespace std;

class Resistor {
    double resistance;
    float r, g, b;
    string name;
public:
    Resistor(double val, float R, float G, float B, string n)
        : resistance(val), r(R), g(G), b(B), name(n) {}
    double GetVoltage(double I, double /*T*/) {
        return I * resistance;
    }
    void Update() {}
    void Display();
    string GetName() const { return name; }
};
