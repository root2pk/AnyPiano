/*
  ==============================================================================

    Counter.h
    Created: 27 Apr 2021 1:56:05am
    Author:  Ruthu

    Class to create an instance of a basic counter object 
    Counter can be set (default 0), returned and incremented

  ==============================================================================
*/

#pragma once

class Counter {
public:
    /* Increment the counter +1*/
    void incCounter() {
        counter++;
    }
    /* Set the counter value*/
    void setCounter(int c) {
        counter = c;
    }
    /*Get the counter value*/
    int getCounter() {
        return counter;
    }
private:
    int counter = 0;
};