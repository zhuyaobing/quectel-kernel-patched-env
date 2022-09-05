/****************************************************************************
 *
 * File:
 *     did.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the Bluetooth
 *     Device ID Profile program interface.
 *
 * Copyright 2009-2017 OpenSynergy GmbH.
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

#ifndef __DID_H
#define __DID_H

#include "utils.h"
#include "sdp.h"

/*---------------------------------------------------------------------------
 * Device ID Profile (DID) Layer
 *
 *     The DID layer provides functions for registering and deregistering
 *     device specific Device ID SDP entries as well as a mechanism for querying
 *     the remote device for it's device ID profile information.
 *
 *     Both the mandatory and optional Device ID v1.3 attributes are supported 
 *     (Spec version, Vendor ID, Product ID, Product version, Vendor ID source, 
 *     Client Executable URL, Service Description, and Documentation URL). 
 *     For the Client side functionality, the optional and mandatory attributes 
 *     are supported. On the server side, the service description can be enabled 
 *     using the DEVID_SDP_REGISTER_SERVICE_DESCRIPTION macro, the client 
 *     executable URL can be enabled using the DEVID_SDP_REGISTER_CLIENT_EXEC_URL 
 *     macro, and the documentation URL can be enabled using the 
 *     DEVID_SDP_REGISTER_DOCUMENTATION_URL macro. When enabled, these attributes 
 *     will be included in the SDP record, otherwise they will not be included. 
 *     All of these values can be customized in the DevIdInfo structure provided
 *     when the SDP record(s) are registered. 
 *  
 *     This implementation does not support adding the Device ID attributes to the EIR
 *     response string.
 */

/****************************************************************************
 *
 * Section: Default general constants that can be overridden in overide.h
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * DEVID_SDP_REGISTER_SERVICE_DESCRIPTION Constant
 *
 *  When enabled, the service description will be included in the SDP record,
 *  otherwise it will not be included.
 */ 
#define DEVID_SDP_REGISTER_SERVICE_DESCRIPTION  XA_ENABLED

/*---------------------------------------------------------------------------
 * DEVID_SDP_REGISTER_DOCUMENTATION_URL Constant
 *
 *  When enabled, the documentation URL will be included in the SDP record,
 *  otherwise it will not be included.
 */ 
#define DEVID_SDP_REGISTER_DOCUMENTATION_URL  XA_ENABLED

/*---------------------------------------------------------------------------
 * DEVID_SDP_REGISTER_CLIENT_EXEC_URL Constant
 *
 *  When enabled, the client executable URL will be included in the SDP record,
 *  otherwise it will not be included.
 */ 
#define DEVID_SDP_REGISTER_CLIENT_EXEC_URL  XA_ENABLED

/*---------------------------------------------------------------------------
 * DEVID_MAX_LOCAL_DEVICE_RECORDS Constant
 *
 *  Defines the maximum number of local device records supported
 *
 *  The default value is 1. Set to 0 to disable server support.
 */
#ifndef DEVID_MAX_LOCAL_DEVICE_RECORDS
#define DEVID_MAX_LOCAL_DEVICE_RECORDS            1
#endif

/*---------------------------------------------------------------------------
 * DEVID_MAX_REMOTE_DEVICE_RECORDS Constant
 *
 *  Defines the maximum number of remote device records that will be support.
 *
 *  The default value is 1. Set to 0 to disable server support.
 */
#ifndef DEVID_MAX_REMOTE_DEVICE_RECORDS
#define DEVID_MAX_REMOTE_DEVICE_RECORDS            1
#endif

/*---------------------------------------------------------------------------
 * DEVID_MAX_SERV_DESC_LEN Constant
 *
 *  Defines the maximum number of bytes allocated for a service description.
 *
 *  The default value is 128 bytes.
 */
#ifndef DEVID_MAX_SERV_DESC_LEN
#define DEVID_MAX_SERV_DESC_LEN                   128
#endif

/*---------------------------------------------------------------------------
 * DEVID_MAX_URL_LEN Constant
 *
 *  Defines the maximum number of bytes allocated for the documentation or
 *  client executable URL.
 *
 *  The default value is 128 bytes.
 */
#ifndef DEVID_MAX_URL_LEN
#define DEVID_MAX_URL_LEN                         128
#endif

/****************************************************************************
 *
 * Section: Types
 *
 ***************************************************************************/


/*---------------------------------------------------------------------------
 * DidVendorIdSource type
 *
 * Identifies the source of a vendor ID.
 */
typedef U16 VendorIdSource;

/* Indicates a vendor ID issued by the Bluetooth SIG. */
#define DID_VID_SOURCE_BLUETOOTH    0x0001

/* Indicates a vendor ID issued by the USB Implementer's Forum */
#define DID_VID_SOURCE_USB          0x0002

/* End of DidVendorIdSource */


/*---------------------------------------------------------------------------
 * DevIdEventType type
 *
 *     Indicates the type of a DevId callback's event during an
 *     DevidCallback function call. These events are delivered in the
 *     DevIdbackParms structure's "event" field.
 */
typedef U8 DevIdEventType;

/** The results of the DID_StartQuery() request. 
 */
#define DIDEVENT_QUERY_RESULTS        1

/* End of DevIdEventType */


/****************************************************************************
 *
 * Section: Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * DevIdInfo structure
 *
 * Used to pass information when registering a local device id record or to
 * pass information about a remote device id record found during an SDP query.
 *
 */
