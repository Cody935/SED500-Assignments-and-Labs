// AnalogCircuitMain.cpp - Main function for ANASIM

#include <iostream>
#include <GL/glut.h>
#include <algorithm>  // For std::max
#include <cmath>      // For std::abs
#include "AnalogCircuit.h"

using namespace std;

// External global variables
extern int windowWidth; //Width of the OpenGL window
extern int windowHeight; //Height of the OpenGL window
extern double scalingFactor; //Scaling factor for drawing components
extern AnalogCircuit* currentCircuit; // Pointer to current circuit

// External simulation functions
extern bool isSimulationRunning(); // Check if simulation is running
extern bool isSimulationComplete(); // Check if simulation is complete
extern void simulationStep(); // Perform one simulation step

// External history globals for drawing 
extern std::vector<std::vector<double>> voltageHistory;
extern std::vector<double> inputHistory;  
extern std::vector<float> timeHistory;

// Display callback function 
void display() {
    // Clear with black background to match expected
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the circuit visualization
    // no grid in expected image
    AnalogCircuit::drawAxes();

    // FIXED: Draw voltage traces from history if simulation has started
    if (currentCircuit && (isSimulationRunning() || isSimulationComplete()) && !timeHistory.empty()) {
        // Compute dynamic scale factor 
        float maxVoltage = static_cast<float>(currentCircuit->Vpeak);  // Start with Vpeak
        // Compute actual max for better fit 
        float actualMax = static_cast<float>(currentCircuit->Vpeak);
        for (int i = 0; i < 3; ++i) {
            for (double v : voltageHistory[i]) {
                float absV = static_cast<float>(fabs(v));  // Use fabs for double to avoid macro issues
                if (absV > actualMax) actualMax = absV;
            }
        }
        // Include input history in max calculation
        for (double v : inputHistory) {
            float absV = static_cast<float>(fabs(v));  // Use fabs for double
            if (absV > actualMax) actualMax = absV;
        }
        maxVoltage = actualMax * 1.1f;  // Small margin

        float scaleFactor = (windowHeight / 2.0f - 50.0f) / maxVoltage;

        // Component colors: Red (R), Green (C), Blue (L)
        float colors[3][3] = {
            {1.0f, 0.0f, 0.0f},  // Red for R
            {0.0f, 1.0f, 0.0f},  // Green for C
            {0.0f, 0.0f, 1.0f}   // Blue for L
        };

        for (int comp = 0; comp < 3; ++comp) {
            if (voltageHistory[comp].empty()) continue;

            glColor3f(colors[comp][0], colors[comp][1], colors[comp][2]);
            glLineWidth(2.0f);
            glBegin(GL_LINE_STRIP);

            for (size_t j = 0; j < timeHistory.size(); ++j) {
                // X based on time
                float x = 50.0f + (timeHistory[j] / static_cast<float>(currentCircuit->timeMax)) * (windowWidth - 100.0f);

                // Y based on voltage
                float y = (windowHeight / 2.0f) + static_cast<float>(voltageHistory[comp][j]) * scaleFactor;

                // Clamp to bounds with margin
                if (y < 50.0f) y = 50.0f;
                if (y > windowHeight - 50.0f) y = windowHeight - 50.0f;

                glVertex2f(x, y);
            }

            glEnd();
        }
        glLineWidth(1.0f);

        // ADDED: Draw input voltage waveform in white
        if (!inputHistory.empty()) {
            glColor3f(1.0f, 1.0f, 1.0f);  // White for input
            glLineWidth(2.0f);
            glBegin(GL_LINE_STRIP);

            for (size_t j = 0; j < timeHistory.size(); ++j) {
                // X based on time
                float x = 50.0f + (timeHistory[j] / static_cast<float>(currentCircuit->timeMax)) * (windowWidth - 100.0f);

                // Y based on input voltage
                float y = (windowHeight / 2.0f) + static_cast<float>(inputHistory[j]) * scaleFactor;

                // Clamp to bounds with margin
                if (y < 50.0f) y = 50.0f;
                if (y > windowHeight - 50.0f) y = windowHeight - 50.0f;

                glVertex2f(x, y);
            }

            glEnd();
            glLineWidth(1.0f);
        }
    }

    // Display simulation status in white at bottom right corner of the page
    glColor3f(1.0f, 1.0f, 1.0f);
    if (isSimulationRunning()) {
        glRasterPos2f(windowWidth - 200.0f, 30.0f);
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)"Simulation Running...");
    }
    else if (isSimulationComplete()) {
        glRasterPos2f(windowWidth - 150.0f, 30.0f);
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)"Simulation Complete");
    }

    
    // Draw legend at top left
    glColor3f(1.0f, 1.0f, 1.0f);  // White for label
    glRasterPos2f(10.0f, windowHeight - 50.0f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)"C1");
    glColor3f(0.0f, 1.0f, 0.0f);  // Green dash
    glRasterPos2f(30.0f, windowHeight - 50.0f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)"-");

    // L1: white label, blue dash
    glColor3f(1.0f, 1.0f, 1.0f);  // White for label
    glRasterPos2f(10.0f, windowHeight - 70.0f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)"L1");
    glColor3f(0.0f, 0.0f, 1.0f);  // Blue dash
    glRasterPos2f(30.0f, windowHeight - 70.0f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)"-");

    // R1: white label, red dash
    glColor3f(1.0f, 1.0f, 1.0f);  // White for label
    glRasterPos2f(10.0f, windowHeight - 90.0f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)"R1");
    glColor3f(1.0f, 0.0f, 0.0f);  // Red dash
    glRasterPos2f(30.0f, windowHeight - 90.0f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)"-");

    glutSwapBuffers();
}

// Window resize handler
void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
    glMatrixMode(GL_MODELVIEW);

    glutPostRedisplay();  // No prevX/Y reset needed anymore
}

// Idle function for continuous simulation updates
void idle() {
    simulationStep(); // Run simulation steps
    glutPostRedisplay();
}

// Keyboard handler for exit
void keyboard(unsigned char key, int x, int y) {
    if (key == 27 || key == 'q' || key == 'Q') { // ESC or Q
        delete currentCircuit;  
        exit(0);
    }
}

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // Set window parameters like in sample
    windowWidth = 1000;
    windowHeight = 600;
    scalingFactor = 1.0;

    int windowHeightScaled = static_cast<int>(scalingFactor * windowHeight);

    // Create window
    glutInitWindowSize(windowWidth, windowHeightScaled);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("ANASIM");  // Shortened to match image

    // Set OpenGL state 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);

    // Set up orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, static_cast<GLdouble>(windowWidth),
        0.0, static_cast<GLdouble>(windowHeightScaled));

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);

    // Display simulation information
    cout << "ANASIM - Analog Circuit Simulator Version 1.0" << endl;
    cout << "=============================================" << endl;
    cout << "\nCircuit Configuration:" << endl;
    cout << "----------------------" << endl;
    cout << "* Series RLC Circuit" << endl;
    cout << "* Red: Resistor voltage" << endl;
    cout << "* Green: Capacitor voltage" << endl;
    cout << "* Blue: Inductor voltage" << endl;
    cout << "\nStarting simulation..." << endl;

    // Start the simulation 
    start();

    // Enter the GLUT main loop 
    glutMainLoop();

    return 0;
}
