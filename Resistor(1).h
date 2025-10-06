#pragma once
#include <string>
#include "Component.h"


// Resistor class derived from Component
class Resistor : public Component {
	double resistance; //Resistance in ohms
public:
	//Constructor to initialize resistance and color
    Resistor(double val, float R, float G, float B, std::string n)
        : resistance(val) {
        Red = R; Green = G; Blue = B; name = n;
    }

	//Compute voltage across resistor using Ohm's Law
    virtual double GetVoltage(double I, double /*T*/ ) override {
        return I * resistance;  // V = I * R
    }

    //update component state
    virtual void Update() override {}

	//Render the resistor visually
    virtual void Display() override;

	//Return the component name
    virtual std::string GetName() const override { return name; }
};