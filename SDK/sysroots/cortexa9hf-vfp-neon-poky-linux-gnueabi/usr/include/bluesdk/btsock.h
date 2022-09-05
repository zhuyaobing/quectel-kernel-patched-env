/****************************************************************************
 * File:
 *     btsock.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 * 
 * Copyright 2009-2017 OpenSynergy GmbH.
 * All rights reserved.  All unpublished rights reserved.
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
 ****************************************************************************/

#ifndef BTSOCK_H_
#define BTSOCK_H_

#include "bttypes.h"
#include "rfcomm.h"

/*---------------------------------------------------------------------------
 * Bluetooth Socket API layer
 *
 *     Allows standardized client and server data exchange according to the specifics
 *     of an underlying transport type. 
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BTSOCK_MAX_NAME constant
 *
 *     The maximum length of a discovered service name
 */
#ifndef BTSOCK_MAX_NAME
#define BTSOCK_MAX_NAME             48
#endif

/*---------------------------------------------------------------------------
 * BTSOCK_MAX_REMOTE_SERVICES constant
 *
 *     The maximum number of remote services which can be enumerated
 *     during service searches.
 */
#ifndef BTSOCK_MAX_REMOTE_SERVICES
#define BTSOCK_MAX_REMOTE_SERVICES 12
#endif


/*---------------------------------------------------------------------------
 * BTSOCK_LEL2CAP_MPS constant
 *
 * MPS size used for LEL2CAP connections. Corresponds to the LEL2CAP
 * maxPdu parameter.
 * This and 
 *     
 */
#ifndef BTSOCK_LEL2CAP_MPS
#define BTSOCK_LEL2CAP_MPS LEL2CAP_MPS
#endif


/*---------------------------------------------------------------------------
 * BTSOCK_LEL2CAP_MTU constant
 *
 * Maximum PDU size used for LEL2CAP connections. Corresponds to the LEL2CAP
 * maxSdu parameter. This parameter is used for calculating the receive buffer size.
 *
 */
#ifndef BTSOCK_LEL2CAP_MTU
#define BTSOCK_LEL2CAP_MTU LEL2CAP_MTU
#endif


/*---------------------------------------------------------------------------
 * BTSOCK_LEGATT_ENABLED
 *
 * Is support for serial port over GATT (LE) enabled.
 *
 */
#ifndef BTSOCK_LEGATT_ENABLED
#define BTSOCK_LEGATT_ENABLED XA_DISABLED
#endif

#if NUM_LE_DEVICES == 0
#undef BTSOCK_LEGATT_ENABLED
#define BTSOCK_LEGATT_ENABLED XA_DISABLED
#endif

 /*---------------------------------------------------------------------------
 * BTSOCK_LEL2CAP_ENABLED
 *
 * Is support for serial port over LE L2CAP enabled.
 *
 */
#ifndef BTSOCK_LEL2CAP_ENABLED
#define BTSOCK_LEL2CAP_ENABLED XA_DISABLED
#endif

#if NUM_LE_DEVICES == 0
#undef BTSOCK_LEL2CAP_ENABLED
#define BTSOCK_LEL2CAP_ENABLED XA_DISABLED
#endif

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * BtSockType type
 *
 *     Indicates the types of transports permitted for the socket.
 *     Types may be OR'ed together to require support for multiple types
 *     of transports.
 */
typedef U8 BtSockType;

/* Use RFCOMM as a transport.
 *
 * For a listening socket, either BTSOCK_SetChannel or BTSOCK_Set_Service_Uuid
 * (or both) must be called before calling BTSOCK_Listen. If CHANNEL is not set,
 * one will be allocated automatically. If UUID is set, a corresponding SDP
 * record will be registered. If BTSOCK_Set_Service_Name has also been called, that name
 * will be exposed instead of "Serial Port".
 */
#define BTSOCK_RFCOMM     0x01

