/***************************************************************************
 *
 * File:
 *     bttypes2.h (cffcbbf) 2017-05-16 16:50:44 +0000 Erik Berrio
 *     $Product: OpenSynergy Blue SDK v5.x $
 *     Commit: cffcbbf0fd5215f4b9f726f387893882c32cec3e
 *
 * Description:
 *     This file provides global types part two.
 *
 * Copyright 2010-2017 OpenSynergy GmbH.
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

#ifndef __BTTYPES2_H
#define __BTTYPES2_H

#include "bttypes.h"
#define BD_ADDR_COMPATIBLE_MODE XA_ENABLED

/*---------------------------------------------------------------------------
 * Global Types 2 layer
 *
 *     General types and conversion functions used by stack components
 *     involved with AMP Controllers including AMPMGR, L2CAP and L2capAgent.
 *     Also used by applications that use AMPMGR and/or L2capAgent.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * The following "RX" constants are used to in allocating Receive buffers (RxBuff)
 * for AMPs. They are based on the type of AMP PAL as defined in the Bluetooth
 * core specification. The PAL defines a Maximum PDU size. The RxBuff size is
 * corresponds to HCI ACL buffers data packets which have a 4 octet header
 * before the L2CAP PDU. Thus, a Maximum RX buffer size is the Maximum PDU
 * size defined in the PAL plus 4 octets.
 *
 */

#if NUM_AMP_802_CONTROLLERS > 0
/*---------------------------------------------------------------------------
 * MAX_AMP_802_PDU_SIZE constant
 *
 *     Defines the maximum PDU for an 802.11 PAL as defined in the
 *     Bluetooth Specification.
 */
#define MAX_AMP_802_PDU_SIZE 1492

/*---------------------------------------------------------------------------
 * MAX_AMP_802_RX_DATA_SIZE constant
 *
 *     Defines the maximum size of an RX buffer for an AMP based on the
 *     802.11 PAL. The Maximum PDU size in the 802.11 PAL section of the
 *     Bluetooth Specification is 1492 octets.
 */
#define MAX_AMP_802_RX_DATA_SIZE (MAX_AMP_802_PDU_SIZE+4)

#endif /* NUM_AMP_802_CONTROLLERS > 0 */

#if NUM_AMP_UDP_CONTROLLERS > 0
/*---------------------------------------------------------------------------
 * MAX_AMP_UDP_PDU_SIZE constant
 *
 *     Defines the maximum PDU for an UDP PAL (Fake PAL). This is not
 *     defined in the fake PAL spec but for safety in making a UDP
 *     packet can travel through the internet the maximum size
 *     suggested is 512.
 */
#define MAX_AMP_UDP_PDU_SIZE 512

/*---------------------------------------------------------------------------
 * MAX_AMP_UDP_RX_DATA_SIZE constant
 *
 *     Defines the maximum size of an RX buffer for an AMP based on the
 *     UDP PAL (Fake PAL).
 */
#define MAX_AMP_UDP_RX_DATA_SIZE (MAX_AMP_UDP_PDU_SIZE+4)

#endif /* NUM_AMP_UDP_CONTROLLERS > 0 */


#if NUM_AMP_TCP_CONTROLLERS > 0
/*---------------------------------------------------------------------------
 * MAX_AMP_TCP_PDU_SIZE constant
 *
 *     Defines the maximum PDU for a TCP PAL.  It set to the same value as
 *     used in 802.11 so the TCP PAL can run optimally over an 802.11 radio.
 */
#define MAX_AMP_TCP_PDU_SIZE 1492

/*---------------------------------------------------------------------------
 * MAX_AMP_TCP_RX_DATA_SIZE constant
 *
 *     Defines the maximum size of an RX buffer for an AMP based on the
 *     TCP PAL.
 */
#define MAX_AMP_TCP_RX_DATA_SIZE (MAX_AMP_TCP_PDU_SIZE+4)

#endif /* NUM_AMP_TCP_CONTROLLERS > 0 */

/*---------------------------------------------------------------------------
 * MAX_HCI_ACL_DATA_SIZE constant
 *
 *     Defines the maximum size Rx Buffer used in the system including
 *     the AMP sizes define above and HCI_ACL_DATA_SIZE which is used for
 *     BR/EDR. This constant is used by the FTS module to allocate space to
 *     copy a received packet for decoding.
 */

