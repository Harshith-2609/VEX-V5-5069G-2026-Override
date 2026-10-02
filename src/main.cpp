#include "main.h"
#include "drivetrain.h"
#include "pros/rtos.hpp"
#include "robot-config.h"
#include "cascade.h"
#include "GUI.h"
#include "DSR.h"
//#include "Autons.h"

pros::Task* odomTask = nullptr;
// ============================================================
//  main.cpp — ZIPPY 2 | Override 2026-2027
// ============================================================

// CascadeController lift(
//     cascade,                        // your existing MotorGroup
//     16,                             // "Lift" rotation sensor port
//     {0, 200, 225, 1450, 1800},    // ZERO, START, LOW, MIDDLE, HIGH
//     800.0,                           // degrees of lift per pin already in the hole
//     0.9, 0.0, 0.0                  // kP, kI, kD
// );

dsr_sensor north({-6, 0}, 17);//
dsr_sensor east({5.5, -1.5}, 13);//
dsr_sensor south({-6, -3}, 14);//
dsr_sensor west({-5, -3}, 15);   

dsr_chassis dsr_system(&chassis, {&north, &east, &south, &west});



void odomDebug(void *) {
  master.clear();
  Lift.set_position(0);
  pros::delay(50);
  while (true) {
    lemlib::Pose pose = chassis.getPose();
    //master.print(0, 0, "X%5.1f Y%5.1f H%5.1f", pose.x, pose.y, pose.theta);
   // master.print(2, 0, "X true:%5.1f Y true:%5.1f", trackX.get_position(),trackY.get_position());
    master.print(0, 0, "degrees:%5.1f",Lift.get_position()/100.0);
    pros::delay(50);
  }
}
void initialize() {

    lift.init();
    
    chassis.calibrate(true); // ~3s IMU calibration
    //reverse.calibrate(true); // ~3s IMU calibration

    DriveL.set_brake_mode_all(pros::E_MOTOR_BRAKE_BRAKE);
    DriveR.set_brake_mode_all(pros::E_MOTOR_BRAKE_BRAKE);

    // DriveL_REVERSE.set_brake_mode_all(pros::E_MOTOR_BRAKE_BRAKE);
    // DriveR_REVERSE.set_brake_mode_all(pros::E_MOTOR_BRAKE_BRAKE);

    cascade1.set_brake_mode_all(pros::E_MOTOR_BRAKE_HOLD);
    cascade2.set_brake_mode_all(pros::E_MOTOR_BRAKE_HOLD);

    intake.set_brake_mode_all(pros::E_MOTOR_BRAKE_BRAKE);
    

    

    chassis.setPose(0, 0, 0);
   // reverse.setPose(0, 0, 0);

    odomTask = new pros::Task(odomDebug);
    //GUI_runAutonSelector();
}