/* Use L2CAP as a transport.
 *
 * For a listening socket, BTSOCK_Set_Service_Uuuid must be called first. At
 * present, BTSOCK_Set_Psm may be called but is ignored by BTSOCK_Listen,
 * and a dynamically allocated PSM value is used instead.
 */
#define BTSOCK_L2CAP      0x02

/* Use L2CAP over LE as a transport.
 * 
 */
#define BTSOCK_LEL2CAP    0x04

/* Use GATT over LE as a transport.
 * NOTE requires the use of the BTSOCK_LEGATT_ENABLED option.
  */
#define BTSOCK_LEGATT     0x08

/* End of BtSockType */


/*---------------------------------------------------------------------------
 * BtSockEventType type
 *
 *     Event types delivered to the BtSockCallback.
 */
typedef U8 BtSockEventType;

/* Data is now available to be read with BTSOCK_Read(). This event only
 * arrives after a caller has previously read all available data from the
 * socket.
 */
#define BTSOCK_EVT_READ                 1

/* Buffer space is available for data to be written successfully using
 * BTSOCK_Write(). This event only arrives after a caller has attempted
 * to call BTSOCK_Write() and failed to deliver all of its data to a
 * socket (perhaps due to a socket buffer overflow condition).
 */
#define BTSOCK_EVT_WRITE                2

/* A new server socket is waiting to be accepted using BTSOCK_Accept().
 */
#define BTSOCK_EVT_ACCEPT               3

/* A client socket has transitioned to an open state as a result of
 * BTSOCK_Connect() or BTSOCK_Accept()
 */
#define BTSOCK_EVT_CONNECTED            4

/* A client or server socket has become disconnected and the handle
 * should be discarded. "error" will contain an error code or 0 if not
 * known.
 */
#define BTSOCK_EVT_CLOSED               5

/* A list of service names has been retrieved during the connect process.
 * The names can be retrieved by calling BTSOCK_GetAvailableServices(). This 
 * event is only delivered when BTSOCK_SetSelectService() has been enabled.
 */
#define BTSOCK_EVT_SERVICES_FOUND       6

/* A RFEVENT_MODEM_STATUS_IND has been received from RFCOMM. The new modem
 * status can now be read with BTSOCK_GetModemStatus().
 */
#define BTSOCK_EVT_MODEM_STATUS         7

/* A RFEVENT_LINE_STATUS_IND has been received from RFCOMM. The new line
 *   status can now be read with BTSOCK_GetLineStatus().
 */
#define BTSOCK_EVT_LINE_STATUS          8

/* An RFEVENT_PORT_NEG_IND or RFEVENT_PORT_NEG_CNF has been received from RFCOMM.
 * The new port settings can now be read with BTSOCK_GetPortSettings().
 */
#define BTSOCK_EVT_PORT_NEGOTIATION     9

/* End of BtSockEventType */

/* Forward reference to BtSockEvent */
typedef struct _BtSockEvent BtSockEvent;

/*---------------------------------------------------------------------------
 * BtSockCallback type
 *
 *     Callback type to receive socket-related events
 */
typedef void (*BtSockCallback)(const BtSockEvent *evt);

/* End of BtSockCallback */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtSockEvent structure
 *
 *     A socket-related event.
 */
struct _BtSockEvent {

    /* Event type */
    BtSockEventType     type;

    /* Handle of interest */
    U16                 handle;

    /* Status code associated with the event */
    BtStatus            status;
};

/*---------------------------------------------------------------------------
 * BtSockRfServiceDesc structure
 *
 *     Describes an RFCOMM service found on a remote device.
 */
typedef struct _BtSockRfServiceDesc {
    /* NULL-terminated name of the service. May be zero-length if service
     * name not given.
     */
    char    name[BTSOCK_MAX_NAME];

    /* RFCOMM ID of this service */
    U16     serviceId;
} BtSockRfServiceDesc;

/*---------------------------------------------------------------------------
 * BtSockHooks structure
 *
 *     Defines global hooks that may be specified to handle events of
 *     interest. All unused hooks should be set to 0.
 */
