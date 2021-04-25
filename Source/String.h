#pragma once
/**
 Class to create instances of a harmonic sine oscillator
 By setting the fundamental frequency, number of harmonics and strength,
 multiple sine tone harmonics can be created

 Uses Oscillator.h objects to create sine tones
*/
#include<math.h>
#include<vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Note {

public:
	/*
	*
	Process returns the signal of the string for each sample using FDTD
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

	void updateGrid() {
		for (int l = lstart; l < lend; l++) {
			u0[l] = 2 * u1[l] - u2[l] + pow(lambda, 2) * (u1[l - 1] - 2 * u1[l] + u1[l + 1]) 
				- pow(mu, 2) * (u1[l + 2] - 4 * u1[l + 1] + 6 * u1[l] - 4 * u1[l - 1] + u1[l - 2]);
		}

	}

	void updateBoundary() {
		u0[0] = 2 * u1[0] - u2[0] + pow(lambda, 2) * (-2 * u1[0] + u1[1])
			- pow(mu, 2) * (u1[2] - 4 * u1[1] + 5 * u1[0]);

		u0[1] = 2 * u1[1] - u2[1] + pow(lambda, 2) * (u1[0] - 2 * u1[1] + u1[2])
			- pow(mu, 2) * (u1[3] - 4 * u1[2] + 6 * u1[1] - 4 * u1[0]);

		u0[N - 2] = 2 * u1[N - 2] - u2[N - 2] + pow(lambda, 2) * (u1[N - 3] - 2 * u1[N - 2] + u1[N - 1])
			- pow(mu, 2) * (u1[N - 4] - 4 * u1[N - 3] + 6 * u1[N - 2] - 4 * u1[N - 1]);

		u0[N - 1] = 2 * u1[N - 1] - u2[N - 1] + pow(lambda, 2) * (-2 * u1[N - 1] + u1[N - 2])
			- pow(mu, 2) * (u1[N - 3] - 4 * u1[N - 2] + 5 * u1[N - 1]);
	}

	void addForce() {
		u0[li] += forceCoeff * force;
	}
	/*
	Sets frequency and sampleRate values for each harmonics
	*/
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
	void setLength(int length) {
		L = length;
	}
	float getLength() {
		return L;
	}

	///* Set Force*/
	//void setForce(float f) {
	//	force = f;
	//}


	/* Sets Parameters of String */
	void setParameters(){
		T = 4 * M_PI * rho * pow(L,2) * pow(freq,2) * pow(r,2);
		A = M_PI * pow(r, 2);
		I = 0.25 * M_PI * pow(r,4);

		//sig = 6 * log(10) / T60;% loss parameter(σ)
		K = sqrt(E * I / (rho * A));
		k = 1 / SR;

		forceCoeff = pow(k, 2) / (rho * A * h);

		// Stability condition
		float hmin = sqrt(0.5 * (pow(c, 2) * pow(k, 2) + sqrt(pow(c, 4) * pow(k, 4) + 16 * pow(K, 2) * pow(k, 2))));
		N = floor(L / hmin);
		h = L / N;
		lambda = c * k / h;
		mu = k * K / pow(h, 2);	 

	}

	void initGrid() {
		std::vector<float> temp(N, 0);
		u0 = temp;
		u1 = temp;
		u2 = temp;

		lend = N - 2;
		li = floor(xi * N);
		lo = floor(xo * N);	
	}

	/* Iniitialises grids*/

private:
	float freq;                         // Frequency of note
	float SR = 44100;                   // Sample Rate 
	float r;                            // radius of string
	float L;                            // Length of string
	float T;                            // Tension
	float A;                            // Area of cross section of string
	float I;                            // Moment of inertia
	float sig;                          // Loss Parameter
	float rho = 7860;                   // Density
	float E = 1.95e11;                  // Young's Modulus
		
	float K;							// Stiffness Constant
	float k;							// Timestep

	float c;							// Wavespeed
	float h;							// Grid Spacing
	
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
	float force = 1;
	float xi = 0.3;
	float xo = 0.5;
	float forceCoeff;

};