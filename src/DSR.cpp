/**
 * DSR.cpp
 * -------
 * Implementation for DSR.h. Consolidated from TRSensor.cpp + TRChassis.cpp,
 * with every tr_ prefixed identifier renamed to dsr_. Logic is unchanged
 * from the source you provided, except update_display() now reads heading
 * from the drivebase pose instead of a never-assigned pros::Imu* member -
 * see the note at the top of DSR.h.
 */

#include "DSR.h"
#include "pros/llemu.hpp"
#include <fstream>

// ============================================================================
// dsr_sensor (from TRSensor.cpp)
// ============================================================================

dsr_sensor::dsr_sensor(dsr_vector2 offset, int port) : offset(offset), sensor(port) {}

float dsr_sensor::relative_square(float heading) {
    float wrapped = fmod(heading, 360.0f);
    if (wrapped < 0) wrapped += 360.0f;

    // Find distance to the nearest 90-degree increment
    // This gives you how "un-square" the robot is to the wall
    float relative = fmod(wrapped + 45.0f, 90.0f) - 45.0f;
    return relative;
}

dsr_conf_pair<float> dsr_sensor::distance() {
    int sensor_reading = sensor.get_distance();
    float sensor_confidence = (sensor.get_confidence() / confidence_domain);
    if (sensor_reading == err_reading_value) return dsr_conf_pair<float>(err_reading_value, 0.0);

    return dsr_conf_pair<float>(sensor_reading * mm_inch_conversion_factor, sensor_confidence);
}

dsr_conf_pair<float> dsr_sensor::distance(float heading) {
    int sensor_reading = sensor.get_distance();
    float sensor_confidence = (sensor.get_confidence() / confidence_domain);
    if (sensor_reading == err_reading_value) return dsr_conf_pair<float>(err_reading_value, 0.0);

    auto reading = sensor_reading * mm_inch_conversion_factor;

    heading = dsr_sensor::relative_square(heading);

    float heading_err_rad = heading * deg_rad_conversion_factor;

    float actual_reading = cos(heading_err_rad) * reading;
    float parallel_offset = cos(heading_err_rad) * offset.x;
    float perpendicular_offset = sin(heading_err_rad) * offset.y * -1;

    return dsr_conf_pair<float>(actual_reading + parallel_offset + perpendicular_offset, sensor_confidence);
}

// ============================================================================
// dsr_chassis (from TRChassis.cpp)
// ============================================================================

float dsr_chassis::quadrant_recursive(float heading) {
    if (heading < 0.0f) {
        return quadrant_recursive(heading + 360.0f);
    }

    if (heading > 360.0f) {
        return quadrant_recursive(heading - 360.0f);
    }

    return heading;
}

bool dsr_chassis::can_position_exist(dsr_vector3 pose) {
    return (pose.x > -70.00 && pose.x < 70.00) && (pose.y > -70.00 && pose.y < 70.00);
}

std::string dsr_chassis::get_quadrant_string(dsr_quadrant quadr) {
    switch (quadr) {
        case POS_POS:
            return "POS_POS";
        case NEG_POS:
            return "NEG_POS";
        case NEG_NEG:
            return "NEG_NEG";
        case POS_NEG:
            return "POS_NEG";
    }
    return "";
}

void dsr_chassis::set_active_sensors(int sensors) {
    active_sensors = 0;
    active_sensors |= sensors;
}

dsr_chassis::dsr_chassis(dsr_drivebase_abstract* chas, std::array<dsr_sensor*, 4> sensors, const float field_radius) :
    active_sensors(0), b_display(false), location_task(nullptr), wall_cord(field_radius) {
    north = sensors.at(0);
    east = sensors.at(1);
    south = sensors.at(2);
    west = sensors.at(3);
    chassis = chas;
}

dsr_chassis::~dsr_chassis() {
    delete chassis;
}

