#pragma once

#include <vector>
#include <cmath>
#include "pros/motor_group.hpp"
#include "pros/rotation.hpp"
#include "pros/rtos.hpp"

// Add or remove entries here to match however many stopping points you
// actually want. Keep COUNT last -- it auto-sizes the position arrays.
enum class CascadeLevel {
    START  = 0,
    LOW_GOAL    = 1,
    MIDDLE_GOAL = 2,
    HIGH_GOAL   = 3,
    COUNT
};

class CascadeController {
public:
    /**
     * The constructor ONLY stores configuration. It deliberately touches no
     * hardware, because globals in different .cpp files are constructed in
     * an unspecified order -- touching the MotorGroup or sensor here can run
     * before they exist and crashes the Brain with a DATA ABORT.
     * Call init() from initialize() instead.
     *
     * @param motors        reference to your EXISTING MotorGroup from robot-config
     * @param rotationPort  port for the "Lift" rotation sensor (negative to reverse)
     * @param basePositions target sensor-degrees, one per CascadeLevel, in order
     * @param pinHeightDeg  sensor-degrees of extra height per pin already in a hole
     * @param kP, kI, kD    PID gains, tune empirically
     */
    CascadeController(pros::MotorGroup& motors, int rotationPort,
                      std::vector<double> basePositions,
                      double pinHeightDeg,
                      double kP, double kI, double kD);

    // Call from initialize(). Resets the sensor, sets brake modes, and
    // starts the PID task. Safe to call once; repeat calls are ignored.
    void init();
    void inchesTOdegrees(double myinches);

    // Go to `level` assuming `pinsAlreadyInHole` pins are stacked there.
    // 0 means an empty hole -- the bare base height for that level.
    // This is absolute, not cumulative: the count you pass is the count used.
    void setTarget(CascadeLevel level, int pinsAlreadyInHole = 0);

    void setTargetDegrees(double degrees);

    // How tall the cascade WOULD go for a given level/count, without moving.
    double heightFor(CascadeLevel level, int pinsAlreadyInHole) const;

    double getPosition();
    double getTarget();
    double getError();

    bool isSettled(double toleranceDeg = 1.0);
    void waitUntilSettled(double toleranceDeg = 1.0, int timeoutMs = 3000);

    void disable();     // stop PID holding, e.g. handing off to driver control
    void enable();

private:
    void step();
    static void taskFn(void* param);

    pros::MotorGroup& m_motors;
    int m_rotationPort;
    pros::Rotation* m_rotation = nullptr; // created in init(), not at static time

    double m_kP, m_kI, m_kD;
    double m_integral  = 0;
    double m_prevError = 0;
    double m_target    = 0;
    bool   m_enabled   = true;
    bool   m_initialized = false;

    std::vector<double> m_basePositions;
    double m_pinHeightDeg;

    pros::Task* m_task = nullptr;

    static constexpr double INTEGRAL_LIMIT = 20.0;
    static constexpr double MAX_OUTPUT     = 127.0;
};