#if HCI_ACL_DATA_SIZE > MAX_AMP_DATA_SIZE
#define MAX_HCI_ACL_DATA_SIZE HCI_ACL_DATA_SIZE
#else
#define MAX_HCI_ACL_DATA_SIZE MAX_AMP_DATA_SIZE
#endif

#if NUM_AMP_802_CONTROLLERS > 0
#if MAX_AMP_802_RX_DATA_SIZE > MAX_HCI_ACL_DATA_SIZE
#undef MAX_HCI_ACL_DATA_SIZE
#define MAX_HCI_ACL_DATA_SIZE MAX_AMP_802_RX_DATA_SIZE
#endif
#endif /* NUM_AMP_802_CONTROLLERS > 0 */

#if NUM_AMP_UDP_CONTROLLERS > 0
#if MAX_AMP_UDP_RX_DATA_SIZE > MAX_HCI_ACL_DATA_SIZE
#undef MAX_HCI_ACL_DATA_SIZE
#define MAX_HCI_ACL_DATA_SIZE MAX_AMP_UDP_RX_DATA_SIZE
#endif
#endif /* NUM_AMP_UDP_CONTROLLERS > 0 */

#if NUM_AMP_TCP_CONTROLLERS > 0
#if MAX_AMP_TCP_RX_DATA_SIZE > MAX_HCI_ACL_DATA_SIZE
#undef MAX_HCI_ACL_DATA_SIZE
#define MAX_HCI_ACL_DATA_SIZE MAX_AMP_TCP_RX_DATA_SIZE
#endif
#endif /* NUM_AMP_TCP_CONTROLLERS > 0 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtEventTypeEx type
 *
 *     Type of event. All stack layers returning an event have a
 *     base type
 *
 */
typedef U32 BtEventTypeEx;

#define BTE_TYPE_BASE_MEMON         0x00000100
#define BTE_TYPE_BASE_MECON         0x00000200
#define BTE_TYPE_BASE_L2CAP         0x00000300
#define BTE_TYPE_BASE_RFCOMM        0x00000400
#define BTE_TYPE_BASE_BMG           0x00000500
#define BTE_TYPE_BASE_CNTRL         0x00000600
#define BTE_TYPE_BASE_A2ME          0x00000700
#define BTE_TYPE_BASE_LINK          0x00000800
#define BTE_TYPE_BASE_HCITRANSPORT  0x00000900
#define BTE_TYPE_BASE_PRESCAN       0x00000A00
#define BTE_TYPE_BASE_AMPMGR        0x00000B00
#define BTE_TYPE_BASE_HBPOOL        0x00000C00
#define BTE_TYPE_BASE_CHANMGR       0x00000D00
#define BTE_TYPE_BASE_MELE          0x00000E00
#define BTE_TYPE_BASE_GATTC         0x00000F00
#define BTE_TYPE_BASE_GATTS         0x00001000
#define BTE_TYPE_BASE_LEATT         0x00001100
#define BTE_TYPE_BASE_LESEC         0x00001200
#define BTE_TYPE_BASE_CMGR          0x00001300
#define BTE_TYPE_BASE_GATTSEC       0x00001400
#define BTE_TYPE_BASE_HRCLCTR       0x00002000  /* Heart Rate Collector event type */
#define BTE_TYPE_BASE_HRSENSOR      0x00002100  /* Heart Rate Sensor event type */
#define BTE_TYPE_BASE_HTCLCTR       0x00002200  /* Health Thermometer Collector event type */
#define BTE_TYPE_BASE_HTTHERM       0x00002300  /* Health Thermometer Server event type */
#define BTE_TYPE_BASE_PXPFMPC       0x00002400  /* Proximity/FindMe client event type */
#define BTE_TYPE_BASE_PXPFMPS       0x00002500  /* Proximity/FindMe server event type */
#define BTE_TYPE_BASE_ANCLIENT      0x00002600  /* Alert Notification client event type */
#define BTE_TYPE_BASE_ANSERVER      0x00002700  /* Alert Notification server event type */
#define BTE_TYPE_BASE_SPPC          0x00002800  /* SPP Data Exchange LE profile client event type */
#define BTE_TYPE_BASE_SPPS          0x00002900  /* SPP Data Exchange LE profile server event type */
#define BTE_TYPE_BASE_DEVINFOCLIENT 0x00002A00  /* Device Information client event type */
#define BTE_TYPE_BASE_GDCLIENT      0x00002B00  /* GATT Demo profile client event type */
#define BTE_TYPE_BASE_GDSERVER      0x00002C00  /* GATT Demo profile server event type */

