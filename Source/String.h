#pragma once
/**
 Class to create instances of a string usin finite-difference tim-domain simulation
 Set frequency, radius of string, length of string and T60 time of string
 Then call setParameters() and initGrid() to initialise the grid for the string
 setForce() must also be called to set the input force

 process() can then be called to return the per sample output of the string at location xo 

*/
#include<math.h>
#include<vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class String {

public:
	/*
	*
	Process returns the signal at xo of the string for each sample using FDTD
	*/
	float process() {
		updateGrid();
		updateBoundary();
		addForce();
		float sample = u0[lo];

		u2 = u1;
		u1 = u0;

		return sample;
	}

	/* Updates the grid for n+1 timestep*/
	void updateGrid() {
		for (int l = lstart; l < lend; l++) {
			u0[l] = (2 * u1[l] + param1 * u2[l] + pow(lambda, 2) * (u1[l - 1] - 2 * u1[l] + u1[l + 1]) 
				- pow(mu, 2) * (u1[l + 2] - 4 * u1[l + 1] + 6 * u1[l] - 4 * u1[l - 1] + u1[l - 2]))/param2;
		}

	}

	/* Updates the boundary for n+1 timestep*/
	void updateBoundary() {
		u0[0] = (2 * u1[0] + param1 * u2[0] + pow(lambda, 2) * (-2 * u1[0] + u1[1])
			- pow(mu, 2) * (u1[2] - 4 * u1[1] + 5 * u1[0]))/param2;

		u0[1] = (2 * u1[1] + param1 * u2[1] + pow(lambda, 2) * (u1[0] - 2 * u1[1] + u1[2])
			- pow(mu, 2) * (u1[3] - 4 * u1[2] + 6 * u1[1] - 4 * u1[0]))/param2;

		u0[N - 2] = (2 * u1[N - 2] - param1 * u2[N - 2] + pow(lambda, 2) * (u1[N - 3] - 2 * u1[N - 2] + u1[N - 1])
			- pow(mu, 2) * (u1[N - 4] - 4 * u1[N - 3] + 6 * u1[N - 2] - 4 * u1[N - 1]))/param2;

		u0[N - 1] = (2 * u1[N - 1] - param1 * u2[N - 1] + pow(lambda, 2) * (-2 * u1[N - 1] + u1[N - 2])
			- pow(mu, 2) * (u1[N - 3] - 4 * u1[N - 2] + 5 * u1[N - 1]))/param2;
	}

	/* Adds the input force at coordinate xi*/
	void addForce() {
		u0[li] += forceCoeff * force;
	}

	/*	Sets frequency	*/
	void setFrequency(float f) {
		freq = f;
	}
	float getFrequency() {
		return freq;

	}

	/* Set sample rate*/
	void setsampleRate(float samprate) {
		SR = samprate;
	}
	float getsampleRate() {
		return SR;
	}

	/* Set radius */
	void setRadius(float rad) {
		r = rad;
	}
	float getRadius() {
		return r;
	}

	/* Set Length */
	void setLength(float length) {
		L = length;
	}
	float getLength() {
		return L;
	}

	/* Set Length */
	void setT60(float T60time) {
		T60 = T60time;
	}
	float getT60() {
		return T60;
	}

	/* Set Force*/
	void setForce(float f) {
		force = f;
	}


	/* Sets Parameters of String (call only after length, radius, frequency, sampleRate and T60 time have been set) */
	void setParameters(){

		// Time step (1/sampleRate)
		k = 1 / SR;

		// String Parameters
		T = 4 * M_PI * rho * pow(L,2) * pow(freq,2) * pow(r,2);
		A = M_PI * pow(r, 2);
		I = 0.25 * M_PI * pow(r,4);
		c = sqrt(T / (rho * A));
		
		// Loss Parameters
		sig = 6 * log(10) / T60;
		param1 = sig * k - 1.0f;
		param2 = sig * k + 1.0f;
		K = sqrt(E * I / (rho * A));

		// Stability condition
		hmin = sqrt(0.5 * (pow(c, 2) * pow(k, 2) + sqrt(pow(c, 4) * pow(k, 4) + 16 * pow(K, 2) * pow(k, 2))));
		N = floor(L / hmin);
		h = L / float(N);
		lambda = c * k / h;
		mu = k * K / pow(h, 2);	 

		// Input Force
		forceCoeff = pow(k, 2) / (rho * A * h);

	}

	/* Iniitialises grids (call only after setParameters() has been called)*/
	void initGrid() {
		std::vector<float> temp(N, 0);
		u0 = temp;
		u1 = temp;
		u2 = temp;

		lend = N - 2;
		li = floor(xi * N);
		lo = floor(xo * N);	
	}


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

	float rho = 7860;                   // Density
	float E = 1.95e11;                  // Young's Modulus
		
	float K;							// Stiffness Constant
	float k;							// Timestep

	float c;							// Wavespeed
	float h;							// Grid Spacing
	float hmin;                         // Minimum grid spacing
	
	// Coefficients
	float lambda;						// Courant Number
	float mu;							// Numerical Stiffness Constant

	// Grid Parameters

	std::vector<float> u0;              // State at time n+1
	std::vector<float> u1;				// State at time n
	std::vector<float> u2;				// State at time n-1

	int N;                              // Number of Grid spaces
	int lstart = 2;                     // Start index
	int lend;							// End Index
	int li;								// Index of Excitation
	int lo;								// Index of Output

	// Input force parameters
	float force;                        // Force at current timestep (N)
	float xi = 0.3;                     // Coordinate of excitation (striking/plucking point)(0-1)
	float xo = 0.5;						// Coordinate of output (0-1)
	float forceCoeff;                   // Force Coefficient 

};