typedef struct _DevIdInfo {

    ListEntry       node;     /* For internal use only. */

    /* Device ID Specification Version */
    U16                         specVer;

    /* Source used to assign the vendor ID. */
    VendorIdSource              vendorIdSrc;

    /* Vendor ID of this device's manufacturer, as assigned by the
     * entity specified in device.vendorIdSrc.
     */
    U16                         vendorId;

    /* Product ID for the device (assigned by vendor) */
    U16                         productId;

    /* Version code for the device. This value is vendor-supplied in 0xJJMN
     * format where JJ is the major version number, M is the minor version
     * number, and N is the sub-minor version number. The version number
     * increases with subsequent versions of the device specified in
     * device.productId.
     */
    U16                         version;

    /* This flag designates the record as the primary Device ID record for the device
     * when set to TRUE when multiple records are present. This field shall
     * be set to TRUE where a single Device ID record exists in the device.
     * If multiple Device ID records exist one of the records MAY be idendified
     * as the primary record, but all records may set this to FALSE.
     * */
    BOOL                        primaryRecord;

    /* The language code field represents the natural language. This field shall be set
     * with the code according to ISO 639:1988 (E/F): "Code for the representation of names
     * of languages.
     */
    U16                         languageCode;

    /* The encodingCode is an identifier that specifies a character encoding used for the
     * language. Values for character encoding can be found in IANA's database.
     */
    U16                         encodingCode;

    /* The service description is a string containing a brief description of the
     * service. This value is not null terminated.
     */
    U8                          serviceDesc[DEVID_MAX_SERV_DESC_LEN];

    /* The length of the string in the serviceDesc field. */
    U8                          serviceDescLen;

    /* The documentation URL is a path to documentation which can be instructions
     * on how to install drivers or an explanation of different drivers available.
     */
    U8                          documentationUrl[DEVID_MAX_URL_LEN];

    /* The length of the string in the documentationUrl field. */
    U8                          documentationUrlLen;

    /* The client executable URL is a method to distribute device executables. It points
     * to an implentation specific path which contains the executables to be run.
     */
    U8                          clientExecUrl[DEVID_MAX_URL_LEN];

    /* The length of the string in the clientExecUrl field. */
    U8                          clientExecUrlLen;

} DevIdInfo;

/*---------------------------------------------------------------------------
 * DevIdRegisterParms structure
 *
 * Contains parameters used by DID registration functions. The entire
 * structure should be initialized to zero, and parameters filled in as
 * required.
 */
typedef struct _DevIdRegisterParms {

    /* Information about the device being registered */
    DevIdInfo       devIdInfo[DEVID_MAX_LOCAL_DEVICE_RECORDS];

    /* The number of device records to add */
    U8              numRecords;

} DevIdRegisterParms;

/*---------------------------------------------------------------------------
 * DevIdCallbackParms structure
 *
 * Contains parameters by the DevId query function.
 *
 */
typedef struct _DevIdCallbackParms {


    /* DID upper layer event. Always valid. */
    DevIdEventType event;

    /* Status - In the case of a parsing failure, partially parsed service
     * information will be made available in last record instance.
     */
    BtStatus        status;

    /* The number of device records found (last record may be partially 
     * parsed if the status indicates a failure).
     */
    U8              numRecords;

    /* List of DevIdInfo's */
    ListEntry       recordList;

} DevIdCallbackParms;

/*---------------------------------------------------------------------------
 * DevIdCallback type
 *
 *     A function of this type is called to indicate DevId Callback Events.
 */
typedef void (*DevIdCallback)(BtRemoteDevice *remDev, DevIdCallbackParms *cbParms);

/* End of DevIdCallback */

/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * DID_Init()
 *
 *      Initialize the DID component.  This must be the first DID
 *      function called by the application layer.
 *
 * Returns:
 *     TRUE - Initialization was successful.
 *
 *     FALSE - Initialization failed.
 */
BOOL DID_Init(void);

/*---------------------------------------------------------------------------
 * DID_Deinit()
 *
 *     Deinitializes the DID module. DID_Init() must later be called
 *     again to use any DID function call.  This function does no
 *     error checking so do NOT call when DID is in use.
 *
 * Requires:
 *     L2CAP_DEREGISTER_FUNC set to XA_ENABLED
 */
void DID_Deinit(void);

#if SDP_SERVER_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * DID_RegisterRecords()
 *
 *     Registers the Device ID Profile SDP records.
 *
 * Parameters:
 *
 * Returns:
 *     BT_STATUS_INVALID_PARM - Invalid or in use profile type.
 *
 *     BT_STATUS_SUCCESS - The DID is initialized.
 *
 *     BT_STATUS_FAILED - Problem initializing the DID profile.
 *
 *     BT_STATUS_BUSY - DID is already registered.
 */
BtStatus DID_RegisterRecords(const DevIdRegisterParms *Parms);

/*---------------------------------------------------------------------------
 * DID_DeregisterRecords()
 *
 *     Deregisters the Device ID Profile including removing the SDP records.
 *
 * Parameters:
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The DID profile was deregistered.
 *
 *     BT_STATUS_INVALID_PARM - The DID profile is not registered.
 *
 *     BT_STATUS_BUSY - The DID profile could not be deregistered because
 *          it is currently processing an operation.
 *
 *     BT_STATUS_FAILED - The DID profile failed to deinitialize.
 */
BtStatus DID_DeregisterRecords(void);
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */


/*---------------------------------------------------------------------------
 * DID_StartQuery()
 *
 *     Initiates an SDP query to get the Device Id information from the remote
 *     device.
 *
 * Parameters:
 *      remDev - the pointer to a BtRemoteDevice.
 *
 *      callback - the callback routine for indicating the DevId results.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The DID profile was deregistered.
 *
 *     BT_STATUS_INVALID_PARM - The remDev is invalid.
 *
 *     BT_STATUS_FAILED - The SDP query failed to be initiated.
 */
BtStatus DID_StartQuery(BtRemoteDevice *remDev, DevIdCallback callback);

#endif /* __DID_H */
