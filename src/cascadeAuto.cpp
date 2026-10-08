#include "cascade.h"

#include "pros/rtos.hpp"
#include "robot-config.h"
#include "main.h"


double CascadeController::getPosition() {
  if (!m_initialized || m_rotation == nullptr) {
    return 0.0;
  }

  return m_rotation->get_position() / 100.0;
}


CascadeController lift(cascade, 16, {
  -12,//-12
  -30,
  0,
  23, 
  4,
  188, /// CONTROLLER cup and pin height TUNE // 188
  195, 
  225,// low goal
  325, // SOMETHING
  1450,
  1800
}
  , 810.0, 0.9, 0.0, 0.0);

CascadeController::CascadeController(pros::MotorGroup &motors, int rotationPort,
                                     std::vector<double> basePositions,
                                     double pinHeightDeg, double kP, double kI,
                                     double kD)
    : m_motors(motors), m_rotationPort(rotationPort), m_kP(kP), m_kI(kI),
      m_kD(kD), m_basePositions(std::move(basePositions)),
      m_pinHeightDeg(pinHeightDeg) {

  m_basePositions.resize(static_cast<size_t>(CascadeLevel::COUNT), 0.0);
}

void CascadeController::init() {
  if (m_initialized) {
    return;
  }

  m_rotation = new pros::Rotation(m_rotationPort);

  m_rotation->reset_position();

  m_motors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

  m_target = getPosition();
  m_prevError = 0.0;
  m_integral = 0.0;

  m_initialized = true;
  m_enabled = false;

  m_task = new pros::Task(taskFn, this, "cascadePID");
}

void CascadeController::enable() {
  if (!m_initialized) {
    return;
  }

  m_integral = 0.0;
  m_prevError = getError();
  m_enabled = true;
}

void CascadeController::disable() {
  m_enabled = false;
  m_integral = 0.0;
  m_prevError = 0.0;
  m_motors.brake();
}

bool CascadeController::isEnabled() { return m_enabled; }

double CascadeController::getTarget() { return m_target; }

double CascadeController::getError() { return m_target - getPosition(); }

double CascadeController::heightFor(CascadeLevel level,
                                    int pinsAlreadyInHole) const {
  size_t index = static_cast<size_t>(level);

  if (index >= m_basePositions.size()) {
    return 0.0;
  }

  if (pinsAlreadyInHole < 0) {
    pinsAlreadyInHole = 0;
  }

  return m_basePositions[index] + (pinsAlreadyInHole * m_pinHeightDeg);
}

void CascadeController::setTarget(CascadeLevel level, int pinsAlreadyInHole) {

  size_t index = static_cast<size_t>(level);
    
  if (index >= m_basePositions.size()) {
    return;
  }

  m_target = heightFor(level, pinsAlreadyInHole);
  ///////////////////////////////////////////////////////////
  m_enabled = true;
  ///////////////////////////////////////////////////////////////
  m_integral = 0.0;
  m_prevError = getError();
}

void CascadeController::setTargetDegrees(double degrees) {
  m_target = degrees;
  m_integral = 0.0;
  m_prevError = getError();
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

void CascadeController::step() {
  if (!m_initialized || !m_enabled) {
    return;
  }

  double error = getError();

  m_integral += error;

  m_integral = std::clamp(m_integral, -INTEGRAL_LIMIT, INTEGRAL_LIMIT);

  double derivative = error - m_prevError;

  m_prevError = error;

  double output = (m_kP * error) + (m_kI * m_integral) + (m_kD * derivative);

  output = std::clamp(output, -MAX_OUTPUT, MAX_OUTPUT);

  m_motors.move(static_cast<int>(output));
}

void CascadeController::taskFn(void *param) {
  CascadeController *self = static_cast<CascadeController *>(param);

  while (true) {
    self->step();
    pros::delay(10);
  }
}

void CascadeController::kill() {
  m_enabled = false;

  if (m_task != nullptr) {
    delete m_task;
    m_task = nullptr;
  }

  if (m_rotation != nullptr) {
    delete m_rotation;
    m_rotation = nullptr;
  }

  m_initialized = false;
}

void CascadeController::inchesTOdegrees(double inches) { (void)inches; }