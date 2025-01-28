#include <Arduino.h>

// Pin definitions
const int ROW_PINS[] = { A0, A1, A2, A3, A4 };  // Analog input pins for rows
const int COL_PINS[] = { 2, 3, 4, 5, 6 };      // Digital output pins for columns
const int NUM_ROWS = 5;  // Total number of rows
const int NUM_COLS = 5;  // Total number of columns

// Arrays to store positional values for rows (tx) and columns (ty)
const float tx[] = { 0.1, 0.25, 0.5, 0.75, 1 };  // Row values
const float ty[] = { 0.1, 0.25, 0.5, 0.75, 1 };  // Column values

// Arrays to store sensor readings
int currentValues[NUM_ROWS][NUM_COLS];   // Current readings
int baselineValues[NUM_ROWS][NUM_COLS];  // Calibration baseline
const int NUM_CALIBRATION_SAMPLES = 20;  // Number of samples for calibration

// Pressure detection constants
const int PRESSURE_THRESHOLD = 75;  // Threshold value to detect a press
const int MAX_PRESSURE_VALUE = 1023; // Maximum analog value (10-bit ADC)

// Debouncing and state tracking
bool isPressed = false;           // Tracks if any key is pressed
const int debounceDelay = 60;     // Delay in milliseconds for debouncing
const int stableReadingsThreshold = 5;  // Number of stable readings required
int stableCount = 0;              // Counter for stable readings

void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud rate

  // Set row pins as INPUT
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(ROW_PINS[r], INPUT);
  }

  // Set column pins as OUTPUT and initialize them LOW
  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(COL_PINS[c], OUTPUT);
    digitalWrite(COL_PINS[c], LOW);
  }

  delay(1000);  // Delay for stability during setup
  calibrate();  // Calibrate baseline values
}

void loop() {
  readMat();  // Read current values from the matrix

  // Loop through all rows and columns to detect pressure
  boolean somethingChanged=false;
  for (int r = 0; r < NUM_ROWS; r++) {
    for (int c = 0; c < NUM_COLS; c++) {
      // Calculate the difference between current and baseline values
      int difference = abs(currentValues[r][c] - baselineValues[r][c]);

      // If the pressure difference exceeds the threshold
      if (difference > PRESSURE_THRESHOLD) {
        // Print the tx (row) value and ty (column) value to the Serial Monitor
        Serial.println(tx[r]);  // Print the corresponding row value
        Serial.println(ty[c]);  // Print the corresponding column value
        somethingChanged=true;
        // delay(100);  // Short delay to avoid flooding the serial monitor
      }
    }
  }
  if (somethingChanged==false) Serial.println(0);

  delay(10);  // Small delay for stability
}

void calibrate() {
  // Initialize baseline values to 0
  for (int r = 0; r < NUM_ROWS; r++) {
    for (int c = 0; c < NUM_COLS; c++) {
      baselineValues[r][c] = 0;
    }
  }

  // Take multiple readings to calculate the baseline
  for (int sample = 0; sample < NUM_CALIBRATION_SAMPLES; sample++) {
    for (int c = 0; c < NUM_COLS; c++) {
      digitalWrite(COL_PINS[c], HIGH);  // Set current column HIGH
      delay(10);  // Allow the readings to stabilize

      // Read all rows for the current column
      for (int r = 0; r < NUM_ROWS; r++) {
        baselineValues[r][c] += analogRead(ROW_PINS[r]);
      }

      digitalWrite(COL_PINS[c], LOW);  // Reset the column to LOW
    }
  }

  // Calculate the average for the baseline
  for (int r = 0; r < NUM_ROWS; r++) {
    for (int c = 0; c < NUM_COLS; c++) {
      baselineValues[r][c] /= NUM_CALIBRATION_SAMPLES;  // Average the samples
      Serial.print("Baseline R");
      Serial.print(r + 1);
      Serial.print("C");
      Serial.print(c + 1);
      Serial.print(": ");
      Serial.println(baselineValues[r][c]);  // Print baseline values
    }
  }
  Serial.println("Calibration complete!");  // Indicate calibration is done
}

void readMat() {
  // Read values for each row and column
  for (int c = 0; c < NUM_COLS; c++) {
    digitalWrite(COL_PINS[c], HIGH);  // Activate current column
    delay(10);  // Allow the readings to stabilize

    // Read all rows for the active column
    for (int r = 0; r < NUM_ROWS; r++) {
      currentValues[r][c] = analogRead(ROW_PINS[r]);
    }

    digitalWrite(COL_PINS[c], LOW);  // Deactivate column
  }
}