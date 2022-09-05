/****************************************************************************
 *
 * File:
 *     gattuuid.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Defines common GATT UUID's.
 *
 * Copyright 2010-2017 OpenSynergy GmbH. All rights reserved. All
 * unpublished rights reserved. Unpublished Confidential Information of
 * OpenSynergy GmbH. Do Not Disclose. No part of this work may be
 * used or reproduced in any form or by any means, or stored in a database
 * or retrieval system, without prior written permission of OpenSynergy
 * GmbH. Use of this work is governed by a license granted by
 * OpenSynergy GmbH.  This work contains confidential and proprietary
 * information of OpenSynergy GmbH. which is protected by copyright,
 * trade secret, trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef _GATTUUID_H__
#define _GATTUUID_H__

#include "xatypes.h"

/*---------------------------------------------------------------------------
 * Common GATT UUID definitions layer
 *
 * The common GATT UUID definition contains indexes, handles UUID's
 * and data structures for the Generic Attribute Profile (GATT) service.
 *
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GAPSERV_START_HANDLE constant
 *
 * Indexes and Handles for the Generic Access Profile (GAP) and Generic
 * Attribute Profile (GATT) services.
 */
/* Starting handle for the GAP service */
#define GAPSERV_START_HANDLE                1

/*---------------------------------------------------------------------------
 * GAPSERV_DEVICE_NAME constant
 *
 * Index from the start handle for the Device Name Characteristic
 */
#define GAPSERV_DEVICE_NAME                 1

/*---------------------------------------------------------------------------
 * GAPSERV_DEVICE_NAME_VAL constant
 *
 * Index from the start handle for the Device Name Characteristic
 * value
 */
#define GAPSERV_DEVICE_NAME_VAL             2

/*---------------------------------------------------------------------------
 * GAPSERV_APPEARANCE constant
 *
 * Index from the start handle for the Appearance Characteristic
 */
#define GAPSERV_APPEARANCE                  3

/*---------------------------------------------------------------------------
 * GAPSERV_APPEARANCE_VAL constant
 *
 * Index from the start handle for the Appearance Characteristic
 * value
 */
#define GAPSERV_APPEARANCE_VAL              4

/*---------------------------------------------------------------------------
 * GAPSERV_PERIPH_PREF_CONN_PARMS constant
 *
 * Index from the start handle for the Peripheral Preferred Connection
 * Parameters Characteristic
 */
#define GAPSERV_PERIPH_PREF_CONN_PARMS      5

/*---------------------------------------------------------------------------
 * GAPSERV_PERIPH_PREF_CONN_PARMS_VAL constant
 *
 * Index from the start handle for the Peripheral Preferred Connection
 * Parameters Characteristic value
 */
#define GAPSERV_PERIPH_PREF_CONN_PARMS_VAL  6

/*---------------------------------------------------------------------------
 * GAPSERV_CENTRAL_ADDR_RESOLUTION constant
 *
 * Index from the start handle for the Central Address Resolution Characteristic
 */
#define GAPSERV_CENTRAL_ADDR_RESOLUTION     7

/*---------------------------------------------------------------------------
 * GAPSERV_CENTRAL_ADDR_RESOLUTION_VAL constant
 *
 * Index from the start handle for the Central Address Resolution Characteristic
 * value
 */
#define GAPSERV_CENTRAL_ADDR_RESOLUTION_VAL 8

/*---------------------------------------------------------------------------
 * GAPSERV_MAX_ATTRIBS constant
 *
 * Maximum number of attributes allocated for the GAP service. Note that
 * currently 9 are defined in the specification but the idea is that the
 * handles for the GATT service will not change so in order to plan for
 * possible expansion some extra space is reserved.
 */
#define GAPSERV_MAX_ATTRIBS                 11

/*---------------------------------------------------------------------------
 * GATTSERV_START_HANDLE constant
 *
 * Starting handle for the GATT service
 */
