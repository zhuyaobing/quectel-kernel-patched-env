/****************************************************************************
 *
 * File:
 *     le_att_i.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     Public types, defines, and prototypes for LE Signaling.
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

#ifndef __LE_ATT_I_H
#define __LE_ATT_I_H

#include "bttypes2.h"
#include "l2cap.h"
#include "l2cap_a.h"
#include "gatt.h"
#include "eventmgr.h"
#include "sys/gattclienti.h"
#include "sys/gattserveri.h"
#include "sys/le_attop_i.h"

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

#define LEATT_DEFAULT_MTU               23

#define LEATT_MAX_COLLISIONS             4

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * LeAttOpCode type
 *
 *     Defines the LE Attrubute Protocol Operation Codes.
 */
typedef U8 LeAttOpCode;

#define LEATT_ERROR_RESPONSE            0x01
#define LEATT_EXCH_MTU_REQ              0x02
#define LEATT_EXCH_MTU_RSP              0x03
#define LEATT_FIND_INFO_REQ             0x04
#define LEATT_FIND_INFO_RSP             0x05
#define LEATT_FIND_BY_TYPE_REQ          0x06
#define LEATT_FIND_BY_TYPE_RSP          0x07
#define LEATT_READ_BY_TYPE_REQ          0x08
#define LEATT_READ_BY_TYPE_RSP          0x09
#define LEATT_READ_REQ                  0x0A
#define LEATT_READ_RSP                  0x0B
#define LEATT_READ_BLOB_REQ             0x0C
#define LEATT_READ_BLOB_RSP             0x0D
#define LEATT_READ_MULTI_REQ            0x0E
#define LEATT_READ_MULTI_RSP            0x0F
#define LEATT_READ_BY_GROUP_TYPE_REQ    0x10
#define LEATT_READ_BY_GROUP_TYPE_RSP    0x11
#define LEATT_WRITE_REQ                 0x12
#define LEATT_WRITE_RSP                 0x13
#define LEATT_PREPARE_WRITE_REQ         0x16
#define LEATT_PREPARE_WRITE_RSP         0x17
#define LEATT_EXECUTE_WRITE_REQ         0x18
#define LEATT_EXECUTE_WRITE_RSP         0x19
#define LEATT_HANDLE_VALUE_NOTIFY       0x1B
#define LEATT_HANDLE_VALUE_IND          0x1D
#define LEATT_HANDLE_VALUE_CNF          0x1E
#define LEATT_WRITE_COMMAND             0x52
#define LEATT_SIGNED_WRITE_COMMAND      0xD2

#define LEATT_LAST_REQUEST              0x1E
#define LEATT_LAST_COMMAND              0x52
#define LEATT_COMMAND_BIT               0x40

/* End of LeAttOpCode */

/* -- LE ATT Events ---------------------------------------------------------
 * LeAttEventType
 *
 *     LE ATT Events indicated to the layer above. The last byte of these
 *     events match the opcodes in the LE ATT Request/Response packets.
 */
typedef U32 LeAttEventType;

