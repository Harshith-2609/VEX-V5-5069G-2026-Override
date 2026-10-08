#pragma once

#include "main.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <utility>

//just dont worry about this
enum class CascadeLevel {
    FUCK_ITAIGNTWORKING,//0
    NEGATIVE,//1
    ZERO,  //2
    ZERO_DRIVER,//3
    LOW_GOAL_DRIVER, //4
    MATCHLOAD,///221 //5
    START, //6
    LOW_GOAL, //7
    SOMTHING, //8
    MIDDLE_GOAL, //9
    HIGH_GOAL, //10
    COUNT
};

class CascadeController {
public:
    CascadeController(
        pros::MotorGroup& motors,
        int rotationPort,
        std::vector<double> basePositions,
        double pinHeightDeg,
        double kP,
        double kI,
        double kD
    );

    void init();

    void enable();
    void disable();
    bool isEnabled();

    void setTarget(
        CascadeLevel level,
        int pinsAlreadyInHole = 0
    );

    void setTargetDegrees(double degrees);

    double getPosition();
    double getTarget();
    double getError();

    bool isSettled(double toleranceDeg = 10.0);

    void waitUntilSettled(
        double toleranceDeg = 10.0,
        int timeoutMs = 2000
    );

    double heightFor(
        CascadeLevel level,
        int pinsAlreadyInHole = 0
    ) const;

    void step();

    static void taskFn(void* param);

    void inchesTOdegrees(double inches);

    void kill();

private:
    pros::MotorGroup& m_motors;

    int m_rotationPort;

    pros::Rotation* m_rotation = nullptr;

    double m_kP;
    double m_kI;
    double m_kD;

    std::vector<double> m_basePositions;

    double m_pinHeightDeg;

    double m_target = 0.0;
    double m_prevError = 0.0;
    double m_integral = 0.0;

    bool m_initialized = false;
    bool m_enabled = false;

    pros::Task* m_task = nullptr;

    static constexpr double MAX_OUTPUT = 127.0;
    static constexpr double INTEGRAL_LIMIT = 1000.0;
};

extern CascadeController lift;