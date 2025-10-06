#ifndef _COMPONENTH
#define _COMPONENTH

#include <string>

class Component {
protected:
    //Color for display
    float Red = 0.0f; 
    float Green = 0.0f;
    float Blue = 0.0f;
	std::string name; //Name of the component

public:
	//Virtual destructor for proper cleanup
    virtual ~Component() {}


    //Pure virtual methods that must be implemented
    virtual std::string GetName() const = 0; //Return component name
    virtual void        Update() = 0; //Update component state
	virtual double      GetVoltage(double current, double timestep) = 0; //Return voltage across  component
    virtual void        Display() = 0; //Render the component
};

#endif // _COMPONENTH