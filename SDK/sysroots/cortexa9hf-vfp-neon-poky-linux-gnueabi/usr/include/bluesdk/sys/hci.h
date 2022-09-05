/****************************************************************************
 *
 * File:
 *     hci.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file contains the upper layer API for the Host Controller
 *     Interface driver.
 *
 * Copyright 1999-2017 OpenSynergy GmbH.
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

#ifndef __HCI_H
#define __HCI_H

#include "bttypes2.h"

/*---------------------------------------------------------------------------
 * Host Controller Interface (HCI) layer
 *
 *     In a typical Bluetooth implementation, the "Host" system communicates
 *     with its Bluetooth "Host Controller" using the Host Controller
 *     Interface (HCI). This interface provides a standardized way to
 *     control Bluetooth operations like inquiry, connection establishment,
 *     piconet management, and data exchange.
 *
 *     The "Host Controller" receives HCI commands from the host. Internally,
 *     it uses the Link Manager and Baseband protocols to carry out the
 *     commands, and generates events in response to the host. The HCI
 *     also allows the exchange of ACL and SCO data.
 *
 *     The HCI layer uses an HCI Transport Driver to exchange commands,
 *     events, and data with the Host Controller. The Transport Driver
 *     is specific to the transport used, such as UART, RS232, or USB.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HCI_NUM_HANDLES constant
 *
 *     Total number of HCI handles that HCI will be required to manage.
 *     This is equal to the number of ACL and SCO links allowed, plus
 *     two more for L2CAP broadcast handles.
 *
 */
#define HCI_NUM_HANDLES (NUM_BT_DEVICES + NUM_LE_DEVICES + NUM_SCO_CONNS + 2)

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HciCallbackEvent type
 *
 *     These events are passed up to the Management Entity in the
 *     ME_Callback() function. The callback parameters (in HciCallbackParms)
 *     contain a union of different pointer types that are used depending
 *     on the event received.
 */
typedef U8 HciCallbackEvent;

/* An event was received from the host controller. Events occur in response
 * to commands sent by the host, although several events can be received
 * for a single command.
 *
 * The "hciEvent" callback parameter contains the data associated with the
 * command. The "status" field will always indicate success, and the
 * "hciHandle" field is not used.
 */
#define HCI_CONTROLLER_EVENT 1

/* Either SCO or ACL data was received from another device. This data can
 * be found in the "rxBuff" field. The type of data (ACL or SCO)
 * is identified in the rxBuff->flags field. The "hciHandle" field will also
 * be set with the ACL or SCO connection handle. The "status" field will
 * always indicate success.
 */
#define HCI_DATA_IND         2

/* A transmitted ACL or SCO packet is free to be re-used by
 * higher layers. The returned packet is identified in the "packet" field,
 * and the "hciHandle" field indicates the connection on which the packet
 * was sent.
 *
 * If the "status" field indicates success, the packet was successfully sent
 * to the host controller, but the remote device might not have received
 * the data yet.
 *
 * If the "status" field indicates failure, the packet was not sent
 * successfully due to link disconnection or a transport error.
 */
#define HCI_PACKET_HANDLED   3

/* A transmitted command is free to be re-used by higher layers.
 * The returned command is identified in the "cmd" field. The
 * "hciHandle" field is not used.
 *
 * If the "status" field indicates success, the command was successfully
 * transmitted to the host controller. One or more events will follow
 * in response.
 *
 * If the "status" field indicates failure, the command could not be
 * sent due to a transport error.
 */
#define HCI_COMMAND_HANDLED  4

/* The HCI layer is ready for SCO or ACL transmit data. This event is
 * generated sometime after HCI_RequestToSend() is called by a higher layer.
 * The "status" field always indicates success.
 *
 * The "hciHandle" field contains the handle of the connection. During
 * the callback, data for this connection should be stored in the
 * "packet" field. The HCI layer will continue to generate this event
 * until the "packet" field is set to NULL.
 */
#define HCI_SEND_IND         5

/* HCI initialization state has changed. The "status" field indicates whether
 * initialization succeeded or failed.   If "status" is BT_STATUS_FAILED,
 * synchronization with the radio will be retried BT_HCI_NUM_INIT_RETRIES times.
 * If "status" is BT_STATUS_HCI_INIT_ERR, no more retries will be made.
 * All other callback fields are not used.
 */
#define HCI_INIT_STATUS      6

/* HCI deinitialization is complete. The "status" field indicates whether
 * deinitialization completed gracefully  or not (succeeded or failed). All other
 * callback fields are not used.
 */
#define HCI_DEINIT_STATUS    7

/* An HCI transport error occurred.  This can be the result of an error in the
 * the transport, or an error generated by the Bluetooth radio.
 * An error code may be found in the "error" field. The "status" field will 
 * indicate that a failure has occurred.  All other callback field 
 * are not used.
 */
#define HCI_TRANSPORT_ERROR  8

/* Event for bluetooth logo testing */
#define HCI_BLUETOOTH_LOGO   9

/* Vendor specific event (debugging) */
#define HCI_VENDOR_SPECIFIC  10

/* End of HciCallbackEvent */


/*---------------------------------------------------------------------------
 * HciCommandType type
 *
 *     Commands are indicated to the HCI layer when calling
 *     HCI_SendCommand. A command type from the list below is specified
 *     in the "opCode" parameter to this function. Additional
 *     command-specific information is provided in the "cmd" parameter.
 */
typedef U16 HciCommandType;

