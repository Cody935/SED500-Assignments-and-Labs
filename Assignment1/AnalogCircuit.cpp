// AnalogCircuit.cpp - Implementation file for ANASIM Analog Circuit Simulator

#define _USE_MATH_DEFINES
#include "AnalogCircuit.h" // Include the header file for the AnalogCircuit class
#include "Capacitor.h" // Include the header file for the Capacitor class
#include "Inductor.h" // Include the header file for the Inductor class
#include "Resistor.h" // Include the header file for the Resistor class

#include <cmath> // For math functions like sin, fabs
#include <cstdlib> // For exit()
#include <GL/glut.h> // GLUT for windowing and input
#include <Windows.h> // Windows API for message handling
#undef max  // Undefine Windows max macro to avoid conflict with std::max
#undef min  // Undefine Windows min macro as well
#include <iomanip>
#include <iostream>
#include <fstream>
#include <algorithm>  // For std::max
#include <limits>     // For numeric_limits
#include <ios>        // For std::streamsize
#include <sstream>    // For istringstream if needed

using namespace std;

// Global variables definition
int windowWidth = 1000; // Width of the OpenGL window
int windowHeight = 600; //  Height of the OpenGL window
double scalingFactor = 1.0;     // Scaling factor for drawing components

// Global voltage history storage 
vector<vector<double>> voltageHistory(3);
vector<double> inputHistory;  // Store input voltage history for white trace
vector<float> timeHistory; // Store time history

// Global simulation state
static bool globalSimulationRunning = false; // True while any simulation is active
static bool globalSimulationComplete = false; //    True when simulation has finished
AnalogCircuit* currentCircuit = nullptr; // Pointer to current circuit


// Windows message pump for responsive GUI
static inline void PumpMessages() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

//  ------------------------------------------------------------------------------
void start() {
	// Display simulation information
    cout << "ANASIM - Analog Circuit Simulator" << endl;
    cout << "=================================" << endl;

    // Defaults
	double R = 20.0; // Ohms
	double L = 0.05; // Henries
	double C = 0.00007; // Farads
	double freq = 50.0; // Hz
	double Vpeak = 10.0; // Volts
	double simTime = 0.1; // seconds

    // Get user input for circuit parameters with defaults on blank/empty input
    cout << "Enter resistor value (ohms) [default " << R << "]: "; 
	string inputLine; // input line
    getline(cin, inputLine);
    if (inputLine.empty() || !(istringstream(inputLine) >> R)) {
        R = 20.0;
        cout << "Using default: " << R << endl;
    }
    cout << "Enter inductor value (henries) [default " << L << "]: ";
	getline(cin, inputLine); // input line
    if (inputLine.empty() || !(istringstream(inputLine) >> L)) {
        L = 0.05;
        cout << "Using default: " << L << endl;
    }
    cout << "Enter capacitor value (farads) [default " << C << "]: ";
	getline(cin, inputLine); // input line
    if (inputLine.empty() || !(istringstream(inputLine) >> C)) {
        C = 0.00007;
        cout << "Using default: " << C << endl;
    }
    cout << "Enter frequency (Hz) [default " << freq << "]: ";
	getline(cin, inputLine); // input line
    if (inputLine.empty() || !(istringstream(inputLine) >> freq)) {
        freq = 50.0;
        cout << "Using default: " << freq << endl;
    }
    cout << "Enter peak voltage (V) [default " << Vpeak << "]: ";
	getline(cin, inputLine); // input line
    if (inputLine.empty() || !(istringstream(inputLine) >> Vpeak)) {
        Vpeak = 10.0;
        cout << "Using default: " << Vpeak << endl;
    }
    cout << "Enter simulation time (seconds) [default " << simTime << "]: ";
	getline(cin, inputLine); // input line
    if (inputLine.empty() || !(istringstream(inputLine) >> simTime)) {
        simTime = 0.1;
        cout << "Using default: " << simTime << endl;
    }

	// Display chosen parameters
    cout << "\nStarting simulation with:" << endl;
    cout << "R = " << R << " ohms, L = " << L << " H, C = " << C << " F" << endl;
    cout << "Frequency = " << freq << " Hz, Vpeak = " << Vpeak << " V" << endl;
    cout << "Simulation time = " << simTime << " seconds" << endl;

	// Create the circuit instance
    AnalogCircuit* circuit = new AnalogCircuit("RLC.dat", R, L, C, freq, Vpeak, simTime);
    currentCircuit = circuit;
    circuit->run();
}