/* This can be used be apps for their own events */
#define BTE_TYPE_BASE_APP           0x80000000u

/* Base mask used to get to the Base type. Given an
 * event the base type can be determined by "AND"ing
 * the event type with the BTE_TYPE_BASE_MASK
 */
#define BTE_TYPE_BASE_MASK          0xffffff00
/* End of BtEventTypeEx */

/*---------------------------------------------------------------------------
 * BtAmpStatus type
 *
 */
typedef U8 BtAmpStatus;

/* AMP Controller radio is available but is currently physically powered down */
#define AMPS_DOWN          0x00

/* The AMP Controller is only used by Bluetooth Technology and will not be
 * shared with other non-Bluetooth technologies
 */
#define AMPS_BT_ONLY       0x01

/* The AMP Controller has no capacity available for Bluetooth operation */
#define AMPS_NONBT_NONE    0x02

/* The AMP Controller has low capacity available for Bluetooth operation.
 * This value indicates that the majority of the AMP controller bandwidth
 * is currently allocated to servicing non Bluetooth technology. An AMP
 * Controller with capacity in the approximate range of 0 < capacity < 30%
 * should indicate this value.
 */
#define AMPS_NONBT_LOW     0x03

/* The AMP Controller has medium capacity available for Bluetooth
 * operation. An AMP Controller with capacity in the approximate range of
 * 30% < capacity < 70% should indicate this value.
 */
#define AMPS_NONBT_MED     0x04

/* The AMP Controller has high capacity available for Bluetooth operation.
 * This value indicates that the majority of the AMP Controllers bandwidth
 * is currently allocated to servicing the Bluetooth technology.
 * An AMP Controller with capacity in the approximate range of 70% <
 * capacity < 100% should indicate this value.
 */
#define AMPS_NONBT_HIGH    0x05

/* The AMP Controller has full capacity available for Bluetooth operation
 * This value indicates that while currently the AMP is only being used
 * by Bluetooth the device allows a different technology to share the
 * radio. This value shall be used as the default Controller Status value
 * for devices that are not capable of determining the available capacity
 * for Bluetooth operation for an AMP Controller.
 */
#define AMPS_NONBT_MAX     0x06

/* End of BtAmpStatus */


/*---------------------------------------------------------------------------
 * BtControllerType type
 *
 */
typedef U8 BtControllerType;

#define BCT_BR_EDR   0x00
#define BCT_802      0x01
#define BCT_ECMA_368 0x02
#define BCT_UDP      0xEF
#define BCT_TCP      0xF0   /* Made up value */
#define BCT_UNKNOWN  0xFF   /* Made up value */

/* End of BtControllerType */

/*---------------------------------------------------------------------------
 * BtFactory type
 *
 *     Function of this type is called to return objects. The caller knows
 *     the type of the object.
 */
typedef void *(*BtFactory)(void);
/* End of BtFactory */


/*  Forward declaration of BtHandler */
typedef struct _BtHandlerEx BtHandlerEx;

/*  Forward declaration of BtEventEx */
typedef struct _BtEventEx BtEventEx;

/*  Forward declaration of BtRemoteDevice */
typedef struct _BtRemoteDevice BtRemoteDevice;

/*---------------------------------------------------------------------------
 * BtCallbackEx type
 *
 *     Function of this type is called to indicate events. All callbacks
 *     in the stack are based on this type. The context is the 
 *     "context" field of the BtHandlerEx.
 */
typedef void (*BtCallbackEx)(void *context, BtEventEx *event);
/* End of BtCallbackEx */

/*---------------------------------------------------------------------------
 * BtConnectionRole type
 *
 *     Specifies the role of a ACL connection
 */
typedef U8 BtConnectionRole;

#define BCR_MASTER   0x00
#define BCR_SLAVE    0x01
#define BCR_ANY      0x02
#define BCR_UNKNOWN  0x03

/* === Internal use only === */
#define BCR_PSLAVE   0x04
#define BCR_PMASTER  0x05

