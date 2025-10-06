#pragma once
#include <string>
#include "Component.h"

class Capacitor : public Component {
	double capacitance; //Capacitance in farads
	double voltage; // Current voltage across capacitor
public:
    Capacitor(double val, float R, float G, float B, std::string n)
		: capacitance(val), voltage(0) { //Initialize voltage to 0
        Red = R; Green = G; Blue = B; name = n;
    }
	//Get current voltage across capacitor
    virtual double GetVoltage(double I, double T) override {
        // Return current voltage state
        return voltage;
    }
	//Update capacitor state based on current and timestep
    virtual void Update() override {
        // Update will be called with current from CostFunctionV
        // This will be called from AnalogCircuit after current is found
    }

    //Update voltage based on current 
    void UpdateVoltage(double I, double T) {
        // EXACTLY as specified in requirements: voltage[1] = voltage[0] + current * timestep / capacitance
        voltage += I * T / capacitance;
    }
    //Display the capacitor visually
    virtual void Display() override;
	//Return the component name
    virtual std::string GetName() const override { return name; }
};
