/*
==============================================================================

String.cpp
Created: 3 May 2021 9:45:00pm
Author:  Ruthu

==============================================================================
*/

#include "String.h"

String::~String() {
	delete[] u0;
	delete[] u1;
	delete[] u2;
}

float String::process() {
	updateGrid();
	updateBoundary();
	addForce();
	float sample = u0[lo];

	// Copy array values after timestep
	float* tempPtr = u2;
	u2 = u1;
	u1 = u0;
	u0 = tempPtr;

	return sample;
}

void String::updateGrid() {
	for (int l = lstart; l < lend; l++) {
		u0[l] = (2 * u1[l] + param1 * u2[l] + lambdasq * (u1[l - 1] - 2 * u1[l] + u1[l + 1])
			- musq * (u1[l + 2] - 4 * u1[l + 1] + 6 * u1[l] - 4 * u1[l - 1] + u1[l - 2])) / param2;
	}

}

void String::updateBoundary() {
	u0[0] = (2 * u1[0] + param1 * u2[0] + lambdasq * (-2 * u1[0] + u1[1])
		- musq * (u1[2] - 4 * u1[1] + 5 * u1[0])) / param2;

	u0[1] = (2 * u1[1] + param1 * u2[1] + lambdasq * (u1[0] - 2 * u1[1] + u1[2])
		- musq * (u1[3] - 4 * u1[2] + 6 * u1[1] - 4 * u1[0])) / param2;

	u0[N - 2] = (2 * u1[N - 2] + param1 * u2[N - 2] + lambdasq * (u1[N - 3] - 2 * u1[N - 2] + u1[N - 1])
		- musq * (u1[N - 4] - 4 * u1[N - 3] + 6 * u1[N - 2] - 4 * u1[N - 1])) / param2;

	u0[N - 1] = (2 * u1[N - 1] + param1 * u2[N - 1] + lambdasq * (-2 * u1[N - 1] + u1[N - 2])
		- musq * (u1[N - 3] - 4 * u1[N - 2] + 5 * u1[N - 1])) / param2;
}

void String::addForce() {
	u0[li] += forceCoeff * force;
}

float String::getFrequency() {
	return freq;
}

void String::setsampleRate(float samprate) {
	SR = samprate;
}

float String::getsampleRate() {
	return SR;
}

float String::getRadius() {
	return r;
}

float String::getLength() {
	return L;
}

float String::getT60() {
	return T60;
}

void String::setForce(float f) {
	force = f;
}

void String::setParameters(float frequencyInHz, float lengthInMetres, float radiusInMetres, float T60InSeconds) {
	freq = frequencyInHz;
	L = lengthInMetres;
	r = radiusInMetres;
	T60 = T60InSeconds;

	// Time step (1/sampleRate)
	k = 1 / SR;

	// String Parameters
	T = 4 * M_PI * rho * pow(L, 2) * pow(freq, 2) * pow(r, 2);
	A = M_PI * pow(r, 2);
	I = 0.25 * M_PI * pow(r, 4);
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
	lambdasq = pow((c * k / h), 2);
	musq = k * k * K * K / pow(h, 4);

	// Input Force
	forceCoeff = pow(k, 2) / (rho * A * h);

}

void String::setExcCoordinates(float inCoordinate, float outCoordinate) {
	xi = inCoordinate;
	xo = outCoordinate;
}

void String::setMaterial(float youngsModulus, float density) {
	E = youngsModulus;
	rho = density;
}

void String::initGrid() {

	u0 = new float[N] {0};
	u1 = new float[N] {0};
	u2 = new float[N] {0};

	lend = N - 2;
	li = floor(xi * N);
	lo = floor(xo * N);
}