void autonRed() {
    

    chassis.setPose(12, -62, 180); // set this
    intake.move(127);
    chassis.moveToPoint(12, -54, 400, {.forwards = false}, false); /// toggle
    chassis.moveToPoint(12, -72, 400, {.forwards = true}, false); /// toggle
    intake.brake();
    chassis.moveToPoint(12, -54, 400, {.forwards = false}, false); /// toggle
    chassis.moveToPoint(12, -72, 400, {.forwards = true}, false); /// toggle
    lift.setTarget(CascadeLevel::LOW_GOAL, 0);
    chassis.moveToPoint(12, -54, 400); 
    chassis.moveToPose(28,-48 ,270 , 850, {.forwards = false, .lead = 0.2}, false); /// into goal
    // chassis.moveToPoint(26, -48, 400);
    pros::delay(200);
    lift.setTarget(CascadeLevel::ZERO);
    pros::delay(400);
    claw.extend();
    chassis.moveToPoint(10,-50 , 1000);
    chassis.turnToHeading(190, 1000);

    // chassis.moveToPoint(25, -38, 1000, {.forwards = false}, false);
    lift.setTarget(CascadeLevel::START);
    ///TUNE
    chassis.moveToPoint(22,-27, 1000, {.forwards = false, .maxSpeed = 55}, true);
    ////TUNE
    pros::delay(1500);//680
    claw.retract(); //////////////////////////////////////////////////////////// stack one grab
    pros::delay(500);
    lift.setTarget(CascadeLevel::LOW_GOAL, 2);
    chassis.swingToHeading(315, DriveSide ::RIGHT, 1000);
    chassis.moveToPoint(21.5,-46.5, 1050, {.forwards = false}, false);/////////to goal

    // // chassis.moveToPoint(18, -43, 1000, {.forwards = false}, false);
    // lift.setTarget(CascadeLevel::LOW_GOAL, 1);
    // pros::delay(200);
    // claw.extend();
    // pros::delay(300);
    chassis.swingToHeading(45, DriveSide::RIGHT, 1000);
    // chassis.turnToHeading(285, 1000);
    lift.setTarget(CascadeLevel::ZERO, 0);
    // ///////TUNE
    // chassis.moveToPoint(41.5, -46, 900, {.forwards = false, .minSpeed = 35}, true);
    // ///////tUNE
    // pros::delay(1000);
    // claw.retract();
    // pros::delay(500);
    // lift.setTarget(CascadeLevel::LOW_GOAL, 2);
    // chassis.turnToHeading(80, 1000);
    // chassis.moveToPoint(32, -47, 1000, {.forwards = false, .minSpeed = 70}, false);
    // pros::delay(500);
    // lift.setTarget(CascadeLevel::LOW_GOAL, 1);
    // claw.extend();
    // chassis.moveToPoint(26, -54, 1000);
    // lift.setTarget(CascadeLevel::ZERO);

    //extention
    // pros::delay(800);
    // claw.extend();
    // chassis.turnToHeading(75, 1000);
    // chassis.moveToPoint(30.4, -45, 1000);
















    // /////////////////////////////////////////////////////////////////////////////////////////
    // /////////////////////////////////////////////////////////////////////////////////////////
    // chassis.moveToPoint(0, -45, 1000, {.forwards = true}, false);
    // chassis.turnToHeading(300, 1000, {}, false);
    // chassis.moveToPoint(21, -62, 1000, {.forwards = false}, false);
    //chassis.moveToPose(24, -64, 0, 3000, {.forwards = false, .minSpeed = 40}, false);
    // lift.setTarget(CascadeLevel::START);
    // chassis.turnToHeading(45, 1000);
    // chassis.moveToPoint(-33.6, -20.6, 1000, {.forwards = false, .maxSpeed = 70}, true);
    // pros::delay(400);
    // claw.retract();
    // pros::delay(400);
    // lift.setTarget(CascadeLevel::LOW_GOAL, 3);
    // chassis.turnToHeading(63, 1000);
    // pros::delay(400);
    //we need to coordinates for going back into goal



    


    // TODO: build the left-side routine once the robot exists
    // and field coordinates are measured.
}
void autonRednew() {
    

    chassis.setPose(12, -62, 180); // set this
    intake.move(127);
    chassis.moveToPoint(12, -54, 400, {.forwards = false}, false); /// toggle
    chassis.moveToPoint(12, -72, 400, {.forwards = true}, false); /// toggle
    intake.brake();
    chassis.moveToPoint(12, -54, 400, {.forwards = false}, false); /// toggle
    chassis.moveToPoint(12, -72, 400, {.forwards = true}, false); /// toggle
    lift.setTarget(CascadeLevel::LOW_GOAL, 0);
    chassis.moveToPoint(12, -54, 400); 
    chassis.moveToPose(28,-48 ,270 , 850, {.forwards = false, .lead = 0.2}, false); /// into goal
    // chassis.moveToPoint(26, -48, 400);
    pros::delay(200);
    lift.setTarget(CascadeLevel::ZERO);
    pros::delay(400);
    claw.extend();
    pros::delay(100);
    chassis.moveToPoint(10,-50 , 1000);
    chassis.turnToHeading(190, 1000);

    // chassis.moveToPoint(25, -38, 1000, {.forwards = false}, false);
    lift.setTarget(CascadeLevel::START);
    ///TUNE
    chassis.moveToPoint(24,-27, 1200, {.forwards = false, .maxSpeed = 55}, true);
    ////TUNE
    pros::delay(1350);//680
    claw.retract(); //////////////////////////////////////////////////////////// stack one grab
    pros::delay(500);
    lift.setTarget(CascadeLevel::LOW_GOAL, 2);
    chassis.swingToHeading(315, DriveSide ::RIGHT, 1000);
    chassis.moveToPoint(22,-46, 1050, {.forwards = false}, false);/////////to goal

    // chassis.moveToPoint(18, -43, 1000, {.forwards = false}, false);
    lift.setTarget(CascadeLevel::LOW_GOAL, 1);
    pros::delay(200);
    claw.extend();
    pros::delay(300);
    chassis.swingToHeading(45, DriveSide::RIGHT, 1000);
    chassis.turnToHeading(285, 1000);
    lift.setTarget(CascadeLevel::START, 0);
    ///////TUNE
    chassis.moveToPoint(44, -47, 800, {.forwards = false, .minSpeed = 35}, true);
    ///////tUNE
    pros::delay(1200);
    claw.retract();
    pros::delay(500);
    lift.setTarget(CascadeLevel::LOW_GOAL, 1); // was for second arg "2"
    chassis.turnToHeading(170, 1000);
    chassis.moveToPoint(50, -29, 1000, {.forwards = false, .minSpeed = 70}, false);
    pros::delay(500);
    lift.setTarget(CascadeLevel::LOW_GOAL, 0);
    claw.extend();

    //extention
    // pros::delay(800);
    // claw.extend();
    // chassis.turnToHeading(75, 1000);
    // chassis.moveToPoint(30.4, -45, 1000);
















    // /////////////////////////////////////////////////////////////////////////////////////////
    // /////////////////////////////////////////////////////////////////////////////////////////
    // chassis.moveToPoint(0, -45, 1000, {.forwards = true}, false);
    // chassis.turnToHeading(300, 1000, {}, false);
    // chassis.moveToPoint(21, -62, 1000, {.forwards = false}, false);
    //chassis.moveToPose(24, -64, 0, 3000, {.forwards = false, .minSpeed = 40}, false);
    // lift.setTarget(CascadeLevel::START);
    // chassis.turnToHeading(45, 1000);
    // chassis.moveToPoint(-33.6, -20.6, 1000, {.forwards = false, .maxSpeed = 70}, true);
    // pros::delay(400);
    // claw.retract();
    // pros::delay(400);
    // lift.setTarget(CascadeLevel::LOW_GOAL, 3);
    // chassis.turnToHeading(63, 1000);
    // pros::delay(400);
    //we need to coordinates for going back into goal



    


    // TODO: build the left-side routine once the robot exists
    // and field coordinates are measured.
}