dsr_quadrant dsr_chassis::sensor_relevancy() {
    float heading = quadrant_recursive(chassis->getPose().z);

    if ((heading >= 0 && heading <= 45) || (heading <= 360 && heading > 315)) {
        return dsr_quadrant::POS_POS;
    }

    if (heading > 45 && heading <= 135) {
        return dsr_quadrant::NEG_POS;
    }

    if (heading > 135 && heading <= 225) {
        return dsr_quadrant::NEG_NEG;
    }

    if (heading > 225 && heading <= 315) {
        return dsr_quadrant::POS_NEG;
    }

    return dsr_quadrant::NEG_NEG;
}

dsr_quadrant dsr_chassis::sensor_relevancy(float heading) {
    if ((heading >= 0 && heading <= 45) || (heading <= 360 && heading > 315)) {
        return dsr_quadrant::POS_POS;
    }

    if (heading > 45 && heading <= 135) {
        return dsr_quadrant::NEG_POS;
    }

    if (heading > 135 && heading <= 225) {
        return dsr_quadrant::NEG_NEG;
    }

    if (heading > 225 && heading <= 315) {
        return dsr_quadrant::POS_NEG;
    }

    return dsr_quadrant::NEG_NEG;
}

dsr_quadrant dsr_chassis::get_quadrant() {
    dsr_vector3 cur_pose = chassis->getPose();

    if (cur_pose.x > 0 && cur_pose.y > 0) {
        return dsr_quadrant::POS_POS;
    }

    if (cur_pose.x < 0 && cur_pose.y > 0) {
        return dsr_quadrant::NEG_POS;
    }

    if (cur_pose.x < 0 && cur_pose.y < 0) {
        return dsr_quadrant::NEG_NEG;
    }

    if (cur_pose.x > 0 && cur_pose.y < 0) {
        return dsr_quadrant::POS_NEG;
    }

    return dsr_quadrant::POS_POS;
}

// n_p, n_p
dsr_conf_pair<dsr_vector3> dsr_chassis::get_position_calculation(dsr_quadrant quadrant) {
    return get_position_calculation(quadrant, chassis->getPose().z);
}

