cmake_minimum_required(VERSION 3.16)

project(Project_Orion VERSION 0.3.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS
    Quick
    Multimedia
)

qt_standard_project_setup(REQUIRES 6.10)

find_path(RTLSDR_INCLUDE_DIR
    NAMES rtl-sdr.h
    PATHS
        C:/vcpkg/installed/x64-windows/include
    REQUIRED
)

find_library(RTLSDR_LIBRARY
    NAMES rtlsdr
    PATHS
        C:/vcpkg/installed/x64-windows/lib
    REQUIRED
)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(importedcontent)
endif()

qt_add_executable(Project_Orion
    main.cpp
)

qt_add_qml_module(Project_Orion
    URI HF_SDR_Display
    VERSION 1.0

    QML_FILES
        qml/Main.qml
        qml/FrequencyDisplay.qml
        qml/SpectrumDisplay.qml
        qml/WaterfallDisplay.qml
        qml/ControlBar.qml

    SOURCES
        src/core/IQBuffer.h
        src/core/IQBuffer.cpp
        src/core/IQBlockRingBuffer.h
        src/core/IQBlockRingBuffer.cpp
        src/core/Logger.h
        src/core/Logger.cpp
        src/core/Version.h
        src/core/RadioTypes.h

        src/sources/IQSource.h
        src/sources/IQSource.cpp
        src/sources/IQSourceWorker.h
        src/sources/IQSourceWorker.cpp
        src/sources/RTLDevice.h
        src/sources/RTLDevice.cpp
        src/sources/SDRManager.h
        src/sources/SDRManager.cpp
        src/sources/SignalGenerator.h
        src/sources/SignalGenerator.cpp

        src/dsp/DSPProcessor.h
        src/dsp/DSPProcessor.cpp
        src/dsp/DSPWorker.h src/dsp/DSPWorker.cpp
        src/dsp/FIRFilter.h
        src/dsp/FIRFilter.cpp
        src/dsp/DigitalDownConverter.h
        src/dsp/DigitalDownConverter.cpp
        src/dsp/Decimator.h
        src/dsp/Decimator.cpp
        src/dsp/AGCProcessor.h
        src/dsp/AGCProcessor.cpp
        src/dsp/AMDemodulator.h
        src/dsp/AMDemodulator.cpp
        src/dsp/DSBDemodulator.h
        src/dsp/DSBDemodulator.cpp

        src/audio/AudioDecimator.h
        src/audio/AudioDecimator.cpp
        src/audio/AudioFIRFilter.h
        src/audio/AudioFIRFilter.cpp
        src/audio/AudioOutput.h
        src/audio/AudioOutput.cpp

        src/radio/Radio.h
        src/radio/Radio.cpp
        src/radio/RadioController.h
        src/radio/RadioController.cpp
        src/radio/Receiver.h
        src/radio/Receiver.cpp
        src/radio/vfo.h
        src/radio/vfo.cpp

        src/gui/Orion.h
        src/gui/Orion.cpp
        src/gui/Displaysettings.h
        src/gui/Displaysettings.cpp
        src/core/ReceiverConfiguration.h
        src/dsp/NFMDemodulator.h
        src/dsp/NFMDemodulator.cpp
        src/dsp/WFMDemodulator.h
        src/dsp/WFMDemodulator.cpp
        src/dsp/ProductDetector.h
        src/dsp/ProductDetector.cpp
        src/dsp/ComplexSidebandFilter.h
        src/dsp/ComplexSidebandFilter.cpp

        src/spectrum/FFTAccumulator.h
        src/spectrum/FFTAccumulator.cpp
        src/spectrum/FFTProcessor.h
        src/spectrum/FFTProcessor.cpp
        src/spectrum/SpectrumCalibrator.h
        src/spectrum/SpectrumCalibrator.cpp
        src/spectrum/SpectrumDetector.h
        src/spectrum/SpectrumDetector.cpp
        src/spectrum/SpectrumSmoother.h
        src/spectrum/SpectrumSmoother.cpp
        src/spectrum/SpectrumEngine.h
        src/spectrum/SpectrumEngine.cpp
        src/spectrum/SpectrumWorker.h
        src/spectrum/SpectrumWorker.cpp

        src/core/IQSourceCapabilities.h


)

set_target_properties(Project_Orion PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING
        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(Project_Orion
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sources
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dsp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/audio
        ${CMAKE_CURRENT_SOURCE_DIR}/src/radio
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
        ${CMAKE_CURRENT_SOURCE_DIR}/src/spectrum
        ${RTLSDR_INCLUDE_DIR}
)

target_link_libraries(Project_Orion
    PRIVATE
        Qt6::Quick
        Qt6::Multimedia
        ${RTLSDR_LIBRARY}
)