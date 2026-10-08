#include "Autons.h"
#include "robot-config.h"
#include "main.h"
#include "cascade.h"
#include "DSR.h"

// ============================================================
//  Autons.cpp — ZIPPY 2 | Override 2026-2027
// ============================================================

void autonRed() {// old auto for 3 pins
    
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
    chassis.turnToHeading(210, 1000);

    // chassis.moveToPoint(25, -38, 1000, {.forwards = false}, false);
    lift.setTarget(CascadeLevel::ZERO);
    ///TUNE good
    //chassis.moveToPoint(20,-30, 1000, {.forwards = false, .maxSpeed = 55}, false);
    ////TUNE good
    //chassis.waitUntilDone();

    //pros::delay(150);//680
    //claw.retract(); //////////////////////////////////////////////////////////// stack one grab
    
    // pros::delay(500);
    // lift.setTarget(CascadeLevel::LOW_GOAL, 2);
    //chassis.swingToHeading(315, DriveSide ::RIGHT, 1000);
    
    // chassis.moveToPoint(20.5,-47, 1800, {.forwards = false, .maxSpeed = 60}, false);/////////to goal

    // // chassis.moveToPoint(18, -43, 1000, {.forwards = false}, false);
    // lift.setTarget(CascadeLevel::LOW_GOAL, 1);
    // pros::delay(700);
    // claw.extend();
    // pros::delay(300);
    
    
    // chassis.swingToHeading(45, DriveSide::RIGHT, 1000, {}, false);
    //lift.setTarget(CascadeLevel::ZERO);
    // chassis.turnToHeading(0, 1000);
    // chassis.moveToPoint(20, -32,1000);
    // // chassis.turnToHeading(285, 1000);
    // lift.setTarget(CascadeLevel::ZERO);
    // lift.waitUntilSettled(10, 1500);

    // chassis.turnToHeading(290, 800);
    // ///////TUNE
    // chassis.moveToPoint(43, -42.5, 1800, {.forwards = false, .maxSpeed = 75}, false);
    // ///////tUNE
    // pros::delay(400);
    // claw.retract();
    // pros::delay(500);
    // lift.setTarget(CascadeLevel::LOW_GOAL, 3);
    // chassis.turnToHeading(80, 1000);
    // chassis.moveToPoint(31.5, -51, 700, {.forwards = false, .minSpeed = 70}, false);
    // pros::delay(500);
    // lift.setTarget(CascadeLevel::LOW_GOAL, 2);
    // claw.extend();
    // pros::delay(500);
    // chassis.moveToPoint(33, -42.5, 1000);

}

void wallPINauto() {

    chassis.setPose(12, -62, 180); // set this
    intake.move(127);
    chassis.moveToPoint(12, -54, 400, {.forwards = false}, false); /// toggle
    chassis.moveToPoint(12, -72, 400, {.forwards = true}, false); /// toggle
    intake.brake();
    chassis.moveToPoint(12, -54, 400, {.forwards = false}, false); /// toggle
    chassis.moveToPoint(12, -72, 400, {.forwards = true}, false); /// toggle
    lift.setTarget(CascadeLevel::LOW_GOAL, 0);
    chassis.moveToPoint(12, -54, 400); 
    chassis.moveToPose(28,-48 ,225 , 850, {.forwards = false, .lead = 0.2}, false); /// into goal // 270
    // chassis.moveToPoint(26, -48, 400);
    pros::delay(200);
    lift.setTarget(CascadeLevel::ZERO);
    pros::delay(400);
    claw.extend();
    chassis.moveToPose(11,-60 ,135 , 850, {.forwards = true, .lead = 0.4}, false);
    chassis.moveToPoint(19, -62, 1000);
    flip.extend();
    
}

void farside() {    

    chassis.setPose(-12, -62, 180);
    intake.move(127);
    chassis.moveToPoint(-12, -54, 400, {.forwards = false}, false); /// toggle
    chassis.moveToPoint(-12, -72, 400, {.forwards = true}, false); /// toggle
    intake.brake();
    chassis.moveToPoint(-12, -54, 400, {.forwards = false}, false); /// toggle
    chassis.moveToPoint(-12, -72, 400, {.forwards = true}, false); /// toggle
    lift.setTarget(CascadeLevel::LOW_GOAL, 0);
    chassis.moveToPoint(-12, -54, 400); 
    chassis.moveToPoint(-7.6, -48.7, 1000);
}

void SKILLS() {
    
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
    chassis.turnToHeading(210, 1000);

    // chassis.moveToPoint(25, -38, 1000, {.forwards = false}, false);
    lift.setTarget(CascadeLevel::START);
    ///TUNE good
    chassis.moveToPoint(20,-30, 1000, {.forwards = false, .maxSpeed = 55}, false);
    ////TUNE good
    //chassis.waitUntilDone();

    //pros::delay(150);//680
    claw.retract(); //////////////////////////////////////////////////////////// stack one grab
    pros::delay(500);
    lift.setTarget(CascadeLevel::LOW_GOAL, 2);
    chassis.swingToHeading(315, DriveSide ::RIGHT, 1000);
    
    chassis.moveToPoint(20.5,-47, 1800, {.forwards = false, .maxSpeed = 60}, false);/////////to goal

    // chassis.moveToPoint(18, -43, 1000, {.forwards = false}, false);
    lift.setTarget(CascadeLevel::LOW_GOAL, 1);
    pros::delay(700);
    claw.extend();
    pros::delay(300);
    //chassis.swingToHeading(45, DriveSide::RIGHT, 1000, {}, false);
    chassis.turnToHeading(0, 1000);
    chassis.moveToPoint(20, -32,1000);
    chassis.turnToHeading(135, 1000);
    chassis.moveToPoint(6, 2, 1500, {.forwards = false}, false);
    // chassis.turnToHeading(285, 1000);
    // lift.setTarget(CascadeLevel::ZERO);
    // lift.waitUntilSettled(10, 1500);
}


const AutonEntry AUTONS[] = {
    {"Aliiance Goals",     autonRed},
    {"Nuetral Goals",       farside},
    {"walls auto",      wallPINauto},
    {"SKILLS",               SKILLS},
};

const int AUTON_COUNT = sizeof(AUTONS) / sizeof(AUTONS[0]);