/* Group: Link control commands */
#define HCC_INQUIRY                         0x0401
#define HCC_INQUIRY_CANCEL                  0x0402
#define HCC_START_PERIODIC_INQ_MODE         0x0403
#define HCC_EXIT_PERIODIC_INQ_MODE          0x0404
#define HCC_CREATE_CONNECTION               0x0405
#define HCC_DISCONNECT                      0x0406
#define HCC_ADD_SCO_CONNECTION              0x0407 /* Not in 1.2 */
#define HCC_CREATE_CONNECTION_CANCEL        0x0408 /* 1.2 */
#define HCC_ACCEPT_CON_REQ                  0x0409
#define HCC_REJECT_CON_REQ                  0x040A
#define HCC_LINK_KEY_REQ_REPL               0x040B
#define HCC_LINK_KEY_REQ_NEG_REPL           0x040C
#define HCC_PIN_CODE_REQ_REPL               0x040D
#define HCC_PIN_CODE_REQ_NEG_REPL           0x040E
#define HCC_CHNG_CONN_PACKET_TYPE           0x040F
#define HCC_AUTH_REQ                        0x0411
#define HCC_SET_CONN_ENCRYPT                0x0413
#define HCC_CHNG_CONN_LINK_KEY              0x0415
#define HCC_MASTER_LINK_KEY                 0x0417
#define HCC_REM_NAME_REQ                    0x0419
#define HCC_REM_NAME_REQ_CANCEL             0x041A /* 1.2 */
#define HCC_READ_REMOTE_FEATURES            0x041B
#define HCC_READ_REMOTE_EXT_FEATURES        0x041C /* 1.2 */
#define HCC_READ_REMOTE_VERSION             0x041D
#define HCC_READ_CLOCK_OFFSET               0x041F
#define HCC_READ_LMP_HANDLE                 0x0420 /* 1.2 */
#define HCC_EXCHANGE_FIXED_INFO             0x0421 /* 1.2, not in 2.1 */
#define HCC_EXCHANGE_ALIAS_INFO             0x0422 /* 1.2, not in 2.1 */
#define HCC_PRIVATE_PAIRING_REQ_REPL        0x0423 /* 1.2, not in 2.1 */
#define HCC_PRIVATE_PAIRING_REQ_NEG_REPL    0x0424 /* 1.2, not in 2.1 */
#define HCC_GENERATED_ALIAS                 0x0425 /* 1.2, not in 2.1 */
#define HCC_ALIAS_ADDRESS_REQ_REPL          0x0426 /* 1.2, not in 2.1 */
#define HCC_ALIAS_ADDRESS_REQ_NEG_REPL      0x0427 /* 1.2, not in 2.1 */
#define HCC_SETUP_SYNC_CONNECTION           0x0428 /* 1.2 */
#define HCC_ACCEPT_SYNC_CON_REQ             0x0429 /* 1.2 */
#define HCC_REJECT_SYNC_CON_REQ             0x042A /* 1.2 */
#define HCC_IO_CAPABILITY_RESPONSE          0x042B /* 2.1 */
#define HCC_USER_CONFIRM_REQ_REPL           0x042C /* 2.1 */
#define HCC_USER_CONFIRM_REQ_NEG_REPL       0x042D /* 2.1 */
#define HCC_USER_PASSKEY_REQ_REPL           0x042E /* 2.1 */
#define HCC_USER_PASSKEY_REQ_NEG_REPL       0x042F /* 2.1 */
#define HCC_REM_OOB_DATA_REQ_REPL           0x0430 /* 2.1 */
#define HCC_REM_OOB_DATA_REQ_NEG_REPL       0x0433 /* 2.1 */
#define HCC_IO_CAPABILITY_NEG_REPL          0x0434 /* 2.1 */
#define HCC_CREATE_PHY_LINK                 0x0435 /* 3.0 */
#define HCC_ACCEPT_PHY_LINK                 0x0436 /* 3.0 */
#define HCC_DISCONNECT_PHY_LINK             0x0437 /* 3.0 */
#define HCC_CREATE_LOG_LINK                 0x0438 /* 3.0 */
#define HCC_ACCEPT_LOG_LINK                 0x0439 /* 3.0 */
#define HCC_DISCONNECT_LOG_LINK             0x043A /* 3.0 */
#define HCC_CANCEL_LOG_LINK                 0x043B /* 3.0 */
#define HCC_FLOW_SPEC_MODIFY                0x043C /* 3.0 */
#define HCC_ENHANCED_SETUP_SYNC_CONN        0x043D /* 4.0 CSA2 */
#define HCC_ENHANCED_ACCEPT_SYNC_CONN       0x043E /* 4.0 CSA2 */
#define HCC_TRUNCATE_PAGE_COMMAND           0x043F /* 4.1 */
#define HCC_TRUNCATE_PAGE_CANCEL            0x0440 /* 4.1 */
#define HCC_SET_CONN_LESS_BROADCAST         0x0441 /* 4.1 */
#define HCC_SET_CONN_LESS_BROADCAST_RCV     0x0442 /* 4.1 */
#define HCC_START_SYNC_TRAIN                0x0443 /* 4.1 */
#define HCC_RECEIVE_SYNC_TRAIN              0x0444 /* 4.1 */
#define HCC_REM_OOB_EXT_DATA_REQ_REPL       0x0445 /* 4.1 */

/* Group: Link policy commands */
#define HCC_HOLD_MODE                       0x0801
#define HCC_SNIFF_MODE                      0x0803
#define HCC_EXIT_SNIFF_MODE                 0x0804
#define HCC_PARK_MODE                       0x0805
#define HCC_EXIT_PARK_MODE                  0x0806
#define HCC_QOS_SETUP                       0x0807
#define HCC_ROLE_DISCOVERY                  0x0809
#define HCC_SWITCH_ROLE                     0x080B
#define HCC_READ_LINK_POLICY                0x080C
#define HCC_WRITE_LINK_POLICY               0x080D
#define HCC_READ_DEFAULT_LINK_POLICY        0x080E /* 1.2 */
#define HCC_WRITE_DEFAULT_LINK_POLICY       0x080F /* 1.2 */
#define HCC_FLOW_SPECIFICATION              0x0810 /* 1.2 */
#define HCC_SNIFF_SUBRATING                 0x0811 /* 2.1 */

