#include <iostream>
#include <memory>

#include "VisionaryControl.h"
#include "VisionarySData.h"
#include "VisionaryDataStream.h"
#include "FrameGrabber.h"
#include "ITransport.h"

void close(std::shared_ptr<visionary::VisionaryControl> visControl) {
    visControl->logout();
    visControl->close();
}

int main(int argc, char* argv[]) {
    const std::string ipAddr = "10.11.11.57";
    const uint32_t timeout = 5000;

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

    // Open data stream connection on default port (2122 for data)
    // auto dataStream = std::make_shared<visionary::VisionaryDataStream>(visData);
    // if (!dataStream->open(ipAddr, 2122)) {
    //     std::cerr << "Failed to open data stream!" << std::endl;
    //     close(visControl);
    //     return 1;
    // } else {
    //     std::cout << "Successfully opened data stream!" << std::endl;
    // }
    //
    // // Start acquisition
    // visControl->stopAcquisition();
    // if (!visControl->startAcquisition()) {
    //     std::cerr << "Failed to start acquisition!" << std::endl;
    //     close(visControl);
    //     return 1;
    // } else {
    //     std::cout << "Successfully started acquisition" << std::endl;
    // }

    // Get next frame
    // if (dataStream->getNextFrame()) {
    //     std::cout << "Received frame from sensor" << std::endl;
    //
    //     const auto& zMap = visData->getZMap();
    //     std::cout << "Z map size: " << zMap.size() << std::endl;
    //
    //     const auto& rgba = visData->getRGBAMap();
    //     std::cout << "First 5 RGBA values: ";
    //     for (size_t i = 0; i < std::min(rgba.size(), size_t(5)); ++i) {
    //         std::cout << std::hex << rgba[i] << " ";
    //     }
    //     std::cout << std::dec << std::endl;
    // } else {
    //     std::cerr << "Failed to grab frame!" << std::endl;
    // }
    // //
    //
    // dataStream->close();

    close(visControl);

    return 0;

}