/* End of BtConnectionRole */

/*---------------------------------------------------------------------------
 * BtRemDevState type
 *
 *     Indicates the current state of a Remote Device.
 */
typedef U8 BtRemDevState;

#define BDS_DISCONNECTED  0x00
#define BDS_OUT_CON       0x01   /* Starting an outgoing connection */
#define BDS_IN_CON        0x02   /* In process of incoming connection */
#define BDS_CONNECTED     0x03   /* Connected */
#define BDS_OUT_DISC      0x04   /* Starting an out going disconnect */
#define BDS_OUT_DISC2     0x05   /* Disconnect status received */
#define BDS_OUT_CON2      0x06   /* In SCO, used when connection request has
                                    been sent */

#define BDS_MOD_FLOW_SPEC 0x07   /* AMP Links flow spec is being modified */

/* End of BtRemDevState */

/*---------------------------------------------------------------------------
 * BtLinkState type
 *
 *     Indicates the current state of a Link. Same as BtRemDevState.
 */
typedef U8 BtLinkState;

/* End of BtLinkState */

/*---------------------------------------------------------------------------
 * BtLinkMode type
 */
typedef U8 BtLinkMode;

#define BLM_ACTIVE_MODE     0x00
#define BLM_HOLD_MODE       0x01
#define BLM_SNIFF_MODE      0x02
#define BLM_PARK_MODE       0x03
#define BLM_SCATTER_MODE    0x04

/* End of BtLinkMode */

/*---------------------------------------------------------------------------
 * BtRemDevType type
 *
 *    Indicates the type of the remote device structure where the base class
 *    is BtRemoteDevice, a BR/EDR is BrEdrRemoteDevice and an LE is
 *    MeLeRemoteDevice.
 */
typedef U8 BtRemDevType;

#define BRDT_INVALID        0x00
#define BRDT_BR_EDR         0x01   /* BrEdrRemoteDevice structure */
#define BRDT_LE             0x02   /* MeLeRemoteDevice structure */

/* Used when disconnecting an incoming LE connection when unable to
 * accept it (e.g. no more MeLeRemoteDevice structures). This is
 * a base class structure (BtRemoteDevice) so it does not have all
 * the fields necessary for a BR/EDR or LE link.
 */
#define BRDT_LE_DISC        0x03

/* End of BtRemDevType */

/*---------------------------------------------------------------------------
 * BtAddressType type
 *
 *     The Address Type specifies the type of Bluetooth address.
 */
typedef U8 BtAddressType;

/* Public device address from MeLeAddressType */
#define BTADDR_PUBLIC    0x00 

/* Random device address from MeLeAddressType */
#define BTADDR_RANDOM    0x01 

/* BR/EDR device address */
#define BTADDR_BR_EDR    0x20 

/* LE identifier address used as Identity address when the remote
 * device sends 0 as its identity address during pairing. */
#define BTADDR_LE_ID     0x81 

#define BTADDR_INVALID   0xFF

/* End of BtAddressType */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtEventEx structure
 *
 *     Base class from which other event structures inherit.
 */
struct _BtEventEx
{
    BtEventTypeEx  type;    /* Event Type */
    BtStatus       status;  /* Status of the event */
    BtHandlerEx   *handler; /* Pointer to handler handling the event */
};

/*---------------------------------------------------------------------------
 * BtHandlerEx structure
 *
 *     Used to register a callback with the stack.
 */
struct _BtHandlerEx
{
    ListEntry    node;     /* For internal use to place handler on a list */
    void *       context;  /* Context of object providing callback */
    BtCallbackEx callback; /* pointer to callback function */
};

/*---------------------------------------------------------------------------
 * BtAmpInfo structure
 *
 */