/* Group: Host controller and baseband commands */
#define HCC_SET_EVENT_MASK                  0x0C01
#define HCC_RESET                           0x0C03
#define HCC_EVENT_FILTER                    0x0C05
#define HCC_FLUSH                           0x0C08
#define HCC_READ_PIN_TYPE                   0x0C09
#define HCC_WRITE_PIN_TYPE                  0x0C0A
#define HCC_CREATE_NEW_UNIT_KEY             0x0C0B
#define HCC_READ_STORED_LINK_KEY            0x0C0D
#define HCC_WRITE_STORED_LINK_KEY           0x0C11
#define HCC_DEL_STORED_LINK_KEY             0x0C12
#define HCC_CHNG_LOCAL_NAME                 0x0C13
#define HCC_READ_LOCAL_NAME                 0x0C14
#define HCC_READ_CONN_ACCEPT_TIMEOUT        0x0C15
#define HCC_WRITE_CONN_ACCEPT_TIMEOUT       0x0C16
#define HCC_READ_PAGE_TIMEOUT               0x0C17
#define HCC_WRITE_PAGE_TIMEOUT              0x0C18
#define HCC_READ_SCAN_ENABLE                0x0C19
#define HCC_WRITE_SCAN_ENABLE               0x0C1A
#define HCC_READ_PAGE_SCAN_ACTIVITY         0x0C1B
#define HCC_WRITE_PAGE_SCAN_ACTIVITY        0x0C1C
#define HCC_READ_INQ_SCAN_ACTIVITY          0x0C1D
#define HCC_WRITE_INQ_SCAN_ACTIVITY         0x0C1E
#define HCC_READ_AUTH_ENABLE                0x0C1F
#define HCC_WRITE_AUTH_ENABLE               0x0C20
#define HCC_READ_ENCRYPT_MODE               0x0C21 /* Not in 2.1 */
#define HCC_WRITE_ENCRYPT_MODE              0x0C22 /* Not in 2.1 */
#define HCC_READ_CLASS_OF_DEVICE            0x0C23
#define HCC_WRITE_CLASS_OF_DEVICE           0x0C24
#define HCC_READ_VOICE_SETTING              0x0C25
#define HCC_WRITE_VOICE_SETTING             0x0C26
#define HCC_READ_AUTO_FLUSH_TIMEOUT         0x0C27
#define HCC_WRITE_AUTO_FLUSH_TIMEOUT        0x0C28
#define HCC_READ_NUM_BC_RETRANSMIT          0x0C29
#define HCC_WRITE_NUM_BC_RETRANSMIT         0x0C2A
#define HCC_READ_HOLD_MODE_ACTIVITY         0x0C2B
#define HCC_WRITE_HOLD_MODE_ACTIVITY        0x0C2C
#define HCC_READ_XMIT_POWER_LEVEL           0x0C2D
#define HCC_READ_SCO_FC_ENABLE              0x0C2E
#define HCC_WRITE_SCO_FC_ENABLE             0x0C2F
#define HCC_SET_CTRLR_TO_HOST_FLOW_CTRL     0x0C31
#define HCC_HOST_BUFFER_SIZE                0x0C33
#define HCC_HOST_NUM_COMPLETED_PACKETS      0x0C35
#define HCC_READ_LINK_SUPERV_TIMEOUT        0x0C36
#define HCC_WRITE_LINK_SUPERV_TIMEOUT       0x0C37
#define HCC_READ_NUM_IAC                    0x0C38
#define HCC_READ_CURRENT_IAC_LAP            0x0C39
#define HCC_WRITE_CURRENT_IAC_LAP           0x0C3A
#define HCC_READ_PAGE_SCAN_PERIOD_MODE      0x0C3B /* Not in 2.1 */
#define HCC_WRITE_PAGE_SCAN_PERIOD_MODE     0x0C3C /* Not in 2.1 */
#define HCC_READ_PAGE_SCAN_MODE             0x0C3D /* Not in 1.2 */
#define HCC_WRITE_PAGE_SCAN_MODE            0x0C3E /* Not in 1.2 */
#define HCC_SET_AFH_HOST_CHAN_CLASS         0x0C3F /* 1.2 */
#define HCC_READ_INQ_SCAN_TYPE              0x0C42 /* 1.2 */
#define HCC_WRITE_INQ_SCAN_TYPE             0x0C43 /* 1.2 */
#define HCC_READ_INQ_MODE                   0x0C44 /* 1.2 */
#define HCC_WRITE_INQ_MODE                  0x0C45 /* 1.2 */
#define HCC_READ_PAGE_SCAN_TYPE             0x0C46 /* 1.2 */
#define HCC_WRITE_PAGE_SCAN_TYPE            0x0C47 /* 1.2 */
#define HCC_READ_AFH_CHAN_ASSESS_MODE       0x0C48 /* 1.2 */
#define HCC_WRITE_AFH_CHAN_ASSESS_MODE      0x0C49 /* 1.2 */
#define HCC_READ_ANONYMITY_MODE             0x0C4A /* 1.2, not in 2.1 */
#define HCC_WRITE_ANONYMITY_MODE            0x0C4B /* 1.2, not in 2.1 */
#define HCC_READ_ALIAS_AUTH_ENABLE          0x0C4C /* 1.2, not in 2.1 */
#define HCC_WRITE_ALIAS_AUTH_ENABLE         0x0C4D /* 1.2, not in 2.1 */
#define HCC_READ_ANON_ADDR_CHNG_PARM        0x0C4E /* 1.2, not in 2.1 */
#define HCC_WRITE_ANON_ADDR_CHNG_PARM       0x0C4F /* 1.2, not in 2.1 */
#define HCC_RESET_FIXED_ADDR_COUNTER        0x0C50 /* 1.2, not in 2.1 */
#define HCC_READ_EXTENDED_INQ_RESP          0x0C51 /* 2.1 */
#define HCC_WRITE_EXTENDED_INQ_RESP         0x0C52 /* 2.1 */
#define HCC_REFRESH_ENCRYPT_KEY             0x0C53 /* 2.1 */
#define HCC_READ_SIMPLE_PAIRING_MODE        0x0C55 /* 2.1 */
#define HCC_WRITE_SIMPLE_PAIRING_MODE       0x0C56 /* 2.1 */
#define HCC_READ_LOCAL_OOB_DATA             0x0C57 /* 2.1 */
#define HCC_READ_INQ_RESP_TRANS_PWR_LVL     0x0C58 /* 2.1 */
#define HCC_WRITE_INQ_TRANS_PWR_LVL         0x0C59 /* 2.1 */
#define HCC_READ_DEF_ERR_DATA_REPORTING     0x0C5A /* 2.1 */
#define HCC_WRITE_DEF_ERR_DATA_REPORTING    0x0C5B /* 2.1 */
#define HCC_ENHANCED_FLUSH                  0x0C5F /* 2.1 */
#define HCC_SEND_KEYPRESS_NOTIFY            0x0C60 /* 2.1 */
#define HCC_READ_LOG_ACCEPT_TIMEOUT         0x0C61 /* 3.0 */
#define HCC_WRITE_LOG_ACCEPT_TIMEOUT        0x0C62 /* 3.0 */
#define HCC_SET_EVENT_MASK2                 0x0C63 /* 3.0 */
#define HCC_READ_LOC_DATA                   0x0C64 /* 3.0 */
#define HCC_WRITE_LOC_DATA                  0x0C65 /* 3.0 */
#define HCC_READ_FLOW_MODE_COMMAND          0x0C66 /* 3.0 */
#define HCC_WRITE_FLOW_MODE_COMMAND         0x0C67 /* 3.0 */
#define HCC_READ_ENH_TRANS_POWER_LEVEL      0x0C68 /* 3.0 */
#define HCC_READ_BE_FLUSH_TIMEOUT           0x0C69 /* 3.0 */
#define HCC_WRITE_BE_FLUSH_TIMEOUT          0x0C6A /* 3.0 */
#define HCC_SHORT_RANGE_MODE                0x0C6B /* 3.0 */
#define HCC_READ_LE_HOST_SUPPORTED          0x0C6C /* 4.0 */
#define HCC_WRITE_LE_HOST_SUPPORTED         0x0C6D /* 4.0 */
#define HCC_READ_SYNC_TRAIN_PARMS           0x0C77 /* 4.1 */
#define HCC_WRITE_SYNC_TRAIN_PARMS          0x0C78 /* 4.1 */
#define HCC_READ_SECURE_CONN_HOST_SUPPORT   0x0C79 /* 4.1 */
#define HCC_WRITE_SECURE_CONN_HOST_SUPPORT  0x0C7A /* 4.1 */
#define HCC_READ_AUTH_PAYLOAD_TIMEOUT       0x0C7B /* 4.1 */
#define HCC_WRITE_AUTH_PAYLOAD_TIMEOUT      0x0C7C /* 4.1 */
#define HCC_READ_LOCAL_OOB_EXT_DATA         0x0C7D /* 4.1 */
#define HCC_READ_EXT_PAGE_TIMEOUT           0x0C7E /* 4.1 */
#define HCC_WRITE_EXT_PAGE_TIMEOUT          0x0C7F /* 4.1 */
#define HCC_READ_EXT_INQUIRY_LENGTH         0x0C80 /* 4.1 */
#define HCC_WRITE_EXT_INQUIRY_LENGTH        0x0C81 /* 4.1 */

/* Group: Informational parameters */
#define HCC_READ_LOCAL_VERSION              0x1001
#define HCC_READ_LOCAL_COMMANDS             0x1002 /* 1.2 */
#define HCC_READ_LOCAL_FEATURES             0x1003
#define HCC_READ_LOCAL_EXT_FEATURES         0x1004 /* 1.2 */
#define HCC_READ_BUFFER_SIZE                0x1005
#define HCC_READ_COUNTRY_CODE               0x1007 /* Not in 1.2 */
#define HCC_READ_BD_ADDR                    0x1009
#define HCC_READ_DATA_BLOCK_SIZE            0x100A /* 3.0 */
#define HCC_READ_LOCAL_SUPPORTED_CODECS     0x100B /* 4.0 CSA2 */

/* Group: Status parameters */
#define HCC_READ_FAILED_CONTACT_COUNT       0x1401
#define HCC_RESET_FAILED_CONTACT_COUNT      0x1402
#define HCC_GET_LINK_QUALITY                0x1403
#define HCC_READ_RSSI                       0x1405
#define HCC_READ_AFH_CHANNEL_MAP            0x1406 /* 1.2 */
#define HCC_READ_CLOCK                      0x1407 /* 1.2 */
#define HCC_READ_ENCRYPT_KEY_SIZE           0x1408 /* 3.0 */
#define HCC_READ_LOCAL_AMP_INFO             0x1409 /* 3.0 */
#define HCC_READ_LOCAL_AMP_ASSOC            0x140A /* 3.0 */
#define HCC_WRITE_REMOTE_AMP_ASSOC          0x140B /* 3.0 */

/* Group: Testing commands */
#define HCC_READ_LOOPBACK_MODE              0x1801
#define HCC_WRITE_LOOPBACK_MODE             0x1802
#define HCC_ENABLE_DUT                      0x1803
#define HCC_WRITE_SIMPLE_PAIR_DBG_MODE      0x1804 /* 2.1 */
#define HCC_ENABLE_AMP_RECV_REPORTS         0x1807 /* 3.0 */
#define HCC_AMP_TEST_END                    0x1808 /* 3.0 */
#define HCC_AMP_TEST                        0x1809 /* 3.0 */