#define GATTSERV_START_HANDLE               12

/*---------------------------------------------------------------------------
 * GATTSERV_SERVCHANGE_CHAR_I constant
 *
 * Index from the start handle for the Service Change Characteristic
 * declaration attribute
 */
#define GATTSERV_SERVCHANGE_CHAR_I          1

/*---------------------------------------------------------------------------
 * GATTSERV_SERVCHANGE_VAL_I constant
 *
 * Index from the start handle for the Service Change Characteristic value
 * attribute
 */
#define GATTSERV_SERVCHANGE_VAL_I           2

/*---------------------------------------------------------------------------
 * GATTSERV_SERVCHANGE_CCC_I constant
 *
 * Index from the start handle for the Service Change Characteristic
 * Client characteristic configuration descriptor.
 */
#define GATTSERV_SERVCHANGE_CCC_I           3

/*---------------------------------------------------------------------------
 * GATTSERV_MAX_ATTRIBS constant
 *
 * Maximum number of attributes allocated for the GATT service. Note that
 * currently 4 are defined in the specification but the idea is that the
 * handles for the GATT service will not change so in order to plan for
 * possible expansion some extra space is reserved.
 */
#define GATTSERV_MAX_ATTRIBS                7

/*---------------------------------------------------------------------------
 * GATTSERV_FIRST_FREE_HANDLE constant
 *
 * The first free handle available for additional services
 */
#define GATTSERV_FIRST_FREE_HANDLE          (GATTSERV_START_HANDLE + GATTSERV_MAX_ATTRIBS + 1)

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * GattUuid16 type
 *
 *     Represents 16-bit UUID defined by the GATT specification.
 */
typedef U16 GattUuid16;

/* Service UUIDs */
#define GU16_SVC_LE_GAP                     0x1800 /* GAP Service */
#define GU16_SVC_GATT                       0x1801 /* GATT Service */
#define GU16_SVC_IMMEDIATE_ALERT            0x1802 /* Immediate Alert Service */
#define GU16_SVC_LINK_LOSS_ALERT            0x1803 /* Link Loss Service */
#define GU16_SVC_TX_POWER                   0x1804 /* Tx Power Service */
#define GU16_SVC_CURRENT_TIME               0x1805 /* Current Time Service */
#define GU16_SVC_REF_TIME_UPDATE            0x1806 /* Reference Time Update Service */
#define GU16_SVC_DST_CHANGE                 0x1807 /* Next DST Change Service */
#define GU16_SVC_HEALTH_THERM               0x1809 /* Health Thermometer Service */
#define GU16_SVC_DEV_INFO                   0x180A /* Device Information Service */
#define GU16_SVC_HEART_RATE                 0x180D /* Heart Rate Service */
#define GU16_SVC_PHONE_ALERT                0x180E /* Phone Alert Status Service */
#define GU16_SVC_BATTERY                    0x180F /* Battery Service */
#define GU16_SVC_BLOOD_PRESSURE             0x1810 /* Blood Pressure Service */
#define GU16_SVC_ALERT_NOTIFICATION         0x1811 /* Alert Notification Service */
#define GU16_SVC_HUMAN_INTERFACE_DEV        0x1812 /* Human Interface Device Service */
#define GU16_SVC_SCAN_PARAMETERS            0x1813 /* Scan Parameters Service */
#define GU16_SVC_OBJECT_TRANSFER            0x1825 /* Object Transfer Service */

/* Declaration UUIDS */
#define GU16_DCL_PRIMARY_SERVICE            0x2800 /* GATT Primary Service Declaration */
#define GU16_DCL_SECONDARY_SERVICE          0x2801 /* GATT Secondary Service Declaration */
#define GU16_DCL_INCLUDE                    0x2802 /* GATT Include Declaration */
#define GU16_DCL_CHARACTERISTIC             0x2803 /* GATT Characteristic Declaration */

