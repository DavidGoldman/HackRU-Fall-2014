// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>


// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>

#define UPDATES_PER_SEC 5

// Main class for the project, listens to myo data.
class DataCollector : public myo::DeviceListener {
private:
    myo::Arm whichArm;
    myo::Pose currentPose;
public:
	bool onArm;
	int reps = 0;
	int roll_w, pitch_w, yaw_w;
	int roll_thresh, pitch_thresh, yaw_thresh;
	const int THRESH_S = 10;
    DataCollector()
    : onArm(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose()
    {
    }

    // onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
    void onUnpair(myo::Myo* myo, uint64_t timestamp)
    {
        // We've lost a Myo.
        // Let's clean up some leftover state.
        roll_w = 0;
        pitch_w = 0;
        yaw_w = 0;
        onArm = false;
    }

    // onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
    // as a unit quaternion.
    void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
    {
        using std::atan2;
        using std::asin;
        using std::sqrt;
        using std::max;
        using std::min;

        // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
        float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                           1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
        float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
        float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                        1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));

        // Convert the floating point angles in radians to degrees (0 to 360)
        roll_w = static_cast<int>((roll + (float)M_PI)/(M_PI * 2.0f) * 360);
        pitch_w = static_cast<int>((pitch + (float)M_PI/2.0f)/M_PI * 360);
        yaw_w = static_cast<int>((yaw + (float)M_PI)/(M_PI * 2.0f) * 360);
    }

    // onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
    // making a fist, or not making a fist anymore.
    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
        if (pose != currentPose) {
            std::cout << "Pose changed to " << pose.toString() << std::endl;
        }
        currentPose = pose;

        // Vibrate the Myo whenever we've detected that the user has made a fist.
        if (pose == myo::Pose::fist) {
            myo->vibrate(myo::Myo::vibrationMedium);
        }
    }

    // onArmRecognized() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
    // arm. This lets Myo know which arm it's on and which way it's facing.
    void onArmRecognized(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
    {
        onArm = true;
        whichArm = arm;
    }

    // onArmLost() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
    // it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
    // when Myo is moved around on the arm.
    void onArmLost(myo::Myo* myo, uint64_t timestamp)
    {
        onArm = false;
    }

    // There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
    // For this example, the functions overridden above are sufficient.

    // We define this function to print the current values that were updated by the on...() functions above.
    void print()
    {
        // Clear the current line
        std::cout << '\r';

        // Print out the orientation data.
        // Scale the angles to be [0, 18] for printing.
        int scaledRoll = roll_w / 20;
        int scaledPitch = pitch_w / 20;
        int scaledYaw = yaw_w / 20;
        std::cout << '[' << std::string(scaledRoll, '*') << std::string(18 - scaledRoll, ' ') << ']'
                  << '[' << std::string(scaledPitch, '*') << std::string(18 - scaledPitch, ' ') << ']'
                  << '[' << std::string(scaledYaw, '*') << std::string(18 - scaledYaw, ' ') << ']';

        if (onArm) {
            // Print out the currently recognized pose and which arm Myo is being worn on.

            // Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
            // output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
            // that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().
            std::string poseString = currentPose.toString();

            std::cout << '[' << (whichArm == myo::armLeft ? "L" : "R") << ']'
                      << '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
        } else {
            // Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
            std::cout << "[?]" << '[' << std::string(14, ' ') << ']';
        }

        std::cout << std::flush;
    }

    void calibrationPrint()
    {
        std::cout << "{ " << roll_w << ", " << pitch_w << ", " << yaw_w << " }," << std::endl << std::flush;

    }
};

