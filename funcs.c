// ELEC2645 Unit 2 Project - Engineering Calculator
// Function Implementations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "funcs.h"

// ==================== CONSTANTS ====================
#define PI 3.14159265359
#define SPEED_OF_LIGHT 299792458.0f
#define MAX_SAMPLES 50

// ==================== DATA STRUCTURES ====================
typedef struct {
    float values[MAX_SAMPLES];
    int count;
    float sample_rate;
} SignalData;

typedef struct {
    float cutoff_freq;
    float resistance;
    float capacitance;
    char type[20];
} FilterConfig;

// ==================== UTILITY FUNCTIONS ====================

/* Get a float value with validation */
static int get_float(const char *prompt, float *value) {
    char buf[64];
    printf("%s", prompt);
    
    if (!fgets(buf, sizeof(buf), stdin)) {
        return 0;
    }
    
    buf[strcspn(buf, "\r\n")] = '\0';
    
    char *start = buf;
    while (*start == ' ' || *start == '\t')
        start++;

    char *endptr;
    *value = strtof(buf, &endptr);

    while (*endptr == ' ' || *endptr == '\t')
        endptr++;
    
    if (endptr == start || *endptr != '\0') {
        printf("Invalid number!\n");
        return 0;
    }
    
    return 1;
}

/* Get an integer value with validation */
static int get_int(const char *prompt, int *value) {
    char buf[64];
    printf("%s", prompt);
    
    if (!fgets(buf, sizeof(buf), stdin)) {
        return 0;
    }
    
    buf[strcspn(buf, "\r\n")] = '\0';
    
    char *start = buf;
    while (*start == ' ' || *start == '\t')
        start++;

    char *endptr;
    long val = strtol(start, &endptr, 10);

    while (*endptr == ' ' || *endptr == '\t')
        endptr++;

    if (endptr == start || *endptr != '\0') {
        printf("Invalid integer!\n");
        return 0;
    }
    
    *value = (int)val;
    return 1;
}

/* Print a separator line */
static void print_separator(void) {
    printf("========================================================\n");
}

/* Print a header */
static void print_header(const char *title) {
    printf("\n");
    print_separator();
    printf("  %s\n", title);
    print_separator();
}

/* Save result to file */
static void save_result(const char *text) {
    FILE *f = fopen("results.txt", "a");
    if (f != NULL) {
        fprintf(f, "%s\n", text);
        fclose(f);
    }
}

// ==================== SIGNAL ANALYSIS FUNCTIONS ====================

/* Calculate RMS (Root Mean Square) of a signal */
static float calculate_rms(SignalData *sig) {
    if (sig->count == 0) return 0.0f;
    
    float sum_squares = 0.0f;
    for (int i = 0; i < sig->count; i++) {
        sum_squares += sig->values[i] * sig->values[i];
    }
    
    return sqrtf(sum_squares / sig->count);
}

/* Calculate peak-to-peak amplitude */
static float calculate_peak_to_peak(SignalData *sig) {
    if (sig->count == 0) return 0.0f;
    
    float min = sig->values[0];
    float max = sig->values[0];
    
    for (int i = 1; i < sig->count; i++) {
        if (sig->values[i] < min) min = sig->values[i];
        if (sig->values[i] > max) max = sig->values[i];
    }
    
    return max - min;
}

/* Calculate average (DC component) */
static float calculate_average(SignalData *sig) {
    if (sig->count == 0) return 0.0f;
    
    float sum = 0.0f;
    for (int i = 0; i < sig->count; i++) {
        sum += sig->values[i];
    }
    
    return sum / sig->count;
}

// ==================== FILTER DESIGN FUNCTIONS ====================

/* Design RC low-pass filter */
static void design_lowpass(FilterConfig *filter) {
    printf("\nRC Low-Pass Filter Design\n");
    printf("Formula: fc = 1 / (2πRC)\n\n");
    
    if (!get_float("Enter cutoff frequency (Hz): ", &filter->cutoff_freq)) return;
    if (!get_float("Enter resistance (Ω): ", &filter->resistance)) return;
    
    if (filter->cutoff_freq <= 0 || filter->resistance <= 0) {
        printf("Values must be positive!\n");
        return;
    }
    
    // C = 1 / (2π * R * fc)
    filter->capacitance = 1.0f / (2.0f * PI * filter->resistance * filter->cutoff_freq);
    strcpy(filter->type, "Low-Pass");
}

/* Design RC high-pass filter */
static void design_highpass(FilterConfig *filter) {
    printf("\nRC High-Pass Filter Design\n");
    printf("Formula: fc = 1 / (2πRC)\n\n");
    
    if (!get_float("Enter cutoff frequency (Hz): ", &filter->cutoff_freq)) return;
    if (!get_float("Enter capacitance (F, e.g. 1e-6): ", &filter->capacitance)) return;
    
    if (filter->cutoff_freq <= 0 || filter->capacitance <= 0) {
        printf("Values must be positive!\n");
        return;
    }
    
    // R = 1 / (2π * C * fc)
    filter->resistance = 1.0f / (2.0f * PI * filter->capacitance * filter->cutoff_freq);
    strcpy(filter->type, "High-Pass");
}

