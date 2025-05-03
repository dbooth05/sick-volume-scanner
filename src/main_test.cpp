/**
 * Below is the code we have been attempting to use. This does not contain any
 * volumetric calculations yet, as we have not been able to successfully grab
 * frames from the sensor. The failure points in this code are:
 * - lines 59-68 (startAcquisition)
 * - lines 108-111 (attempting to grab frames)
 */

#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>

#include <iostream>

#include "VisionarySData.h"
#include "FrameGrabber.h"
#include "VisionaryControl.h"

const std::string ipAddr = "10.11.11.57";
const int32_t port = 2122;
const uint32_t timeout = 5000;

using namespace visionary;

/**
 * Connect & login to sensor
 * 
 * /param shared pointer to a VisionaryControl object
 * 
 * /retval true, else false if fail to connect or login
 */
bool openSensor(std::shared_ptr<VisionaryControl>& visControl) {
    if (!visControl->open(VisionaryControl::ProtocolType::COLA_2, ipAddr, port)) {
        std::cerr << "Failed to connect to sensor with ip: " << ipAddr << std::endl;
        return false;
    } else {
        std::clog << "Connected to sensor with ip: " << ipAddr << std::endl;
    }

    if (!visControl->login(IAuthentication::UserLevel::AUTHORIZED_CLIENT, "CLIENT")) {
        std::cerr << "Failed to login to sensor" << std::endl;
        visControl->close();
        return false;
    } else {
        std::clog << "Logged into sensor" << std::endl;
    }

    return true;
}

/**
 * Attempts to start at acquisition. 
 * This is the MAIN failure point we have had in testing.
 * 
 * /param shared pointer to a VisionaryControl object
 * 
 * /retval true, else false if fail to start acquisition
 */
bool startAcquisition(std::shared_ptr<VisionaryControl>& visControl) {
    if (!visControl->startAcquisition()) {
        std::cerr << "Failed to start acquisition" << std::endl;
        return false;
    } else {
        std::clog << "Started acquisition" << std::endl;
    }

    return true;
}

/**
 * Logs out from sensor and disconnects
 * 
 * /param shared pointer to a VisionaryControl object
 */
void closeSensor(std::shared_ptr<VisionaryControl>& visControl) {
    visControl->logout();
    visControl->close();

    std::clog << "Closed sensor" << std::endl;
}

int main(int argc, char* argv[]) {

    std::shared_ptr<VisionaryControl> visControl = std::make_shared<VisionaryControl>();

    if (!openSensor(visControl)) {
        return 1;
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));

    if (!startAcquisition(visControl)) {
        closeSensor(visControl);
        return 1;
    }

    /**
     * Infinite loop to attempt to grab frames from sensor.
     * 
     * Will immediately error on line 100 (frameGrabber.getNextFrame(visData)).
     * This is 1 of the failure points we have ran into in testing.
     */
    FrameGrabber<VisionarySData> frameGrabber(ipAddr, port, timeout);
    std::shared_ptr<VisionarySData> visData = std::make_shared<VisionarySData>();

    while (true) {
        if (!frameGrabber.getNextFrame(visData)) {
            std::cout << "Failed to grab frame" << std::endl;
            closeSensor(visControl);
            return 1;
        } else {
            std::clog << "Successfully grabbed frame" << std::endl;
        }
    }
    /**
     * End of attempting to grab frames
     */

    std::this_thread::sleep_for(std::chrono::seconds(1));

    closeSensor(visControl);

    return 0;
}