typedef struct _BtSockHooks {

    /* Called when an l2cap or lel2cap channel is dropped */
    void (*l2capClose)(U16 handle, void *serviceId);

    /* Called when an l2cap or lel2cap channel is opened */
    void (*l2capOpen)(U16 handle, void *serviceId);

    /* Called when l2cap or lel2cap data is received from a remote device */
    void (*l2capReceive)(U16 handle, void *serviceId, const void *data, U32 dataLen);

    /* Called when l2cap or lel2cap data is to be sent to a remote device */
    void (*l2capSend)(U16 handle, void *serviceId, const void *data, U32 dataLen);

} BtSockHooks;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BTSOCK_Init()
 *
 *     Initializes the BTSOCK API. Must be called once before any other
 *     BTSOCK_ calls.
 *
 * Returns:
 *    TRUE - when successful. Currently, TRUE is always returned.
 */
BOOL BTSOCK_Init(void);

/*---------------------------------------------------------------------------
 * BTSOCK_Deinit()
 *
 *     Deinitializes the BTSOCK API. Frees up resources allocated by BTSOCK. 
 */
void BTSOCK_Deinit(void);

/*---------------------------------------------------------------------------
 * BTSOCK_SetCallback()
 *
 *     Sets the callback for socket events. Any previously supplied callback is
 *     replaced. May be set to 0 for no events. See the documentation of type
 *     BtSockCallback for a description of the callbacks.
 * 
 * Parameters: 
 *     cb - A structure containing pointers to callback functions. May be 0.
 */
void BTSOCK_SetCallback(BtSockCallback cb);

/*---------------------------------------------------------------------------
 * BTSOCK_Create()
 *
 *     Create a socket of the specified type.
 *
 * Parameters:
 *     type - Type of socket to create 
 *
 * Returns: 
 *     > 0 - a valid socket ID
 *
 *     0 - unable to allocate a new socket
 */
U16 BTSOCK_Create(BtSockType type);

/*---------------------------------------------------------------------------
 * BTSOCK_Connect()
 *
 *     Creates a connection. When the connection attempt has completed, delivers
 *     BTSOCK_EVT_CONNECTED or BTSOCK_EVT_CLOSED.
 *
 *     For an RFCOMM socket, Either uuid/uuidLen or channel must be specified. 
 *     If uuid is present, channel is ignored. For an L2CAP or LEL2CAP socket, psm must be 
 *     specified.
 *
 * Parameters:
 *     socketId - The socket for the connection
 *
 *     targetAddr - Address of the remote device to connect to
 *
 *     uuid - For RFCOMM socket: UUID of the service to connect to. Must be either a 16- or 128-bit
 *            UUID and supplied in big-endian format.
 *
 *     uuidLen - length of the uuid. 
 *
 *     channelNum - RFCOMM channel on the remote device to connect to
 *
 *     psm - The PSM value to use in case of a L2CAP/LEL2CPA socket
 *
 * Returns: 
 *     BT_STATUS_PENDING - Successfully started the create connection process.
 *
 *     BT_STATUS_FAILED - Failed to start create connection process.
 *
 *     BT_STATUS_NOT_FOUND - Socket does not exist.
 */
BtStatus BTSOCK_Connect(U16 socketId, BtDevAddr *targetAddr, const void *uuid,
                        U8 uuidLen, U8 channelNum, U16 psm);

/*---------------------------------------------------------------------------
 * BTSOCK_Close()
 *
 *     Closes the specified socket. After this call, the socket ID is considered
 *     invalid for further calls. However, when the socket is completely closed,
 *     BTSOCK_EVT_CLOSED will be delivered.
 *
 * Parameters:
 *     socketId - ID of the socket to be closed
 */
void BTSOCK_Close(U16 socketId);