#define LEATT_EVENT_ERROR_RESPONSE          (BTE_TYPE_BASE_LEATT + LEATT_ERROR_RESPONSE        )
#define LEATT_EVENT_EXCH_MTU_REQ            (BTE_TYPE_BASE_LEATT + LEATT_EXCH_MTU_REQ          )
#define LEATT_EVENT_EXCH_MTU_RSP            (BTE_TYPE_BASE_LEATT + LEATT_EXCH_MTU_RSP          )
#define LEATT_EVENT_FIND_INFO_REQ           (BTE_TYPE_BASE_LEATT + LEATT_FIND_INFO_REQ         )
#define LEATT_EVENT_FIND_INFO_RSP           (BTE_TYPE_BASE_LEATT + LEATT_FIND_INFO_RSP         )
#define LEATT_EVENT_FIND_BY_TYPE_REQ        (BTE_TYPE_BASE_LEATT + LEATT_FIND_BY_TYPE_REQ      )
#define LEATT_EVENT_FIND_BY_TYPE_RSP        (BTE_TYPE_BASE_LEATT + LEATT_FIND_BY_TYPE_RSP      )
#define LEATT_EVENT_READ_BY_TYPE_REQ        (BTE_TYPE_BASE_LEATT + LEATT_READ_BY_TYPE_REQ      )
#define LEATT_EVENT_READ_BY_TYPE_RSP        (BTE_TYPE_BASE_LEATT + LEATT_READ_BY_TYPE_RSP      )
#define LEATT_EVENT_READ_REQ                (BTE_TYPE_BASE_LEATT + LEATT_READ_REQ              )
#define LEATT_EVENT_READ_RSP                (BTE_TYPE_BASE_LEATT + LEATT_READ_RSP              )
#define LEATT_EVENT_READ_BLOB_REQ           (BTE_TYPE_BASE_LEATT + LEATT_READ_BLOB_REQ         )
#define LEATT_EVENT_READ_BLOB_RSP           (BTE_TYPE_BASE_LEATT + LEATT_READ_BLOB_RSP         )
#define LEATT_EVENT_READ_MULTI_REQ          (BTE_TYPE_BASE_LEATT + LEATT_READ_MULTI_REQ        )
#define LEATT_EVENT_READ_MULTI_RSP          (BTE_TYPE_BASE_LEATT + LEATT_READ_MULTI_RSP        )
#define LEATT_EVENT_READ_BY_GROUP_TYPE_REQ  (BTE_TYPE_BASE_LEATT + LEATT_READ_BY_GROUP_TYPE_REQ)
#define LEATT_EVENT_READ_BY_GROUP_TYPE_RSP  (BTE_TYPE_BASE_LEATT + LEATT_READ_BY_GROUP_TYPE_RSP)
#define LEATT_EVENT_WRITE_REQ               (BTE_TYPE_BASE_LEATT + LEATT_WRITE_REQ             )
#define LEATT_EVENT_WRITE_RSP               (BTE_TYPE_BASE_LEATT + LEATT_WRITE_RSP             )
#define LEATT_EVENT_PREPARE_WRITE_REQ       (BTE_TYPE_BASE_LEATT + LEATT_PREPARE_WRITE_REQ     )
#define LEATT_EVENT_PREPARE_WRITE_RSP       (BTE_TYPE_BASE_LEATT + LEATT_PREPARE_WRITE_RSP     )
#define LEATT_EVENT_EXECUTE_WRITE_REQ       (BTE_TYPE_BASE_LEATT + LEATT_EXECUTE_WRITE_REQ     )
#define LEATT_EVENT_EXECUTE_WRITE_RSP       (BTE_TYPE_BASE_LEATT + LEATT_EXECUTE_WRITE_RSP     )
#define LEATT_EVENT_HANDLE_VALUE_NOTIFY     (BTE_TYPE_BASE_LEATT + LEATT_HANDLE_VALUE_NOTIFY   )
#define LEATT_EVENT_HANDLE_VALUE_IND        (BTE_TYPE_BASE_LEATT + LEATT_HANDLE_VALUE_IND      )
#define LEATT_EVENT_HANDLE_VALUE_CNF        (BTE_TYPE_BASE_LEATT + LEATT_HANDLE_VALUE_CNF      )
#define LEATT_EVENT_WRITE_COMMAND           (BTE_TYPE_BASE_LEATT + LEATT_WRITE_COMMAND         )
#define LEATT_EVENT_SIGNED_WRITE_COMMAND    (BTE_TYPE_BASE_LEATT + LEATT_SIGNED_WRITE_COMMAND  )

/* The LE ATT channel is connected and ready to exchange data. This event
 * is only sent to client handlers if LEATT_Open() returned status "Pending". It is sent
 * to the server handler when the ATT channel connected.
 */
#define LEATT_EVENT_OPEN                    (BTE_TYPE_BASE_LEATT + 0x21)

/* The LE ATT channel is closed. This event occurs only if LEATT_Close()
 * returned status "Pending" or some other event caused the channel to be
 * closed.
 */
#define LEATT_EVENT_CLOSE                   (BTE_TYPE_BASE_LEATT + 0x22)

/* An LE ATT PDU has been sent. The application may re-use or
 * deallocate the packet in the "event.pkt" field. The status of the
 * transmit is provided in the "event.e.status" field of the event
 * parameters structure.
 */
#define LEATT_EVENT_PACKET_HANDLED          (BTE_TYPE_BASE_LEATT + 0x23)

/* The LE ATT transaction expired before completion. The LE ATT channel
 * will be closed and will need to be reopened if further use is desired.
 */
#define LEATT_EVENT_TRANSACTION_TIMEOUT     (BTE_TYPE_BASE_LEATT + 0x24)

/* End of LeAttEvent */

/*---------------------------------------------------------------------------
 * LeAttStates enum
 *
 *     Defines the LE ATT protocol states.
 */
enum LeAttStates {
    S_LeAttNull,
    S_LeAttIdle,
    S_LeAttConnect,
    S_LeAttOpen,
    S_LeAttSendPduIndicate,
    S_LeAttHandleReq,
    S_LeAttWaitRsp,
};

/*---------------------------------------------------------------------------
 * LeAttEvents enum
 *
 *     Defines the LE ATT protocol events.
 */
