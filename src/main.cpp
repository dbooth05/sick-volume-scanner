#include <iostream>
#include <memory>

#include "VisionaryControl.h"
#include "VisionarySData.h"
#include "FrameGrabber.h"
#include "ITransport.h"

#include "CoLaCommand.h"
#include "CoLaParameterWriter.h"
#include "CoLaParameterReader.h"
#include "CoLaCommandType.h"

const std::string ipAddr = "10.11.11.57";
const uint32_t timeout = 5000;

using namespace visionary;

namespace visionary {

    void cola(std::shared_ptr<VisionaryControl> visControl) {

        CoLaParameterWriter writer(CoLaCommandType::READ_VARIABLE, "DeviceIdent");
        CoLaCommand cmd = writer.build();

        CoLaCommand response = visControl->sendCommand(cmd);

        CoLaParameterReader reader(response.getBuffer());

        if (response.getError() != CoLaError::OK) {
            std::cerr << "cola cmd failed with error: " << static_cast<int>(response.getError()) << std::endl;
            return;
        }

        std::string currApp = reader.readFlexString();

        std::cout << "Current App: " << currApp << std::endl;
    }

    void cola_run(std::shared_ptr<VisionaryControl> visControl) {
        CoLaParameterWriter writer(CoLaCommandType::METHOD_INVOCATION, "Run");
        CoLaCommand cmd = writer.build();
        CoLaCommand response = visControl->sendCommand(cmd);
        if (response.getError() != CoLaError::OK) {
            std::cerr << "Failed to invoke Run: Error Code: " << static_cast<int>(response.getError()) << std::endl;
        } else {
            std::cout << "Successfully started run" << std::endl;
        }
    
        // Wait for acquisition state to be 'running'
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
        CoLaParameterWriter statusWriter(CoLaCommandType::READ_VARIABLE, "AcquisitionMode"); // Acq...State, State, AppState
        CoLaCommand statusCmd = statusWriter.build();
        CoLaCommand statusResponse = visControl->sendCommand(statusCmd);
        CoLaParameterReader statusReader(statusResponse.getBuffer());
        
        if (statusResponse.getError() == visionary::CoLaError::OK) {
            uint8_t state = statusReader.readUSInt(); // 0: stopped, 1: running
            std::cout << "AcquisitionState: " << static_cast<int>(state) << std::endl;
        } else {
            std::cerr << "Failed getting acquisition state" << std::endl;
        }
    
        // Ensure acquisition is running
        if (statusReader.readUSInt() != 1) {
            std::cerr << "Acquisition is not running!" << std::endl;
            return;
        }
    
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
        // Start acquisition if it was not already started
        if (!visControl->startAcquisition()) {
            std::cerr << "Failed to start acquisition" << std::endl;
            return;
        }
    
        // Grab a frame
        std::shared_ptr<visionary::VisionarySData> visData = std::make_shared<visionary::VisionarySData>();
        std::shared_ptr<visionary::FrameGrabber<visionary::VisionarySData>> frameGrabber =
        std::make_shared<visionary::FrameGrabber<visionary::VisionarySData>>(ipAddr, 2123, timeout);
    
        if (frameGrabber->getNextFrame(visData)) {
            std::cout << "Grabbed a new frame" << std::endl;
        } else {
            std::cerr << "Failed to grab a frame" << std::endl;
        }
    }
    
}

void close(std::shared_ptr<visionary::VisionaryControl> visControl) {
    visControl->logout();
    visControl->close();
}

int main(int argc, char* argv[]) {

    // Create data interface
    std::shared_ptr<visionary::VisionaryControl> visControl = std::make_shared<visionary::VisionaryControl>();

    // Open control connection on default port (2111 for CoLa)
    if (!visControl->open(visionary::VisionaryControl::ProtocolType::COLA_2, ipAddr, timeout)) {
        std::cerr << "Failed to open control connection." << std::endl;
        return 1;
    } else {
        std::cout << "Successfully connected to sensor at: " << ipAddr << std::endl;
    }

    // Login
    if (!visControl->login(visionary::IAuthentication::UserLevel::AUTHORIZED_CLIENT, "CLIENT")) {
        std::cerr << "Failed to login!" << std::endl;
        return 1;
    } else {
        std::cout << "Successfully logged into sensor at: " << ipAddr << std::endl;
    }

    visionary::cola(visControl);
    visionary::cola_run(visControl);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    visControl->stopAcquisition();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // if (!visControl->startAcquisition()) {
    //     std::cerr << "Failed to start acquisition" << std::endl;
    //     close(visControl);
    //     return 1;
    // }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::shared_ptr<visionary::VisionarySData> visdata = std::make_shared<visionary::VisionarySData>();

    std::shared_ptr<visionary::FrameGrabber<visionary::VisionarySData>> frameGrabber = std::make_shared<visionary::FrameGrabber<visionary::VisionarySData>>(ipAddr, 2123, timeout);

    std::shared_ptr<visionary::VisionarySData> visData = std::make_shared<visionary::VisionarySData>();
    if (frameGrabber->getNextFrame(visData)) {
        std::cout << "Grabbed a new frame" << std::endl;
    } else {
        std::cerr << "failed to grab a frame" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    close(visControl);

    return 0;

}
