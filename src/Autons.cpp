/*#include "Autons.h"
#include "robot-config.h"
//#include "main.h"
#include "main.cpp"

// ============================================================
//  Autons.cpp — ZIPPY 2 | Override 2026-2027
// ============================================================

void autonRed() {
    chassis.setPose(0, -62, 180);
    chassis.moveToPose(-14,-48 ,90 , 1000, {.forwards = false, .lead = 0.3}, false);
    pros::delay(400);
    lift.setTarget(CascadeLevel::MIDDLE_GOAL, 0);
    chassis.turnToHeading(180, 1000);
    pros::delay(500);
    chassis.moveToPoint(-24,-60,1000);
    chassis.turnToHeading(0, 1000);
    


    // TODO: build the left-side routine once the robot exists
    // and field coordinates are measured.
}

void autonRight() {
    chassis.setPose(0, 0, 0);
    // TODO: build the right-side routine.
}

void autonSkills() {
    chassis.setPose(0, 0, 0);
    // TODO: build the skills routine.
}

// Edit this array to add, remove, or reorder autons.
// Both the selector GUI and autonomous() read directly from it.
const AutonEntry AUTONS[] = {
    {"Do Nothing", autonNone},
    {"Left",       autonLeft},
    {"Right",      autonRight},
    {"Skills",     autonSkills},
};

const int AUTON_COUNT = sizeof(AUTONS) / sizeof(AUTONS[0]);*/