/* Group: LE commands */
#define HCC_LE_SET_EVENT_MASK               0x2001 /* 4.0 */
#define HCC_LE_READ_BUFFER_SIZE             0x2002 /* 4.0 */
#define HCC_LE_READ_LOCAL_FEATURES          0x2003 /* 4.0 */
#define HCC_LE_SET_RANDOM_ADDRESS           0x2005 /* 4.0 */
#define HCC_LE_SET_ADVERT_PARMS             0x2006 /* 4.0 */
#define HCC_LE_READ_ADVERT_CHAN_TX_PWR      0x2007 /* 4.0 */
#define HCC_LE_SET_ADVERT_DATA              0x2008 /* 4.0 */
#define HCC_LE_SET_SCAN_RESP_DATA           0x2009 /* 4.0 */
#define HCC_LE_SET_ADVERT_ENABLE            0x200A /* 4.0 */
#define HCC_LE_SET_SCAN_PARMS               0x200B /* 4.0 */
#define HCC_LE_SET_SCAN_ENABLE              0x200C /* 4.0 */
#define HCC_LE_CREATE_CONNECTION            0x200D /* 4.0 */
#define HCC_LE_CREATE_CONNECTION_CANCEL     0x200E /* 4.0 */
#define HCC_LE_READ_WHITE_LIST_SIZE         0x200F /* 4.0 */
#define HCC_LE_CLEAR_WHITE_LIST             0x2010 /* 4.0 */
#define HCC_LE_ADD_DEVICE_WHITE_LIST        0x2011 /* 4.0 */
#define HCC_LE_REMOVE_DEVICE_WHITE_LIST     0x2012 /* 4.0 */
#define HCC_LE_CONNECTION_UPDATE            0x2013 /* 4.0 */
#define HCC_LE_SET_HOST_CHANNEL_CLASS       0x2014 /* 4.0 */
#define HCC_LE_READ_CHANNEL_MAP             0x2015 /* 4.0 */
#define HCC_LE_READ_REMOTE_USED_FEATURES    0x2016 /* 4.0 */
#define HCC_LE_ENCRYPT                      0x2017 /* 4.0 */
#define HCC_LE_RAND                         0x2018 /* 4.0 */
#define HCC_LE_START_ENCRYPTION             0x2019 /* 4.0 */
#define HCC_LE_LT_KEY_REQ_REPLY             0x201A /* 4.0 */
#define HCC_LE_LT_KEY_REQ_NEG_REPLY         0x201B /* 4.0 */
#define HCC_LE_READ_SUPPORTED_STATES        0x201C /* 4.0 */
#define HCC_LE_RECEIVER_TEST                0x201D /* 4.0 */
#define HCC_LE_TRANSMITTER_TEST             0x201E /* 4.0 */
#define HCC_LE_TEST_END                     0x201F /* 4.0 */
#define HCC_LE_REM_CONNECT_PARM_REPL        0x2020 /* 4.1 */
#define HCC_LE_REM_CONNECT_PARM_NEG_REPL    0x2021 /* 4.1 */
#define HCC_LE_SET_DATA_LENGTH              0x2022 /* 4.2 */
#define HCC_LE_READ_DEFAULT_DATA_LENGTH     0x2023 /* 4.2 */
#define HCC_LE_WRITE_DEFAULT_DATA_LENGTH    0x2024 /* 4.2 */
#define HCC_LE_READ_LOCAL_PUBLIC_KEY        0x2025 /* 4.2 */
#define HCC_LE_GEN_DHKEY                    0x2026 /* 4.2 */
#define HCC_LE_ADD_DEVICE_RESOLVING_LIST    0x2027 /* 4.2 */
#define HCC_LE_REMOVE_DEVICE_RESOLVING_LIST 0x2028 /* 4.2 */
#define HCC_LE_CLEAR_RESOLVING_LIST         0x2029 /* 4.2 */
#define HCC_LE_READ_RESOLVING_LIST_SIZE     0x202A /* 4.2 */
#define HCC_LE_READ_PEER_RESOLVE_ADDR       0x202B /* 4.2 */
#define HCC_LE_READ_LOCAL_RESOLVE_ADDR      0x202C /* 4.2 */
#define HCC_LE_ENABLE_ADDR_RESOLUTION       0x202D /* 4.2 */
#define HCC_LE_SET_RESOLVE_ADDR_TIMEOUT     0x202E /* 4.2 */
#define HCC_LE_READ_MAX_DATA_LENGTH         0x202F /* 4.2 */

/* End of HciCommandType */


/*---------------------------------------------------------------------------
 * HciEventType type
 *
 *     These events are indicated by HCI to the Management Entity during
 *     a HCI_CONTROLLER_EVENT callback. The event type is found in the
 *     "hciEvent->event" field of the callback parameters structure
 *     (HciCallbackParms).
 */
typedef U8 HciEventType;