dsr_conf_pair<dsr_vector3> dsr_chassis::get_position_calculation(dsr_quadrant quadrant, float heading) {
    float normal_heading = quadrant_recursive(heading);
    dsr_quadrant theta_quad = sensor_relevancy(normal_heading);

    dsr_conf_pair<float> n_dist = north->distance(normal_heading);
    dsr_conf_pair<float> e_dist = east->distance(normal_heading);
    dsr_conf_pair<float> s_dist = south->distance(normal_heading);
    dsr_conf_pair<float> w_dist = west->distance(normal_heading);

    dsr_conf_pair<dsr_vector3> ret = dsr_conf_pair<dsr_vector3>();

    float x = 0;
    float y = 0;

    if (quadrant == POS_POS) {
        switch (theta_quad) {
            case POS_POS: {
                ret.set_confidence(conf_avg(e_dist, n_dist));
                x = wall_cord - e_dist.get_value();
                y = wall_cord - n_dist.get_value();
                set_active_sensors(NORTH | EAST);
                break;
            }

            case NEG_POS: {
                ret.set_confidence(conf_avg(n_dist, w_dist));
                x = wall_cord - n_dist.get_value();
                y = wall_cord - w_dist.get_value();
                set_active_sensors(NORTH | WEST);
                break;
            }

            case NEG_NEG: {
                ret.set_confidence(conf_avg(w_dist, s_dist));
                x = wall_cord - w_dist.get_value();
                y = wall_cord - s_dist.get_value();
                set_active_sensors(WEST | SOUTH);
                break;
            }

            case POS_NEG: {
                ret.set_confidence(conf_avg(s_dist, e_dist));
                x = wall_cord - s_dist.get_value();
                y = wall_cord - e_dist.get_value();
                set_active_sensors(SOUTH | EAST);
                break;
            }
        }
    } else if (quadrant == NEG_POS) {
        switch (theta_quad) {
            case POS_POS: {
                ret.set_confidence(conf_avg(n_dist, w_dist));
                x = -wall_cord + w_dist.get_value();
                y = wall_cord - n_dist.get_value();
                set_active_sensors(WEST | NORTH);
                break;
            }

            case NEG_POS: {
                ret.set_confidence(conf_avg(s_dist, w_dist));
                x = -wall_cord + s_dist.get_value();
                y = wall_cord - w_dist.get_value();
                set_active_sensors(SOUTH | WEST);
                break;
            }

            case NEG_NEG: {
                ret.set_confidence(conf_avg(e_dist, s_dist));
                x = -wall_cord + e_dist.get_value();
                y = wall_cord - s_dist.get_value();
                set_active_sensors(EAST | SOUTH);
                break;
            }

            case POS_NEG: {
                ret.set_confidence(conf_avg(e_dist, n_dist));
                x = -wall_cord + n_dist.get_value();
                y = wall_cord - e_dist.get_value();
                set_active_sensors(NORTH | EAST);
                break;
            }
        }
    } else if (quadrant == NEG_NEG) {
        switch (theta_quad) {
            case POS_POS: {
                ret.set_confidence(conf_avg(w_dist, s_dist));
                x = -wall_cord + w_dist.get_value();
                y = -wall_cord + s_dist.get_value();
                set_active_sensors(WEST | SOUTH);
                break;
            }

            case NEG_POS: {
                ret.set_confidence(conf_avg(s_dist, e_dist));
                x = -wall_cord + s_dist.get_value();
                y = -wall_cord + e_dist.get_value();
                set_active_sensors(SOUTH | EAST);
                break;
            }

            case NEG_NEG: {
                ret.set_confidence(conf_avg(e_dist, n_dist));
                x = -wall_cord + e_dist.get_value();
                y = -wall_cord + n_dist.get_value();
                set_active_sensors(EAST | NORTH);
                break;
            }

            case POS_NEG: {
                ret.set_confidence(conf_avg(n_dist, w_dist));
                x = -wall_cord + n_dist.get_value();
                y = -wall_cord + w_dist.get_value();
                set_active_sensors(NORTH | WEST);
                break;
            }
        }
    } else if (quadrant == POS_NEG) {
        switch (theta_quad) {
            case POS_POS: {
                ret.set_confidence(conf_avg(e_dist, s_dist));
                x = wall_cord - e_dist.get_value();
                y = -wall_cord + s_dist.get_value();
                set_active_sensors(EAST | SOUTH);
                break;
            }

            case NEG_POS: {
                ret.set_confidence(conf_avg(n_dist, e_dist));
                x = wall_cord - n_dist.get_value();
                y = -wall_cord + e_dist.get_value();
                set_active_sensors(NORTH | EAST);
                break;
            }

            case NEG_NEG: {
                ret.set_confidence(conf_avg(w_dist, n_dist));
                x = wall_cord - w_dist.get_value();
                y = -wall_cord + n_dist.get_value();
                set_active_sensors(WEST | NORTH);
                break;
            }

            case POS_NEG: {
                ret.set_confidence(conf_avg(s_dist, w_dist));
                x = wall_cord - s_dist.get_value();
                y = -wall_cord + w_dist.get_value();
                set_active_sensors(SOUTH | WEST);
                break;
            }
        }
    } else {
        x = 0;
        y = 0;
        ret.set_confidence(0);
    }

    ret.set_value(dsr_vector3(x, y, normal_heading));

    if (!can_position_exist(dsr_vector3(x, y, normal_heading))) ret.set_confidence(0);

    return ret;
}

float dsr_chassis::conf_avg(dsr_distance one, dsr_distance two) {
    return (one.get_confidence() + two.get_confidence()) / 2.0f;
}

bool dsr_chassis::perform_dsr() {
    return perform_dsr_quad(get_quadrant());
}