/*---------------------------------------------------------------------------
 * BTSOCK_Listen()
 *
 *     Binds the socket to an underlying channel and starts listening for incoming
 *     connections. Incoming connections are notified with BTSOCK_EVT_ACCEPT and
 *     retrieved using BTSOCK_Accept.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Operation failed to complete
 *
 *     BT_STATUS_NOT_FOUND - Socket does not exist.
 */
BtStatus BTSOCK_Listen(U16 socketId);

/*---------------------------------------------------------------------------
 * BTSOCK_Accept()
 *
 *     Accepts or rejects a new incoming connection from the specified listening
 *     socketId.
 *
 * Parameters:
 *     socketID - The ID of the socket
 *
 *     accept - Set to FALSE to reject the incoming connection. Set to TRUE
 *              to accept the connection. The socket is then immediately
 *              considered open.
 *
 * Returns: 
 *
 *     > 0 - New socket id that can be used for data transmission on the
 *           new connection
 *
 *     0 - Socket was rejected or error occurred
 */
U16 BTSOCK_Accept(U16 socketId, BOOL accept);

/*---------------------------------------------------------------------------
 * BTSOCK_Read()
 *
 *     Reads at most "len" bytes of data into the supplied buffer.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 *     data - pointer to the buffer
 *
 *     len - size of the buffer
 *
 * Returns:
 *     > 0 - Number of bytes read
 *
 *     0 - Error occurred or no bytes are available for reading at the moment
 */
U32 BTSOCK_Read(U16 socketId, void *data, U32 len);

/*---------------------------------------------------------------------------
 * BTSOCK_ReadAvailable()
 *
 *     Returns the number of bytes available to be read or < 0 if this
 *     is an invalid request.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 * Returns:
 *     > 0 - Number of bytes available to read
 *
 *     0 - Invalid request or no bytes available to read
 */
U32 BTSOCK_ReadAvailable(U16 socketId);

/*---------------------------------------------------------------------------
 * BTSOCK_Write()
 *
 *     Delivers data to the socket to be sent.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 *     data - pointer to data to be sent
 *
 *     len -  length of the data to be sent
 *
 * Returns:
 *     > 0 - Number of bytes accepted for writing
 *
 *     0 - Error occurred or no bytes can be written at this time
 */
U32 BTSOCK_Write(U16 socketId, const void *data, U32 len);

/*---------------------------------------------------------------------------
 * BTSOCK_SetHooks()
 * 
 *     Allows hooks to be set that are called when events of interest occur.
 *     May be set to 0 to disable all hooks. See the documentation of BtSockHooks
 *     for more details.
 *
 * Parameters:
 *     hooks - pointer to a BtSockHooks structure. Can be set to 0.
 */
void BTSOCK_SetHooks(BtSockHooks *hooks);

/*---------------------------------------------------------------------------
 * BTSOCK_GetModemStatus()
 *
 *    Returns the last known remote modem status for a given socket. 
 *    Especially useful to call after having received a 
 *    BTSOCK_EVT_MODEM_STATUS event.
 *
 * Parameters:
 *     socketId - ID of the socket 
 *
 * Returns:
 *     Pointer to a RfModemStatus structure if successful, NULL in case of 
 *     failure: Socket does not exist or is in inconsistent state.
 *      
 */
RfModemStatus* BTSOCK_GetModemStatus(U16 socketId);

/*---------------------------------------------------------------------------
 * BTSOCK_SetModemStatus()
 *
 *     Sends or queues the local modem status to be sent over RFCOMM.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 *     modemStatus - pointer to a RfModemStatus structure. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Socket is in an inconsistent state.
 *
 *     BT_STATUS_NOT_FOUND - Socket does not exist.
 */
BtStatus BTSOCK_SetModemStatus(U16 socketId, RfModemStatus *modemStatus);