/* Descriptor UUIDs */
#define GU16_DSC_CHARACTER_EXT_PROPS        0x2900 /* Characteristic Extended Properties Descriptor */
#define GU16_DSC_CHARACTER_USER_DESC        0x2901 /* Characteristic User Description Descriptor */
#define GU16_DSC_CHARACTER_CLIENT_CONFIG    0x2902 /* Client Characteristic Configuration Descriptor */
#define GU16_DSC_CHARACTER_SERVER_CONFIG    0x2903 /* Server Characteristic Configuration Descriptor */
#define GU16_DSC_CHARACTER_PRES_FORMAT      0x2904 /* Characteristic Presentation Format Descriptor */
#define GU16_DSC_CHARACTER_AGGR_FORMAT      0x2905 /* Characteristic Aggregate Format Descriptor */
#define GU16_DSC_VALID_RANGE                0x2906 /* Valid Range Descriptor */
#define GU16_DSC_EXTERNAL_REPORT_REF        0x2907 /* External Report Reference Descriptor */
#define GU16_DSC_REPORT_REFERENCE           0x2908 /* Report Reference Descriptor */

/* Characteristic UUIDs */
#define GU16_CHR_DEVICE_NAME                0x2A00 /* Device Name Characteristic */
#define GU16_CHR_APPEARANCE                 0x2A01 /* Appearance Characteristic */
#define GU16_CHR_PERIPHERAL_PRIVACY_FLAG    0x2A02 /* Peripheral Privacy Flag Characteristic */
#define GU16_CHR_RECONNECT_ADDRESS          0x2A03 /* Reconnection Address Characteristic */
#define GU16_CHR_PERIPHERAL_PREF_CONN_PARMS 0x2A04 /* Peripheral Preferred Connection Parameters Characteristic */
#define GU16_CHR_SERVICE_CHANGED            0x2A05 /* Service Changed Characteristic */
#define GU16_CHR_ALERT_LEVEL                0x2A06 /* Alert Level Characteristic */
#define GU16_CHR_TX_POWER_LEVEL             0x2A07 /* Tx Power Level Characteristic */
#define GU16_CHR_DATE_TIME                  0x2A08 /* Date Time Characteristic */
#define GU16_CHR_DAY_OF_WEEK                0x2A09 /* Day of Week Characteristic */
#define GU16_CHR_DAY_DATE_TIME              0x2A0A /* Day Date Time Characteristic */
#define GU16_CHR_EXACT_TIME_256             0x2A0C /* Exact Time 256 Characteristic */
#define GU16_CHR_DST_OFFSET                 0x2A0D /* DST Offset Characteristic */
#define GU16_CHR_TIME_ZONE                  0x2A0E /* Time Zone Characteristic */
#define GU16_CHR_LOCAL_TIME_INFO            0x2A0F /* Local Time Information Characteristic */
#define GU16_CHR_TIME_WITH_DST              0x2A11 /* Time with DST Characteristic */
#define GU16_CHR_TIME_ACCURACY              0x2A12 /* Time Accuracy Characteristic */
#define GU16_CHR_TIME_SOURCE                0x2A13 /* Time Source Characteristic */
#define GU16_CHR_REFERENCE_TIME_INFO        0x2A14 /* Reference Time Information Characteristic */
#define GU16_CHR_TIME_UPDATE_CONTROL_POINT  0x2A16 /* Time Update Control Point Characteristic */
#define GU16_CHR_TIME_UPDATE_STATE          0x2A17 /* Time Update State Characteristic */
#define GU16_CHR_BATTERY_LEVEL              0x2A19 /* Battery Level Characteristic */
#define GU16_CHR_TEMPERATURE_MEASUREMENT    0x2A1C /* Temperature Measurement Characteristic */
#define GU16_CHR_TEMPERATURE_TYPE           0x2A1D /* Temperature Type Characteristic */
#define GU16_CHR_INTERMEDIATE_TEMPERATURE   0x2A1E /* Intermediate Temperature Characteristic */
#define GU16_CHR_MEASUREMENT_INTERVAL       0x2A21 /* Measurement Interval Characteristic */
#define GU16_CHR_BOOT_KBD_INPUT_REPORT      0x2A22 /* Boot Keyboard Input Report Characteristic */
#define GU16_CHR_SYSTEM_ID                  0x2A23 /* System ID Characteristic */
#define GU16_CHR_MODEL                      0x2A24 /* Model Number String Characteristic */
#define GU16_CHR_SERIAL                     0x2A25 /* Serial Number String Characteristic */
#define GU16_CHR_FIRMWARE                   0x2A26 /* Firmware Revision String Characteristic */
#define GU16_CHR_HARDWARE                   0x2A27 /* Hardware Revision String Characteristic */
#define GU16_CHR_SOFTWARE                   0x2A28 /* Software Revision String Characteristic */
#define GU16_CHR_MANUFACTURE                0x2A29 /* Manufacturer Name String Characteristic */
#define GU16_CHR_IEEE_REG_CERT_DATA_LIST    0x2A2A /* IEEE 11073-20601 Regulatory Certification Data List Characteristic */
#define GU16_CHR_CURRENT_TIME               0x2A2B /* Current Time Characteristic */
#define GU16_CHR_SCAN_REFRESH               0x2A31 /* Scan Refresh Characteristic */
#define GU16_CHR_BOOT_KBD_OUTPUT_REPORT     0x2A32 /* Boot Keyboard Output Report Characteristic */
#define GU16_CHR_BOOT_MOUSE_INPUT_REPORT    0x2A33 /* Boot Mouse Input Report Characteristic */
#define GU16_CHR_BLOOD_PRESSURE_MEASURMENT  0x2A35 /* Blood Pressure Measurement Characteristic */
#define GU16_CHR_INTERMEDIATE_CUFF_PRESSURE 0x2A36 /* Intermediate Cuff Pressure Characteristic */
#define GU16_CHR_HEART_RATE_MEASUREMENT     0x2A37 /* Heart Rate Measurement Characteristic */
#define GU16_CHR_BODY_SENSOR_LOCATION       0x2A38 /* Body Sensor Location Characteristic */
#define GU16_CHR_HEART_RATE_CONTROL_POINT   0x2A39 /* Heart Rate Control Point Characteristic */
#define GU16_CHR_ALERT_STATUS               0x2A3F /* Alert Status Characteristic */
#define GU16_CHR_RINGER_CONTROL_POINT       0x2A40 /* Ringer Control Point Characteristic */
#define GU16_CHR_RINGER_SETTING             0x2A41 /* Ringer Setting Characteristic */
#define GU16_CHR_ALERT_CATEGORY_ID_BIT_MASK 0x2A42 /* Alert Category ID Bit Mask Characteristic */
#define GU16_CHR_ALERT_CATEGORY_ID          0x2A43 /* Alert Category ID Characteristic */
#define GU16_CHR_ALERT_NOTIFY_CONTROL_POINT 0x2A44 /* Alert Notification Control Point Characteristic */
#define GU16_CHR_UNREAD_ALERT_STATUS        0x2A45 /* Unread Alert Status Characteristic */
#define GU16_CHR_NEW_ALERT                  0x2A46 /* New Alert Characteristic */
#define GU16_CHR_SUPPORTED_NEW_ALERT_CAT    0x2A47 /* Supported New Alert Category Characteristic */
#define GU16_CHR_SUPPORTED_UNREAD_ALERT_CAT 0x2A48 /* Supported Unread Alert Category Characteristic */
#define GU16_CHR_BLOOD_PRESSURE_FEATURE     0x2A49 /* Blood Pressure Feature Characteristic */
#define GU16_CHR_HID_INFORMATION            0x2A4A /* HID Information Characteristic */
#define GU16_CHR_REPORT_MAP                 0x2A4B /* Report Map Characteristic */
#define GU16_CHR_HID_CONTROL_POINT          0x2A4C /* HID Control Point Characteristic */
#define GU16_CHR_REPORT                     0x2A4D /* Report Characteristic */
#define GU16_CHR_PROTOCOL_MODE              0x2A4E /* Protocol Mode Characteristic */
#define GU16_CHR_SCAN_INTERVAL_WINDOW       0x2A4F /* Scan Interval Window Characteristic */
#define GU16_CHR_PNP_ID                     0x2A50 /* PnP ID Characteristic */
#define GU16_CENTRAL_ADDR_RESOLUTION        0x2AA6 /* Central Address Resolution*/
#define GU16_CHR_OBJ_ACTION_CONTROL_POINT   0x2AC5 /* Object Action Control Point Characteristic */
#define GU16_CHR_OBJECT_CHANGED             0x2AC8 /* Object Changed Characteristic */
#define GU16_CHR_OBJECT_FIRST_CREATED       0x2AC1 /* Object First Created Characteristic */
#define GU16_CHR_OBJECT_ID                  0x2AC3 /* Object ID Characteristic */
#define GU16_CHR_OBJECT_LAST_MODIFIED       0x2AC2 /* Object Last Modified Characteristic */
#define GU16_CHR_OBJECT_LIST_CONTROL_POINT  0x2AC6 /* Object List Control Point Characteristic */
#define GU16_CHR_OBJECT_LIST_FILTER         0x2AC7 /* Object List Filter Characteristic */
#define GU16_CHR_OBJECT_NAME                0x2ABE /* Object Name Characteristic */
#define GU16_CHR_OBJECT_PROPERTIES          0x2AC4 /* Object Properties Characteristic */
#define GU16_CHR_OBJECT_SIZE                0x2AC0 /* Object Size Characteristic */
#define GU16_CHR_OBJECT_TYPE                0x2ABF /* Object Type Characteristic */
#define GU16_CHR_OTS_FEATURE                0x2ABD /* Object Transfer Service Features Characteristic */

