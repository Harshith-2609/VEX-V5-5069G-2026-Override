#pragma once
 
/**
 * DSR.h
 * -----
 * Single-header version of TitanReset's public API
 * (https://github.com/tubaplayerdis/TitanReset), consolidated from
 * TRTypes.hpp + TRConstants.hpp + TRSensor.hpp + TRChassis.hpp +
 * TitanReset.hpp into one file, with every tr_ prefixed identifier renamed
 * to dsr_. Behavior is unchanged from the source you provided, with one
 * fix noted below.
 *
 * Fix: the original tr_chassis declared a `pros::Imu* imu` member that was
 * never assigned by any constructor, then dereferenced it in
 * update_display() (chassis->imu->get_heading()) - a guaranteed
 * null-pointer crash if that function was ever called. That unused member
 * is removed here; update_display() now reads heading from the drivebase
 * pose (chassis->getPose().z), same as every other function in this class
 * already does.
 */
 
#include <array>
#include <string>
#include <utility>
#include <cmath>
 
#include "pros/imu.hpp"
#include "pros/distance.hpp"
#include "lemlib/chassis/chassis.hpp"
 
// ============================================================================
// Constants (from TRConstants.hpp) - compiled-in, reference only
// ============================================================================
 
/** Erroneous reading value when the V5 Distance Sensor cannot read a distance */
static constexpr int err_reading_value = 9999;
 
/** MM to IN conversion factor. */
static constexpr float mm_inch_conversion_factor = 0.0393701;
 
/** Degree to Radian conversion factor */
static constexpr float deg_rad_conversion_factor = 0.0174532;
 
/** Radian to Degree conversion factor */
static constexpr float rad_deg_conversion_factor = 57.2958;
 
/** Domain of the confidence readings from the V5 Distance Sensor */
static constexpr float confidence_domain = 63.0;
 
// ============================================================================
// Types (from TRTypes.hpp)
// ============================================================================
 
/** DSR Quadrant enumeration. */
enum dsr_quadrant {
    POS_POS,
    NEG_POS,
    NEG_NEG,
    POS_NEG,
};
 
/** DSR Sensor flags. */
enum dsr_sensors {
    NORTH = 1,
    EAST = 2,
    SOUTH = 4,
    WEST = 8,
};
 
/** Standard probability type definition */
typedef float dsr_probability;
 
/**
 * Confidence Pair
 * Templated pair abstraction with the confidence value.
 */
template<typename T>
class dsr_conf_pair {
    private:
        /** Internal pair value */
        std::pair<T, dsr_probability> value;
 
    public:
        /** @brief confidence pair default constructor */
        dsr_conf_pair() { value = std::pair<T, dsr_probability>(T(), 0); }
 
        /**
         * @brief confidence pair passing the confidence as a float
         * @param principal value of the pair
         * @param confidence confidence as a float
         */
        dsr_conf_pair(T principal, dsr_probability confidence) {
            value = std::pair<T, dsr_probability>(principal, confidence);
        }
 
        /** @brief Sets the confidence of the confidence pair */
        void set_confidence(dsr_probability confidence) { value.second = confidence; }
 
        /** @brief Sets the value of the confidence pair */
        void set_value(T principal) { value.first = principal; }
 
        /** @brief Gets the value of the confidence pair as T */
        T get_value() { return value.first; }
 
        /** @brief Gets the confidence of the confidence pair as a float */
        dsr_probability get_confidence() { return value.second; }
};
 
/** Standard distance confidence pair definition */
typedef dsr_conf_pair<float> dsr_distance;
 
/** 3D vector data structure. Z is expressed as theta for DSR */
struct dsr_vector3 {
    float x;
    float y;
    float z;
 
    /** @brief Default constructor for vector, initializes X, Y, and Z to zero. */
    dsr_vector3() {
        x = 0;
        y = 0;
        z = 0;
    }
 
    /**
     * @brief Standard constructor for vector, initializes values to input parameters
     * @param X x value of vector
     * @param Y y value of vector
     * @param Z z or theta value of vector depending on plane interpreted
     */
    dsr_vector3(float X, float Y, float Z) {
        x = X;
        y = Y;
        z = Z;
    }
 
    /** @brief Standard constructor for vector, initializes values to input parameters */
    dsr_vector3(std::array<float, 3> arr) {
        x = arr[0];
        y = arr[1];
        z = arr[2];
    }
};
 
/** Two dimensional vector object used by DSR */
struct dsr_vector2 {
    float x;
    float y;
 
    dsr_vector2() {
        x = 0.0f;
        y = 0.0f;
    }
 
    dsr_vector2(float X, float Y) {
        x = X;
        y = Y;
    }
 
    /** @brief Standard constructor for vector, initializes values to input parameters */
    dsr_vector2(std::array<float, 2> arr) {
        x = arr[0];
        y = arr[1];
    }
};
 
/**
 * @brief Abstract drivebase class to allow support of any template.
 */
