/*
 ***************************************************************************************
 ***************************************************************************************
 ***
 ***    File: systemd.h
 ***
 ***    System: IPC lib
 ***
 ***    Project: Alexandra
 ***
 ***    Copyright (C) Stoneridge Electronics AB, 1990 - 2016
 ***
 ***    The information in this document belongs to Stoneridge, Inc. or its
 ***    subsidiaries, and may be confidential. This document and information
 ***    in it may not be copied, reproduced, revealed or appropriated, either
 ***    in whole or in part, without the prior written consent of Stoneridge, Inc.
 ***
 ***************************************************************************************
 ***************************************************************************************
 */


/*
 **=====================================================================================
 ** Header definition
 **=====================================================================================
 */

#ifndef SPIDER_SYSTEMD_H
#define SPIDER_SYSTEMD_H


/*
 **=====================================================================================
 ** Imported definitions
 **=====================================================================================
 */
extern "C"
{
#include <systemd/sd-bus.h>
}

#include <memory>
#include <string>
#include <vector>


/*
 **=====================================================================================
 ** Public type definitions
 **=====================================================================================
 */

namespace ecu {
namespace lapi {
namespace ipc {


/**
 *  @brief Wrapper class around systemd's dbus interface.
 *
 */
class Systemd
{
public:
    /**
     * @brief Convenient pointer to class.
     */
    typedef std::unique_ptr<Systemd> ptr_t;

    // DBUS types
    typedef std::string DbusService_t;
    typedef std::string DbusInterface_t;
    typedef std::string DbusObjectInstance_t;

    // service(s)
    const std::string SYSTEMD_SERVICE           = "org.freedesktop.systemd1";

    // interfaces
    const std::string SYSTEMD_MANAGER_INTERFACE = "org.freedesktop.systemd1.Manager";
    const std::string SYSTEMD_UNIT_INTERFACE    = "org.freedesktop.systemd1.Unit";
    const std::string SYSTEMD_SERVICE_INTERFACE  = "org.freedesktop.systemd1.Service";
    const std::string SYSTEMD_TARGET_INTERFACE  = "org.freedesktop.systemd1.Target";

    // object instance(s)
    const std::string SYSTEMD_ROOT_OBJECT       = "/org/freedesktop/systemd1";

    /**
     ****************************************************************************
     **
     ** @brief Construct the instance.
     **
     ****************************************************************************
     */
    Systemd();

    virtual ~Systemd();

    /**
     ****************************************************************************
     **
     **  @brief Start the systemd target @param in "isolate" mode
     **
     **  @param[in] target The target that should be started needs to be
     **                    able to be isolated. If no ".target" suffix is provided, it
     **                    will be added automatically.
     **
     **  @return True if the target could be started, otherwise false
     **
     ***************************************************************************
     */
    virtual bool
    start_isolate_target( const std::string& target );


    /**
     * @brief DEPRECATED because of misleading name.
     *
     * @details Use @isolate_target instead. This method will be removed.
     */
    virtual inline bool
    change_target( const std::string& target ) { return start_isolate_target( target ); }


    /**
     * @brief Stops all services that belong to a certain target
     *
     * @param[in] target The target to query. Suffix ".target" will be appended
     *                    if missing.
     */
    virtual void
    stop_target_services( const std::string& target );


    /**
     * @brief Starts all services that belong to a certain target
     *
     * @param[in] target The target to query. Suffix ".target" will be appended
     *                   if missing.
     */
    virtual void
    start_target_services( const std::string& target );


    /**
     ****************************************************************************
     **
     **  @brief Tells whether a certain systemd target is active
     **
     **  @param[in] target The target to query. Suffix ".target" will be appended
     **                    if missing.
     **
     **  @sa get_unit_activestate if you don't want '.target' to be appended
     **  @return True if the target is active, otherwise false
     **
     ***************************************************************************
     */
    virtual bool
    is_target_active( const std::string& target );


    /**
     * @brief Lists a unit's all dependencies.
     *
     * @param[in] unitname   The unit to query. Must end with proper suffix (".unit",
     *                       ".target", ...)
     *
     * @param[in] dependency The dependency to query for. Some examples are "Wants",
     *                       "Requires", "After", "Before".
     *
     * @return The list of units.
     */
    virtual std::vector<std::string>
    list_unit_dependencies( const std::string& unitname, const std::string& dependency );


    /**
     * @brief Lists a target's all services.
     *
     * @param[in] target The target to query. Suffix ".target" will be appended
     *                   if missing.
     *
     * @return The list of services.
     */
    virtual std::vector<std::string>
    list_target_services( const std::string& target );


    /**
     * @brief Tells whether a certain systemd target's all services are stopped.
     *
     * @param[in] target The target to query. Suffix ".target" will be appended
     *                   if missing.
     *
     * @return True if the target's services are all stopped, otherwise false
     */
    virtual bool
    are_target_services_stopped( const std::string& target );


    enum ActiveState {
       ACTIVE_STATE_ERROR = 0,
       ACTIVE_STATE_ACTIVE,
       ACTIVE_STATE_INACTIVE,
       ACTIVE_STATE_ACTIVATING,
       ACTIVE_STATE_DEACTIVATING,
       ACTIVE_STATE_FAILED,
       ACTIVE_STATE_RELOADING
    };

    /**
     * @brief Get unit active state
     *
     * @details ACTIVE_STATE_ERROR indicates request/dbus errors
     *
     * @param unit Unit identifier string
     *
     * @return ActiveState representing current state
     */
    virtual ActiveState
    get_unit_activestate(const std::string& unit);

