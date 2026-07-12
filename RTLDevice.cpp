#include "RTLDevice.h"
#include "Logger.h"

#include <rtl-sdr.h>
#include <vector>

RTLDevice::RTLDevice(QObject* parent)
    : QObject(parent)
{
    HFSDR::Logger::info("RTLDevice created.");
}

RTLDevice::~RTLDevice()
{
    // Do not call the virtual close() method from the destructor.
    // Perform the hardware cleanup directly instead.
    if (m_device) {
        HFSDR::Logger::info(
            "Closing RTL-SDR device during shutdown."
            );

        rtlsdr_cancel_async(m_device);
        rtlsdr_close(m_device);
        m_device = nullptr;
    }
}

QString RTLDevice::status() const
{
    return m_status;
}

bool RTLDevice::connected() const
{
    return m_connected;
}

quint64 RTLDevice::centerFrequencyHz() const
{
    return m_centerFrequencyHz;
}

quint32 RTLDevice::sampleRate() const
{
    return m_sampleRate;
}

void RTLDevice::open()
{
    if (m_device) {
        HFSDR::Logger::warning(
            "RTL-SDR device is already open."
            );
        return;
    }

    const uint32_t deviceCount =
        rtlsdr_get_device_count();

    HFSDR::Logger::info(
        QString("RTL-SDR devices found: %1")
            .arg(deviceCount)
        );

    if (deviceCount == 0) {
        setConnected(false);
        setStatus("No RTL-SDR Found");
        return;
    }

    HFSDR::Logger::info(
        "Opening RTL-SDR device..."
        );

    const int openResult =
        rtlsdr_open(&m_device, 0);

    if (openResult != 0 || m_device == nullptr) {
        m_device = nullptr;

        setConnected(false);
        setStatus("RTL-SDR Open Failed");

        HFSDR::Logger::error(
            "Failed to open RTL-SDR device."
            );

        return;
    }

    HFSDR::Logger::info(
        "RTL-SDR opened successfully."
        );

    const int sampleRateResult =
        rtlsdr_set_sample_rate(
            m_device,
            m_sampleRate
            );

    if (sampleRateResult != 0) {
        HFSDR::Logger::warning(
            "Failed to set RTL-SDR sample rate."
            );
    }

    const int frequencyResult =
        rtlsdr_set_center_freq(
            m_device,
            static_cast<uint32_t>(
                m_centerFrequencyHz
                )
            );

    if (frequencyResult != 0) {
        HFSDR::Logger::warning(
            "Failed to set RTL-SDR centre frequency."
            );
    }

    const int gainResult =
        rtlsdr_set_tuner_gain_mode(
            m_device,
            0
            );

    if (gainResult != 0) {
        HFSDR::Logger::warning(
            "Failed to set RTL-SDR automatic gain mode."
            );
    }

    const int resetResult =
        rtlsdr_reset_buffer(m_device);

    if (resetResult != 0) {
        HFSDR::Logger::warning(
            "Failed to reset RTL-SDR buffer."
            );
    }

    HFSDR::Logger::info(
        QString("RTL-SDR sample rate: %1")
            .arg(m_sampleRate)
        );

    HFSDR::Logger::info(
        QString("RTL-SDR centre frequency: %1 Hz")
            .arg(m_centerFrequencyHz)
        );

    HFSDR::Logger::info(
        "RTL-SDR gain mode: auto"
        );

    setConnected(true);
    setStatus("RTL-SDR Connected");
}

void RTLDevice::close()
{
    if (m_device) {
        HFSDR::Logger::info(
            "Closing RTL-SDR device."
            );

        rtlsdr_cancel_async(m_device);
        rtlsdr_close(m_device);
        m_device = nullptr;
    }

    setConnected(false);
    setStatus("Not Connected");
}

bool RTLDevice::readSamples(
    HFSDR::IQBuffer& buffer)
{
    if (!m_device ||
        !m_connected ||
        buffer.empty()) {
        return false;
    }

    const int sampleCount =
        static_cast<int>(buffer.size());

    const int byteCount =
        sampleCount * 2;

    std::vector<unsigned char> raw(
        static_cast<std::size_t>(byteCount)
        );

    int bytesRead = 0;

    const int readResult =
        rtlsdr_read_sync(
            m_device,
            raw.data(),
            byteCount,
            &bytesRead
            );

    if (readResult != 0 ||
        bytesRead != byteCount) {

        HFSDR::Logger::warning(
            QString(
                "RTL-SDR sync read failed: "
                "requested %1 bytes, received %2."
                )
                .arg(byteCount)
                .arg(bytesRead)
            );

        return false;
    }

    for (int i = 0; i < sampleCount; ++i) {
        const std::size_t byteIndex =
            static_cast<std::size_t>(i * 2);

        const float iSample =
            (static_cast<float>(raw[byteIndex])
             - 127.5f) / 127.5f;

        const float qSample =
            (static_cast<float>(
                 raw[byteIndex + 1])
             - 127.5f) / 127.5f;

        buffer.samples()[
            static_cast<std::size_t>(i)
        ] = HFSDR::IQBuffer::Sample(
            iSample,
            qSample
            );
    }

    return true;
}

void RTLDevice::setStatus(
    const QString& status)
{
    if (m_status == status)
        return;

    m_status = status;
    emit statusChanged();
}

void RTLDevice::setConnected(
    bool connected)
{
    if (m_connected == connected)
        return;

    m_connected = connected;
    emit connectedChanged();
}