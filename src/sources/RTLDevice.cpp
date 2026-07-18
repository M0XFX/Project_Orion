#include "RTLDevice.h"
#include "Logger.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

RTLDevice::RTLDevice(QObject* parent)
    : QObject(parent)
{
    HFSDR::Logger::info("RTLDevice created.");
}

RTLDevice::~RTLDevice()
{
    close();
}

QString RTLDevice::status() const { return m_status; }
bool RTLDevice::connected() const { return m_connected; }
quint64 RTLDevice::centerFrequencyHz() const { return m_appliedCenterFrequencyHz; }
quint32 RTLDevice::sampleRate() const { return m_sampleRate; }
bool RTLDevice::automaticGain() const { return m_automaticGain; }
double RTLDevice::gainDb() const { return static_cast<double>(m_appliedGainTenthsDb) / 10.0; }

HFSDR::IQSourceCapabilities RTLDevice::capabilities() const
{
    HFSDR::IQSourceCapabilities c;
    c.deviceName = "RTL-SDR";
    c.manufacturer = "Generic RTL2832U";
    c.sampleRateHz = m_sampleRate;
    c.maximumBandwidthHz = m_sampleRate;
    c.usableBandwidthHz = static_cast<quint32>(static_cast<double>(m_sampleRate) * 0.88);
    c.defaultSpectrumSpanHz = c.usableBandwidthHz;
    c.supportedSpectrumSpansHz = {10000, 25000, 50000, 100000, 250000, 500000, 1000000, c.usableBandwidthHz};
    for (int gain : m_supportedGainsTenthsDb)
        c.supportedGainValuesDb.append(static_cast<double>(gain) / 10.0);
    c.supportsAutomaticGain = true;
    c.supportsManualGain = true;
    c.supportsDirectSampling = true;
    c.supportsBiasTee = false;
    c.supportsTransmit = false;
    c.supportsFullDuplex = false;
    c.receiveChannelCount = 1;
    c.transmitChannelCount = 0;
    return c;
}

void RTLDevice::open()
{
    if (m_device) {
        HFSDR::Logger::warning("RTL-SDR device is already open.");
        return;
    }

    const uint32_t count = rtlsdr_get_device_count();
    HFSDR::Logger::info(QString("RTL-SDR devices found: %1").arg(count));
    if (count == 0) {
        setConnected(false);
        setStatus("No RTL-SDR Found");
        return;
    }

    if (rtlsdr_open(&m_device, 0) != 0 || !m_device) {
        m_device = nullptr;
        setConnected(false);
        setStatus("RTL-SDR Open Failed");
        HFSDR::Logger::error("Failed to open RTL-SDR device.");
        return;
    }

    HFSDR::Logger::info("RTL-SDR opened successfully.");

    if (rtlsdr_set_sample_rate(m_device, m_sampleRate) != 0)
        HFSDR::Logger::warning("Failed to set RTL-SDR sample rate.");

    querySupportedGains();

    if (rtlsdr_reset_buffer(m_device) != 0)
        HFSDR::Logger::warning("Failed to reset RTL-SDR buffer.");

    setConnected(true);
    setStatus("RTL-SDR Connected");

    HFSDR::Logger::info(QString("RTL-SDR sample rate: %1").arg(m_sampleRate));
    HFSDR::Logger::info(QString("RTL-SDR reported %1 manual gain steps.").arg(m_supportedGainsTenthsDb.size()));
}

void RTLDevice::close()
{
    if (m_device) {
        HFSDR::Logger::info("Closing RTL-SDR device.");
        rtlsdr_cancel_async(m_device);
        rtlsdr_close(m_device);
        m_device = nullptr;
    }

    m_appliedCenterFrequencyHz = 0;
    m_supportedGainsTenthsDb.clear();
    setConnected(false);
    setStatus("Not Connected");
}

bool RTLDevice::setCenterFrequencyHz(quint64 frequencyHz)
{
    if (!m_device || !m_connected) {
        HFSDR::Logger::warning("Cannot tune RTL-SDR because it is not connected.");
        return false;
    }

    if (frequencyHz > std::numeric_limits<uint32_t>::max()) {
        HFSDR::Logger::error("Requested RTL-SDR frequency exceeds librtlsdr range.");
        return false;
    }

    if (rtlsdr_set_center_freq(m_device, static_cast<uint32_t>(frequencyHz)) != 0) {
        HFSDR::Logger::warning(QString("Failed to tune RTL-SDR to %1 Hz.").arg(frequencyHz));
        return false;
    }

    const uint32_t actual = rtlsdr_get_center_freq(m_device);
    m_appliedCenterFrequencyHz = actual ? static_cast<quint64>(actual) : frequencyHz;
    emit centerFrequencyChanged();
    HFSDR::Logger::info(QString("RTL-SDR tuned to %1 Hz.").arg(m_appliedCenterFrequencyHz));
    return true;
}