#define HCE_INQUIRY_COMPLETE               0x01
#define HCE_INQUIRY_RESULT                 0x02
#define HCE_CONNECT_COMPLETE               0x03
#define HCE_CONNECT_REQUEST                0x04
#define HCE_DISCONNECT_COMPLETE            0x05
#define HCE_AUTH_COMPLETE                  0x06
#define HCE_REMOTE_NAME_REQ_COMPLETE       0x07
#define HCE_ENCRYPT_CHNG                   0x08
#define HCE_CHNG_CONN_LINK_KEY_COMPLETE    0x09
#define HCE_MASTER_LINK_KEY_COMPLETE       0x0A
#define HCE_READ_REMOTE_FEATURES_COMPLETE  0x0B
#define HCE_READ_REMOTE_VERSION_COMPLETE   0x0C
#define HCE_QOS_SETUP_COMPLETE             0x0D
#define HCE_COMMAND_COMPLETE               0x0E
#define HCE_COMMAND_STATUS                 0x0F
#define HCE_HARDWARE_ERROR                 0x10
#define HCE_FLUSH_OCCURRED                 0x11
#define HCE_ROLE_CHANGE                    0x12
#define HCE_NUM_COMPLETED_PACKETS          0x13
#define HCE_MODE_CHNG                      0x14
#define HCE_RETURN_LINK_KEYS               0x15
#define HCE_PIN_CODE_REQ                   0x16
#define HCE_LINK_KEY_REQ                   0x17
#define HCE_LINK_KEY_NOTIFY                0x18
#define HCE_LOOPBACK_COMMAND               0x19
#define HCE_DATA_BUFFER_OVERFLOW           0x1A
#define HCE_MAX_SLOTS_CHNG                 0x1B
#define HCE_READ_CLOCK_OFFSET_COMPLETE     0x1C
#define HCE_CONN_PACKET_TYPE_CHNG          0x1D
#define HCE_QOS_VIOLATION                  0x1E
#define HCE_PAGE_SCAN_MODE_CHANGE          0x1F /* Not in 1.2 */
#define HCE_PAGE_SCAN_REPETITION_MODE      0x20
#define HCE_FLOW_SPECIFICATION_COMPLETE    0x21 /* 1.2 */
#define HCE_INQUIRY_RESULT_WITH_RSSI       0x22 /* 1.2 */
#define HCE_READ_REMOTE_EXT_FEAT_COMPLETE  0x23 /* 1.2 */
#define HCE_FIXED_ADDRESS                  0x24 /* 1.2 */
#define HCE_ALIAS_ADDRESS                  0x25 /* 1.2 */
#define HCE_GENERATE_ALIAS_REQ             0x26 /* 1.2 */
#define HCE_ACTIVE_ADDRESS                 0x27 /* 1.2 */
#define HCE_ALLOW_PRIVATE_PAIRING          0x28 /* 1.2 */
#define HCE_ALIAS_ADDRESS_REQ              0x29 /* 1.2 */
#define HCE_ALIAS_NOT_RECOGNIZED           0x2A /* 1.2 */
#define HCE_FIXED_ADDRESS_ATTEMPT          0x2B /* 1.2 */
#define HCE_SYNC_CONNECT_COMPLETE          0x2C /* 1.2 */
#define HCE_SYNC_CONN_CHANGED              0x2D /* 1.2 */
#define HCE_HCI_SNIFF_SUBRATING            0x2E /* 2.1 */
#define HCE_EXTENDED_INQUIRY_RESULT        0x2F /* 2.1 */
#define HCE_ENCRYPT_KEY_REFRESH_COMPLETE   0x30 /* 2.1 */
#define HCE_IO_CAPABILITY_REQUEST          0x31 /* 2.1 */
#define HCE_IO_CAPABILITY_RESPONSE         0x32 /* 2.1 */
#define HCE_USER_CONFIRMATION_REQUEST      0x33 /* 2.1 */
#define HCE_USER_PASSKEY_REQUEST           0x34 /* 2.1 */
#define HCE_REMOTE_OOB_DATA_REQUEST        0x35 /* 2.1 */
#define HCE_SIMPLE_PAIRING_COMPLETE        0x36 /* 2.1 */
#define HCE_LINK_SUPERV_TIMEOUT_CHANGED    0x38 /* 2.1 */
#define HCE_ENHANCED_FLUSH_COMPLETE        0x39 /* 2.1 */
#define HCE_USER_PASSKEY_NOTIFICATION      0x3B /* 2.1 */
#define HCE_KEYPRESS_NOTIFICATION          0x3C /* 2.1 */
#define HCE_REMOTE_HOST_SUPPORTED_FEATURES 0x3D /* 2.1 */
#define HCE_LE_META_EVENT                  0x3E /* 4.0 */
#define HCE_PHY_LINK_COMPLETE              0x40 /* 3.0 */
#define HCE_CHANNEL_SELECTED               0x41 /* 3.0 */
#define HCE_PHY_LINK_DISCONNECT_COMPLETE   0x42 /* 3.0 */
#define HCE_PHY_LINK_LOSS_EARLY_WARNING    0x43 /* 3.0 */
#define HCE_PHY_LINK_RECOVERY              0x44 /* 3.0 */
#define HCE_LOG_LINK_COMPLETE              0x45 /* 3.0 */
#define HCE_LOG_LINK_DISCONNECT_COMPLETE   0x46 /* 3.0 */
#define HCE_FLOW_SPEC_MODIFY_COMPLETE      0x47 /* 3.0 */
#define HCE_NUM_COMPLETED_DATA_BLOCKS      0x48 /* 3.0 */
#define HCE_SHORT_RANGE_MODE_COMPLETE      0x4C /* 3.0 */
#define HCE_AMP_STATUS_CHANGE              0x4D /* 3.0 */
#define HCE_AMP_START_TEST                 0x49 /* 3.0 */
#define HCE_HCI_AMP_TEST_END               0x4A /* 3.0 */
#define HCE_HCI_AMP_RECEIVER_REPORT        0x4B /* 3.0 */
#define HCE_TRIGGERED_CLOCK_CAPTURE        0x4E /* 4.1 */
#define HCE_SYNC_TRAIN_COMPLETE            0x4F /* 4.1 */
#define HCE_SYNC_TRAIN_RECEIVED            0x50 /* 4.1 */
#define HCE_BROADCAST_RECEIVE              0x51 /* 4.1 */
#define HCE_BROADCAST_TIMEOUT              0x52 /* 4.1 */
#define HCE_TRUNCATED_PAGE_COMPLETE        0x53 /* 4.1 */
#define HCE_PAGE_RESPONSE_TIMEOUT          0x54 /* 4.1 */
#define HCE_BROADCAST_CHANNEL_MAP_CHNG     0x55 /* 4.1 */
#define HCE_INQUIRY_RESPONSE_NOTIFY        0x56 /* 4.1 */
#define HCE_AUTH_PAYLOAD_TIMEOUT_EXPIRED   0x57 /* 4.1 */

#define HCE_BLUETOOTH_LOGO                 0xFE
#define HCE_VENDOR_SPECIFIC                0xFF

/* End of HciEventType */

/*---------------------------------------------------------------------------
 * HciLeSubEventType type
 *
 *     These sub events are indicated by HCI to the Management Entity
 *     within the HCE_LE_META_EVENT during a HCI_CONTROLLER_EVENT callback.
 *     The HCE_LE_META_EVENT event type is found in the "hciEvent->event"
 *     field of the callback parameters structure (HciCallbackParms). This
 *     LE Subevent is the indicated in the first byte of the event
 *     parameters.
 */
typedef U8 HciLeSubEventType;

#define HCE_LE_CONNECT_COMPLETE            0x01 /* 4.0 */
#define HCE_LE_ADVERT_REPORT               0x02 /* 4.0 */
#define HCE_LE_CONNECT_UPDATE_COMPLETE     0x03 /* 4.0 */
#define HCE_LE_READ_REM_USED_FEAT_COMPLETE 0x04 /* 4.0 */
#define HCE_LE_LT_KEY_REQUEST              0x05 /* 4.0 */
#define HCE_LE_REM_CONNECT_PARM_REQUEST    0x06 /* 4.1 */
#define HCE_LE_DATA_LENGTH_CHANGE          0x07 /* 4.2 */
#define HCE_LE_READ_LOCAL_P256_COMPLETE    0x08 /* 4.2 */
#define HCE_LE_GEN_DHKEY_COMPLETE          0x09 /* 4.2 */
#define HCE_LE_ENHANCED_CONNECT_COMPLETE   0x0A /* 4.2 */
#define HCE_LE_DIRECT_ADVERT_REPORT        0x0B /* 4.2 */

/* End of HciLeSubEventType */


/*---------------------------------------------------------------------------
 * HcStatus type
 *
 *     The first parameter in an HCI event often contains a "status" value.
 *     0x00 means pending or success, according to the event type, but
 *     other values provide a specific reason for the failure. These
 *     values are listed below.
 */
typedef U8  HcStatus;

