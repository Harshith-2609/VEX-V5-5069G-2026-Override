#include "cascade.h"
#include "main.h"

CascadeController lift(
    cascade,                        // your existing MotorGroup
    16,                             // "Lift" rotation sensor port
    {0, 200, 225, 1450, 1800},    // ZERO, START, LOW, MIDDLE, HIGH
    800.0,                           // degrees of lift per pin already in the hole
    0.9, 0.0, 0.0                  // kP, kI, kD
);

CascadeController::CascadeController(pros::MotorGroup& motors, int rotationPort,
                                     std::vector<double> basePositions,
                                     double pinHeightDeg,
                                     double kP, double kI, double kD)
    : m_motors(motors),
      m_rotationPort(rotationPort),
      m_kP(kP), m_kI(kI), m_kD(kD),
      m_basePositions(std::move(basePositions)),
      m_pinHeightDeg(pinHeightDeg) {

    // Pure data only. No sensor reads, no motor calls, no task creation --
    // none of that is safe before PROS has finished constructing globals.
    m_basePositions.resize(static_cast<size_t>(CascadeLevel::COUNT), 0.0);
}

void CascadeController::inchesTOdegrees(double myinches) {
    
    double inchesTodegree = myinches*360*(360/0.25*M_PI);

}

void CascadeController::init() {
    if (m_initialized) return;

    m_rotation = new pros::Rotation(m_rotationPort);
    m_rotation->reset_position();

    m_motors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    m_target = getPosition();   // hold wherever it currently sits
    m_prevError = 0;
    m_integral = 0;
    m_initialized = true;

    m_task = new pros::Task(taskFn, this, "cascadePID");
}

void CascadeController::kill() {
    delete m_task;
    m_task = nullptr;
}

double CascadeController::getPosition() {
    if (!m_initialized || m_rotation == nullptr) return 0.0;
    // get_position() is centidegrees and accumulates past 360, unlike
    // get_angle() which wraps 0-360. Correct choice for a cascade.
    return m_rotation->get_position() / 100.0;
}

double CascadeController::getTarget() { return m_target; }
double CascadeController::getError()  { return m_target - getPosition(); }

double CascadeController::heightFor(CascadeLevel level, int pinsAlreadyInHole) const {
    size_t i = static_cast<size_t>(level);
    if (i >= m_basePositions.size()) return 0.0;
    if (pinsAlreadyInHole < 0) pinsAlreadyInHole = 0; // guard against bad input
    return m_basePositions[i] + (pinsAlreadyInHole * m_pinHeightDeg);
}

void CascadeController::setTarget(CascadeLevel level, int pinsAlreadyInHole) {
    size_t i = static_cast<size_t>(level);
    if (i >= m_basePositions.size()) return;
    m_target = heightFor(level, pinsAlreadyInHole);
    m_integral = 0;
}

void CascadeController::setTargetDegrees(double degrees) {
    m_target = degrees;
    m_integral = 0;
}


bool CascadeController::isSettled(double toleranceDeg) {
    return std::fabs(getError()) <= toleranceDeg;
}

void CascadeController::waitUntilSettled(double toleranceDeg, int timeoutMs) {
    int waited = 0;
    while (!isSettled(toleranceDeg) && waited < timeoutMs) {
        pros::delay(10);
        waited += 10;
    }
}

void CascadeController::enable() {
    m_integral = 0;
    m_prevError = getError();
    m_target = getPosition();
    m_enabled = true;
}

void CascadeController::disable() {
    m_enabled = false;
    m_motors.brake();
}

void CascadeController::step() {
    if (!m_initialized || !m_enabled) return;

    double error = getError();

    m_integral += error;
    m_integral = std::clamp(m_integral, -INTEGRAL_LIMIT, INTEGRAL_LIMIT);

    double derivative = error - m_prevError;
    m_prevError = error;

    double output = m_kP * error + m_kI * m_integral + m_kD * derivative;
    output = std::clamp(output, -MAX_OUTPUT, MAX_OUTPUT);

    m_motors.move(output);
}

void CascadeController::taskFn(void* param) {
    CascadeController* self = static_cast<CascadeController*>(param);
    while (true) {
        self->step();
        pros::delay(10);
    }
}