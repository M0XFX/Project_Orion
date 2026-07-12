cmake_minimum_required(VERSION 3.16)

project(Project_Orion VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Multimedia)

qt_standard_project_setup(REQUIRES 6.10)

# Content of downloaded design from Figma to Qt or similar
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

qt_add_executable(appHF_SDR_Display
    main.cpp
)

qt_add_qml_module(appHF_SDR_Display
    URI HF_SDR_Display
    QML_FILES
	QML_FILES qml/Main.qml
	QML_FILES qml/FrequencyDisplay.qml
	QML_FILES qml/SpectrumDisplay.qml
	QML_FILES qml/WaterfallDisplay.qml
	QML_FILES qml/ControlBar.qml
        SOURCES
        SOURCES Radio.h Radio.cpp
        SOURCES vfo.h vfo.cpp
        SOURCES FFTProcessor.h FFTProcessor.cpp
        SOURCES IQBuffer.h IQBuffer.cpp
        SOURCES SignalGenerator.h SignalGenerator.cpp
        SOURCES Orion.h Orion.cpp
        SOURCES Receiver.h Receiver.cpp
        SOURCES Logger.h Logger.cpp
        SOURCES RTLDevice.h RTLDevice.cpp
        SOURCES Displaysettings.h Displaysettings.cpp
        SOURCES IQSource.h IQSource.cpp
        SOURCES DSPProcessor.h DSPProcessor.cpp
        SOURCES FIRFilter.h FIRFilter.cpp
        SOURCES DigitalDownConverter.h DigitalDownConverter.cpp
        SOURCES Decimator.h Decimator.cpp
        SOURCES AGCProcessor.h AGCProcessor.cpp
        SOURCES AMDemodulator.h AMDemodulator.cpp
        SOURCES AudioDecimator.h AudioDecimator.cpp
        SOURCES AudioFIRFilter.h AudioFIRFilter.cpp
        SOURCES AudioOutput.h AudioOutput.cpp
        SOURCES IQBlockRingBuffer.h IQBlockRingBuffer.cpp
        SOURCES IQSourceWorker.h IQSourceWorker.cpp
        SOURCES Version.h
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appHF_SDR_Display PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appHF_SDR_Display
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appHF_SDR_Display
    PRIVATE
        Qt6::Quick
        Qt6::Multimedia
        ${RTLSDR_LIBRARY}
)

find_path(RTLSDR_INCLUDE_DIR rtl-sdr.h
    PATHS C:/vcpkg/installed/x64-windows/include
)

find_library(RTLSDR_LIBRARY rtlsdr
    PATHS C:/vcpkg/installed/x64-windows/lib
)

target_include_directories(appHF_SDR_Display PRIVATE
    ${RTLSDR_INCLUDE_DIR}
)

target_link_libraries(appHF_SDR_Display
    PRIVATE
        Qt6::Quick
        ${RTLSDR_LIBRARY}
)