#define HC_STATUS_SUCCESS                           0x00
#define HC_STATUS_UNKNOWN_HCI_CMD                   0x01
#define HC_STATUS_NO_CONNECTION                     0x02
#define HC_STATUS_HARDWARE_FAILURE                  0x03
#define HC_STATUS_PAGE_TIMEOUT                      0x04
#define HC_STATUS_AUTH_FAILURE                      0x05
#define HC_STATUS_KEY_MISSING                       0x06
#define HC_STATUS_MEMORY_FULL                       0x07
#define HC_STATUS_CONN_TIMEOUT                      0x08
#define HC_STATUS_MAX_NUM_CONNS                     0x09
#define HC_STATUS_MAX_SCO_CONNS                     0x0A
#define HC_STATUS_ACL_ALREADY_EXISTS                0x0B
#define HC_STATUS_CMD_DISALLOWED                    0x0C
#define HC_STATUS_HOST_REJ_NO_RESOURCES             0x0D
#define HC_STATUS_HOST_REJ_SECURITY                 0x0E
#define HC_STATUS_HOST_REJ_PERSONAL_DEV             0x0F
#define HC_STATUS_HOST_TIMEOUT                      0x10
#define HC_STATUS_UNSUPP_FEATUR_PARM_VAL            0x11
#define HC_STATUS_INVAL_HCI_PARM_VAL                0x12
#define HC_STATUS_CONN_TERM_USER_REQ                0x13
#define HC_STATUS_CONN_TERM_LOW_RESOURCES           0x14
#define HC_STATUS_CONN_TERM_POWER_OFF               0x15
#define HC_STATUS_CONN_TERM_LOCAL_HOST              0x16
#define HC_STATUS_REPEATED_ATTEMPTS                 0x17
#define HC_STATUS_PAIRING_DISALLOWED                0x18
#define HC_STATUS_UNKNOWN_LMP_PDU                   0x19
#define HC_STATUS_UNSUPP_REMOTE_FEATURE             0x1A
#define HC_STATUS_SCO_OFFSET_REJECTED               0x1B
#define HC_STATUS_SCO_INTERVAL_REJECTED             0x1C
#define HC_STATUS_SCO_AIR_MODE_REJECTED             0x1D
#define HC_STATUS_INVALID_LMP_PARM                  0x1E
#define HC_STATUS_UNSPECIFIED_ERROR                 0x1F
#define HC_STATUS_UNSUPP_LMP_PARM                   0x20
#define HC_STATUS_ROLE_CHANGE_DISALLOWED            0x21
#define HC_STATUS_LMP_RESPONSE_TIMEDOUT             0x22
#define HC_STATUS_LMP_ERR_TRANSACT_COLL             0x23
#define HC_STATUS_LMP_PDU_DISALLOWED                0x24
#define HC_STATUS_ENCRYPTN_MODE_UNACCEPT            0x25
#define HC_STATUS_UNIT_KEY_USED                     0x26
#define HC_STATUS_QOS_NOT_SUPPORTED                 0x27
#define HC_STATUS_INSTANT_PASSED                    0x28
#define HC_STATUS_PAIRING_W_UNIT_KEY_UNSUPP         0x29
#define HC_STATUS_DIFFERENT_TRANSACTION_COLLISION   0x2A
#define HC_STATUS_INSUFF_RESOURCES_FOR_SCATTER_MODE 0x2B
#define HC_STATUS_QOS_UNACCEPTABLE_PARAMETER        0x2C
#define HC_STATUS_QOS_REJECTED                      0x2D
#define HC_STATUS_CHANNEL_CLASSIF_NOT_SUPPORTED     0x2E
#define HC_STATUS_INSUFFICIENT_SECURITY             0x2F
#define HC_STATUS_PARAMETER_OUT_OF_MANDATORY_RANGE  0x30
#define HC_STATUS_SCATTER_MODE_NO_LONGER_REQUIRED   0x31
#define HC_STATUS_ROLE_SWITCH_PENDING               0x32
#define HC_STATUS_SCATTER_MODE_PARM_CHNG_PENDING    0x33
#define HC_STATUS_RESERVED_SLOT_VIOLATION           0x34
#define HC_STATUS_SWITCH_FAILED                     0x35
#define HC_STATUS_EXTENDED_INQ_RESP_TOO_LARGE       0x36
#define HC_STATUS_SECURE_SIMPLE_PAIR_NOT_SUPPORTED  0x37
#define HC_STATUS_HOST_BUSY_PAIRING                 0x38
#define HC_STATUS_CHANNEL_REJECT_NONE_SUITABLE      0x39 /* 3.0 */
#define HC_STATUS_CONTROLLER_BUSY                   0x3A /* 3.0 */
#define HC_STATUS_UNACCEPTABLE_CONTROLLER_INTERVAL  0x3B /* 4.0 */
#define HC_STATUS_DIRECTED_ADVERTISING_TIMEOUT      0x3C /* 4.0 */
#define HC_STATUS_CONNECTION_TERMINTATED_MIC_FAIL   0x3D /* 4.0 */
#define HC_STATUS_CONNECTION_FAILED_ESTABLISHMENT   0x3E /* 4.0 */
#define HC_STATUS_MAC_CONNECTION_FAILED             0x3F /* 4.0 */

/* End of HcStatus */


/*---------------------------------------------------------------------------
 * HciHandle type
 *
 * The HciHandle contains an 12-bit connection identifier as well as
 * two bits for packet boundary flags and two bits for broadcast flags.
 * These fields are combined together to exchange information about the
 * packet data exchanged between the Host Controller and L2CAP.
 *
 */
typedef U16 HciHandle;

/* Group: Broadcast Flags. When receiving a packet from the Host Controller,
 * one of these flags may be set. If no flags are set, the packet is
 * a normal, point-to-point transmission.
 */

/* The Host Controller sets this flag in a received packet's handle
 * to indicate it was received as an Active or Piconet Broadcast packet.
 *
 * L2CAP should not use this flag for transmitted packets, but use
 * ACTIVE_BC_HANDLE instead.
 */
#define ACTIVE_BROADCAST       0x4000

/* The Host Controller sets this flag in a received packet's handle
 * to indicate it was received as a Piconet Broadcast packet while
 * in "park" mode.
 *
 * L2CAP should not use this flag for transmitted packets, but use
 * PICONET_BC_HANDLE instead.
 */
#define PICONET_BROADCAST      0x8000

/* Represents an invalid Host Controller handle. */
#define INVALID_HCI_HANDLE      0xFFFF

/* Broadcast handle used by L2CAP to transmit to all active slaves
 * (not including parked slaves).
 */
#define ACTIVE_BC_HANDLE       (0x0F01 | ACTIVE_BROADCAST)

/* Broadcast handle used by L2CAP to transmit to all slaves, including
 * both parked and active slaves.
 */
#define PICONET_BC_HANDLE       (0x0F00 | PICONET_BROADCAST)

/* Group: Packet Boundary Flags. When receiving a packet from the Host
 * Controller, either of these two flags will be set. When sending a
 * packet to the Host Controller, the HCI layer fills in these
 * fields automatically.
 */
#define PBF_MASK               0x3000 /* Mask for operations on the
                                       * Packet Boundry Flags (PBF).
                                       */
#define FIRST_NON_FLUSH_PKT    0x0000 /* This packet fragment is the
                                       * non-flushable first fragment
                                       * of an L2CAP packet.
                                       */
#define CONTINUING_PACKET      0x1000 /* This packet fragment continues
                                       * a previously started L2CAP packet.
                                       */
#define FIRST_FLUSHABLE_PKT    0x2000 /* This packet fragment is the
                                       * flushable first fragment of
                                       * an L2CAP TX packet.
                                       */
#define FIRST_PACKET           0x2000 /* This packet fragment is the
                                       * flushable first fragment of
                                       * an L2CAP RX packet.
                                       */
#define COMPLETE_FLUSHABLE_PKT 0x3000 /* A complete, flushable packet.
                                       * AMP Controller use only.
                                       */

/* Group: When Erroneous Data Reporting enabled, a buffer received from the
 * Host Controller will set these bits as described below.
 */
#define SCO_DATA_CORRECT       0x0000 /* This SCO packet is valid */

#define SCO_DATA_INVALID       0x1000 /* This SCO packet is invalid */

#define SCO_DATA_NONE          0x2000 /* No data was received for this
                                       * SCO packet
                                       */

#define SCO_DATA_PARTIAL       0x3000 /* Partial data was received for this
                                       * SCO packet
                                       */

/* End of HciHandle */


/*---------------------------------------------------------------------------
 * HciPrescanFlags type
 *
 *     Describes the order in which prescan structures will be registered.
 */
typedef U8 HciPrescanFlags;

#define PRESCAN_FLAG_LAST      0x00
#define PRESCAN_FLAG_FIRST     0x01

/* End of HciPrescanFlags */


/* Forward reference for HciPacket */
typedef struct _HciPacket HciPacket;

/* Forward reference for HciBuffer */
typedef struct _HciBuffer HciBuffer;

/* Forward reference for HciBufferBlock */
typedef struct _HciBufferBlock HciBufferBlock;

/*---------------------------------------------------------------------------
 * HciBufferFreeFunc type
 *
 *     Function used to free an HciBuffer
 */
typedef void (*HciBufferFreeFunc)(HciBuffer *hciBuff);
/* End of HciBufferFreeFunc */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HciBufferBlock structure
 *
 *     Represents a block of HciBuffer structures that are reserved by an upper
 *     layer such as L2CAP.  
 */
