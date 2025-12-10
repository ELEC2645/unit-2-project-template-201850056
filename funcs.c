// ELEC2645 Unit 2 Project - Engineering Calculator
// Function Implementations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "funcs.h"

// CONTANTS
#define PI 3.14159265359
#define SPEED_OF_LIGHT 299792458.0f
#define MAX_SAMPLES 50
#define PLOT_WIDTH 60
#define PLOT_HEIGHT 20

// DATA STRUCTURES
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

typedef struct {
    float samples[200];
    int count;
    float amplitude;
    float frequency;
    float sample_rate;
    char waveform_type[20];
} Waveform;

// UTILITY FUNCTIONS

// Get a float value with validation
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

// Get an integer value with validation
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

// Print a separator line
static void print_separator(void) {
    printf("========================================================\n");
}

// Print a header
static void print_header(const char *title) {
    printf("\n");
    print_separator();
    printf("  %s\n", title);
    print_separator();
}

// Save result to file
static void save_result(const char *text) {
    FILE *f = fopen("results.txt", "a");
    if (f != NULL) {
        fprintf(f, "%s\n", text);
        fclose(f);
    }
}

// SIGNAL ANALYSIS FUNCTIONS

// Calculate RMS (Root Mean Square) of a signal
static float calculate_rms(SignalData *sig) {
    if (sig->count == 0) return 0.0f;
    
    float sum_squares = 0.0f;
    for (int i = 0; i < sig->count; i++) {
        sum_squares += sig->values[i] * sig->values[i];
    }
    
    return sqrtf(sum_squares / sig->count);
}

// Calculate peak-to-peak amplitude
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

// Calculate average (DC component)
static float calculate_average(SignalData *sig) {
    if (sig->count == 0) return 0.0f;
    
    float sum = 0.0f;
    for (int i = 0; i < sig->count; i++) {
        sum += sig->values[i];
    }
    
    return sum / sig->count;
}

// FILTER DESIGN FUNCTIONS

// Design RC filter with flexible known parameter
static void design_filter(FilterConfig *filter, int is_lowpass) {
    if (is_lowpass) {
        printf("\nRC Low-Pass Filter Design\n");
        strcpy(filter->type, "Low-Pass");
    } else {
        printf("\nRC High-Pass Filter Design\n");
        strcpy(filter->type, "High-Pass");
    }
    
    printf("Formula: fc = 1 / (2πRC)\n\n");
    
    if (!get_float("Enter cutoff frequency (Hz): ", &filter->cutoff_freq)) return;
    
    if (filter->cutoff_freq <= 0) {
        printf("Frequency must be positive!\n");
        return;
    }
    
    printf("\nWhat is known?\n");
    printf("  1. Resistance (R)\n");
    printf("  2. Capacitance (C)\n");
    
    int known;
    if (!get_int("\nChoice: ", &known)) return;
    
    if (known == 1) {
        // Known: R, calculate C
        if (!get_float("\nEnter resistance (Ω): ", &filter->resistance)) return;
        
        if (filter->resistance <= 0) {
            printf("Resistance must be positive!\n");
            return;
        }
        
        // C = 1 / (2π * R * fc)
        filter->capacitance = 1.0f / (2.0f * PI * filter->resistance * filter->cutoff_freq);
        
    } else if (known == 2) {
        // Known: C, calculate R
        if (!get_float("\nEnter capacitance (F, e.g. 1e-6): ", &filter->capacitance)) return;
        
        if (filter->capacitance <= 0) {
            printf("Capacitance must be positive!\n");
            return;
        }
        
        // R = 1 / (2π * C * fc)
        filter->resistance = 1.0f / (2.0f * PI * filter->capacitance * filter->cutoff_freq);
        
    } else {
        printf("Invalid choice!\n");
        return;
    }
}

// Display filter design results
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

// WAVEFORM GENERATION FUNCTIONS

// Generate sine wave 
static void generate_sine(Waveform *wave) {
    for (int i = 0; i < wave->count; i++) {
        float t = (float)i / wave->sample_rate;
        wave->samples[i] = wave->amplitude * sinf(2.0f * PI * wave->frequency * t);
    }
}

// Generate square wave
static void generate_square(Waveform *wave) {
    for (int i = 0; i < wave->count; i++) {
        float t = (float)i / wave->sample_rate;
        float phase = fmodf(wave->frequency * t, 1.0f);
        wave->samples[i] = (phase < 0.5f) ? wave->amplitude : -wave->amplitude;
    }
}

// Generate triangle wave
static void generate_triangle(Waveform *wave) {
    for (int i = 0; i < wave->count; i++) {
        float t = (float)i / wave->sample_rate;
        float phase = fmodf(wave->frequency * t, 1.0f);
        wave->samples[i] = wave->amplitude * (4.0f * fabsf(phase - 0.5f) - 1.0f);
    }
}

// Generate noise
static void generate_noise(Waveform *wave) {
    for (int i = 0; i < wave->count; i++) {
        wave->samples[i] = wave->amplitude * (2.0f * ((float)rand() / RAND_MAX) - 1.0f);
    }
}