typedef struct _BtAmpInfo {
    /* An upper bound on the total data rate that can be achieved by the
     * AMP over HCI. It accounts for the total bandwidth provided by the
     * HCI transport. No sustained combination of transmit and receive
     * operations shall exceed this value. This can be used to help in AMP
     * selection and admission control. Expressed in Kbps.
     */
    U32 totBandwidth;

    /* An upper bound on the maximum data the AMP can guarantee for a
     * single logical link over HCI. Any request made by an application
     * above this threshold would be rejected. It accounts for any bandwidth
     * limitations of the HCI transport. No sustained combination of
     * transmit and receive operations shall exceed this value. This can be
     * used to help in AMP selection and admission control. Expressed in Kbps.
     */
    U32 maxGuarBandwidth;

    /* A lower bound on the latency in microseconds that the AMP can
     * guarantee for a logical channel. It accounts for the minimum latency
     * of the HCI transport. This can be used to help in AMP selection and
     * admission control.
     */
    U32 minLatency;

    /* An upper bound on the size of L2CAP PDU which may be provided
     * for transmission or reception on this AMP. The Host shall not require
     * the AMP to transport L2CAP PDUs larger than this value.
     * Expressed in octets.
     */
    U32 maxPduSize;

    /* "Service Type = Guaranteed" is supported by PAL = 1
     *  Bits 15-1: Reserved (shall be set to 0)
     */
    U16 palCapabilities;

    /* AMP_ASSOC maximum length for this local AMP Controller. For
     * use with Read and Write AMP_ASSOC commands. This value is sent
     * to the remote AMP Manager in the AMP Get Info Response
     */
    U16 maxAmpAssocLen;

    /* Maximum time period, in microseconds, which the AMP device uses
     * to attempt to transmit a frame on a Guaranteed Logical Link.
     * If the Controller is configured to retry frames for an unbounded time
     * (i.e. there is no flushing at all), then it shall set this value to
     * 0xFFFFFFFF.
     */
    U32 maxFlushTimeout;

    /* The typical time period, in microseconds, which the AMP device
     * may use to attempt to transmit a frame on a Best Effort Logical Link.
     * The value shall not exceed the value given in Max_Flush_Timeout.
     * If the Controller is configured to retry frames for an unbounded time
     * (i.e. there is no flushing at all), then it shall set this value to
     * 0xFFFFFFFF.
     */
    U32 beFlushTimeout;

} BtAmpInfo;

/*---------------------------------------------------------------------------
 * BtRemoteAmpInfo structure
 *
 */
typedef struct _BtRemoteAmpInfo {
    /* An upper bound on the total data rate that can be achieved by the
     * AMP over HCI. It accounts for the total bandwidth provided by the
     * HCI transport. No sustained combination of transmit and receive
     * operations shall exceed this value. This can be used to help in AMP
     * selection and admission control. Expressed in Kbps.
     */
    U32 totBandwidth;

    /* An upper bound on the maximum data the AMP can guarantee for a
     * single logical link over HCI. Any request made by an application
     * above this threshold would be rejected. It accounts for any bandwidth
     * limitations of the HCI transport. No sustained combination of
     * transmit and receive operations shall exceed this value. This can be
     * used to help in AMP selection and admission control. Expressed in Kbps.
     */
    U32 maxGuarBandwidth;

    /* A lower bound on the latency in microseconds that the AMP can
     * guarantee for a logical channel. It accounts for the minimum latency
     * of the HCI transport. This can be used to help in AMP selection and
     * admission control.
     */
    U32 minLatency;

    /* "Service Type = Guaranteed" is supported by PAL = 1
     *  Bits 15-1: Reserved (shall be set to 0)
     */
    U16 palCapabilities;

    /* AMP_ASSOC maximum length for this local AMP Controller. For
     * use with Read and Write AMP_ASSOC commands. This value is sent
     * to the remote AMP Manager in the AMP Get Info Response
     */
    U16 maxAmpAssocLen;

} BtRemoteAmpInfo;

/*-----------------------------------------------------------------------
 * BtAmpPhyLinkDesc structure
 *
 *     Descriptor for an Amp Physical Link
 */
typedef struct _BtAmpPhyLinkDesc {
    BtRemoteDevice  *remDev;
    U8               localCrlId;
    U8               remCrlId;
    BtControllerType type;

} BtAmpPhyLinkDesc;

/*-----------------------------------------------------------------------
 * HciEvent structure
 *
 *     Describes an event received from a controller.
 */
typedef struct _HciEvent {
    U8  event;      /* Type of the event (see HciEventType). */
    U8 *parms;      /* Pointer to parameters for the command. */
    U8  len;        /* Length of parameter data in bytes. */
} HciEvent;

/*---------------------------------------------------------------------------
 * HciCommand structure
 *
 *     Represents an HCI command to be sent to the host controller.
 *
 *     The parameters buffer contains a set of command-specific parameters.
 *     The format and description for these parameters can be found in
 *     the Bluetooth HCI specification.
 */