struct _HciBufferBlock
{
    ListEntry  node;  /* Node element for link list management */
    U16        size;  /* Number of HciBuffers in the block */
};

/*---------------------------------------------------------------------------
 * HciBuffer structure
 *
 *     Represents a buffer of data received from the hardware by the
 *     transport. Except for "node", all of the fields in this structure
 *     must be initialized before providing it to the HCI layer with
 *     HCI_BufferInd.
 */
struct _HciBuffer
{
    ListEntry     node;     /* Node element for linked list management. */
    U16           len;      /* Length of data in "buffer". */
    U8           *buffer;   /* Pointer to received data buffer. */
    U8            flags;    /* Flags to identify the type of packet.
                             * Actually uses the HciBufFlags type.
                             */
    /* The following two fields are set by the allocator (either HciBufferPool
     * or rxbuff) and should not be modified
     */

    void         *hbpool;   /* Pointer to the buffer pool used by the controller
                             * Set to 0 for HciBuffers from rxbuff.c
                             */
    /* Function used to free the HciBuffer. This function calls the equivalent
     * of OS_StopHardware before freeing the buffer and OS_ResumeHardware
     * before returning so the caller doesn't have to
     */
    HciBufferFreeFunc freeBuff;

    BtPriorityToken  *priToken;

#if XA_STATISTICS == XA_ENABLED
    U32           hci_timer;
#endif /* XA_STATISTICS == XA_ENABLED */
};

/*---------------------------------------------------------------------------
 * HciCallbackParms structure
 *
 *     To indicate different kinds of events to the Management Entity,
 *     the HCI layer calls the ME_Callback function with an HciCallbackEvent
 *     and a callback parameters structure (this one).
 *
 *     Different fields in the "ptr" union are valid depending on the
 *     event type. See the definition for HciCallbackEvent for more
 *     information.
 */
typedef struct _HciCallbackParms {
    BtStatus  status;       /* May indicate the status of an operation */
    HciHandle hciHandle;    /* May indicate the HCI handle concerned with the
                             * event.
                             */
    BtPriorityToken *priToken;

    /* Group: Only one of the following fields may be valid. The correct
     * field to use will depend on the HciCallbackEvent received by
     * the ME_Callback.
     */
    union {
        HciEvent   *hciEvent;   /* Host controller event      */
        HciBuffer  *rxBuff;     /* Received packet  */
        BtPacket   *packet;     /* Returned transmit packet   */
        HciCommand *cmd;        /* Host controller command    */
        HciTransError error;    /* Error that caused transport failure */
    } ptr;
} HciCallbackParms;

/*---------------------------------------------------------------------------
 * HciFowParms structure
 *
 *     A structure used to hold the current HCI flow parameters.  This
 *     structure contains the maximum size of ACL and SCO transmit packets,
 *     as well as the current number of available buffers on the Host
 *     Controller.
 */
typedef struct _HciFlowParms {
    U16  aclTxPacketLen;
    U16  aclTxPacketsLeft;
    U8   scoTxPacketLen;
    U16  scoTxPacketsLeft;
    U16  aclMaxTxPackets;
    U16  leTxPacketLen;
    U16  leTxPacketsLeft;
    U16  leMaxTxPackets;
} HciFlowParms;

/* ME callback function */
void ME_Callback(U8 event, HciCallbackParms *parms);

/*---------------------------------------------------------------------------
 * RadioHandlerCallback type
 *
 *     Called by the HCI driver to allow radio initialization.
 */
typedef void (*RadioHandlerCallback)(U8 event, HciCallbackParms *parms);

/* End of RadioHandlerCallback */

/****************************************************************************
 *
 * Function Reference - Upper layer functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HCI_Init()
 *
 *     Initialize the HCI queues, receive buffers, and broadcast handles.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - HCI is initialized.
 *
 *     BT_STATUS_FAILED - hardware failed to initialize.
 */
BtStatus HCI_Init(void);

/*---------------------------------------------------------------------------
 * HCI_Deinit()
 *
 *     Deinitialize the HCI.  Return all resources to the upper layers.
 */
void HCI_Deinit(void);

/*---------------------------------------------------------------------------
 * HCI_Reset()
 *
 *     Sends the HCC_RESET command only. Called by Radio_Init in the module
 *     initialization code to give flexibility on how and when it is called
 *     during the radio initialization.
 *
 * Returns:
 *     BT_STATUS_PENDING - HCI is initializing the radio to a known state.
 *       HCI is initializing the radio to a known state. The status will
 *       indicate successful or unsucessfull attempt.
 *
 */
BtStatus HCI_Reset(void);

/*---------------------------------------------------------------------------
 * HCI_Config()
 *
 *    Initiates the sequence of commands for negotiating HCI Flow Control
 *    including the following:
 *      HCC_READ_BUFFER_SIZE,
 *      HCC_SET_CTRLR_TO_HOST_FLOW_CTRL,
 *      HCC_HOST_BUFFER_SIZE,
 *      HCC_WRITE_SCO_FC_ENABLE
 *    When the process is completed, callbacks of one of these commands will
 *    indicate to the ME that the HCI is initialized vi HCI_RadioInitialized.
 *
 * Returns:
 *     BT_STATUS_PENDING - HCI is now initializing the Host Controller.
 *         When initialization is complete, the Management Entity will be
 *         called with an HCI_INIT_STATUS event. The "status" field will
 *         indicate whether the initialization was successful.
 */
BtStatus HCI_Config(void);

/*---------------------------------------------------------------------------
 * HCI_RegisterRadioHandler()
 *
 *     Called by the Radio Manager to register a radio handler.  Radio
 *     handlers are modules that send vendor specific commands and handle
 *     events required to properly initialize a radio.  Only one handler is
 *     allowed to be registered at a time (since only one radio is supported
 *     at a time).  A handler must be registered before the HCI is initialized.
 *
 *     Once the HCI is properly initialized, the handler will be called with
 *     an HCI_INIT_STATUS event.  At this time the handler can begin to send
 *     any vendor specific events that it requires.
 *
 * Parameters:
 *     radioHandlerCallback - Radio handler callback entry point.  This parameter
 *         should contain a pointer to the radio handler's callback function.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The radio handler callback was successfully
 *         registered with HCI.
 *
 *     BT_STATUS_FAILED - The radio handler was not registered, because the HCI
 *         has already been initialized.
 */
void HCI_RegisterRadioHandler(RadioHandlerCallback radioHandlerCallback);

/*---------------------------------------------------------------------------
 * HCI_DeregisterRadioHandler()
 *
 *     Called by the transport driver to deregister a radio handler.  Radio
 *     handlers are modules that send vendor specific commands and handle
 *     events required to properly initialize a radio.  Once this function is
 *     called, the HCI will no longer call the radio handler, but direct
 *     events to the ME.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     void
 */
void HCI_DeregisterRadioHandler(void);
#define HCI_DeregisterRadioHandler() HCI_RegisterRadioHandler(0)

/*---------------------------------------------------------------------------
 * HCI_RadioInitialized()
 *
 *     Called by the radio manager to indicates to the HCI that radio
 *     initialization is complete.
 *
 * Parameters:
 *     Status - Tells the status of initialization.  If successful, then
 *         the HCI is operational.  If not successful, then initialization
 *         failed.
 *
 * Returns:
 *     void
 */
void HCI_RadioInitialized(BtStatus Status);


/*---------------------------------------------------------------------------
 * HCI_RadioDeinitializing()
 *
 *     Called by the radio manager to indicate the current status of radio
 *     deinitialization.
 *
 * Parameters:
 *     status - BT_STATUS_PENDING to indicate that deinit has
 *         begun, or BT_STATUS_SUCCESS to indicate that deinit is complete.
 *
 */
