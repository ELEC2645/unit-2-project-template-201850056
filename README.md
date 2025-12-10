[![Open in Codespaces](https://classroom.github.com/assets/launch-codespace-2972f46106e565e64193e422d61a12cf1da4916b45550586e14ef0a7c637dd04.svg)](https://classroom.github.com/open-in-codespaces?assignment_repo_id=21970466)
# ELEC2645 Unit 2 Project 

# 1 Overview

This project is a menu-driven electronic circuit and signal analysis tool. This tool includes four functions: Unit Converter, RC Filter Designer, Signal Analysis, and Waveform Generator. It facilitates efficient and accurate calculations for experimental data, as well as more detailed analysis of signals.

The menu system is implemented in `main.c` and all engineering functions implemented in `funcs.c`, linked through `funcs.h`.


# 2 How to Run the Code 

You can build the code as we have been using in the labs with 
`gcc main.c funcs.c -o main.out -lm` (the `-lm` is required to link the math library). You can also use `make -B` to force a rebuild using the provided `Makefile`.

Then run the code with `./main.out`


# 3 Function Descriptions

Unit Converter: This function performs conversions frequently used in circuit and communications engineering:
dBm - mW (power)
Hz - rad/s (frequency & angular frequency)
Celsius - Fahrenheit (temperature)
This saves computational time and enhances the accuracy of the calculations.

RC Filter Designer: This function first determines the type of filter. By inputting the known cutoff frequency and either capacitance or resistance, it rapidly calculates the other unknown quantity. It supports rapid prototyping and component selection in practical circuit design.

Signal Analysis：When working with measured or simulated signals, engineers often need to calculate RMS, peak-to-peak, and average values to understand signal strength and behaviour.
This function helps users analyse sampled voltage data efficiently without relying on external tools.

Waveform Generator: This generator allows users to create sine, square, triangle, and noise signals with custom parameters, while the ASCII plot provides an intuitive visualisation directly in the terminal.


# 4 Test command

The `test.sh` script is provided to check that your code compiles correctly. This is what the autograder will use to check your submission. You can run it with `bash test.sh` or `./test.sh` or just `make test`. 