typedef struct _HciCommand {
    ListEntry node;      /* For internal use only */

    U8        header[3]; /* For internal use only */

    /* Command-specific parameters provided by the caller of
     * HCI_SendCommand. Note that the length of this buffer is
     * limited to HCI_CMD_PARM_LEN in btconfig.h. If a larger parameters
     * buffer is required, override this configuration setting
     * in overide.h.
     */
    U8        parms[HCI_CMD_PARM_LEN];

} HciCommand;

/*---------------------------------------------------------------------------
 * BtPriorityToken structure
 *
 *     Represents a prioritized token of work to be scheduled.
 */
typedef struct _BtPriorityToken {
    void       *ctx;
    BtPriority  priority;
    U32         count;
} BtPriorityToken;

/* Internal types required for BtRemoteDevice structure */
typedef U8 BtOpType;

typedef struct _BtOperation {
    ListEntry   node;
    BtOpType    opType;
} BtOperation;


/*---------------------------------------------------------------------------
 * BtDevAddr structure
 *
 *     Represents a 48-bit Bluetooth device address.
 */
typedef struct _BtDevAddr {
    BD_ADDR       bda;
    BtAddressType type;
} BtDevAddr;

/*---------------------------------------------------------------------------
 * BtRemoteDevice structure
 *
 *     Base class to manage a link to a remote device. Used by both BR/EDR
 *     and LE links.
 */
struct _BtRemoteDevice
{
    BtOperation      op;            /* Used internally by the stack */

    BtRemDevType     type;          /* Type of the remote device */
#if BD_ADDR_COMPATIBLE_MODE == XA_ENABLED
    BD_ADDR          bdAddr;        /* 48-bit address of the remote device */
#endif /* BD_ADDR_COMPATIBLE_MODE == XA_ENABLED */
    BtDevAddr        devAddr;       /* 48-bit address and address type of the remote device */
    BtConnectionRole role;          /* The local device's role in this connection */
    BtLinkMode       mode;

    ListEntry        handlers;      /* List of bound handlers */
    U16              hciHandle;     /* Connection handle for HCI */
    BtRemDevState    state;         /* State of the ACL link */
    U8               restricted;    /* TRUE indicates restricted link */

    /* Parameters used for Disconnect - both LE and BR/EDR use the same
     * HCI disconnect command
     */
    U8               discFlag;      /* TRUE indicates disconnect request */
    U8               discReason;    /* Non-zero = disconnect reason */

    void            *pLink;

    /* Group: L2CAP uses the following fields internally. */
    U16             refCount;
    U32             l2Flags;
    U32             flushTimeout;
    U8              linkModes;          /* L2capLinkMode defined in l2cap.h */
    U32             extendedFeatures;
    U8              fixedChannels[8];

};



/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtNotifyEx()
 *
 *     Notifies a particular event handler using the event information
 *     supplied. To do this, the contents of the supplied BtEventEx are
 *     overwritten, and the handler's callback function is invoked.
 *
 *     It is assumed that the handler and its callback pointer are non-NULL.
 *
 * Parameters:
 *     handler - The handler to be notified
 *
 *     parms - A pointer to the BtEventEx parameters to be supplied
 *         to the handler.
 *
 *     type - The event type code, to be written into parms.
 *
 *     status - The event status code, to be written into parms.
 */
void BtNotifyEx(BtHandlerEx *handler, BtEventEx *parms, BtEventTypeEx type,
                BtStatus status);

#define BtNotifyEx(handlerEx, parms, typeCode, statusCode) \
    do { Assert(handlerEx && parms && (handlerEx)->callback); \
    (parms)->status = statusCode; (parms)->handler = handlerEx; \
    (parms)->type = typeCode; \
    (handlerEx)->callback((handlerEx)->context, parms); \
    } while(0)


/*---------------------------------------------------------------------------
 * BtNotifyCbEx()
 *
 *     Invokes a callback with the event information supplied. To do this,
 *     the contents of the supplied BtEventEx are overwritten, and the callback
 *     function is invoked.
 *
 *     If a BtHandlerEx structure is available, BtNotifyEx() should be used
 *     instead of this macro.
 *
 *     It is assumed that the callback pointer is non-NULL.
 *
 * Parameters:
 *     callback - The callback to be invoked
 *
 *     context - The context to be provided to the callback
 *
 *     parms - A pointer to the BtEventEx parameters to be supplied
 *         to the callback.
 *
 *     type - The event type code, to be written into parms.
 *
 *     status - The event status code, to be written into parms.
 */
