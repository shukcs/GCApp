/****************************************************************************
 *
 *   Copyright (c) 2013-2016 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file px4_custom_mode.h
 * PX4 custom flight modes
 *
 */

#ifndef PX4_CUSTOM_MODE_H_
#define PX4_CUSTOM_MODE_H_

#include <stdint.h>

enum PX4_CUSTOM_MAIN_MODE {
	PX4_CUSTOM_MAIN_MODE_MANUAL = 1,
	PX4_CUSTOM_MAIN_MODE_ALTCTL,
	PX4_CUSTOM_MAIN_MODE_POSCTL,
	PX4_CUSTOM_MAIN_MODE_AUTO,
	PX4_CUSTOM_MAIN_MODE_ACRO,
	PX4_CUSTOM_MAIN_MODE_OFFBOARD,
	PX4_CUSTOM_MAIN_MODE_STABILIZED,
	PX4_CUSTOM_MAIN_MODE_RATTITUDE,
	PX4_CUSTOM_MAIN_MODE_SIMPLE /* unused, but reserved for future use */
};

enum PX4_CUSTOM_SUB_MODE_AUTO {
	PX4_CUSTOM_SUB_MODE_AUTO_READY = 1,
	PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF,
	PX4_CUSTOM_SUB_MODE_AUTO_LOITER,
	PX4_CUSTOM_SUB_MODE_AUTO_MISSION,
	PX4_CUSTOM_SUB_MODE_AUTO_MAG_MISSION,
	PX4_CUSTOM_SUB_MODE_AUTO_RTL,
	PX4_CUSTOM_SUB_MODE_AUTO_LAND,
	PX4_CUSTOM_SUB_MODE_AUTO_RTGS,
	PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET,
	PX4_CUSTOM_SUB_MODE_AUTO_PRECLAND
};
namespace APM {
    enum APM_CUSTOM_MODE {
        STABILIZE = 0,  // manual airframe angle with manual throttle
        ACRO = 1,  // manual body-frame angular rate with manual throttle
        ALT_HOLD = 2,  // manual airframe angle with automatic throttle
        AUTO = 3,  // fully automatic waypoint control using mission commands
        GUIDED = 4,  // fully automatic fly to coordinate or fly at velocity/direction using GCS immediate commands
        LOITER = 5,  // automatic horizontal acceleration with automatic throttle
        RTL = 6,  // automatic return to launching point
        CIRCLE = 7,  // automatic circular flight with automatic throttle

        LAND = 9,  // automatic landing with horizontal position control
        DRIFT = 11,  // semi-autonomous position, yaw and throttle control

        SPORT = 13,  // manual earth-frame angular rate control with manual throttle
        FLIP = 14,  // automatically flip the vehicle on the roll axis
        AUTOTUNE = 15,  // automatically tune the vehicle's roll and pitch gains
        POSHOLD = 16,  // automatic position hold with manual override, with automatic throttle
        BRAKE = 17,  // full-brake using inertial/GPS system, no pilot input
        THROW = 18,  // throw to launch mode using inertial/GPS system, no pilot input
        AVOID_ADSB = 19,  // automatic avoidance of obstacles in the macro scale - e.g. full-sized aircraft
        GUIDED_NOGPS = 20,  // guided mode but only accepts attitude and altitude
        SMART_RTL = 21,  // SMART_RTL returns to home by retracing its steps
        FLOWHOLD = 22,  // FLOWHOLD holds position with optical flow without rangefinder
        FOLLOW = 23,  // follow attempts to follow another vehicle or ground station
        ZIGZAG = 24,  // ZIGZAG mode is able to fly in a zigzag manner with predefined point A and point B
        SYSTEMID = 25,  // System ID mode produces automated system identification signals in the controllers
        AUTOROTATE = 26,  // Autonomous autorotation
        AUTO_RTL = 27,  // Auto RTL, this is not a true mode, AUTO will report as this mode if entered to perform a DO_LAND_START Landing sequence
        TURTLE = 28,  // Flip over after crash
    };
}

union px4_custom_mode {
	struct {
		uint16_t apmRes;
		uint8_t main_mode;
		uint8_t sub_mode;
	};
	uint32_t data;
	float data_float;
	struct {
		uint16_t reserved_hl;
		uint16_t custom_mode_hl;
	};
};

#endif /* PX4_CUSTOM_MODE_H_ */