/* Display filter design results */
static void display_filter_results(FilterConfig *filter) {
    printf("\n");
    print_header("FILTER DESIGN RESULTS");
    
    printf("  Type:           %s\n", filter->type);
    printf("  Cutoff (-3dB):  %.2f Hz\n", filter->cutoff_freq);
    printf("  Resistance:     %.2f Ω", filter->resistance);
    if (filter->resistance > 1000) {
        printf(" (%.2f kΩ)", filter->resistance / 1000.0f);
    }
    printf("\n");
    
    printf("  Capacitance:    %.9f F\n", filter->capacitance);
    if (filter->capacitance >= 1e-6) {
        printf("                  %.2f µF\n", filter->capacitance * 1e6);
    } else if (filter->capacitance >= 1e-9) {
        printf("                  %.2f nF\n", filter->capacitance * 1e9);
    } else {
        printf("                  %.2f pF\n", filter->capacitance * 1e12);
    }
    
    float tau = filter->resistance * filter->capacitance;
    printf("  Time Constant:  %.6f s", tau);
    if (tau < 1e-3) {
        printf(" (%.2f µs)", tau * 1e6);
    } else if (tau < 1.0) {
        printf(" (%.2f ms)", tau * 1e3);
    }
    printf("\n");
    print_separator();
}

// ==================== UNIT CONVERSION FUNCTIONS ====================

/* Power conversions */
static float dbm_to_mw(float dbm) {
    return powf(10.0f, dbm / 10.0f);
}

static float mw_to_dbm(float mw) {
    return 10.0f * log10f(mw);
}

/* Frequency conversions */
static float hz_to_rad_s(float hz) {
    return 2.0f * PI * hz;
}

static float rad_s_to_hz(float rad_s) {
    return rad_s / (2.0f * PI);
}

/* Temperature conversions */
static float celsius_to_fahrenheit(float c) {
    return (c * 9.0f / 5.0f) + 32.0f;
}

static float fahrenheit_to_celsius(float f) {
    return (f - 32.0f) * 5.0f / 9.0f;
}

// ==================== MENU ITEM IMPLEMENTATIONS ====================

/* Menu Item 1: Signal Analysis */
void menu_item_1(void) {
    print_header("SIGNAL ANALYSIS");
    
    SignalData signal;
    signal.count = 0;
    
    printf("\nAnalyze signal properties (RMS, Peak-to-Peak, Average)\n");
    
    if (!get_float("Enter sample rate (Hz): ", &signal.sample_rate)) return;
    if (!get_int("Enter number of samples (1-50): ", &signal.count)) return;
    
    if (signal.count <= 0 || signal.count > MAX_SAMPLES) {
        printf("Invalid sample count!\n");
        return;
    }
    
    printf("\nEnter %d voltage values:\n", signal.count);
    for (int i = 0; i < signal.count; i++) {
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "  V[%d]: ", i);
        if (!get_float(prompt, &signal.values[i])) {
            printf("Aborting.\n");
            return;
        }
    }
    
    // Calculate and display results
    float rms = calculate_rms(&signal);
    float pk_pk = calculate_peak_to_peak(&signal);
    float avg = calculate_average(&signal);
    
    printf("\n");
    print_header("ANALYSIS RESULTS");
    printf("  Samples:        %d\n", signal.count);
    printf("  Sample Rate:    %.2f Hz\n", signal.sample_rate);
    printf("  RMS Value:      %.4f V\n", rms);
    printf("  Peak-to-Peak:   %.4f V\n", pk_pk);
    printf("  Average (DC):   %.4f V\n", avg);
    print_separator();

char buf[256];
snprintf(buf, sizeof(buf), 
    "Signal Analysis: RMS=%.4f V, Peak-to-Peak=%.4f V, Average=%.4f V", rms, pk_pk, avg);
save_result(buf);
}

/* Menu Item 2: RC Filter Designer */
void menu_item_2(void) {
    print_header("RC FILTER DESIGNER");
    
    FilterConfig filter;
    
    printf("\nSelect filter type:\n");
    printf("  1. Low-Pass Filter\n");
    printf("  2. High-Pass Filter\n");
    
    int choice;
    if (!get_int("\nChoice: ", &choice)) return;
    
    if (choice == 1) {
        design_lowpass(&filter);
    } else if (choice == 2) {
        design_highpass(&filter);
    } else {
        printf("Invalid choice!\n");
        return;
    }
    
    display_filter_results(&filter);
}