void BtNotifyCbEx(BtCallbackEx handler, void *context, BtEventEx *parms,
                  BtEventTypeEx type, BtStatus status);

#define BtNotifyCbEx(callbackEx, context, parms, typeCode, statusCode) \
    do { Assert(callbackEx); (parms)->status = statusCode; \
    (parms)->handler = 0; (parms)->type = typeCode; \
    (callbackEx)(context, parms); \
    } while(0)

/*---------------------------------------------------------------------------
 * BtInitHandlerEx()
 *
 *     Initialize a handler.
 *
 * Parameters:
 *     handler - pointer to a BtHandlerEx structure.
 */
void BtInitHandlerEx(BtHandlerEx *handler);
#define BtInitHandlerEx(h) InitializeListEntry(&((h)->node))

/*---------------------------------------------------------------------------
 * HCC_InitCommand()
 *
 *     Initializes a controller command with no parameters. After this
 *     call, HCC_Write...() functions may be used to add parameters to the
 *     command.
 *
 * Parameters:
 *     cmd - The command to initialize
 *
 *     cmdType - The type of the command
 */
void HCC_InitCommand(HciCommand *cmd, U16 cmdType);

/*---------------------------------------------------------------------------
 * HCC_Write8()
 *
 *     Writes an 8-bit value as the next parameter into the command.
 *
 *     Attempting to write more data into the cmd buffer than permitted
 *     by HCI_CMD_PARM_LEN will result in an Assert in XA_DEBUG completions,
 *     and no output in non-debug compilations.
 *
 * Parameters:
 *     cmd - The command to initialize
 *
 *     srcValue - The value to write as a parameter
 */
void HCC_Write8(HciCommand *cmd, U8 srcValue);


/*---------------------------------------------------------------------------
 * HCC_Write16()
 *
 *     Writes a 16-bit value as the next parameter into the command. The value
 *     is written in little-endian format.
 *
 *     Attempting to write more data into the cmd buffer than permitted
 *     by HCI_CMD_PARM_LEN will result in an Assert in XA_DEBUG completions,
 *     and no output in non-debug compilations.
 *
 * Parameters:
 *     cmd - The command to initialize
 *
 *     srcValue - The value to write as a parameter
 */
void HCC_Write16(HciCommand *cmd, U16 srcValue);

/*---------------------------------------------------------------------------
 * HCC_Write24()
 *
 *     Writes a 24-bit value as the next parameter into the command. The value
 *     is written in little-endian format.
 *
 *     Attempting to write more data into the cmd buffer than permitted
 *     by HCI_CMD_PARM_LEN will result in an Assert in XA_DEBUG completions,
 *     and no output in non-debug compilations.
 *
 * Parameters:
 *     cmd - The command to initialize
 *
 *     srcValue - The value to write as a parameter
 */
void HCC_Write24(HciCommand *cmd, U32 srcValue);

/*---------------------------------------------------------------------------
 * HCC_Write32()
 *
 *     Writes a 32-bit value as the next parameter into the command. The value
 *     is written in little-endian format.
 *
 *     Attempting to write more data into the cmd buffer than permitted
 *     by HCI_CMD_PARM_LEN will result in an Assert in XA_DEBUG completions,
 *     and no output in non-debug compilations.
 *
 * Parameters:
 *     cmd - The command to initialize
 *
 *     srcValue - The value to write as a parameter
 */
void HCC_Write32(HciCommand *cmd, U32 srcValue);

/*---------------------------------------------------------------------------
 * HCC_WriteBuffer()
 *
 *     Writes the specified buffer as the next parameter into the command.
 *
 *     Attempting to write more data into the cmd buffer than permitted
 *     by HCI_CMD_PARM_LEN will result in an Assert in XA_DEBUG completions,
 *     and no output in non-debug compilations.
 *
 * Parameters:
 *     cmd - The command to initialize
 *
 *     srcBuffer - The buffer to write into the command
 *
 *     length - The length of the buffer to write
 */
