#pragma once
#include <string>
#include "Component.h"


// Inductor class derived from Component
class Inductor : public Component {
	double inductance; //Inductance in henrys
	double lastCurrent; // Last current through inductor
public:
    Inductor(double val, float R, float G, float B, std::string n)
		: inductance(val), lastCurrent(0) { //Initialize previous current to 0
        Red = R; Green = G; Blue = B; name = n;
    }
	//Get current voltage across inductor
    virtual double GetVoltage(double I, double T) override {
        // EXACTLY as specified in requirements: V = L * (current[1] - current[0]) / timestep
        double voltage = inductance * (I - lastCurrent) / T;
        return voltage;
    }
    //Update the inductor state
    virtual void Update() override {
        // Update current after correct current is determined (called from CostFunctionV)
        // This will be called from AnalogCircuit after current is found
    }

	//set the last current through the inductor
    void SetCurrent(double current) {
        lastCurrent = current;
    }

	//Display the inductor visually
    virtual void Display() override;

	//Return the component name
    virtual std::string GetName() const override { return name; }
};
