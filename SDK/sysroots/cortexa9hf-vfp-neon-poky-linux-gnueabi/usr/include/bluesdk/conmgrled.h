/****************************************************************************
 *
 * File:
 *     conmgrled.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains public definitions for the LE Discovery capability
 *     of Connection Manager.
 *
 * Created:
 *     November 23, 2011
 *
 * Copyright 2005-2017 OpenSynergy GmbH.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of OpenSynergy GmbH.
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of OpenSynergy GmbH.
 *
 * Use of this work is governed by a license granted by OpenSynergy GmbH.
 * This work contains confidential and proprietary information of
 * OpenSynergy GmbH. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __CONMGRLED_H_
#define __CONMGRLED_H_

#include "conmgr.h"
#include "mele.h"

#if NUM_LE_DEVICES > 0
/*---------------------------------------------------------------------------
 * Connection Manager LE Discovery API layer
 *
 *     The Connection Manager LE Discovery API provides a set of functions for
 *     managing the LE Discovery (Scanning) process. Profiles make calls
 *     to this API in order to start, stop and set the parameters for LE
 *     scanning. Scanning is also is used for connections. Scanning for
 *     creating a connection takes precedence over scanning for discovery
 *     so when connections are taking place scanning is controlled by the
 *     connection portion of the Connection Manager. This API must be used
 *     instead of calling MELE directly. If both APIs are used by applications
 *     then problems will occur.
 *
 *     See the Blue SDK Connection Manager API layer for core connection
 *     creation and disconnection API.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * CmgrLeAdvertType type
 *
 * Type of LE Advertising reports that should be sent to handlers associated
 * with "discovery" scanning.
 */
typedef U8 CmgrLeAdvertType;

/* Send LE advertising events that have either the General Discovery or
 * Limited discovery AD flags set
 */
#define CMGRLE_ADTYPE_GENERAL 0

/* Send LE advertising events that have only the Limited Discovery AD flags
 * set
 */
#define CMGRLE_ADTYPE_LIMITED 1

/* Send all LE advertising reports events */
#define CMGRLE_ADTYPE_ALL     2

/* End of CmgrLeAdvertType */

/* Forward declaration of CmgrLeScanHandler structure */
typedef struct _CmgrLeScanHandler CmgrLeScanHandler;


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * CmgrLeScanHandler structure
 *
 *     The handler for Connection Manager LE scan (discovery) events.
 */
struct _CmgrLeScanHandler {
    /* Inherit from BtHandlerEx */
    BtHandlerEx     h;

    /* Group: Read-only parameters */
   CmgrLeAdvertType type;
};

/*---------------------------------------------------------------------------
 * CmgrLeScanEvent structure
 *
 *     Used to pass LE discovery information from the CMGR layer to CMGR
 *     client applications.
 */
typedef struct _CmgrLeScanEvent {
    /* Inherit from BtEventEx. Note that "e.handler" contains a pointer
     * to the handler which is the CmgrLeScanHandler
     */
    BtEventEx            e;

    /* Error code valid during callback for events ... */
    BtErrorCode          errCode;

    /* Pointer to device context valid during callback for event CMEVENTEX_LE_REPORT.
     * This is a pointer into the Device Selector layer.
     */
    BtDeviceContext     *bdc;

    /* Pointer to device address valid during callback for event CMEVENTEX_LE_REPORT.
     * This is only needed in the case where bdc is 0 which would only occur if
     * The Device Selector layer is out of resources. This pointer is only valid
     * during the callback.
     */
    BtDevAddr           *devAddr;

    /* Pointer to advertising report from the MELE layer valid during callback for
     * event CMEVENTEX_LE_REPORT. This pointer is only valid during the callback.
     */
    MeLeAdvertReport    *addReport;

} CmgrLeScanEvent;

/*---------------------------------------------------------------------------
 * CmgrLeScanParms structure
 *
 *     Used to hold LE scan (discovery) parameters.
 */