// ASCII visualization of waveform
static void plot_waveform(Waveform *wave) {
    printf("\n");
    print_header("WAVEFORM VISUALIZATION");
    
    // Find min and max for scaling
    float min_val = wave->samples[0];
    float max_val = wave->samples[0];
    for (int i = 1; i < wave->count; i++) {
        if (wave->samples[i] < min_val) min_val = wave->samples[i];
        if (wave->samples[i] > max_val) max_val = wave->samples[i];
    }
    
    // Add margin
    float margin = (max_val - min_val) * 0.1f;
    min_val -= margin;
    max_val += margin;
    
    float range = max_val - min_val;
    if (range < 1e-6f) range = 1.0f;
    
    // Determine samples to plot (subsample if needed)
    int step = (wave->count > PLOT_WIDTH) ? (wave->count / PLOT_WIDTH) : 1;
    int plot_samples = (wave->count + step - 1) / step;
    
    // Plot from top to bottom
    for (int row = 0; row < PLOT_HEIGHT; row++) {
        float y_level = max_val - (row * range / (PLOT_HEIGHT - 1));
        
        printf("|");
        for (int col = 0; col < plot_samples && col * step < wave->count; col++) {
            float sample = wave->samples[col * step];
            
            // Check if sample is at this height level
            float tolerance = range / (PLOT_HEIGHT * 2);
            if (fabsf(sample - y_level) < tolerance) {
                printf("*");
            } else if (row == PLOT_HEIGHT / 2) {
                // Zero line
                printf("-");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
    
    // Print axis
    printf("+");
    for (int i = 0; i < plot_samples; i++) printf("-");
    printf("\n");
    
    printf("\nWaveform: %s, Freq: %.2f Hz, Amp: %.2f, Samples: %d\n", 
           wave->waveform_type, wave->frequency, wave->amplitude, wave->count);
    print_separator();
}

// UNIT CONVERSION FUNCTIONS

// Power conversions
static float dbm_to_mw(float dbm) {
    return powf(10.0f, dbm / 10.0f);
}

static float mw_to_dbm(float mw) {
    return 10.0f * log10f(mw);
}

// Frequency conversions
static float hz_to_rad_s(float hz) {
    return 2.0f * PI * hz;
}

static float rad_s_to_hz(float rad_s) {
    return rad_s / (2.0f * PI);
}

// Temperature conversions 
static float celsius_to_fahrenheit(float c) {
    return (c * 9.0f / 5.0f) + 32.0f;
}

static float fahrenheit_to_celsius(float f) {
    return (f - 32.0f) * 5.0f / 9.0f;
}

// ==================== MENU ITEM IMPLEMENTATIONS ====================

/* Menu Item 1: Unit Converter */
void menu_item_1(void) {
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
            } else if (direction == 2) {
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
            } else if (direction == 2) {
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
        design_filter(&filter, 1);  // Low-pass
    } else if (choice == 2) {
        design_filter(&filter, 0);  // High-pass
    } else {
        printf("Invalid choice!\n");
        return;
    }
    
    display_filter_results(&filter);
}

/* Menu Item 3: Signal Analysis */
void menu_item_3(void) {
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

/* Menu Item 4: Waveform Generator */
void menu_item_4(void) {
    print_header("WAVEFORM GENERATOR");
    
    Waveform wave;
    
    printf("\nSelect waveform type:\n");
    printf("  1. Sine Wave\n");
    printf("  2. Square Wave\n");
    printf("  3. Triangle Wave\n");
    printf("  4. Noise\n");
    
    int type;
    if (!get_int("\nChoice: ", &type)) return;
    
    if (type < 1 || type > 4) {
        printf("Invalid choice!\n");
        return;
    }
    
    printf("\n");
    if (!get_float("Enter amplitude: ", &wave.amplitude)) return;
    if (!get_float("Enter frequency (Hz): ", &wave.frequency)) return;
    if (!get_float("Enter sample rate (Hz): ", &wave.sample_rate)) return;
    if (!get_int("Enter number of samples (10-200): ", &wave.count)) return;
    
    if (wave.count < 10 || wave.count > 200) {
        printf("Sample count must be between 10 and 200!\n");
        return;
    }
    
    if (wave.amplitude <= 0 || wave.frequency <= 0 || wave.sample_rate <= 0) {
        printf("All parameters must be positive!\n");
        return;
    }
    
    // Generate waveform
    switch (type) {
        case 1:
            strcpy(wave.waveform_type, "Sine");
            generate_sine(&wave);
            break;
        case 2:
            strcpy(wave.waveform_type, "Square");
            generate_square(&wave);
            break;
        case 3:
            strcpy(wave.waveform_type, "Triangle");
            generate_triangle(&wave);
            break;
        case 4:
            strcpy(wave.waveform_type, "Noise");
            generate_noise(&wave);
            break;
    }
    
    // Display waveform
    plot_waveform(&wave);
    
    // Save to file
    char result_buf[256];
    snprintf(result_buf, sizeof(result_buf), 
        "Waveform Generated: %s, Freq=%.2f Hz, Amp=%.2f, Samples=%d", 
        wave.waveform_type, wave.frequency, wave.amplitude, wave.count);
    save_result(result_buf);
}