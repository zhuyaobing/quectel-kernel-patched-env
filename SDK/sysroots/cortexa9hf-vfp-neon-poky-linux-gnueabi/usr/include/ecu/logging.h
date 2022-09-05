#ifndef LOGGING_H
#define LOGGING_H

#include <ostream>
#include <streambuf>
#include <string>
#include <map>
#include <thread>
#include <vector>
#include <memory>

#include <syslog.h>

#include <ecu/config.h>

#define LOG_CRITICAL LOG_CRIT
#define LOG_ERROR LOG_ERR


#ifndef DOXYGEN_IGNORE
/**
 *******************************************************************************
 * internal use
 *******************************************************************************
 */
#define _LOG_LEVEL_ENABLED(severity) \
   ecu::lapi::logging::get_log_level() >= LOG_##severity

/**
 *******************************************************************************
 * internal use
 *******************************************************************************
 */
#define _LOG_GET_STREAM(severity) \
  _LOG_LEVEL_ENABLED(severity) \
      ? &ecu::lapi::logging::severity \
      : nullptr

/**
 *******************************************************************************
 * internal use
 *******************************************************************************
 */
#define _LOG_GET_STREAM_MOD(severity, module) \
   (module && (LOG_##severity <= LOG_WARNING || ( _LOG_LEVEL_ENABLED(severity) && module->is_enabled() )) ) \
      ? &ecu::lapi::logging::severity \
      : nullptr
#endif // DOXYGEN_IGNORE

/**
 *******************************************************************************
 *
 * @brief Macro helps insert and flush log message LogStream with given severity
 *
 * @param severity level: EMERG, ALERT, CRITICAL, ERROR, WARNING,
 *                        NOTICE, INFO, DEBUG
 *
 *******************************************************************************
 */
#define LOG(severity) \
  for (auto _log_stream = _LOG_GET_STREAM(severity); \
       _log_stream; \
       _log_stream->do_flush(), _log_stream = nullptr) *_log_stream

/**
 *******************************************************************************
 *
 * @brief Macro helps insert and flush log message LogStream with given
 * severity and a LogModule
 *
 * @param severity level: EMERG, ALERT, CRITICAL, ERROR, WARNING,
 *                        NOTICE, INFO, DEBUG
 *
 * @param module The LogModule to which the log should be put to
 *
 *******************************************************************************
 */
#define LOG_MOD(severity, module) \
  for (auto _log_stream = _LOG_GET_STREAM_MOD(severity, module); \
       _log_stream; \
       _log_stream->do_flush(), _log_stream = nullptr) *_log_stream << module


namespace ecu {
namespace lapi {
/**
 * @brief Contains all logging related functionality
 */
namespace logging {

/**
 * @brief Destinations of where the logs can be redirected to.
 *
 * @details This can be set via set_log_mode().
 */
enum LogMode
{
   SYSLOG,  ///< Log to journal
   STDOUT,  ///< Log to standard out
   STDERR   ///< Log to standard err
};


/**
 *******************************************************************************
 *
 * @brief Sets the log mode to what the user of the LAPI desires.
 *
 * @details default log mode is SYSLOG
 *
 * @param mode the desired LogMode
 *
 *******************************************************************************
 */
void set_log_mode(LogMode mode);

/**
 *******************************************************************************
 *
 * @brief Sets the log mode to what the user of the LAPI desires.
 *
 * @details Available log-modes are (in that order):
 *   SYSLOG
 *   STDOUT
 *   STDERR
 *   default log mode is SYSLOG
 *   The specified log-mode configures where log entries are written
 *
 * @param mode_name The mode name which shall be configured, see @details
 *
 *******************************************************************************
 */
void set_log_mode(const char* mode_name);

/**
 *******************************************************************************
 *
 * @brief Gets the current log mode
 *
 * @details default log mode is SYSLOG
 *
 * @return mode the desired LogMode
 *
 *******************************************************************************
 */
LogMode get_log_mode();

/**
 *******************************************************************************
 *
 * @brief Gets the current logmode as name
 *
 * @details default log mode is SYSLOG, Available log-levels are:
 *   SYSLOG
 *   STDOUT
 *   STDERR
 *
 * @return The default log-mode as c-string
 *
 *******************************************************************************
 */
const char* get_log_mode_name();

/**
 *******************************************************************************
 *
 * @brief Sets the loglevel based on name
 *
 * @details Available log-levels are (in that order):
 *   EMERG
 *   ALERT
 *   CRITICAL
 *   ERROR
 *   WARNING
 *   NOTICE
 *   INFO
 *   DEBUG
 *
 *  The specified log-level configures the minimum log-level, meaning level with
 *  higher priority will also be include.
 *
 * @param level_name The log level name which shall be configured, see @details
 *
 *******************************************************************************
 */
void set_log_level(const char* level_name);

/**
 *******************************************************************************
 *
 * @brief Sets the loglevel based on enum
 *
 * @details Available log-levels are (in that order):
 *   EMERG
 *   ALERT
 *   CRITICAL
 *   ERROR
 *   WARNING
 *   NOTICE
 *   INFO
 *   DEBUG
 *
 *  The specified log-level configures the minimum log-level, meaning level with
 *  higher priority will also be include.
 *
 * @param level The log level which shall be configured, see @details
 *
 *******************************************************************************
 */
void set_log_level(int level);

/**
 *******************************************************************************
 *
 * Gets the set loglevel as int, see syslog.h
 *
 * @return The configured log-level
 *
 *******************************************************************************
 */
int get_log_level();

/**
 *******************************************************************************
 *
 * Gets the set loglevel as name, see syslog.h
 *
 * @return The set log-level
 *
 *******************************************************************************
 */
const char* get_log_level_name();

/**
 *******************************************************************************
 *
 * @brief Configure logging based on the provided parameter and the ENVIRONMENT
 * variable "LOG_LEVEL".
 *
 * @details The parameter enforce_level will override the environment variable
 * setting with the provided value.
 *
 *******************************************************************************
 */
void configure_logging(const std::string& enforce_level = "");


/**
 *******************************************************************************
 *
 * Returns the name of the default log level, set in LAPI.
 *
 * @return The default log-level as c-string
 *
 *******************************************************************************
 */
const char* get_default_log_level_name();

/**
 *******************************************************************************
 *
 * @brief Returns the name of the default log mode, set in LAPI.
 *
 * @return The default log-level as c-string
 *
 *******************************************************************************
 */
const char* get_default_log_mode_name();

//------------------------------------------------------------------------------

#ifndef DOXYGEN_IGNORE
namespace _internal{
   class LogStream;
   class moduleWrap;
   class LogStreambuf;
   void reset_modules();
}
#endif

/**
 *******************************************************************************
 *
 * @brief LogModule class for improved logging functionality
 *
 * @details The LogModule class provides the possibility to implement a hierarchy
 * structure to the logging based on a rootModule and levels of subModules.
 *
 * By using this LogModule class the control over the logging output streams
 * increase and also more visualization to the printouts is obtained.
 *
 * Example usage of 'existing_module' in logging stream:
 * @code
 * LOG_MOD(DEBUG, existing_module) << "DEBUG logging msg";
 * @endcode
 * Note: The logmodule 'existing_module' has to be created and enabled in
 * the example above to get expected debug print.
 *
 *
 *******************************************************************************
 */

class LogModule
{
public:
      /**
       *******************************************************************************
       *
       * Convenience typedef of shared pointer of this class
       *
       *******************************************************************************
       */
      using ptr_t = std::shared_ptr<LogModule>;

      /**
      *******************************************************************************
      *
      *@brief Gets the module state
      *
      *@return True if module is enabled, false otherwise
      *
      *******************************************************************************
      */
      bool is_enabled() const { return m_enabled; }

      /**
       *******************************************************************************
       *
       *@brief Enables the module itself
       *
       * @details If a module is enabled and recursive is true, all the submodules
       * of that module will also be enabled
       *
       * @param recursive Flag to enable submodules, if true than all submodules
       * of this module will be enabled
       *
       *******************************************************************************
       */
      void enable(bool recursive = true) { change(true, recursive); }

      /**
       *******************************************************************************
       *
       *@brief Disables the module itself
       *
       * @details If recursive is true, all the submodules of that module will
       * also be disabled
       *
       * @param recursive Flag to disable submodules, if true than all submodules
       * of this module will be disabled
       *
       *******************************************************************************
       */
      void disable(bool recursive = true) { change(false, recursive); }

      /**
       *******************************************************************************
       *
       *@brief Function to get module name
       *
       *@return Returns the module name including any existing parent modules
       *
       *******************************************************************************
       */
      std::string name() const;

      LogModule(const LogModule&) = delete;
      LogModule& operator = (const LogModule&) = delete;

#ifndef DOXYGEN_IGNORE
private:
      LogModule(std::string name, LogModule::ptr_t parent) :
         m_enabled(false),
         m_name(name),
         m_parent(parent)
      { }
      using SubModuleMap = std::map<std::string, LogModule::ptr_t>;

      void change(bool value, bool recursive);

      bool m_enabled;
      std::string m_name;
      SubModuleMap m_subModules;
      ptr_t m_parent;

      friend class _internal::LogStream;
      friend class _internal::LogStreambuf;
      friend class _internal::moduleWrap;
      friend void _internal::reset_modules();
#endif
};

//------------------------------------------------------------------------------
#ifndef DOXYGEN_IGNORE
namespace _internal {
   typedef int LogLevel;

   class LogStreambuf: public std::basic_streambuf<char>
   {
   protected:
      LogStreambuf(LogLevel level): std::basic_streambuf<char>(), m_level(level)
      { }
      int_type overflow(int_type c = traits_type::eof()) override;
      std::streamsize xsputn(const char_type* __s, std::streamsize __n) override;
      int sync() override;

      friend LogStream & operator<<(LogStream & out,
                   const LogModule::ptr_t& mod);

      template<typename T>
      friend LogStream& operator<<(LogStream &out, const T& v);


      friend class LogStream;
      void set_level(LogLevel new_level) noexcept { m_level = new_level; }

      static LogModule::ptr_t get_active_module();

   private:
      LogLevel m_level;
      static thread_local std::string m_buffer;
      static thread_local LogModule::ptr_t m_activeModule;
   };

   class LogStream: public std::basic_ostream<char>
   {
   public:
      explicit LogStream(LogLevel level):
         std::basic_ostream<char>(&streambuf),
         streambuf(level)
      { }

      void do_flush();

     friend LogStream& operator<<(LogStream& out,const LogModule::ptr_t& mod);

     template<typename T>
     friend LogStream& operator<<(LogStream &out, const T& v)
     {
        if (out.streambuf.m_level <= LOG_WARNING
              || ( out.streambuf.m_level <= get_log_level()
                 && LogStreambuf::get_active_module()->is_enabled()))
        {
           static_cast<std::basic_ostream<char>&>(out) << v;
           return out;
        }
        return out;
     }

   private:
      LogStreambuf streambuf;
   };
}
#endif

//------------------------------------------------------------------------------

/**
 *******************************************************************************
 *
 * @brief C++ like logging stream for EMERG channel
 *
 *******************************************************************************
 */
extern _internal::LogStream EMERG;

/**
 *******************************************************************************
 *
 * @brief C++ like logging stream for ALERT channel
 *
 *******************************************************************************
 */
extern _internal::LogStream ALERT;

/**
 *******************************************************************************
 *
 * @brief C++ like logging stream for CRITICAL channel
 *
 *******************************************************************************
 */
extern _internal::LogStream CRITICAL;

/**
 *******************************************************************************
 *
 * @brief C++ like logging stream for ERROR channel
 *
 *******************************************************************************
 */
extern _internal::LogStream ERROR;

/**
 *******************************************************************************
 *
 * @brief C++ like logging stream for WARNING channel
 *
 *******************************************************************************
 */
extern _internal::LogStream WARNING;

/**
 *******************************************************************************
 *
 * @brief C++ like logging stream for NOTICE channel
 *
 *******************************************************************************
 */
extern _internal::LogStream NOTICE;

/**
 *******************************************************************************
 *
 * @brief C++ like logging stream for INFO channel
 *
 *******************************************************************************
 */
extern _internal::LogStream INFO;

/**
 *******************************************************************************
 *
 * @brief C++ like logging stream for DEBUG channel
 *
 *******************************************************************************
 */
extern _internal::LogStream DEBUG;

//------------------------------------------------------------------------------

/**
 *******************************************************************************
 *
 * @brief C-style logging function, for EMERG channel
 *
 * @details Use like printf(fmt, va)
 *
 *******************************************************************************
 */
void emergency(const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @details Use like printf(module,fmt, va)
 *
 * @see emergency
 *
 *******************************************************************************
 */
void emergency(const LogModule::ptr_t& module, const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @brief C-style logging function, for ALERT channel
 *
 * @details Use like printf(fmt, va)
 *
 *******************************************************************************
 */
void alert(const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @details Use like printf(module,fmt, va)
 *
 * @see alert
 *
 *******************************************************************************
 */
void alert(const LogModule::ptr_t& module, const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @brief C-style logging function, for CRITICAL channel
 *
 * @details Use like printf(fmt, va)
 *
 *******************************************************************************
 */
void critical(const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @details Use like printf(module,fmt, va)
 *
 * @see critical
 *
 *******************************************************************************
 */
void critical(const LogModule::ptr_t& module, const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @brief C-style logging function, for ERROR channel
 *
 * @details Use like printf(fmt, va)
 *
 *******************************************************************************
 */
void error(const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @details Use like printf(module,fmt, va)
 *
 * @see error
 *
 *******************************************************************************
 */
void error(const LogModule::ptr_t& module, const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @brief C-style logging function, for WARNING channel
 *
 * @details Use like printf(fmt, va)
 *
 *******************************************************************************
 */
void warning(const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @details Use like printf(module,fmt, va)
 *
 * @see warning
 *
 *******************************************************************************
 */
void warning(const LogModule::ptr_t& module, const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @brief C-style logging function, for NOTICE channel
 *
 * @details Use like printf(fmt, va)
 *
 *******************************************************************************
 */
void notice(const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @details Use like printf(module,fmt, va)
 *
 * @see notice
 *
 *******************************************************************************
 */
void notice(const LogModule::ptr_t& module, const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @brief C-style logging function, for INFO channel
 *
 * @details Use like printf(fmt, va)
 *
 *******************************************************************************
 */
void info(const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @details Use like printf(module,fmt, va)
 *
 * @see info
 *
 *******************************************************************************
 */
void info(const LogModule::ptr_t& module, const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @brief C-style logging function, for DEBUG channel
 *
 * @details Use like printf(fmt, va)
 *
 *******************************************************************************
 */
void debug(const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @details Use like printf(module,fmt, va)
 *
 * @see debug
 *
 *******************************************************************************
 */
void debug(const LogModule::ptr_t& module, const char *fmt, ...);

/**
 *******************************************************************************
 *
 * @brief Creates new logging modules
 *
 * @details Used to create new logging modules as subModules to rootModule.
 * All created modules is disable as default. The input
 * string '("moduleOne.moduleTwo")' will create the subModule "moduleOne" as
 * a subModule to "rootModule" and "moduleTwo" as a subModule of "moduleOne"
 *
 * Modules are only created if they do not already exist
 *
 *
 * @param modStr The name of logging modules which shall be created.
 *
 * @return Pointer to the last created subModule that can be used in the
 * logging streams
 *
 *******************************************************************************
 */
LogModule::ptr_t module(const std::string& modStr);

/**
 *******************************************************************************
 *
 * @param modStr Vector containing the names of logging modules which shall
 * be created
 *
 * @return Pointer to the last created subModule that can be used in the
 * logging streams
 *
 * @see module
 *
 *******************************************************************************
 */
LogModule::ptr_t module(const std::vector<std::string>& modStr);

/**
 *******************************************************************************
 *
 * @brief Enables selected logging module by name of the object.
 *
 * @details If a module is enabled and recursive is true, all the submodules
 * of that module will also be enabled
 *
 * @param moduleName The name of the module which shall be enabled
 * @param recursive Flag to enable submodules, if true than all submodules
 * of this module will be enabled
 *
 * @return True if success, false if module not found
 *
 *******************************************************************************
 */
bool enable(const std::string& moduleName, bool recursive = true);

/**
 *******************************************************************************
 *
 * @brief Enables selected logging module by ref to the object.
 *
 * @details If a module is enabled and recursive is true, all the submodules
 * of that module will also be enabled
 *
 * @param moduleName Pointer to existing logModule
 * @param recursive Flag to enable submodules, if true than all submodules
 * of this module will be enabled
 *
 * @return True if success, false if module not found
 *
 * @see enable
 *
 *******************************************************************************
 */
bool enable(LogModule::ptr_t& moduleName, bool recursive = true);

/**
 *******************************************************************************
 *
 * @brief Enables all modules.
 *
 *******************************************************************************
 */
void enable_all();

/**
 *******************************************************************************
 *
 * @brief Disables selected logging module by name, if recursive is true
 * submodules will be disabled as well
 *
 * @param moduleName The name of the module which shall be disabled
 * @param recursive Flag to disable submodules, if true than all submodules
 * of this module will be disabled
 *
 * @return True if success, false if module not found
 *
 *******************************************************************************
 */
bool disable(const std::string& moduleName, bool recursive = true);

/**
 *******************************************************************************
 *
 * @brief Disables selected logging module by ref, if recursive is true
 * submodules will be disabled as well
 *
 * @param module Pointer to existing logModule
 * @param recursive Flag to disable submodules, if true than all submodules
 * of this module will be disabled
 *
 * @return True if success, false if module not found
 *
 * @see disable
 *
 *******************************************************************************
 */
bool disable(LogModule::ptr_t& module, bool recursive = true);

/**
 *******************************************************************************
 *
 * @brief Disables all modules
 *
 *******************************************************************************
 */
void disable_all();

/**
 *******************************************************************************
 *
 * @brief Configure logger from a ecu::lapi::config::Configuration_ptr
 *
 * @details Method reads next keys from configuration pointer:
 *  * "logging.mode": A string representation of a logger sink mode, available
 *              option: "SYSLOG", "STDOUT", "STDERR"
 *  * "logging.level": A string representation of a logger severity, available
 *              option: "EMERG", "ALERT", "CRITICAL", "ERROR", "WARNING",
 *              "NOTICE", "INFO", "DEBUG"
 *  * "logging.modules.enable": Space separated list of modules to enable,
 *              e.g: module1 module2.submodule1 module3
 *  * "logging.modules.disable": Space separated list of modules to disable,
 *              e.g: root hello.world
 *
 * @param config Configuration pointer to initialize the logger internal state
 *
 *******************************************************************************
 */
void set_from_config(const config::Configuration_ptr &config);

}}} // namespace

#endif /* LOGGING_H */
