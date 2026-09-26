#include "main.h"
#include "drivetrain.h"
#include "robot-config.h"
#include "cascade.h"
#include "GUI.h"
#include "DSR.h"
pros::Task* odomTask = nullptr;
// ============================================================
//  main.cpp — ZIPPY 2 | Override 2026-2027
// ============================================================

CascadeController lift(
    cascade,                        // your existing MotorGroup
    -16,                             // "Lift" rotation sensor port
    {0.0, 400.0, 900.0, 1800.0},    // START, LOW, MIDDLE, HIGH
    40.0,                           // degrees of lift per pin already in the hole
    0.1, 0.0, 0.0                  // kP, kI, kD
);

dsr_sensor north({-6, 0}, 17);//
dsr_sensor east({5.5, -1.5}, 13);//
dsr_sensor south({-6, -3}, 14);//
dsr_sensor west({-5, -3}, 15);   

dsr_chassis dsr_system(&chassis, {&north, &east, &south, &west});



void odomDebug(void *) {
  master.clear();
  while (true) {
    lemlib::Pose pose = chassis.getPose();
    master.print(0, 0, "X:%5.1f Y:%5.1f", pose.x, pose.y);
    master.print(1, 0, "H:%5.1f", pose.theta);
    master.print(2, 0, "X true:%5.1f Y true:%5.1f", trackX.get_position(),trackY.get_position());
    //master.print(2, 0, "degrees%5.1f",cascade1.get_position());
    pros::delay(50);
  }
}
void initialize() {
    
    
    chassis.calibrate(true); // ~3s IMU calibration

    DriveL.set_brake_mode_all(pros::E_MOTOR_BRAKE_BRAKE);
    DriveR.set_brake_mode_all(pros::E_MOTOR_BRAKE_BRAKE);
    cascade1.set_brake_mode_all(pros::E_MOTOR_BRAKE_HOLD);
    cascade2.set_brake_mode_all(pros::E_MOTOR_BRAKE_HOLD);
    intake.set_brake_mode_all(pros::E_MOTOR_BRAKE_BRAKE);

    

    chassis.setPose(0, 0, 0);

    odomTask = new pros::Task(odomDebug);
    //GUI_runAutonSelector();
}

void disabled() {
    //GUI_showDebugScreen();
}

void competition_initialize() {
    // Selector screen stays up from initialize() until the match starts.
}

void autonomous() {
    odomTask = new pros::Task(odomDebug);
    lift.init();


    dsr_system.perform_dsr_init(POS_POS, 90);


    //chassis.turnToHeading(90, 1000);
    //chassis.moveToPoint(0.0, 24.0, 1000);
    //pros::delay(1000);
    //chassis.turnToHeading(180, 1000);
    //pros::delay(1000);
    //chassis.moveToPoint(0.0, 0.0, 1000);
    // if (selectedAuton >= 0 && selectedAuton < AUTON_COUNT) {
    //     AUTONS[selectedAuton].run();
    // }

    // Drive to the middle goal with lemlib as usual
    // chassis.moveToPose(...);
 
    // First goal, hole is empty
    //lift.setTarget(CascadeLevel::MIDDLE_GOAL, 0);
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
    //GUI_showDebugScreen();

    new pros::Task(DriveTrainControls);
    new pros::Task(CascadeControls);
    new pros::Task(IntakeControls);



    

}