class dsr_drivebase_abstract {
    public:
        virtual ~dsr_drivebase_abstract() = default;
 
        /**
         * @return struct of type dsr_vector3 representing the "pose" of the
         * robot in X, Y, Theta (in degrees).
         * @note pure virtual function as to be implemented in a subclass appropriately.
         */
        virtual dsr_vector3 getPose() = 0;
 
        /**
         * @brief set the "pose" of the drivebase using a struct of type dsr_vector3
         * representing the "pose" of the robot in X, Y, Theta (in degrees).
         * @note pure virtual function as to be implemented in a subclass appropriately.
         */
        virtual void setPose(dsr_vector3 new_pose) = 0;
};
 
// ============================================================================
// Sensor (from TRSensor.hpp)
// ============================================================================
 
/**
 * @brief Distance sensor wrapper class used for distance sensor resets.
 */
class dsr_sensor {
        /**
         * Offset vector of the DSR sensor.
         * X is the parallel offset (the way it is facing) of the sensor from the center of the robot to the sensor
         * Y is the perpendicular offset (left or right of the way the sensor is facing) of the sensor from the center of the robot to the sensor
         */
        const dsr_vector2 offset;
 
        /** Pros distance sensor object. */
        pros::Distance sensor;
 
    public:
        /**
         * @brief Constructor for DSR sensor.
         * @note Offsets should be done in inches.
         * @param off Offset of the sensor from the origin of the robot with X being in the parallel direction of the sensors facing, and Y being the perpendicular
         * @param port Port of the distance sensor.
         */
        dsr_sensor(dsr_vector2 off, int port);
 
        /**
         * @brief Distance read from the distance sensor as a confidence pair with angle of robot factored in and offset.
         * @note Data returned is in inches.
         * @return confidence and the distance reading and calculation.
         */
        dsr_distance distance(float heading);
 
        /**
         * @brief Distance read from the distance sensor as a confidence pair without factoring in heading.
         * @note Data returned is in inches.
         * @return confidence and the distance reading.
         */
        dsr_distance distance();
 
    public:
        /**
         * @brief Function mapping the heading into the domain of -45 to 45 degrees to use with trigonometric functions
         */
        static float relative_square(float heading);
};
 
// ============================================================================
// Chassis (from TRChassis.hpp)
// ============================================================================
 
/**
 *  Standard field perimeter radii.
 */
namespace dsr_fields {
    constexpr float plastic = 70.205;
    constexpr float metal = 70.336;
}
 
/**
 * @brief Implemented version of the abstract drivebase class to enable support with lemlib.
 *
 * This is an example of what an implementation of any template could look like. Use this as a base if creating an implementation for another template.
 *
 * @note This class is implemented in the header file to be commented out if taking a template neutral approach.
 */
class dsr_lem_base : public dsr_drivebase_abstract {
    public:
        lemlib::Chassis* chassis;
 
        dsr_lem_base(lemlib::Chassis* chassis_ptr) : chassis(chassis_ptr) {}
 
        dsr_vector3 getPose() override {
            dsr_vector3 vec_ret;
            lemlib::Pose current = chassis->getPose();
 
            vec_ret.x = current.x;
            vec_ret.y = current.y;
            vec_ret.z = current.theta;
 
            return vec_ret;
        }
 
        void setPose(dsr_vector3 new_pose) override {
            lemlib::Pose set_pose(0, 0, 0);
 
            set_pose.x = new_pose.x;
            set_pose.y = new_pose.y;
            set_pose.theta = new_pose.z;
 
            chassis->setPose(set_pose);
        }
};
 
/**
 * DSR chassis object. Used to perform distance sensor resets
 */
class dsr_chassis {
    public:
        /**
         * @brief Initialize the localization chassis
         * @note ONLY INITIALIZE THIS WHEN YOUR ROBOT IS NOT MOVING!
         *
         * @param base pointer to the drivebase chassis of the robot
         * @param sensors array of pointers to the localization sensors of the robot
         * @param field_radius field perimeter radius (see dsr_fields)
         */
        dsr_chassis(dsr_drivebase_abstract* base, std::array<dsr_sensor*, 4> sensors,
                    const float field_radius = dsr_fields::plastic);
 
        /**
         * @brief Initialize the localization chassis
         * @note ONLY INITIALIZE THIS WHEN YOUR ROBOT IS NOT MOVING!
         *
         * @details Convenience constructor for LemLib users - wraps the LemLib
         * chassis in a dsr_lem_base automatically.
         *
         * @param base pointer to the LemLib chassis of the robot
         * @param sensors array of pointers to the localization sensors of the robot
         * @param field_radius field perimeter radius (see dsr_fields)
         */
        dsr_chassis(lemlib::Chassis* base, std::array<dsr_sensor*, 4> sensors,
                    const float field_radius = dsr_fields::plastic) :
            dsr_chassis(new dsr_lem_base(base), sensors, field_radius) {}
 
