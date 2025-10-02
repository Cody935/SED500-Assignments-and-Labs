//AnalogCircuit.cpp - function definitions for the analog circuit
//
// 20-May-22  M. Watler         Created.
// 27-May-22  M. Watler         Added graphical display.
#include <GL/glut.h>
#include <iomanip>   //setw
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>    //M_PI
#include <stdlib.h>  //abs
#include <fstream>   //file output

#include "AnalogCircuit.h"
#include "Capacitor.h"
#include "Inductor.h"
#include "Resistor.h"

using namespace std;

int xpos, ypos;
int windowWidth, windowHeight;
double scalingFactor;

void start() {
    cout << "BEGIN" << endl;
    AnalogCircuit circuit("RLC.dat");
    circuit.run();
    cout << "Press any key + ENTER to end..." << endl;
    char key;
    cin >> key;
    glutLeaveMainLoop();
    cout << "DONE!" << endl;
}

void AnalogCircuit::display(float R, float G, float B) { //draw a point on the screen
    glColor3f(R, G, B); //RGB
    glBegin(GL_POINTS);
    glVertex2f(xpos, ypos);
    glEnd();
    glFlush();
}

AnalogCircuit::AnalogCircuit(string filename) {
    fout.open(filename.c_str());
    if (!fout.is_open()) {
        cerr << "Error: could not open file " << filename << endl;
        exit(1);
    }

    // Default simulation parameters
    Vpeak = 10.0;       // Peak input voltage [Volts]
    freq = 60.0;        // Frequency in Hz
    T = 0.0001;         // Time step
    timeMax = 0.1;      // Simulation end time [sec]
    tolerance = 1e-6;   // Numerical tolerance

    windowWidth = 800;
    windowHeight = 600;
    scalingFactor = 1.0;
    xpos = 0;
    ypos = 0;
}

void AnalogCircuit::run() {
    // Define circuit components
    component.push_back(new Capacitor(0.000100, 0.0, 1.0, 0.0, "C1")); //100uF, Green
    component.push_back(new Inductor(0.020, 0.0, 0.0, 1.0, "L1"));     //20mH, Blue
    component.push_back(new Resistor(10, 1.0, 0.0, 0.0, "R1"));        //10ohm, Red

    fout << setw(12) << "Time"
         << setw(12) << "I";

    for (auto &c : component)
        fout << setw(12) << c->GetName();
    fout << endl;

    double I = 0.0;

    // Run first portion with sinusoidal source
    for (double time = 0.0; time < 0.6 * timeMax; time += T) {
        double V = Vpeak * sin(2.0 * M_PI * freq * time);
        fout << setw(12) << time;
        CostFunctionV(I, V);
        xpos++;
    }

    // Run remaining portion with zero source (decay)
    for (double time = 0.6 * timeMax; time < timeMax; time += T) {
        double V = 0.0;
        fout << setw(12) << time;
        CostFunctionV(I, V);
        xpos++;
    }
}

void AnalogCircuit::CostFunctionV(double &current, double voltage) {
    double I1 = current;
    double J1 = 0.0;
    double J0 = 0.0;
    double alpha = 1.0;

    do {
        double sumVoltage = 0.0;
        for (auto &c : component) {
            double Vcomponent = c->GetVoltage(I1, T);
            sumVoltage += Vcomponent;
        }
        J1 = sumVoltage - voltage;

        //Adjust search step
        if ((abs(J0 - J1) != (J0 - J1)) || J0 == J1)
            alpha /= 2.0;

        if (abs(J1) > tolerance) {
            if (J1 < 0)
                I1 += alpha;
            else
                I1 -= alpha;
            J0 = J1;
        }
        if (alpha < tolerance / 1000000.0) {
            alpha = ((double)rand() / (RAND_MAX)) + 0.5; // between 0.5 and 1.5
        }
    } while (abs(J1) > tolerance);

    fout << setw(12) << I1;
    for (auto &c : component) {
        fout << setw(12) << c->GetVoltage(I1, T);
        ypos = (windowHeight * c->GetVoltage(I1, T) / Vpeak) / 2.0 + scalingFactor * windowHeight / 2.0;
        c->Display();
        c->Update();
    }
    fout << endl;

    current = I1;
}

AnalogCircuit::~AnalogCircuit() {
    for (auto &c : component) {
        delete c;
    }
    component.clear();
    if (fout.is_open())
        fout.close();
}