enum LeAttEvents {
    E_LeAttOpen,
    E_LeAttClose,
    E_LeAttReject,
    E_LeAttSendPdu,
    E_LeAttSendPduCmd,
    E_LeAttSendNotify,
    E_LeAttSendIndicate,
    E_LeAttTxDone,
    E_LeAttTxDoneNi,
    E_LeAttReq,
    E_LeAttRsp,
    E_LeAttCmd,
    E_LeAttNI,
};

/*---------------------------------------------------------------------------
 * LeAttFlags type
 *
 *     Defines the LE ATT protocol flags used in the ATT statemachine. These
 *     can be ORed together
 */
typedef U8 LeAttFlags;

#define LEATTF_WAIT_SEND_RSP  0x01
#define LEATTF_WAIT_RSP_DONE  0x02

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * LeAttEvent structure
 *
 *     Provides information to the LE ATT user about an LeAtt callback event.
 */
struct _LeAttEvent {

    /* LE ATT upper layer event. Always valid. */
    BtEventEx       e;

    /* Remote Device associated with event. */
    BtRemoteDevice *remDev;

    /* Valid for LEATT_EVENT_x_REQ, LEATT_EVENT_x_RSP
     * and LEATT_EVENT_PACKET_HANDLED events.
     */
    LeAttOp        *att;

    /* Valid for LEATT_EVENT_PACKET_HANDLED event. */
    BtPacket       *pkt;

    /* Valid for LEATT_EVENT_CLOSE event. */
    BOOL            collision;
};


/*---------------------------------------------------------------------------
 * LeAttConn structure
 *
 *     Maintains LE ATT connection state information.
 */
typedef struct _LeAttConn {
    BtLeAttContext *ctx;
    U8              state;
    L2capPsm       *psm;
    LeAttRole       role;
    LeAttOpCode     transaction;
    EvmTimer        transactTimer;
} LeAttConn;

/*---------------------------------------------------------------------------
 * BtLeAttContext structure
 *
 * This structure provides the context and state information for the LE ATT
 * channel.
 */
struct _BtLeAttContext {

    /* Channel Establishment */
    void               *channel;
    U8                  iClient;
    BtHandlerEx         attHandler;
    L2capChannelId      lCid;       /* Local CID  */
    L2capChannelId      rCid;       /* Remote CID */
    U16                 mtu;
    U16                 tmpMtu;
    U8                  collision;
    BtPacket            sigPkt;

    /* Events and Transactions */
    BtStatus            status;
    BtPacket           *txPkt;
    U8                 *data;
    U16                 dataLen;
    LeAttEvent          event;
    LeAttOpCode         errorOp;
    U16                 errorHandle;
    AttErrorCode        errorCode;
    GattConnectContext  gCtx;
    GattClientContext   gcCtx;
    GattServerContext   gsCtx;

    /* State Machines */
    LeAttConn           cl;         /* Client connection */
    LeAttConn           sv;         /* Server connection */
    BtPacket           *savePkt;    /* Saved packet */
    LeAttFlags          flags;

    /* Used for calling LEATT_Open for a retry attempt */
    LeAttRole           openRole;
    LeAttRole           roles;
    EvmTimer            retryTimer;
    BtRemoteDevice     *remDev;

};

/*---------------------------------------------------------------------------
 * BtBrAttContext structure
 *
 * This structure provides the context and state information for the LE ATT
 * channel.
 */
struct _BtBrAttContext {
    BtLeAttContext c;
    L2capPsm       clientPsm;
};



/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * LEATT_Register()
 *
 *     Register an LE ATT callback handler.
 *
 * Parameters:
 *     Protocol - Pointer to the L2capPsm structure to be registered.
 *
 *     Handler - Pointer to the LeAtt callback handler.
 *
 * Returns:
 *     ctx - Pointer to an LeAtt context or 0 if failure.
 */
BtStatus LEATT_Register(LeAttHandler *Handler);

/*---------------------------------------------------------------------------
 * LEATT_Unregister()
 *
 *     Unregister an LE ATT handler.
 *
 * Parameters:
 *     Protocol - Pointer to L2capPsm structure previously registered.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The opration completed successfully.
 *
 *     BT_STATUS_FAILED - The context or state is invalid.
 */
BtStatus LEATT_Unregister(LeAttHandler *Handler);

/*---------------------------------------------------------------------------
 * LEATT_GetContext()
 *
 *     Find the ATT context associated with the given BtRemoteDevice
 *
 * Parameters:
 *     remDev - Pointer to BtRemoteDevice structure in which ATT context is
 *         is to be associated.
 *
 *     Status - Pointer to receive result of call. If this is
 *          BT_STATUS_SUCCESS on return, the connection is open. If this is
 *          BT_STATUS_PENDING on return, the connection is pending and an
 *          LEATT_EVENT_OPEN event will be indicated when the connection is
 *          complete. If this BT_STATUS_BUSY the connection is already
 *
 * Returns:
 *     Pointer to LeAttContext or 0 if RemDev is not valid.
 */
