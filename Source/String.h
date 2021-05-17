/*
  ==============================================================================
	
	String.h
	Author:  Ruthu Prem Kumar

	 Class to create instances of a string using finite-difference time-domain simulation
	Set parameters of the string using setParameters()
	Set excitation coordinates using setExcCoordinates()
	Set material properties using setMaterial()
	Call initGrid() to initialise the grid for the string
	setForce() must also be called to set the input force for the current sample

	process() can then be called to return the per sample output of the string at location xo 

  ==============================================================================
*/

#pragma once
#include<math.h>
#include<vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class String {

public:

	/* Destructor*/
	~String();

	/* Process returns the signal at xo of the string for each sample using FDTD*/	
	float process();

	/* Updates the grid for n+1 timestep*/
	void updateGrid();

	/* Updates the boundary for n+1 timestep*/
	void updateBoundary();

	/* Adds the input force at coordinate xi*/
	void addForce();

	/* Returns the frequency of the string*/
	float getFrequency();

	/* Set sample rate*/
	void setsampleRate(float samprate);

	/* Returns the sample rate*/
	float getsampleRate();

	/* Returns the radius of the string in metres*/
	float getRadius();

	/* Returns the length of the string in metres*/
	float getLength();

	/* Returns the T60 time of decay in seconds*/
	float getT60();

	/* Set Force*/
	void setForce(float f);

	/* Sets Parameters of String */
	void setParameters(float frequencyInHz, float lengthInMetres, float radiusInMetres, float T60InSeconds);

	/* Sets the coordinates for excitation and output (0-1)*/
	void setExcCoordinates(float inCoordinate, float outCoordinate);
	
	/* Sets the material properties of the string (SI units)*/
	void setMaterial(float youngsModulus, float density);
	
	/* Initialises grids */
	void initGrid();


// Private variables
private:

	float freq;                         // Frequency of note
	float SR;                           // Sample Rate 
	float r;                            // radius of string
	float L;                            // Length of string
	float T;                            // Tension
	float A;                            // Area of cross section of string
	float I;                            // Moment of inertia

	float sig;                          // Loss Parameter
	float T60;							// T60 time
	float param1;
	float param2;

	float rho;				            // Density
	float E;							// Young's Modulus
		
	float K;							// Stiffness Constant
	float k;							// Timestep

	float c;							// Wavespeed
	float h;							// Grid Spacing
	float hmin;                         // Minimum grid spacing
	
	// Coefficients
	float lambdasq;						// Courant Number (squared)
	float musq;							// Numerical Stiffness Constant (squared)

	// Grid Parameters
	float *u0;							// State at time n+1
	float *u1;							// State at time n
	float *u2;							// State at time n-1

	int N;                              // Number of Grid spaces
	int lstart = 2;                     // Start index
	int lend;							// End Index
	int li;								// Index of Excitation
	int lo;								// Index of Output

	// Input force parameters
	float force;                        // Force at current timestep (N)
	float xi;                           // Coordinate of excitation (striking/plucking point)(0-1)
	float xo;							// Coordinate of output (0-1)
	float forceCoeff;                   // Force Coefficient 

};