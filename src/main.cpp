#include "main.h"
#include "drivetrain.h"
#include "pros/rtos.hpp"
#include "robot-config.h"
#include "cascade.h"
#include "GUI.h"
#include "DSR.h"
#include "Autons.h"

pros::Task* odomTask = nullptr;
// ============================================================
//  main.cpp — ZIPPY 2 | Override 2026-2027
// ============================================================

void odomDebug(void *) {
  master.clear();
  Lift.set_position(0);
  pros::delay(50);
  chassis.setPose(12, -62, 180); //rightside
  while (true) {
    lemlib::Pose pose = chassis.getPose();
    master.print(0, 0, "X%5.1f Y%5.1f H%5.1f", pose.x, pose.y, pose.theta);
    //master.print(2, 0, "X true:%5.1f Y true:%5.1f", trackX.get_position(),trackY.get_position());
    //master.print(0, 0, "degrees:%5.1f",Lift.get_position()/100.0);
    pros::delay(50);
  }
} ///  print on the brain degrees or pose
void initialize() {

    lift.init();
    
    chassis.calibrate(true); // ~3s IMU calibration
    chassis2.calibrate(true); // ~3s IMU calibration

    DriveL.set_brake_mode_all(pros::E_MOTOR_BRAKE_BRAKE);
    DriveR.set_brake_mode_all(pros::E_MOTOR_BRAKE_BRAKE);

    cascade1.set_brake_mode_all(pros::E_MOTOR_BRAKE_HOLD);
    cascade2.set_brake_mode_all(pros::E_MOTOR_BRAKE_HOLD);

    intake.set_brake_mode_all(pros::E_MOTOR_BRAKE_BRAKE);
    

    

    chassis.setPose(0, 0, 0);
   // reverse.setPose(0, 0, 0);

    odomTask = new pros::Task(odomDebug);
    //GUI_runAutonSelector();
}

void disabled() {
    //GUI_showDebugScreen();
}

void competition_initialize() {}

void autonomous() {

    odomTask = new pros::Task(odomDebug);
    lift.enable();
    lift.init();

    chassis2.setPose(0, 0, 0);
    chassis2.moveToPose(24, 24, 90, 1000, {.lead = 0.5}, false);
    
    //dsr_system.perform_dsr_init(NEG_NEG, 0);

    //autonRed();
}

void opcontrol() {

    odomTask = new pros::Task(odomDebug);

    //GUI_showDebugScreen();

    new pros::Task(DriveTrainControls);
    new pros::Task(CascadeControls);
    new pros::Task(IntakeControls);
    //new pros::Task(wristControls);
    new pros::Task(ClawControls);
    new pros::Task(zerotech);
    new pros::Task(AfterintakeMACRO);
    new pros::Task(matchloaderhight);
}