typedef struct _CmgrLeScanParms {

    /* scanType specifies the type of scanning that will be used */
    MeLeScanType           scanType; /* Default is MLST_ACTIVE_SCANNING */

    /* Set 1 is the fast scan window/interval used when nothing else is going
     * such as connections or advertising or when time1 expires.
     *
     * If time1 is set to CMGR_TIME_INIFINTY then the window/interval runs until
     * scanning is canceled or something such as a connection or advertising
     * puts it into the background. Time1 must not be set to 0.
     */
    CmgrLeWinInt interval1;  /* Default setting is CMGR_LE_WININT_30MS */
    CmgrLeWinInt window1;    /* Default setting is CMGR_LE_WININT_30MS */
    TimeT        time1;      /* Default setting is 30000 (30 seconds) */

    /* Set 2 for scan window/interval is used for back ground scanning when
     * other processes such as connections or advertising are going on.
     * By default, we will use this scan set if a connection or advertising are 
     * active when scanning is enabled. If this set is preferred when other 
     * processes are active (e.g. BR/EDR discovery), then this can be setup
     * manually.
     *
     * If time2 is set to CMGR_TIME_INIFINTY then the window/interval runs until
     * scanning is canceled.  Time2 cannot be set to 0. When time2 expires and
     * scanning is stopped all scanning handlers will be notified with the
     *
     */
    CmgrLeWinInt interval2;  /* Default setting is CMGR_LE_WININT_1280MS */
    CmgrLeWinInt window2;    /* Default setting is CMGR_LE_WININT_11PT25MS */
    TimeT        time2;      /* Default setting is 30000 (30 seconds) */

    /* filterPolicy specifies the types of addresses that will produce
     * advertising results. If CMGR_SetLeScanParms() is called this field
     * is ignored and set to an appropriate value to maintain backwards
     * compatibility with old that didn't have this field.
     */
    MeLeScanFilterPolicy filterPolicy;
} CmgrLeScanParms;


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * CMGR_GetLeScanParms()
 *
 *     Get the current LE scan parameters that are set in the Connection
 *     manager.
 *
 * Parameters:
 *     leScanParms - Pointer to a CmgrLeScanParms structure to receive the
 *         LE scan parameters.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation was successfully.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because leScanParms
 *         is an invalid pointer.
 *
 */
BtStatus CMGR_GetLeScanParms(CmgrLeScanParms *leScanParms);

/*---------------------------------------------------------------------------
 * CMGR_SetLeScanParms()
 *
 *     Set the scan parameters to be used for all LE scan requests made via
 *     CMGR_StartLeScan(). Note that the parameters will not affect
 *     ongoing scanning. The new parameters will take effect the next time
 *     Connection Manager needs to check the parameters such as when a timer
 *     expires, when scanning is started/stopped or when CMGR_StartLeScanning()
 *     is called.
 *
 * Parameters:
 *     leScanParms - Pointer to a CmgrLeScanParms structure containing the
 *         parameters to be used. The structure is available when the
 *         function returns.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The parameters were updated successfully.
 *
 *     BT_STATUS_FAILED - The operation failed because the parameters within
 *         the structure are not consistent or are invalid.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because leConParms
 *         is an invalid pointer.
 */
BtStatus CMGR_SetLeScanParms(CmgrLeScanParms *leScanParms);

/*---------------------------------------------------------------------------
 * CMGR_SetLeScanParmsEx()
 *
 *     Set the scan parameters to be used for all LE scan requests made via
 *     CMGR_StartLeScan(). Note that the parameters will not affect
 *     ongoing scanning. The new parameters will take effect the next time
 *     Connection Manager needs to check the parameters such as when a timer
 *     expires, when scanning is started/stopped or when CMGR_StartLeScanning()
 *     is called. This version allows scanning to use the white list. To add
 *     items to the white list call CMGR_AddLeWhiteList().
 *
 * Parameters:
 *     leScanParms - Pointer to a CmgrLeScanParms structure containing the
 *         parameters to be used. The structure is available when the
 *         function returns.
 *
 *     useWhiteList - TRUE means that the filter policy will be set to the
 *         the appropriate policy that uses the white list (either 
 *         MLSFP_ACCEPT_WL_ONLY or MLSFP_ACCEPT_ALL_RPA_AND_WL) based on the
 *         capabilities of the controller. FALSE means that the filter policy
 *         will be set to the appropriate policy that does not use the white
 *         list (either MLSFP_ACCEPT_ALL or MLSFP_ACCEPT_ALL_RPA based on
 *         the capabilities of the controller.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The parameters were updated successfully.
 *
 *     BT_STATUS_FAILED - The operation failed because the parameters within
 *         the structure are not consistent or are invalid.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed because leConParms
 *         is an invalid pointer.
 */
