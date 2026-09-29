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



enum class DriveMode {
    ARCADE,
    TANK
};

static DriveMode driveMode = DriveMode::ARCADE;



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

static bool comboTiming = false;
static bool comboTriggered = false;
static std::uint32_t comboStartTime = 0;

void checkDriveModeToggle() {

    const bool upHeld =
        master.get_digital(pros::E_CONTROLLER_DIGITAL_UP);

    const bool xHeld =
        master.get_digital(pros::E_CONTROLLER_DIGITAL_X);

    const bool comboHeld = upHeld && xHeld;


    // Combo started
    if (comboHeld && !comboTiming) {

        comboTiming = true;
        comboTriggered = false;
        comboStartTime = pros::millis();
    }


    // Combo being held
    if (comboHeld && comboTiming && !comboTriggered) {

        const std::uint32_t heldTime =
            pros::millis() - comboStartTime;

        if (heldTime >= DRIVE_TOGGLE_HOLD_MS) {

            if (driveMode == DriveMode::ARCADE) {

                driveMode = DriveMode::TANK;
                master.rumble("-");

            } else {

                driveMode = DriveMode::ARCADE;
                master.rumble(".");
            }

            comboTriggered = true;
        }
    }


    // Combo released
    if (!comboHeld) {

        comboTiming = false;
        comboTriggered = false;
    }
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

        checkDriveModeToggle();


        if (driveMode == DriveMode::ARCADE) {

            arcadeDrive();

        } else {

            tankDrive();
        }


        pros::delay(10);
    }
}


void CascadeControls() {

    while (true) {


        if (master.get_digital(
                pros::E_CONTROLLER_DIGITAL_L1)) {

            // Give motor control back to driver
            lift.disable();

            cascade.move(127);
        }


        else if (master.get_digital(
                     pros::E_CONTROLLER_DIGITAL_L2)) {

            // Give motor control back to driver
            lift.disable();

            cascade.move(-127);
        }


        else {

            if (!lift.isEnabled()) {
                cascade.brake();
            }
        }


        pros::delay(10);
    }
}



void IntakeControls() {

    while (true) {

        if (master.get_digital(
                pros::E_CONTROLLER_DIGITAL_R2)) {

            intake.move(80);

        }

        else if (master.get_digital(
                     pros::E_CONTROLLER_DIGITAL_R1)) {

            intake.move(-127);

        }

        else {

            intake.brake();
        }


        pros::delay(10);
    }
}


void ClawControls() {

    static bool claw1 = false;

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


void wristControls() {

    static bool wrist1 = false;

    while (true) {

        if (master.get_digital_new_press(
                pros::E_CONTROLLER_DIGITAL_DOWN)) {

            wrist1 = !wrist1;


            if (wrist1) {

                wrist.extend();

            } else {

                wrist.retract();
            }
        }


        pros::delay(10);
    }
}


void matchloaderhight() {

    while (true) {

        if (master.get_digital_new_press(
                pros::E_CONTROLLER_DIGITAL_B)) {

            // Set PID target
            lift.setTarget(CascadeLevel::START);

            // Enable PID
            lift.enable();

            // Extend claw
            claw.extend();
        }


        pros::delay(10);
    }
}


void AfterintakeMACRO() {

    while (true) {

        if (master.get_digital_new_press(
                pros::E_CONTROLLER_DIGITAL_B)) {

            claw.retract();

            wrist.retract();

            // Set PID target
            lift.setTarget(CascadeLevel::START);

            // Enable PID
            lift.enable();
        }


        pros::delay(10);
    }
}