/* Menu Item 3: Unit Converter */
void menu_item_3(void) {
    print_header("ENGINEERING UNIT CONVERTER");
    
    printf("\nSelect conversion:\n");
    printf("  1. dBm ↔ mW (Power)\n");
    printf("  2. Hz ↔ rad/s (Frequency)\n");
    printf("  3. °C ↔ °F (Temperature)\n");
    
    int type;
    if (!get_int("\nChoice: ", &type)) return;
    
    int direction;
    float input, result;
    
    switch (type) {
        case 1: // Power
            printf("\n1=dBm→mW, 2=mW→dBm: ");
            if (!get_int("", &direction)) return;
            
            if (direction == 1) {
                if (!get_float("Enter power (dBm): ", &input)) return;
                result = dbm_to_mw(input);
                printf("\n%.2f dBm = %.6f mW = %.6f W\n", input, result, result/1000.0f);
            } 
            else if (direction == 2) {
                if (!get_float("Enter power (mW): ", &input)) return;
                result = mw_to_dbm(input);
                printf("\n%.6f mW = %.2f dBm\n", input, result);
            }
            else {
                printf("Invalid direction!\n");
                return;
            }
            break;
            
        case 2: // Frequency
            printf("\n1=Hz→rad/s, 2=rad/s→Hz: ");
            if (!get_int("", &direction)) return;
            
            if (direction == 1) {
                if (!get_float("Enter frequency (Hz): ", &input)) return;
                result = hz_to_rad_s(input);
                printf("\n%.2f Hz = %.4f rad/s\n", input, result);
            } else  if (direction == 2) {
                if (!get_float("Enter angular frequency (rad/s): ", &input)) return;
                result = rad_s_to_hz(input);
                printf("\n%.4f rad/s = %.2f Hz\n", input, result);
            }
            else {
                printf("Invalid direction!\n");
                return;
            }
            break;
            
        case 3: // Temperature
            printf("\n1=°C→°F, 2=°F→°C: ");
            if (!get_int("", &direction)) return;
            
            if (direction == 1) {
                if (!get_float("Enter temperature (°C): ", &input)) return;
                result = celsius_to_fahrenheit(input);
                printf("\n%.2f°C = %.2f°F\n", input, result);
            } else  if (direction == 2) {
                if (!get_float("Enter temperature (°F): ", &input)) return;
                result = fahrenheit_to_celsius(input);
                printf("\n%.2f°F = %.2f°C\n", input, result);
            }
            else {
                printf("Invalid direction!\n");
                return;
            }
            break;
            
        default:
            printf("Invalid choice!\n");
    }
}

/* Menu Item 4: Ohm's Law Calculator */
void menu_item_4(void) {
    print_header("OHM'S LAW CALCULATOR");
    
    printf("\nOhm's Law: V = I × R, P = V × I\n");
    printf("\nWhat do you want to calculate?\n");
    printf("  1. Voltage (V)\n");
    printf("  2. Current (I)\n");
    printf("  3. Resistance (R)\n");
    printf("  4. Power (P)\n");
    
    int choice;
    if (!get_int("\nChoice: ", &choice)) return;
    
    float v, i, r, p;
    
    printf("\n");
    
    switch (choice) {
        case 1: // Calculate V
            if (!get_float("Enter Current (A): ", &i)) return;
            if (!get_float("Enter Resistance (Ω): ", &r)) return;
            
            if (r <= 0) {
                printf("Resistance must be positive!\n");
                return;
            }
            
            v = i * r;
            p = v * i;
            
            print_header("RESULTS");
            printf("  Voltage (V):    %.4f V\n", v);
            printf("  Power (P):      %.4f W (%.2f mW)\n", p, p * 1000.0f);
            print_separator();
            break;
            
        case 2: // Calculate I
            if (!get_float("Enter Voltage (V): ", &v)) return;
            if (!get_float("Enter Resistance (Ω): ", &r)) return;
            
            if (r <= 0) {
                printf("Resistance must be positive!\n");
                return;
            }
            
            i = v / r;
            p = v * i;
            
            print_header("RESULTS");
            printf("  Current (I):    %.4f A (%.2f mA)\n", i, i * 1000.0f);
            printf("  Power (P):      %.4f W (%.2f mW)\n", p, p * 1000.0f);
            print_separator();
            break;
            
        case 3: // Calculate R
            if (!get_float("Enter Voltage (V): ", &v)) return;
            if (!get_float("Enter Current (A): ", &i)) return;
            
            if (i == 0) {
                printf("Current cannot be zero!\n");
                return;
            }
            
            r = v / i;
            p = v * i;
            
            print_header("RESULTS");
            printf("  Resistance (R): %.4f Ω", r);
            if (r > 1000) {
                printf(" (%.2f kΩ)", r / 1000.0f);
            }
            printf("\n");
            printf("  Power (P):      %.4f W (%.2f mW)\n", p, p * 1000.0f);
            print_separator();
            break;
            
        case 4: // Calculate P
            if (!get_float("Enter Voltage (V): ", &v)) return;
            if (!get_float("Enter Current (A): ", &i)) return;
            
            p = v * i;
            
            print_header("RESULTS");
            printf("  Power (P):      %.4f W\n", p);
            printf("                  %.2f mW\n", p * 1000.0f);
            if (p > 0) {
                printf("                  %.2f dBm\n", mw_to_dbm(p * 1000.0f));
            }
            print_separator();
            break;
            
        default:
            printf("Invalid choice!\n");
    }
}