void HCI_RadioDeinitializing(BtStatus status);

/*---------------------------------------------------------------------------
 * HCI_RadioDeinitialized()
 *
 *     Synonym for HCI_RadioDeinitializing(BT_STATUS_SUCCESS).
 *
 */
void HCI_RadioDeinitialized(void);
#define HCI_RadioDeinitialized() HCI_RadioDeinitializing(BT_STATUS_SUCCESS)

/* Used by the following API */
BtStatus HciSendCommand(U16 opCode, U8 parmLen, HciCommand *cmd, BOOL internal);

/*---------------------------------------------------------------------------
 * HCI_SendCommand()
 *
 *     Starts sending a command to the host controller. After the command
 *     is completely sent to the host controller, HCI calls ME_Callback
 *     with the HCI_COMMAND_HANDLED callback event type. The callback
 *     parameters contain the command that was sent (in "ptr.cmd") and
 *     the success of the operation (in "status").
 *
 *     In response to the command, the host controller will send one
 *     or more events. These events are also passed to the ME_Callback
 *     function as described by the HciCallbackType HCI_CONTROLLER_EVENT
 *     type.
 *
 *     Before calling this function, the command's "parms" field must
 *     be filled with the appropriate command-specific parameters as
 *     defined in the Bluetooth specification. Filling the "header"
 *     field is not required.
 *
 * Parameters:
 *    opCode - The type of HCI command to send.
 *
 *    parmLen - The length of the command parameters.  The command
 *        parameter block is located in the HciCommand structure.
 *
 *    cmd - the command structure. The "cmd->parm" field must be filled
 *        with command-specific parameters.
 *
 * Returns:
 *    BT_STATUS_PENDING - the command was accepted by the HCI. The
 *        upper layer will be notified at a later time of the command
 *        completion and/or status.
 *
 *    BT_STATUS_INVALID_PARM - the "cmd" parameter is not valid
 *        (XA_ERROR_CHECK only).
 */
BtStatus HCI_SendCommand(HciCommandType opCode, U8 parmLen, HciCommand *cmd);
#define HCI_SendCommand(o, p, c) HciSendCommand(o, p, c, FALSE)

/*---------------------------------------------------------------------------
 * HCI_IsCommandBusy()
 *
 *     Checks if HciCommand parameter is still in use.
 *
 * Parameters:
 *    cmd - the command structure to check.
 *
 * Returns:
 *    TRUE - the "cmd" parameter is still in use.
 *
 *    FALSE - the "cmd" parameter is free.
 */
BOOL HCI_IsCommandBusy(HciCommand *cmd);

/****************************************************************************
 *
 * Function Reference - Internal functions.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HCI_RequestToSend()
 *
 *     Called to notify HCI that the Management Entity has SCO or ACL
 *     data ready to send.  Must only be called when the stack is already locked.
 *
 *     Later, when HCI is running on the stack thread and is able to
 *     send, it will call ME_Callback with the HCI_SEND_IND event. The
 *     callback parameters will contain the requested HciHandle to
 *     indicate which connection is ready for data.
 *
 *     During the callback, the "packet" field of the callback
 *     parameters structure is filled with a BtPacket to be sent,
 *     or filled with NULL to indicate that no more packets to send
 *     on that connection. The HCI layer will continue to generate
 *     HCI_SEND_IND events until the "packet" field is returned
 *     as NULL.
 *
 *     When a packet has been completely sent to the Host Controller,
 *     the HCI calls ME again with an HCI_PACKET_HANDLED event.
 *     The "packet" callback parameters field contains a pointer to
 *     the packet that was transmitted.  The "status" field indicates
 *     whether the packet was sent successfully.
 *
 *     Requests to send on multiple simultaneous connections are
 *     allowed.
 *
 * Parameters:
 *     handle - Data is ready to be transmitted for this HCI connection
 *         handle.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The request was successful
 *
 *     BT_STATUS_FAILED - The request failed because the connection
 *         associated with handle could not be found.
 */
BtStatus HCI_RequestToSend(HciHandle handle, BtPriorityToken *PriToken);

/*---------------------------------------------------------------------------
 * HCI_RequestToReceive()
 *
 *     Allows upper layers to notify HCI that a connection is up and
 *     listening for receive data.  When the HCI receives data from the
 *     transport, the work will get scheduled according to the priority
 *     specified by PriToken.
 *
 * Parameters:
 *     handle - Data is ready to be transmitted for this HCI connection
 *         handle.
 *
 * Returns:
 *     BT_STATUS_FAILED - The connection handle is unknown.
 *
 *     BT_STATUS_SUCESS - The request was recognized.
 */
BtStatus HCI_RequestToReceive(HciHandle hciHandle, BtPriorityToken *PriToken);

/*---------------------------------------------------------------------------
 * HCI_GetFlowParms()
 *
 *     Called to retrieve the negotiated flow control parameters of the
 *     HCI transport.  This information may be useful for upper layers that
 *     wish to optimize the transmistion of data in BtPackets.  It is especially
 *     useful for the transmission of SCO/eSCO data, which should be sent in
 *     packets equal to the size advertized by the radio.
 *
 * Parameters:
 *     Parms - A pointer to a memory structure that will receive the flow
 *         control information.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The request was successful
 *
 *     BT_STATUS_FAILED - The request failed because the parameters have
 *         not yet been negotiated.
 */
BtStatus HCI_GetFlowParms(HciFlowParms *Parms);

/*---------------------------------------------------------------------------
 * HCI_SetScoPacketSize()
 *
 *     Called to change the negotiated SCO packet size.  The ability
 *     to change the SCO packet size is important when BT_SCO_HCI_DATA
 *     is enabled and the transport is USB.  USB has very strict
 *     requirements for the packet size, and often radios do not
 *     advertise the appropriate size.  In addition, the size should
 *     be changed if multiple SCO channels are used.  This function should
 *     only be called if the HCI has completed initialization.
 *
 *     Example USB packet sizes:
 *
 *     16 bit audio:  Each SCO channel requires 48 byte packets.
 *     8  bit audio:  Each SCO channel requires 24 byte packets.
 *
 * Parameters:
 *     scoPacketLen - The new SCO packet size.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The request was successful
 *
 *     BT_STATUS_FAILED - The request failed because the parameters have
 *                        not yet been negotiated.
 */
BtStatus HCI_SetScoPacketSize(U8 scoPacketLen);

/*---------------------------------------------------------------------------
 *  HCI_EnableCompletedReturn()
 *
 *     Allows upper layers to enable/disable the wait for completed
 *     return before indicating a HCI_PACKET_HANDLED event.
 *
 * Parameters:
 *     hciHandle - The HCI handle to enable/disable.
 *
 *     Enable - TRUE or FALSE.
 *
 * Returns:
 *     BT_STATUS_FAILED - The connection handle is unknown.
 *
 *     BT_STATUS_SUCESS - The setting was saved.
 */
BtStatus HCI_EnableCompletedReturn(HciHandle hciHandle, BOOL Enable);

/*---------------------------------------------------------------------------
 *  HCI_CommandFlowInc()
 * 
 *      Increments the internal HCI maxCommands flow control value to
 *      allow command flow to continue.
 *  
 *  Parameters:
 *      none
 * 
 *  Returns:
 *      void
 */
void HCI_CommandFlowInc(void);

/****************************************************************************
 *
 * Internal function prototypes
 *
 ****************************************************************************/
#if XA_DECODER == XA_ENABLED
void HciInitTypes(void);
#endif /* XA_DECODER == XA_ENABLED */
#if NUM_SCO_CONNS != 0
#include "hcitrans.h"
void HciSetupScoChannelTypes(HciNumScoCons *scoCons, U16 discHciHandle);
#endif /* NUM_SCO_CONNS != 0 */

#endif /* __HCI_H */