void HCC_WriteBuffer(HciCommand *cmd, const void *srcBuffer, U8 length);

/*---------------------------------------------------------------------------
 * HCE_Read8()
 *
 *     Reads the next 8-bit parameter out of an event.
 *
 * Parameters:
 *     evt - The event to read from
 *
 *     pos - Address of the read position (initialize this to zero to read
 *         the first parameter). Updated to reflect the new read position
 *         on return.
 *
 *     destValue - Address of value to be filled with the parameter from
 *         the event. May be 0 to indicate the parameter should be skipped.
 *
 * Returns:
 *     TRUE if the parameter was read successfully.
 *     FALSE if the event's parameter buffer is of insufficient length.
 */
BOOL HCE_Read8(const HciEvent *evt, U8 *pos, U8 *destValue);

/*---------------------------------------------------------------------------
 * HCE_Read16()
 *
 *     Reads the next 16-bit (little endian) parameter out of an event.
 *
 * Parameters:
 *     evt - The event to read from
 *
 *     pos - Address of the read position (initialize this to zero to read
 *         the first parameter). Updated to reflect the new read position
 *         on return.
 *
 *     destValue - Address of value to be filled with the parameter from
 *         the event. May be 0 to indicate the parameter should be skipped.
 *
 * Returns:
 *     TRUE if the parameter was read successfully.
 *     FALSE if the event's parameter buffer is of insufficient length.
 */
BOOL HCE_Read16(const HciEvent *evt, U8 *pos, U16 *destValue);

/*---------------------------------------------------------------------------
 * HCE_Read24()
 *
 *     Reads the next 24-bit (little endian) parameter out of an event.
 *
 * Parameters:
 *     evt - The event to read from
 *
 *     pos - Address of the read position (initialize this to zero to read
 *         the first parameter). Updated to reflect the new read position
 *         on return.
 *
 *     destValue - Address of value to be filled with the parameter from
 *         the event. May be 0 to indicate the parameter should be skipped.
 *
 * Returns:
 *     TRUE if the parameter was read successfully.
 *     FALSE if the event's parameter buffer is of insufficient length.
 */
BOOL HCE_Read24(const HciEvent *evt, U8 *pos, U32 *destValue);

/*---------------------------------------------------------------------------
 * HCE_Read32()
 *
 *     Reads the next 32-bit (little endian) parameter out of an event.
 *
 * Parameters:
 *     evt - The event to read from
 *
 *     pos - Address of the read position (initialize this to zero to read
 *         the first parameter). Updated to reflect the new read position
 *         on return.
 *
 *     destValue - Address of value to be filled with the parameter from
 *         the event. May be 0 to indicate the parameter should be skipped.
 *
 * Returns:
 *     TRUE if the parameter was read successfully.
 *     FALSE if the event's parameter buffer is of insufficient length.
 */
BOOL HCE_Read32(const HciEvent *evt, U8 *pos, U32 *destValue);

/*---------------------------------------------------------------------------
 * HCE_ReadBuffer()
 *
 *     Reads a buffer of specified length out of the event.
 *
 * Parameters:
 *     evt - The event to read from
 *
 *     pos - Address of the read position (initialize this to zero to read
 *         the first parameter). Updated to reflect the new read position
 *         on return.
 *
 *     destBuffer - A buffer of readLen bytes to be filled with parameter
 *         data. May be 0 to indicate the parameter should be skipped.
 *
 *     readLen - Size of the parameter to read into the buffer.
 *
 * Returns:
 *     TRUE if the parameter was read successfully.
 *     FALSE if the event's parameter buffer is of insufficient length.
 */
BOOL HCE_ReadBuffer(const HciEvent *evt, U8 *pos, U8 *destBuffer, U16 readLen);

/*---------------------------------------------------------------------------
 * AreDevAddrsEqual()
 *
 *     Compares two BtDevAddr for equality.
 *
 * Parameters:
 *     da1 - Pointer to first BtDevAddr to compare.
 *
 *     da2 - Pointer to second BtDevAddr to compare.
 *
 * Returns:
 *     TRUE when the addresses are the same, FALSE when they are different
 */
BOOL AreDevAddrsEqual(const BtDevAddr *da1, const BtDevAddr *da2);

#endif /* __BTTYPES2_H */