int main(int argc, char** argv)
{
	std::string argument = "curl";
    // We catch any exceptions that might occur below -- see the catch statement for more details.
    try {
    myo::Hub hub("com.dgoldman.hackru-fall-2014");

    std::cout << "Attempting to find a Myo..." << std::endl;

    // Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
    // immediately.
    // waitForAnyMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
    // if that fails, the function will return a null pointer.
    myo::Myo* myo = hub.waitForMyo(10000);
    if (!myo) {
        throw std::runtime_error("Unable to find a Myo!");
    }

    // We've found a Myo.
    std::cout << "Connected to a Myo armband!" << std::endl << std::endl;
	std::cout << "Calibrate Myo" << std::endl;
	std::cout << "You are doing a " << argument << " workout." << std::endl;
	system("pause");
		if (argument.compare("Dumbells"))
		{
			// Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
			DataCollector collector;
			hub.addListener(&collector);
			hub.run(1000 / UPDATES_PER_SEC);
			bool down = false;
			int starting_pitch = collector.pitch_w; //average starting pitch based on test
			int count = 0;
			int prev_pitch;
			collector.roll_thresh = 20;
			collector.yaw_thresh = 50;

			std::cout << "Roll thresh = " << collector.roll_thresh << std::endl;
			std::cout << "Yaw thresh = " << collector.yaw_thresh << std::endl;
			std::cout << "Pick up your weight, get in starting position, and hit enter to continue" << std::endl;
			system("pause");
			hub.run(1000 / UPDATES_PER_SEC);
			starting_pitch = collector.pitch_w;
			int roll_s = collector.roll_w;
			int yaw_s = collector.yaw_w;
			down = false;
			count = 0; //counts in between vibrations
			// Finally we enter our main loop.
			while (1) {
				prev_pitch = collector.pitch_w;
				collector.calibrationPrint();
				if (abs(collector.roll_w - roll_s) > collector.roll_thresh && count > 10)
				{
					myo->vibrate(myo::Myo::vibrationMedium);
					count = 0;
				}
				if (abs(collector.yaw_w - yaw_s) > collector.yaw_thresh && count > 10)
				{
					myo->vibrate(myo::Myo::vibrationMedium);
					count = 0;
				}
				count++;
				hub.run(1000 / UPDATES_PER_SEC);
				// collector.print(); // Print info
				if (prev_pitch > collector.pitch_w)
					down = true;
				else if (down && prev_pitch < collector.pitch_w)
				{
					down = false;
					collector.reps++;
					std::cout << "REP" << std::endl;
				}
			}
		}
		else
		{
			// Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
			DataCollector collector;
			hub.addListener(&collector);
			hub.run(1000 / UPDATES_PER_SEC);
			bool down = false;
			int starting_pitch = collector.pitch_w; //average starting pitch based on test
			int count = 0;
			int prev_pitch;
			collector.roll_thresh = 25;
			collector.yaw_thresh = 25;

			std::cout << "Roll thresh = " << collector.roll_thresh << std::endl;
			std::cout << "Yaw thresh = " << collector.yaw_thresh << std::endl;
			std::cout << "Pick up your weight, get in starting position, and hit enter to continue" << std::endl;
			system("pause");
			hub.run(1000 / UPDATES_PER_SEC);
			starting_pitch = collector.pitch_w;
			int roll_s = collector.roll_w;
			int yaw_s = collector.yaw_w;
			down = false;
			count = 0; //counts in between vibrations
			// Finally we enter our main loop.
			while (1) {
				prev_pitch = collector.pitch_w;
				collector.calibrationPrint();
				if (abs(collector.roll_w - roll_s) > collector.roll_thresh && count > 10)
				{
					myo->vibrate(myo::Myo::vibrationMedium);
					count = 0;
				}
				if (abs(collector.yaw_w - yaw_s) > collector.yaw_thresh && count > 10)
				{
					myo->vibrate(myo::Myo::vibrationMedium);
					count = 0;
				}
				count++;
				hub.run(1000 / UPDATES_PER_SEC);
				// collector.print(); // Print info
				if (prev_pitch > collector.pitch_w)
					down = true;
				else if (down && prev_pitch < collector.pitch_w)
				{
					down = false;
					collector.reps++;
					std::cout << "REP" << std::endl;
				}
		}
    }
	/*while (1)
	{
		collector.calibrationPrint();
		hub.run(1000 / UPDATES_PER_SEC);
	}*/

    // If a standard exception occurred, we print out its message and exit.
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
        return 1;
    }
}