bool RTLDevice::setAutomaticGain(bool enabled)
{
    if (!m_device || !m_connected) {
        HFSDR::Logger::warning("Cannot change RTL-SDR gain mode because it is not connected.");
        return false;
    }

    if (rtlsdr_set_tuner_gain_mode(m_device, enabled ? 0 : 1) != 0) {
        HFSDR::Logger::warning("Failed to change RTL-SDR tuner gain mode.");
        return false;
    }

    if (m_automaticGain != enabled) {
        m_automaticGain = enabled;
        emit gainModeChanged();
    }

    HFSDR::Logger::info(QString("RTL-SDR gain mode: %1.").arg(enabled ? "automatic" : "manual"));
    return true;
}

bool RTLDevice::setGainDb(double gainDb)
{
    if (!m_device || !m_connected) {
        HFSDR::Logger::warning("Cannot set RTL-SDR gain because it is not connected.");
        return false;
    }
    if (m_automaticGain) {
        HFSDR::Logger::warning("Cannot set manual RTL-SDR gain while automatic gain is enabled.");
        return false;
    }

    const int requested = static_cast<int>(std::lround(gainDb * 10.0));
    const int selected = nearestSupportedGainTenthsDb(requested);
    if (rtlsdr_set_tuner_gain(m_device, selected) != 0) {
        HFSDR::Logger::warning(QString("Failed to set RTL-SDR gain to %1 dB.").arg(gainDb, 0, 'f', 1));
        return false;
    }

    const int actual = rtlsdr_get_tuner_gain(m_device);
    m_appliedGainTenthsDb = actual != 0 ? actual : selected;
    emit gainChanged();
    HFSDR::Logger::info(QString("RTL-SDR manual gain set to %1 dB (requested %2 dB).")
        .arg(this->gainDb(), 0, 'f', 1)
        .arg(gainDb, 0, 'f', 1));
    return true;
}

bool RTLDevice::readSamples(HFSDR::IQBuffer& buffer)
{
    if (!m_device || !m_connected || buffer.empty())
        return false;

    const int sampleCount = static_cast<int>(buffer.size());
    const int byteCount = sampleCount * 2;
    std::vector<unsigned char> raw(static_cast<std::size_t>(byteCount));
    int bytesRead = 0;

    const int result = rtlsdr_read_sync(m_device, raw.data(), byteCount, &bytesRead);
    if (result != 0 || bytesRead != byteCount) {
        HFSDR::Logger::warning(QString("RTL-SDR sync read failed: requested %1 bytes, received %2.")
            .arg(byteCount).arg(bytesRead));
        return false;
    }

    for (int i = 0; i < sampleCount; ++i) {
        const std::size_t n = static_cast<std::size_t>(i * 2);
        const float iSample = (static_cast<float>(raw[n]) - 127.5f) / 127.5f;
        const float qSample = (static_cast<float>(raw[n + 1]) - 127.5f) / 127.5f;
        buffer.samples()[static_cast<std::size_t>(i)] = HFSDR::IQBuffer::Sample(iSample, qSample);
    }
    return true;
}

void RTLDevice::querySupportedGains()
{
    m_supportedGainsTenthsDb.clear();
    if (!m_device)
        return;

    const int count = rtlsdr_get_tuner_gains(m_device, nullptr);
    if (count <= 0) {
        HFSDR::Logger::warning("RTL-SDR did not report any manual gain steps.");
        return;
    }

    std::vector<int> gains(static_cast<std::size_t>(count));
    const int returned = rtlsdr_get_tuner_gains(m_device, gains.data());
    if (returned <= 0)
        return;

    for (int i = 0; i < returned; ++i)
        m_supportedGainsTenthsDb.append(gains[static_cast<std::size_t>(i)]);

    std::sort(m_supportedGainsTenthsDb.begin(), m_supportedGainsTenthsDb.end());
}

int RTLDevice::nearestSupportedGainTenthsDb(int requested) const
{
    if (m_supportedGainsTenthsDb.isEmpty())
        return requested;

    int nearest = m_supportedGainsTenthsDb.first();
    int difference = std::abs(nearest - requested);
    for (int candidate : m_supportedGainsTenthsDb) {
        const int candidateDifference = std::abs(candidate - requested);
        if (candidateDifference < difference) {
            nearest = candidate;
            difference = candidateDifference;
        }
    }
    return nearest;
}

void RTLDevice::setStatus(const QString& status)
{
    if (m_status == status)
        return;
    m_status = status;
    emit statusChanged();
}

void RTLDevice::setConnected(bool connected)
{
    if (m_connected == connected)
        return;
    m_connected = connected;
    emit connectedChanged();
}