//------------------------------------------------------------------------------
// Display method
void AnalogCircuit::display(float /*R*/, float /*G*/, float /*B*/) {
    // No-op: real-time drawing moved to display callback in main
}

//------------------------------------------------------------------------------
// Draw background grid
void AnalogCircuit::drawGrid() {
    // Light gray grid (darker for black background, but commented out in display)
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);

    // Vertical lines
    for (int x = 50; x < windowWidth; x += 50) {
        glVertex2f((float)x, 0.0f);
        glVertex2f((float)x, (float)windowHeight);
    }

    // Horizontal lines  
    for (int y = 50; y < windowHeight; y += 50) {
        glVertex2f(0.0f, (float)y);
        glVertex2f((float)windowWidth, (float)y);
    }

    glEnd();
}

//------------------------------------------------------------------------------
void AnalogCircuit::drawAxes() {
    // WHITE axes to show on black background
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);

    // Y-axis
    glVertex2f(50.0f, 0.0f);
    glVertex2f(50.0f, (float)windowHeight);

    // X-axis
    glVertex2f(0.0f, (float)windowHeight / 2.0f);
    glVertex2f((float)windowWidth, (float)windowHeight / 2.0f);

    glEnd();
    glLineWidth(1.0f);

    // Labels 
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(30.0f, (float)windowHeight / 2.0f - 15.0f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)"0V");

    glRasterPos2f(30.0f, (float)windowHeight - 30.0f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)"+V");

    glRasterPos2f(30.0f, 30.0f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)"-V");

    glRasterPos2f((float)windowWidth - 50.0f, (float)windowHeight / 2.0f - 15.0f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)"Time");
}

//------------------------------------------------------------------------------
// Draw voltage history
void drawVoltageHistory() {
    // This is now handled by real-time display
}

//------------------------------------------------------------------------------
// Destructor to clean up components and close file
AnalogCircuit::AnalogCircuit(string filename, double R, double L, double C,
    double frequency, double peakVoltage, double simTime)
    : R_val(R), L_val(L), C_val(C), freq(frequency), Vpeak(peakVoltage), timeMax(simTime) {

    fout.open(filename);
    if (!fout.is_open()) {
        cerr << "Error: Could not open output file " << filename << endl;
        exit(1);
    }

    // Set simulation parameters
    T = 0.0001;  // Reasonable time step
    tolerance = 0.001;
    I = 0.0;

    // Simulation state
    simulationRunning = false;
    simulationComplete = false;
    currentTime = 0.0;
    stepCount = 0;

    // FIXED: Initialize voltage history properly
    for (auto& vec : voltageHistory) vec.clear();
    timeHistory.clear();
    inputHistory.clear();

    // Create components with user values and updated names
    components.push_back(new Resistor(R_val, 1.0f, 0.0f, 0.0f, "R1"));      // Red
    components.push_back(new Capacitor(C_val, 0.0f, 1.0f, 0.0f, "C1"));     // Green
    components.push_back(new Inductor(L_val, 0.0f, 0.0f, 1.0f, "L1"));      // Blue
}

//------------------------------------------------------------------------------
void AnalogCircuit::CostFunctionV(double& current, double voltage, double timestep) {
    double I1 = current;        // Predicted current
    double J1 = 0.0, J0 = 0.0; // Current and previous cost
    double alpha = 0.01;        // Reduced for better convergence 
    int iterations = 0;
    const int maxIterations = 1000;

    // Heuristic iteration to minimize cost function
    do {
        PumpMessages();
        iterations++;

        // Calculate sum of component voltages for current guess
        double sumV = 0.0;
        for (auto& c : components) {
            sumV += c->GetVoltage(I1, timestep);
        }

        // Cost = difference between applied voltage and sum of component voltages
        J1 = sumV - voltage;

        // Adjust step size if not making progress
        if ((fabs(J0 - J1) != (J0 - J1)) || J0 == J1) {
            alpha /= 2.0;
        }

        // Adjust current guess based on cost
        if (fabs(J1) > tolerance) {
            if (J1 < 0) I1 += alpha;  // Increase current if sumV < voltage
            else        I1 -= alpha;  // Decrease current if sumV > voltage
            J0 = J1;
        }

        // Reset alpha if it becomes too small
        if (alpha < tolerance / 1e6) {
            alpha = 0.01; // Reset to initial value
        }

        // Safety check to prevent infinite loops
        if (iterations > maxIterations) {
            if (fabs(J1) > 0.1) {
                cout << "Warning: Max iterations reached. Error: " << J1 << endl;
            }
            break;
        }

    } while (fabs(J1) > tolerance);

    current = I1;
}