void disabled() {
    //GUI_showDebugScreen();
}

void competition_initialize() {
    // Selector screen stays up from initialize() until the match starts.
}

void autonomous() {
    odomTask = new pros::Task(odomDebug);
    lift.enable();
    lift.init();


    autonRednew();


    // chassis.turnToHeading(90, 1000);
    // chassis.turnToHeading(180, 1000);
    // chassis.turnToHeading(0, 1000);



    //chassis.moveToPoint(0.0, 24.0, 1000);

    //chassis.moveToPoint(0.0, 0.0, 1000);
    // if (selectedAuton >= 0 && selectedAuton < AUTON_COUNT) {
    //     AUTONS[selectedAuton].run();
    // }

    // Drive to the middle goal with lemlib as usual
    // chassis.moveToPose(...);
 
    // First goal, hole is empty
    //lift.setTarget(CascadeLevel::MIDDLE_GOAL, 1);
    // lift.waitUntilSettled();
    // ... place pin ...

    // Different goal entirely, also empty -- still 0, no carryover
    //  lift.setTarget(CascadeLevel::HIGH_GOAL, 0);
    // lift.waitUntilSettled();
    // ... place pin ...

    // Back to the first goal, which now has 1 pin in it
    // lift.setTarget(CascadeLevel::MIDDLE_GOAL, 1);
    //lift.waitUntilSettled();
}

void opcontrol() {
    odomTask = new pros::Task(odomDebug);

    //GUI_showDebugScreen();

    new pros::Task(DriveTrainControls);
    new pros::Task(CascadeControls);
    new pros::Task(IntakeControls);
    //new pros::Task(wristControls);
    new pros::Task(ClawControls);

    //macros right now they work but wrist aint working got to fix boi
    new pros::Task(AfterintakeMACRO);
    new pros::Task(matchloaderhight);



    

}