        /**
         * @brief Performs a distance sensor reset using the sensors on the robot given the robot already knows where it is and where it is facing.
         * @return Whether the reset was successful given the coordinates calculated were in the functional bounds of the field.
         */
        bool perform_dsr();
 
        /**
         * @brief Performs a distance sensor reset using the sensors on the robot given the robot does not know which quadrant it is in.
         * @note Use this function after a movement that performs an action such as driving over a parking zone which crosses quadrants.
         * @param quadrant The quadrant the robot is currently in
         * @return Whether the reset was successful given the coordinates calculated were in the functional bounds of the field.
         */
        bool perform_dsr_quad(dsr_quadrant quadrant);
 
        /**
         * @brief Performs a distance sensor reset using the sensors on the robot given the robot does not know where it is and the sensors are fully trusted.
         * @note This will set the heading of the chassis as it performs a distance sensor reset.
         * @warning This will always set the location of the robot. Use only in a situation where the robot starts in a familiar place each time like the start of an auton.
         * @param quadrant The quadrant the robot is currently in
         * @param heading The heading of the robot
         * @return Whether the reset was successful given the coordinates calculated were in the functional bounds of the field.
         */
        bool perform_dsr_init(dsr_quadrant quadrant, float heading);
 
        /**
         * @brief Gets the robot's quadrant based on its coordinates
         * @return The quadrant of the robot
         */
        dsr_quadrant get_quadrant();
 
        /**
         * @brief Starts a location-recording background task.
         * @note appends pose lines to a file named "<name>_<date>_<time>_<ms>.txt"
         * @param name name of the recording
         * @param date date of the recording
         * @param time time of the recording
         */
        void start_location_recording(std::string name, std::string date = __DATE__, std::string time = __TIME__);
 
        /** @brief Stops the current location recording */
        void stop_location_recording();
 
        /*
        *   Note - Everything below this line is either utilities to aid with the implementation of DSR and is most likely irrelevant to your goals.
        */
 
    private:
        /** Whether the display is active and being displayed. */
        bool b_display;
 
        /* Active sensors being used by the robot. */
        int active_sensors;
 
        /** Active field radius. */
        const float wall_cord;
 
        /** Sets the active sensors when using debug gui */
        void set_active_sensors(int sensors);
 
    public:
        /**
         * @brief Normalizes heading to the domain of 0-360. Also called finding the coterminal angle
         * @param heading heading to normalize.
         * @return Normalized heading
         */
        static float quadrant_recursive(float heading);
 
        /**
         * @brief Compares the location against locations the robot physically cannot exist at such as out of bounds.
         * @param pose current location vector
         * @return whether the location can physically exist.
         */
        static bool can_position_exist(dsr_vector3 pose);
 
        static std::string get_quadrant_string(dsr_quadrant quadrant);
 
        /** @brief Returns the relevant sensors based on the heading of the robot. */
        dsr_quadrant sensor_relevancy();
 
        /**
         * @brief Returns the relevant sensors based on the heading of the robot.
         * Requires normalized heading 0-360 degrees.
         * For 315 - 45 degrees: ++
         * For 45 - 135 degrees: -+
         * For 135 - 225 degrees: --
         * For 225 - 315 degrees: +-
         */
        dsr_quadrant sensor_relevancy(float heading);
 
        /**
         * @brief Average confidence of value pair
         * @param one first confidence pair
         * @param two second confidence pair
         * @return Average confidence of value pair
         */
        static float conf_avg(dsr_distance one, dsr_distance two);
 
        /**
         * @brief Returns the confidence pair of a coordinate pair representing the robots location gathered from the sensors.
         * @param quadrant Current quadrant of the robot
         * @return Confidence pair of a coordinate pair representing the robots location gathered from the sensors.
         */
        dsr_conf_pair<dsr_vector3> get_position_calculation(dsr_quadrant quadrant);
 
        dsr_conf_pair<dsr_vector3> get_position_calculation(dsr_quadrant quadrant, float heading);
 
        /** Initializes the debug screen. */
        static void init_display();
 
        /** Renders the debug screen. Use in a loop. */
        static void update_display(dsr_chassis* chassis);
 
        /** Shutdown the debug screen */
        static void shutdown_display();
 
        /**
         * @brief Uses flags to return whether a sensor is being used.
         * @note Active sensors are set by performing a distance sensor reset.
         * @param r_sensor sensor flags
         * @return whether that sensor is being used.
         */
        bool is_sensor_used(int r_sensor);
 
        ~dsr_chassis();
 
    private:
        /* Private objects to be used by DSR */
 
        /** Location recording task pointer */
        pros::Task* location_task;
 
        /** Sensors */
        dsr_sensor* north;
        dsr_sensor* east;
        dsr_sensor* south;
        dsr_sensor* west;
 
        /** Drivebase reference */
        dsr_drivebase_abstract* chassis;
};

extern dsr_chassis dsr_system;