#pragma once

#include "main.h"

#include "pros/adi.hpp"

// Controllers
extern pros::Controller master;
extern pros::Controller partner;

// ============================================================
// Drivetrain Motors

extern pros::Motor L1;
extern pros::Motor L2;
extern pros::Motor L3;

extern pros::Motor R1;
extern pros::Motor R2;
extern pros::Motor R3;

// Drivetrain motor groups
extern pros::MotorGroup DriveL;
extern pros::MotorGroup DriveR;
extern pros::MotorGroup DriveL_REVERSE;
extern pros::MotorGroup DriveR_REVERSE;

// ============================================================
// Mechanism Motors
// ============================================================

// Cascade
// Two blue-cartridge motors used to power the cascade mechanism.
extern pros::Motor cascade1;
extern pros::Motor cascade2;
extern pros::MotorGroup cascade;

// Intake
extern pros::Motor intake;

// Legacy / unused four-bar declarations
//extern pros::Motor FourBar1;
//extern pros::Motor FourBar2;
//extern pros::MotorGroup FourBar;

// ============================================================
// Pneumatics
// ============================================================

extern pros::adi::Pneumatics claw;
extern pros::adi::Pneumatics wrist;
// ============================================================
// Sensors
// ============================================================

extern pros::Imu imu;             // IMU — port assignment in robot-config.cpp
extern pros::Rotation trackY;     // Vertical tracking wheel
extern pros::Rotation trackX;     // Horizontal tracking wheel
extern pros::Rotation Lift;       // cascade tracking wheel

// ============================================================
// LemLib
// ============================================================

// Driver-control Expo curves.
// These are used by the LemLib chassis for arcade drive.
extern lemlib::ExpoDriveCurve throttle_curve;
extern lemlib::ExpoDriveCurve steer_curve;

// LemLib chassis
extern lemlib::Chassis chassis;
extern lemlib::Chassis reverse;