BtLeAttContext *LEATT_GetContext(BtRemoteDevice *RemDev);

/*---------------------------------------------------------------------------
 * LEATT_FindContext()
 *
 *     Find the ATT context associated with the given BtDevAddr
 *
 * Parameters:
 *     devAddr - Pointer to BtDevAddr structure in which ATT context is
 *         is to be associated.
 *
 *     Status - Pointer to receive result of call. If this is
 *          BT_STATUS_SUCCESS on return, the connection is open. If this is
 *          BT_STATUS_PENDING on return, the connection is pending and an
 *          LEATT_EVENT_OPEN event will be indicated when the connection is
 *          complete. If this BT_STATUS_BUSY the connection is already
 *
 * Returns:
 *     Pointer to LeAttContext or 0 if RemDev is not valid.
 */
BtLeAttContext *LEATT_FindContext(BtDevAddr *devAddr);

/*---------------------------------------------------------------------------
 * LEATT_Open()
 *
 *     Open an LE ATT connection to the remote LE device.
 *
 * Parameters:
 *     RemDev - pointer to BtRemoteDevice structure representing the
 *         remote device in which to open the connection.
 *
 *     Type - The role type of the requesting LE ATT entity.
 *
 * Returns:
 *     ctx - Pointer to BtLeAttContext representing connection to remote
 *         device or 0 for failure.
 */
BtStatus LEATT_Open(BtRemoteDevice *RemDev, LeAttRole Role);

/*---------------------------------------------------------------------------
 * LEATT_Close()
 *
 *     Close an LE ATT connection.
 *
 * Parameters:
 *     ctx - Pointer to BtLeAttContext representing connection to remote
 *         device.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_PENDING - operation was successfully started. Completion
 *         is indicated by an event to the registered callback.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         channel is no longer active.
 */
BtStatus LEATT_Close(BtLeAttContext *ctx);

/*---------------------------------------------------------------------------
 * LEATT_GetMtu()
 *
 *     Get the MTU of an LE ATT connection.
 *
 * Parameters:
 *     ctx - Pointer to the LeAtt context.
 *
 * Returns:
 *     U16 - The MTU of the LE ATT connection or 0 if failure.
 */
U16 LEATT_GetMtu(BtLeAttContext *ctx);

/*---------------------------------------------------------------------------
 * LEATT_SendPdu()
 *
 *     Send an LE ATT PDU to the remote LE device.
 *
 * Parameters:
 *     ctx - Pointer to the LeAtt context.
 *
 *     Handler - LeAttHandler completing send.
 *
 *     AttOp - Operation to send. Memory must be valid until
 *         LEATT_EVENT_PACKET_HANDLED event is indicated.
 *
 *     Pkt - BtBacket used to send PDU. Memory must be valid until
 *         LEATT_EVENT_PACKET_HANDLED event is indicated.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation was successfully started. Completion
 *         is indicated by an event to the registered callback.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         channel is no longer active.
 */
BtStatus LEATT_SendPdu(BtLeAttContext *ctx, LeAttHandler *Handler,
                       LeAttOp *AttOp, BtPacket *Pkt);

/*---------------------------------------------------------------------------
 * LEATT_SendErrorResponse()
 *
 *     Send an LE ATT Error Response to the remote LE device.
 *
 * Parameters:
 *     ctx - Pointer to the LeAtt context.
 *
 *     Handler - LeAttHandler completing send.
 *
 *     AttOp - Operation to send. Memory must be valid until
 *         LEATT_EVENT_PACKET_HANDLED event is indicated.
 *
 *     Pkt - BtBacket used to send PDU. Memory must be valid until
 *         LEATT_EVENT_PACKET_HANDLED event is indicated.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation was successfully started. Completion
 *         is indicated by an event to the registered callback.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this
 *         channel is no longer active.
 */
BtStatus LEATT_SendErrRsp(BtLeAttContext *ctx, LeAttHandler *Handler,
                          LeAttOp *AttOp, BtPacket *Pkt);

/* Helper functions */

void LEATT_Create(BtLeAttContext *ctx);
void LEATT_InitFixedChannelClient(void *Client);
void LeAttEventHandler(BtLeAttContext *ctx, L2capEvent *Event);
void LeAttNotify(BtLeAttContext *ctx, L2capEventType eType, U16 status);

#endif /* __LE_ATT_I_H */