/* End of GattUuid16 */

/*---------------------------------------------------------------------------
 * GattAlertLevel type
 *
 * Value of the Alert level characteristic used in profiles such as Proximity 
 * and FindMe. 
 */
typedef U8 GattAlertLevel;

/* No alert */
#define GATTALERTLEVEL_NO   0x00

/* Mid level alert */
#define GATTALERTLEVEL_MID  0x01

/* High level alert */
#define GATTALERTLEVEL_HIGH 0x02

/* End of GattAlertLevel */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GattUuid structure
 *
 *     GattUuid holds a 16-bit or 128-bit UUID. Note that 128-bit UUIDs are
 *     stored in little endian format where the least significant octet is
 *     stored at array index 0.  This is used for GATT operations and 
 *     database interactions that can only use 16-bit or 128-bit UUID's.
 */
typedef struct _GattUuid {
    U8           len; /* length of UUID in octets (2 or 16) */

    /* UUID */
    union {
        U8    uuid128[16];
        U16   uuid16;
    } uuid;

} GattUuid;

/*---------------------------------------------------------------------------
 * GattUuidTypes structure
 *
 *     GattAllUuids holds a 16-bit, 32-bit, or 128-bit UUID. Note that 128-bit 
 *     UUIDs are stored in little endian format where the least significant 
 *     octet is stored at array index 0.  This is used for GATT conversion
 *     routines that can convert between any of the three possible UUID sizes.
 */
typedef struct _GattUuidTypes {
    U8           len; /* length of UUID in octets (2, 4, or 16) */

    /* All possible types of UUID's */
    union {
        U8    uuid128[16];
        U32   uuid32;
        U16   uuid16;
    } uuid;

} GattUuidTypes;

#endif /*_GATTUUID_H__ */
