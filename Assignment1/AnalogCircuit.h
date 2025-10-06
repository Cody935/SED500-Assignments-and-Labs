#ifndef _ANALOGCIRCUITH
#define _ANALOGCIRCUITH

#include <fstream>
#include <vector>  // Using vector instead of list for components
#include <string>
#include <Windows.h> // Windows API for timing or console operations
#include <gl/GL.h> //Open GL core functions
#include <gl/GLU.h>   // Open GL utility library
#include <gl/glut.h> // glut for windowing and input
#include <gl/freeglut.h> // FreeGLUT extension for GLUT
#include "Component.h" // User defined component class

// Global variables for graphics
extern int windowWidth; // Width of the OpenGL window
extern int windowHeight; // Height of the OpenGL window
extern double scalingFactor; //Scaling factor for drawing components

// Global voltage history for drawing (now double for precision)
extern std::vector<std::vector<double>> voltageHistory;
extern std::vector<float> timeHistory;

void start(); // Start the simulation
void drawVoltageHistory(); // Draw voltage vs time plot
void simulationStep(); // Perform one simulation step
bool isSimulationRunning(); // Check if simulation is running
bool isSimulationComplete(); // Check if simulation is complete

class AnalogCircuit {
    // Simulation parameters - will be set by user input
    double T; // Time step 
	double tolerance; //Convergence tolerance for iterative methods
	double freq; // Input signal frequency

    // Component values - will be set by user input
	double R_val, L_val, C_val; // Resistance, Inductance, Capacitance

    double I;  // Circuit current
    std::vector<Component*> components;  // FIXED: Changed from std::list for [] access
	std::ofstream fout; //File output stream

public:
    // Simulation state - MADE PUBLIC
    bool simulationRunning; //True while simulation is active
	bool simulationComplete; //True when simulation has finished
	double currentTime; //Current simulation time
	int stepCount; //Number of simulation steps taken
    double timeMax;  // FIXED: Made public for access in display()
    double Vpeak;    // FIXED: Made public for access in display()


	//Constructor with user-defined parameters
    AnalogCircuit(std::string filename, double R, double L, double C,
        double frequency, double peakVoltage, double simTime);

    // Graphics methods
    static void display(float R, float G, float B); //Draw circuit or voltage in color
    static void drawGrid(); // Draw the background grid
    static void drawAxes(); // Draw X and Y axes

    // Simulation methods
	void run(); //run the simulation
    bool runStep(); //Run one time step
    void CostFunctionV(double& current, double voltage, double timestep); //Adjust current based on voltage
    

	// Destructor to clean up components and close file
    ~AnalogCircuit();
};

#endif // _ANALOGCIRCUITH