/*---------------------------------------------------------------------------
 * BTSOCK_GetLineStatus()
 *
 *     Returns the last known remote line status for a given socket. 
 *     Especially useful to call after having received a 
 *     BTSOCK_EVT_LINE_STATUS event.
 *
 * Parameters:
 *     socketId - ID of the socket 
 *
 * Returns:
 *     RfLineStatus describing the last known line states, or 0 if en error occurred:
 *     Socket does not exist or is in inconsistent state. 
 */
RfLineStatus BTSOCK_GetLineStatus(U16 socketId);

/*---------------------------------------------------------------------------
 * BTSOCK_SetLineStatus()
 *
 *     Sends or queues the local line status to be sent over RFCOMM.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 *     lineStatus - line status to be sent
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Socket is in an inconsistent state.
 *
 *     BT_STATUS_NOT_FOUND - Socket does not exist.
 */
BtStatus BTSOCK_SetLineStatus(U16 socketId, RfLineStatus lineStatus);

/*---------------------------------------------------------------------------
 * BTSOCK_SendPortNegotiation()
 *
 *     Send Remote Port Negotiation to remote endpoint. Uses the local port 
 *     settings previously set by BTSOCK_SetLocalPortSettings().
 *     The remote end may sent a Remote Port Negotiation (RPN) request at any 
 *     time. When this happens, the BTSOCK user is notified with a 
 *     BTSOCK_EVT_PORT_NEGOTIATION event and the updated port settings are
 *     readable with BTSOCK_GetPortSettings().
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Socket is in an inconsistent state.
 *
 *     BT_STATUS_NOT_FOUND - Socket does not exist.     
 */
BtStatus BTSOCK_SendPortNegotiation(U16 socketId);

/*---------------------------------------------------------------------------
 * BTSOCK_GetPortSettings()
 *
 *     Returns the port settings for a given socket. These are usually what 
 *     have been set locally with BTSOCK_SetPortSettings(), but in case a 
 *     Remote Port Negotiation request has been received from the remote 
 *     endpoint, then they are the values received from the remote endpoint.
 * 
 * Parameters:
 *     socketId - ID of the socket
 *
 * Returns:
 *     Pointer to RfPortSettings, or NULL if error occurred: socket does not 
 *     exist or is in inconsistent state
 */
RfPortSettings *BTSOCK_GetPortSettings(U16 socketId);

/*---------------------------------------------------------------------------
 * BTSOCK_SetPortSettings()
 *
 *     Set the local port settings for a socket. Can be called at any time. 
 *     These settings will be used during Remote Port Negotiation.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 *     localPortSettings - Pointer to a RfPortSettings structure.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Socket is in an inconsistent state.
 */
BtStatus BTSOCK_SetPortSettings(U16 socketId, RfPortSettings *localPortSettings);

/*---------------------------------------------------------------------------
 * BTSOCK_SetConnectAddr()
 *
 *     Set the connect address. 
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 *     addr - Pointer to a BtDevAddr structure.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Socket is in an inconsistent state.
 */
BtStatus BTSOCK_SetConnectAddr(U16 socketId, BtDevAddr *addr);

/*---------------------------------------------------------------------------
 * BTSOCK_SetServiceId()
 *
 *     Set the Service ID used to identify a channel for security and
 *     other purposes. May be set on a socket before BTSOCK_Listen() is 
 *     called, which will apply the service ID to all sockets returned from 
 *     BTSOCK_Accept().
 *
 *     NOTE: If the service ID is not set, an internally unique value is used.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 *     serviceId - Service ID to set on a channel
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Socket is in an inconsistent state.
 */
BtStatus BTSOCK_SetServiceId(U16 socketId, BtServiceId serviceId);