bool dsr_chassis::perform_dsr_quad(dsr_quadrant quadrant) {
    dsr_vector3 pose = chassis->getPose();
    dsr_conf_pair<dsr_vector3> coords = get_position_calculation(quadrant);

    pose.x = coords.get_value().x;
    pose.y = coords.get_value().y;

    if (coords.get_confidence() == 0) {
        return false;
    }

    chassis->setPose(pose);

    return true;
}

bool dsr_chassis::perform_dsr_init(dsr_quadrant quadrant, float heading) {
    dsr_vector3 pose = chassis->getPose();
    chassis->setPose(dsr_vector3(0, 0, heading));
    return perform_dsr_quad(quadrant);
}

void dsr_chassis::init_display() {
    pros::lcd::initialize();
}

void dsr_chassis::update_display(dsr_chassis* chassis) {
    bool north = chassis->is_sensor_used(NORTH);
    bool east = chassis->is_sensor_used(EAST);
    bool south = chassis->is_sensor_used(SOUTH);
    bool west = chassis->is_sensor_used(WEST);

    // NOTE: original TitanReset code read heading from a pros::Imu* member
    // that was never assigned by any constructor (guaranteed crash if hit).
    // Pulled from the drivebase pose instead, consistent with every other
    // function in this class.
    float heading = quadrant_recursive(chassis->chassis->getPose().z);

    float dis_n = chassis->north->distance(heading).get_value();
    float dis_e = chassis->east->distance(heading).get_value();
    float dis_s = chassis->south->distance(heading).get_value();
    float dis_w = chassis->west->distance(heading).get_value();

    dsr_conf_pair<dsr_vector3> position = chassis->get_position_calculation(chassis->get_quadrant());
    std::string quads = chassis->get_quadrant_string(chassis->get_quadrant());
    std::string squad = chassis->get_quadrant_string(chassis->sensor_relevancy());

    dsr_vector3 pose_lem = chassis->chassis->getPose();

    pros::lcd::print(0, "SQ: %s, %s", quads.c_str(), squad.c_str());
    pros::lcd::print(1, "SU: N %i, E %i, S %i, W %i", north, east, south, west);
    pros::lcd::print(2, "SR: N %.2f, E %.2f, S %.2f, W %.2f", dis_n, dis_e, dis_s, dis_w);
    pros::lcd::print(4, "PS: X: %.2f,Y: %.2f,H: %.2f,C: %.2f", position.get_value().x, position.get_value().y,
                      heading, position.get_confidence());
    pros::lcd::print(5, "LC: X: %.2f,Y: %.2f,H: %.2f", pose_lem.x, pose_lem.y, pose_lem.z);
}

void dsr_chassis::shutdown_display() {
    pros::lcd::shutdown();
}

void dsr_chassis::start_location_recording(std::string name, std::string date, std::string time) {
    if (location_task != nullptr) location_task->remove();
    delete location_task;
    location_task = new pros::Task([this, name, time, date]() -> void {
        // 1. Sanitize the strings (Replace ' ' and ':' with '-')
        // This handles the "Mar  7 2026" and "17:36:11" formats
        std::string clean_date = date;
        std::string clean_time = time;
        std::string clean_ms = std::to_string(pros::millis());

        for (char& c : clean_date)
            if (c == ' ') c = '-';
        for (char& c : clean_time)
            if (c == ':') c = '-';

        std::string name_comp = name + "_" + clean_date + "_" + clean_time + "_" + clean_ms + ".txt";

        std::ofstream output(name_comp);

        if (!output.is_open()) return;

        while (true) {
            dsr_vector3 pose = chassis->getPose();
            output << pose.x << ", " << pose.y << ", " << pose.z << "\n";
            output.flush();
            pros::Task::delay(50);
        }

        output.close();
    });
}

void dsr_chassis::stop_location_recording() {
    if (location_task != nullptr) {
        location_task->suspend();
        delete location_task;
    }
}

bool dsr_chassis::is_sensor_used(int r_sensor) {
    return active_sensors & (r_sensor);
}