    enum InstallState{
        INSTALL_STATE_ERROR = 0,
        INSTALL_STATE_ENABLED,
        INSTALL_STATE_ENABLED_RUNTIME,
        INSTALL_STATE_LINKED,
        INSTALL_STATE_LINKED_RUNTIME,
        INSTALL_STATE_MASKED,
        INSTALL_STATE_MASKED_RUNTIME,
        INSTALL_STATE_STATIC,
        INSTALL_STATE_DISABLED
    };

    /**
     * @brief Get unit installation state
     *
     * @details Provides the low-level state from the Systemd
     * process manager, raw but with no loss of information.
     * Ideally, apps should be either ENABLED or DISABLED, but in theory
     * all of the above states are possible outcomes of the state check,
     * so logic should be prepared to handle them to some extent.
     * The possible return values should be interpreted as follows.
     *
     *  -- ERROR STATES --
     * INSTALL_STATE_ERROR indicates request/dbus errors.
     * (Note that the "app not found" case is detected and will be reported
     * as INSTALL_STATE_DISABLED, not as an error)
     *
     *  -- INSTALLED --
     * Apps can be considered definitely installed if they are in
     * either of the states ENABLED, LINKED.
     *
     *  -- MAYBE INSTALLED --
     * ENABLED_RUNTIME and LINKED_RUNTIME means the app normally isn't
     * enabled/linked but has temporarily been put in this state for
     * this boot. It's implementation-specific whether you should
     * consider an app in this state to be installed or not.
     *
     *  -- NOT INSTALLED --
     * Apps in states MASKED, DISABLED can be considered not
     * installed; even though their metadata files are found, systemd
     * will never start them, and the actual app binaries may or may not
     * be present at the system.
     * Apps in state MASKED_RUNTIME have been disabled for this boot
     * only, and may normally be installed. However, they will
     * definitely not be available for the current boot.
     *
     *
     * @param unit Unit identifier string
     *
     * @return InstallState representing current state
     */
    virtual InstallState
    get_unit_installstate(const std::string& unit);


    /**
     * @brief Start a systemd unit
     *
     * @details Sends a request to Systemd to start a unit,
     * Note that the job is queued at Systemd and that this method
     * returns true before the unit has actually been started.
     * Check @sa get_unit_activestate for progress.
     *
     * @param unit Unit identifier
     *
     * @return true if command was successfully sent, false otherwise
     */
    virtual bool
    start_unit( const std::string& unit );

    /**
     * @brief Stop a systemd unit
     *
     * @details Sends a request to Systemd to stop a unit,
     * Note that the job is queued at Systemd and that this method
     * returns true before the unit has actually been stopped.
     * Check @sa get_unit_activestate for progress.
     *
     * @param unit Unit identifier
     *
     * @return true if command was successfully sent, false otherwise
     */
    virtual bool
    stop_unit( const std::string& unit );

    /**
     * @brief Restart a systemd unit
     *
     * @details Sends a request to Systemd to restart a unit,
     * Note that the job is queued at Systemd and that this method
     * returns true before the unit has actually been restarted.
     * Check @sa get_unit_activestate for progress.
     *
     * @param unit Unit identifier
     *
     * @return true if command was successfully sent, false otherwise
     */
    virtual bool
    restart_unit( const std::string& unit );

    /**
     * @brief Reload a systemd unit
     *
     * @details Sends a request to Systemd to reload a unit,
     * Note that the job is queued at Systemd and that this method
     * returns true before the unit has actually been reloaded.
     * Check @sa get_unit_activestate for progress.
     *
     * @param unit Unit identifier
     *
     * @return true if command was successfully sent, false otherwise
     */
    virtual bool
    reload_unit( const std::string& unit );

    /**
     * @brief Get the string representation of the provided ActiveState
     *
     * @param state The ActiveState to get the string representation for
     *
     * @return The string representation
     */
    static const std::string& to_str(ActiveState state);

    /**
     * @brief Get the string representation of the provided InstallState
     *
     * @param state The InstallState to get the string representation for
     *
     * @return The string representation
     */
    static const std::string& to_str(InstallState state);
protected:

    /**
     ****************************************************************************
     **
     **  @brief Retrieves a systemd object instance
     **
     **  @param[in] member_name Name of object to get
     **
     **  @return The systemd object instance identifier
     **
     ***************************************************************************
     */
    std::string
    get_object_instance(
            const std::string& service,
            const std::string& interface,
            const std::string& object,
            const std::string& member_name );

    /**
     ****************************************************************************
     **
     **  @brief Retrieves a property value
     **
     **  @param[in]
     **  @return The property value
     **
     ***************************************************************************
     */
    std::string
    get_property_value(
            const std::string& service,
            const std::string& interface,
            const std::string& object,
            const std::string& property_name );

    /**
     ****************************************************************************
     **
     **  @brief Retrieves a property array value
     **
     **  @param[in]
     **  @return The property values
     **
     ***************************************************************************
     */
    std::vector<std::string>
    get_property_value_array(
          const std::string& service,
          const std::string& interface,
          const std::string& object,
          const std::string& property_name);


    enum UnitAction {
       UNIT_ACTION_STOP = 0,
       UNIT_ACTION_START,
       UNIT_ACTION_RESTART,
       UNIT_ACTION_RELOAD
    };


    /**
     * Start, stop, restart a unit
     *
     * @param unit Systemd unit name for the action
     * @param action UnitAction to perform
     * @oaram mode Systemd Start-/Stop-/Restart-Unit mode, default is "replace"
     */
    bool
    start_stop_unit( const std::string& unit, const UnitAction action, const std::string& mode = "replace");

};

/*
 **=====================================================================================
 ** Public constant definitions for external access
 **=====================================================================================
 */

// convenience type def
typedef Systemd::ptr_t Systemd_ptr;

}}} // namespace

#endif /* SPIDER_SYSTEMD_H */
