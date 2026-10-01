#include "drivetrain.h"
#include "pros/misc.h"
#include "robot-config.h"
#include "cascade.h"


#include <algorithm>
#include <cmath>
#include <cstdint>

static constexpr double JOYSTICK_DEADBAND = 4.0;
static constexpr double EXPO_CUTOFF = 19.0;
static constexpr std::uint32_t DRIVE_TOGGLE_HOLD_MS = 1200;


double linearJoystick(double input) {
    if (std::fabs(input) < JOYSTICK_DEADBAND) {
        return 0.0;
    }

    return input;
}

int joystickToVoltage(double joystick) {
    joystick = std::clamp(joystick, -127.0, 127.0);

    return static_cast<int>(
        joystick * 12000.0 / 127.0
    );
}

void arcadeDrive() {
    double forward =
        master.get_analog(
            pros::E_CONTROLLER_ANALOG_LEFT_Y
        );

    double turn =
        master.get_analog(
            pros::E_CONTROLLER_ANALOG_RIGHT_X
        );

    if (std::fabs(forward) < EXPO_CUTOFF &&
        std::fabs(turn) < EXPO_CUTOFF) {

        chassis.arcade(forward, turn);
        return;
    }

    forward = linearJoystick(forward);
    turn = linearJoystick(turn);

    double leftPower =
        std::clamp(
            forward + turn,
            -127.0,
            127.0
        );

    double rightPower =
        std::clamp(
            forward - turn,
            -127.0,
            127.0
        );

    DriveL.move_voltage(
        joystickToVoltage(leftPower)
    );

    DriveR.move_voltage(
        joystickToVoltage(rightPower)
    );
}

void tankDrive() {
    double left =
        master.get_analog(
            pros::E_CONTROLLER_ANALOG_LEFT_Y
        );

    double right =
        master.get_analog(
            pros::E_CONTROLLER_ANALOG_RIGHT_Y
        );

    left = linearJoystick(left);
    right = linearJoystick(right);

    DriveL.move_voltage(
        joystickToVoltage(left)
    );

    DriveR.move_voltage(
        joystickToVoltage(right)
    );
}

void DriveTrainControls() {
    while (true) {

        tankDrive();

        pros::delay(10);
    }
}

void CascadeControls() {
    while (true) {
        const bool up =
            master.get_digital(
                pros::E_CONTROLLER_DIGITAL_L1
            );

        const bool down =
            master.get_digital(
                pros::E_CONTROLLER_DIGITAL_L2
            );

        if (up || down) {
            if (lift.isEnabled()) {
                lift.disable();
            }

            cascade.move(
                up ? 127 : -127
            );
        } else if (!lift.isEnabled()) {
            cascade.brake();
        }

        pros::delay(10);
    }
}

void IntakeControls() {
    while (true) {
        if (master.get_digital(
                pros::E_CONTROLLER_DIGITAL_R2)) {

            intake.move(80);

        } else if (master.get_digital(
                       pros::E_CONTROLLER_DIGITAL_R1)) {

            intake.move(-127);

        } else {

            intake.brake();
        }

        pros::delay(10);
    }
}



static bool claw1 = false;

void ClawControls() {
    while (true) {
        if (master.get_digital_new_press(
                pros::E_CONTROLLER_DIGITAL_Y)) {

            claw1 = !claw1;

            if (claw1) {
                claw.extend();
            } else {
                claw.retract();
            }
        }

        pros::delay(10);
    }
}

// static bool wrist1 = false;

// void wristControls() {
//     while (true) {
//         if (master.get_digital_new_press(
//                 pros::E_CONTROLLER_DIGITAL_DOWN)) {

//             wrist1 = !wrist1;

//             if (wrist1) {
//                 wrist.extend();
//             } else {
//                 wrist.retract();
//             }
//         }

//         pros::delay(10);
//     }
// }


void matchloaderhight() {
    while (true) {
        if (master.get_digital_new_press(
                pros::E_CONTROLLER_DIGITAL_B)) {

            lift.setTarget(
                CascadeLevel::START
            );

            //lift.enable();
        }

        pros::delay(10);
    }
}


// finally it fucking works


static bool mac = false;
void AfterintakeMACRO() {
    while (true) {
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {

            mac = !mac;

            if (mac) { ///1st part of the macro

                lift.setTarget(CascadeLevel::LOW_GOAL, 0);
                lift.waitUntilSettled(15, 800);
                pros::delay(200);

                claw.extend();
                claw1 = true;

                wrist.extend();

                lift.setTarget(CascadeLevel::ZERO, 0);
            }
            else { ////2nd part of the macro

                claw.retract();
                claw1 = false;

                pros::delay(500);

                lift.setTarget( CascadeLevel::LOW_GOAL, 1);

                wrist.retract();
            }
        }

        pros::delay(10);
    }
}

