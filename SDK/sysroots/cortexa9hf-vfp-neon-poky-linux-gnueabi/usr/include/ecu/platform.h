/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: platform.h
 ***
 ***     Project: Alexandra
 ***
 ***     Copyright (C) Stoneridge Electronics AB 1990 - 2016
 ***
 *******************************************************************************
 *******************************************************************************
 ***
 ***
 ***   Subject:
 ***
 ***   Document reference:
 ***
 ***   Description of this header file:
 ***
 ***  Version:
 ***
 ***
 *******************************************************************************
 */


/*
 **=============================================================================
 ** Header definition
 **=============================================================================
 */
#ifndef PLATFORM_H
#define PLATFORM_H

/*
 **=============================================================================
 ** Imported definitions
 **=============================================================================
 */

/*
 **=============================================================================
 ** Public type definitions
 **=============================================================================
 */
namespace ecu {
namespace lapi {

/**
 * @brief Shall be used to indicate to the platform that an app is starting.
 *
 * @details This control notification to systemd that the application is
 * currently in the startup phase. This feature is crucial when subscribing
 * to early-bird rt data. It shall be called as soon as the application is
 * starting, preferably in the beginning of your main() function.
 *
 * As soon as the basics are setup and rt topics have been subscribed,
 * application_ready() shall be called.
 *
 * @sa ecu::lapi::application_ready()
 *
 */
void application_starting();

/**
 * @brief Shall be used to indicate to the platform that an app is started and ready.
 *
 * @details Notifies the platform - systemd - that the application has been
 * started successfully and therefore is ready! Application subscribing to
 * early bird data shall mark themselves as ready after all rt-topic subscriptions has
 * been performed and acknowledged by the IPCd, by calling ecu::lapi::com::wait_ready().
 * @sa ecu::lapi::com::wait_ready()
 */
void application_ready();

/*
 **=============================================================================
 ** Public constant definitions for external access
 **=============================================================================
 */

/*
 **=============================================================================
 ** Function prototype declarations for external access
 **=============================================================================
 */
}} /* end namespace */

#endif /* PLATFORM_H */