//------------------------------------------------------------------------------
bool AnalogCircuit::runStep() {
    if (currentTime >= timeMax) {
        simulationComplete = true;
        simulationRunning = false;
        return false;
    }

    PumpMessages();

    // Apply sinusoidal voltage for first part, then 0V (as in sample) - this causes decay
    double V_input = (currentTime < 0.6 * timeMax) ?
        Vpeak * sin(2.0 * M_PI * freq * currentTime) : 0.0;

    // Use heuristic cost function to find current
    CostFunctionV(I, V_input, T);

    // Get component pointers for updates
    Capacitor* capacitor = nullptr;
    Inductor* inductor = nullptr;
    for (auto comp : components) {
        if (dynamic_cast<Capacitor*>(comp)) capacitor = dynamic_cast<Capacitor*>(comp);
        if (dynamic_cast<Inductor*>(comp)) inductor = dynamic_cast<Inductor*>(comp);
    }

    // Compute voltages for output and history BEFORE state updates
    double vR = components[0]->GetVoltage(I, T);
    double vC = components[1]->GetVoltage(I, T);
    double vL = components[2]->GetVoltage(I, T);

    // Store for file output
    fout << setw(12) << currentTime << setw(12) << I
        << setw(12) << vR << setw(12) << vC << setw(12) << vL << endl;

    // Store for history
    timeHistory.push_back(static_cast<float>(currentTime));
    inputHistory.push_back(V_input);  
    voltageHistory[0].push_back(vR);
    voltageHistory[1].push_back(vC);
    voltageHistory[2].push_back(vL);

    // ADDED: Debug print every 100 steps to confirm non-zero voltages (remove if not needed)
    if (stepCount % 100 == 0) {
        cout << "Step " << stepCount << ": vR=" << vR << ", vC=" << vC << ", vL=" << vL << endl;
    }

    // Update states
    if (capacitor) {
        capacitor->UpdateVoltage(I, T);
    }
    if (inductor) {
        inductor->SetCurrent(I);
    }
	// Resistor has no state to update
    currentTime += T;
    stepCount++;

    return true;
}

//------------------------------------------------------------------------------
void AnalogCircuit::run() {
    // File header
    fout << setw(12) << "Time" << setw(12) << "Current";
    for (auto& c : components) fout << setw(12) << c->GetName();
    fout << endl;

    cout << "Running simulation..." << endl;

    // Set up real-time simulation
    simulationRunning = true;
    simulationComplete = false;
    currentCircuit = this;
    globalSimulationRunning = true;
    globalSimulationComplete = false;
}

//------------------------------------------------------------------------------
// Check if simulation is running
bool isSimulationRunning() {
    return globalSimulationRunning && currentCircuit && currentCircuit->simulationRunning;
}
//------------------------------------------------------------------------------
// Check if simulation is complete
bool isSimulationComplete() {
    return globalSimulationComplete || (currentCircuit && currentCircuit->simulationComplete);
}

//------------------------------------------------------------------------------
// Perform one simulation step
void simulationStep() {
    if (!currentCircuit) return;

    if (currentCircuit->runStep()) {
        // Simulation continues
        glutPostRedisplay();
    }
    else {
        // Simulation complete
        globalSimulationRunning = false;
        globalSimulationComplete = true;
        cout << "Simulation completed. " << currentCircuit->stepCount << " time steps executed." << endl;
        cout << "Data written to RLC.dat" << endl;
        glutPostRedisplay(); // Final update
    }
}

//------------------------------------------------------------------------------
// Destructor to clean up components and close file
AnalogCircuit::~AnalogCircuit() {
    for (auto& c : components) delete c;
    components.clear();
    if (fout.is_open()) fout.close();
    currentCircuit = nullptr;
}

//------------------------------------------------------------------------------
void Capacitor::Display() {
}

void Inductor::Display() {
}

void Resistor::Display() {
}
