#pragma once

// ============================================================
//  Autons.h — ZIPPY 2 | Override 2026-2027
// ============================================================
//  Add a new routine: write the function below, then add one
//  line to the AUTONS[] array in Autons.cpp. Nothing else needs
//  to change, GUI.cpp and main.cpp both read from this array.
// ============================================================

struct AutonEntry {
    const char* name;
    void (*run)();
};

void autonNone();
void autonLeft();
void autonRight();
void autonSkills();

extern const AutonEntry AUTONS[];
extern const int AUTON_COUNT;