#pragma once

#include <spdlog/sinks/base_sink.h>
#include <spdlog/spdlog.h>

#include <vector>
#include <mutex>
#include <string>

/**
 * @struct LogEntry
 *
 * @brief Represents a single log message captured from spdlog
 *
 * Stores the formatted message string and its severity level,
 * allowing the console panel to filter and color-code entries by level.
 */
struct LogEntry
{
    std::string                 message;
    spdlog::level::level_enum   level;
};

/**
 * @class ConsoleSink
 *
 * @brief Custom spdlog sink that captures log messages for display in the console panel
 *
 * Inherits from spdlog::sinks::base_sink and stores all incoming log messages
 * as LogEntry objects in an internal vector. The UIManager registers an instance
 * of this sink with the default spdlog logger during initialization, so all
 * engine log calls (SPDLOG_INFO, SPDLOG_WARN, etc.) are automatically captured
 * and made available for rendering in the console panel.
 *
 * @tparam Mutex Mutex type used for thread safety
 */
template<typename Mutex>
class ConsoleSink : public spdlog::sinks::base_sink<Mutex>
{
public:

    /**
     * @brief Returns all captured log entries
     * 
     * @return Const reference to the internal vector of LogEntry objects
     */
    const std::vector<LogEntry>& get_entries() const { return m_entries; }

    /**
     * @brief Clears all captured log entries
     *
     * Called when the user presses the Clear button in the console panel
     */
    void clear() { m_entries.clear(); }

protected:

    /**
     * @brief Formats and stores an incoming spdlog message
     *
     * Called automatically by spdlog when a message is logged. 
     * Formats the message using the sink's formatter and appends it to the entries vector.
     *
     * @param msg The incoming log message from spdlog
     */
    void sink_it_(const spdlog::details::log_msg& msg) override
    {
        spdlog::memory_buf_t formatted;
        this->formatter_->format(msg, formatted);
        m_entries.push_back({ fmt::to_string(formatted), msg.level });
    }

    /**
     * @brief Flush implementation — no-op as entries are stored in memory
     */
    void flush_() override {}

private:

    // Internal storage of all captured log entries
    std::vector<LogEntry> m_entries;
};

// Thread-safe ConsoleSink specialization using std::mutex
using ConsoleSinkMt = ConsoleSink<std::mutex>;