/*---------------------------------------------------------------------------
 * BTSOCK_SetSelectService()
 *
 *     Set whether the user desires control over service selection.
 *
 *     The default behavior when searching for a remote service (when the
 *     channelNum is not known) is to auto-select the first service with a
 *     matching UUID.
 *
 *     When this option is set to a non-zero value, auto-selection does not occur.
 *     Instead, the user selects the service. After BTSOCK_Connect is called,
 *     and matching services are identified, BtSock delivers
 *     BTSOCK_EVT_SERVICES_FOUND. The user then queries
 *     BTSOCK_GetAvailableServices() and selects a service by calling
 *     BTSOCK_SetChannel().
 *
 * Parameters:
 *     socketID - ID of the socket
 *
 *     select - Set to TRUE if you want to select the service, FALSE if 
 *         you want auto-selection.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Socket is in an inconsistent state.
 */
BtStatus BTSOCK_SetSelectService(U16 socketId, BOOL select);

/*---------------------------------------------------------------------------
 * BTSOCK_SetChannel()
 *
 *     Set the RFCOMM channel for a socket.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 *     serviceID - Service ID identifying the RFCOMM channel.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Socket is in an inconsistent state.
 */
BtStatus BTSOCK_SetChannel(U16 socketId, U8 channel);

/*---------------------------------------------------------------------------
 * BTSOCK_SetServiceName()
 *
 *     Set the service name. It may be set before
 *     BTSOCK_Listen to specify the advertised service name, or set during
 *     the client connection process to select a remote service
 *     (see BTSOCK_SetSelectService()).
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 *     name - pointer to service name
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Socket is in an inconsistent state.
 */
BtStatus BTSOCK_SetServiceName(U16 socketId, const char *name);

/*---------------------------------------------------------------------------
 * BTSOCK_SetServiceUuid()
 *
 *     Set the Service UUID. It is a two, four, or 16-byte service UUID, in a 
 *     big-endian formatted buffer.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 *     uuid - pointer to the UUID buffer
 *
 *     len - length of the UUID
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Socket is in an inconsistent state.
 */
BtStatus BTSOCK_SetServiceUuid(U16 socketId, U8 *uuid, U8 len);

/*---------------------------------------------------------------------------
 * BTSOCK_SetPsm()
 *
 *     Set the PSM: An integer containing the selected L2CAP/LEL2CAP PSM 
 *     for this socket.
 *
 * Parameters:
 *     socketID - ID of the socket
 *
 *     psm - The PSM to be set on the socket
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Socket is in an inconsistent state.
 */
BtStatus BTSOCK_SetPsm(U16 socketId, U16 psm);

/*---------------------------------------------------------------------------
 * BTSOCK_SetRxBufferSize()
 *
 *     Set the RX buffer size which specifies the maximum amount of Rx 
 *     data to buffer on this socket.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 *     bufferSize - Rx buffer size
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Socket is in an inconsistent state.
 */
BtStatus BTSOCK_SetRxBufferSize(U16 socketId, U32 bufferSize);

/*---------------------------------------------------------------------------
 * BTSOCK_SetSecurityLevel()
 *
 *     Set the security level. 0 or 1 indicates no special security
 *     requirements; 2 indicates "Level 2" security (encryption required,
 *     MITM protection not required), 3 indicates "Level 3" security (MITM 
 *     protection required), 4 indicates "Level 4" security (MITM protection 
 *     required and a P-256 key). Higher values have undefined effects.
 *
 * Parameters: 
 *     socketId - ID of the socket
 *
 *     securityLevel - The security level to set on the socket
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Socket is in an inconsistent state.
 */
BtStatus BTSOCK_SetSecurityLevel(U16 socketId, U8 securityLevel);

/*---------------------------------------------------------------------------
 * BTSOCK_GetConnectAddr()
 * 
 *     Gets the connect address for a socket. The result is copied into a
 *     BtDevAddr structure provided by the caller.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 *     addr - pointer to a BtDevAddr structure
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Call was successful, address was retrieved
 *
 *     BT_STATUS_FAILED - Failure, socket did not exist         
 */
BtStatus BTSOCK_GetConnectAddr(U16 socketId, BtDevAddr *addr);