BtStatus CMGR_SetLeScanParmsEx(CmgrLeScanParms *leScanParms, BOOL useWhiteList);

/*---------------------------------------------------------------------------
 * CMGR_StartLeScan()
 *
 *     Associate the handler with LE scanning. If the LE controller is not
 *     currently scanning and a connection request is not in progress then
 *     scanning is started using the parameters currently set in the
 *     Connection Manager. Use CMGR_SetLeScanParms() to set the scanning
 *     parameters.
 *
 *     If the handler is already associated with scanning then the scanning
 *     timer will be restarted. This means that if time 1 has expired,
 *     and there is no other reason to be background scanning then foreground
 *     scanning will resume otherwise timer 2 (background scanning timer) will
 *     be restarted.
 *
 *     Each LE device that is discovered while scanning will cause a
 *     CMEVENTEX_LE_REPORT event to be sent to the handler. The handler will
 *     continue to receive events until CMGR_StopLeScan() is called.
 *
 *     Connection attempts have higher priority than scanning therefore if a
 *     connection is requested scanning will be stopped until the
 *     connection attempt completes. The scanning timer will not run during
 *     this time and will be restarted after the connection attempt is complete.
 *
 * Parameters:
 *     handler - pointer to a handler to receive the CMEVENTEX_LE_REPORT
 *         events.
 *
 *     type - indications the type of advertising reports that should be sent to
  *        the handler.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The handler has successfully been associated with scanning
 *         if it was not already. CMGR is currently scanning and the scanning timer 
 *         has been restarted. Note that BT_STATUS_SUCCESS will be returned if CMGR is
 *         currently scanning as part of creating a connection. Also note that CMGR could
 *         be in the process of stopping scanning so the handler could receive an
 *         CMEVENTEX_LE_SCAN_STOPPED event followed by a CMEVENTEX_LE_SCAN_STARTED event.
 *
 *     BT_STATUS_PENDING - The handler has been successfully associated with scanning
 *         and the scanning procedure has been started or was previous started but the
 *         device is not yet scanning. When the scanning has successfully been started 
 *         the handler will receive the CMEVENTEX_LE_SCAN_STARTED event. If an error 
 *         occurs while attempting to start scanning the handler will receive the 
 *         CMEVENTEX_LE_ERROR event. It is possible that the CMEVENTEX_LE_SCAN_STOPPED
 *         could be received by the handler before the CMEVENTEX_LE_SCAN_STARTED
 *         in situations where scanning was in the process of being stopped to change
 *         parameters. 
 *
 *     BT_STATUS_NOT_READY - The scan parameters are setup to use the white list
 *         (CMGR_SetLeScanParmsEx() called with useWhiteList set to TRUE) but there 
 *         are no items in the white list. To add items to the white list call 
 *         CMGR_AddLeWhiteList().
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_INVALID_PARM - Handler or type is not valid.
 *
 */
BtStatus CMGR_StartLeScan(CmgrLeScanHandler *handler, CmgrLeAdvertType type);

/*---------------------------------------------------------------------------
 * CMGR_StopLeScan()
 *
 *     Disassociates the handler from LE scanning. If there are no handlers
 *     associated with LE scanning then scanning will be stopped.
 *
 * Parameters:
 *     handler - pointer to a handler to disassociate.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The handler was disassociated successfully.
 *
 *     BT_STATUS_NOT_FOUND - handler is not currently associated with scanning.
 *
 *     BT_STATUS_RESTRICTED - The operation failed because a fatal error
 *        has occurred in the LE controller. The application should call
 *        ME_RadioShutdown() to reset the LE controller.
 *
 *     BT_STATUS_INVALID_PARM - Handler is not valid.
 *
 */
BtStatus CMGR_StopLeScan(CmgrLeScanHandler *handler);
#endif /* NUM_LE_DEVICES > 0 */


#endif /* __CONMGRLED_H_ */