/*---------------------------------------------------------------------------
 * BTSOCK_GetServiceUuid()
 *
 *     Gets the Service UUID for a socket. The result is copied into the
 *     a buffer pointed to by *uuid and the length is stored into an integer
 *     pointed to by *len.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 *     uuid - pointer to memory to receive uuid
 *
 *     len - length of uuid buffer
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Call was successful, UUID was retrieved
 *
 *     BT_STATUS_FAILED - Failure, socket did not exist 
 */
BtStatus BTSOCK_GetServiceUuid(U16 socketId, U8 *uuid, U8 *len);

/*---------------------------------------------------------------------------
 * BTSOCK_GetChannel
 *
 *     Returns the RFCOMM Channel for a socket.
 *
 * Parameters: 
 *     socketId - ID of the socket
 *
 * Returns:
 *     > 0 - RFCOMM Channel number
 *
 *     0 - Error occurred: socket was not found
 */
U8 BTSOCK_GetChannel(U16 socketId);

/*---------------------------------------------------------------------------
 * BTSOCK_GetPsm()
 *
 *     Returns the L2CAP or LEL2CAP PSM for a socket.
 *
 * Parameters: 
 *     socketId - ID of the socket
 *
 * Returns:
 *     > 0 - L2CAP or LEL2CAP PSM value
 *
 *     0 - Error occurred: socket was not found
 */
U16 BTSOCK_GetPsm(U16 socketId);

/*---------------------------------------------------------------------------
 * BTSOCK_GetRxBufferSize()
 *
 *     Returns the RX buffer size for a socket.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 * Returns:
 *     > 0 - RX buffer size
 *
 *     0 - Error occurred: socket was not found
 */
U32 BTSOCK_GetRxBufferSize(U16 socketId);

/*---------------------------------------------------------------------------
 * BTSOCK_GetTxFrameSize()
 *
 *     Returns the TX frame size.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 * Returns:
 *     > 0 - TX frame size
 *
 *     0 - Error occurred: socket was not found
 */
U16 BTSOCK_GetTxFrameSize(U16 socketId);

/*---------------------------------------------------------------------------
 * BTSOCK_GetAvailableServices()
 *
 *     Gets the available remote services discovered during the SDP (Service 
 *     Discovery) process. They are copied into a buffer pointed to by 
 *     services. The value pointed to by len is used to indicate the size 
 *     of the provided buffer and gets updated to reflect the actual size of 
 *     the available services array. The socket must be a client socket.
 *
 * Parameters:
 *     socketId - ID of the socket
 *     services - Pointer to the buffer where the services discovered will be
 *                copied to.
 *     len - Pointer to the size of the provided buffer. The function call will
 *           update the value to indicate the actual size of the services copied.
 *
 *  Returns:
 *     BT_STATUS_SUCCESS - Operation was successful
 *
 *     BT_STATUS_FAILED - Operation failed to complete
 *
 *     BT_STATUS_NOT_FOUND - Socket does not exist     
 */
BtStatus BTSOCK_GetAvailableServices(U16 socketId, BtSockRfServiceDesc *services, 
                                     U32 *len);

/*---------------------------------------------------------------------------
 * BTSOCK_GetAvailableServicesLen()
 *
 *     Returns the size of the available remote services (which can be 
 *     obtained by calling BTSOCK_GetAvailableServices(). The socket must be 
 *     a client socket.
 *
 * Parameters:
 *     socketID - ID of the socket
 *
 * Returns:
 *     > 0 - Size of the available remote services
 * 
 *     0 - error occurred or no remote services exist
 */
U32 BTSOCK_GetAvailableServicesLen(U16 socketId);

/*---------------------------------------------------------------------------
 * BTSOCK_GetServiceId()
 *
 *     Returns the Service ID used to identify a channel for security and
 *     other purposes.
 *
 * Parameters:
 *     socketId - ID of the socket
 *
 * Returns:
 *     BtServiceId value.  This will be 0 if a socket error occurred.
 */
BtServiceId BTSOCK_GetServiceId(U16 socketId);

#endif /* BTSOCK_H_ */
