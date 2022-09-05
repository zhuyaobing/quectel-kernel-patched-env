/****************************************************************************
 *
 * File:
 *     le_sec_i.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains internal definitions for the Bluetooth
 *     LE Security Management.
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

#ifndef __LE_SEC_I_H
#define __LE_SEC_I_H

#include "bttypes2.h"
#include "eventmgr.h"
#include "mele.h"
#include "lesec.h"

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BT_LE_KEY_LENGTH constant
 *
 *     This define indicates the size of a LE Security Key.
 */
#define BT_LE_KEY_LENGTH 16

/*---------------------------------------------------------------------------
 * BrSecKeyDistState type
 *
 *     Describes the current state of the BR/EDR Key Distribution state
 */
typedef U8 BrSecKeyDistState;

#define BRSES_KEYDIST_SEND_NONE     0x00
#define BRSES_KEYDIST_SEND_IRK      0x01
#define BRSES_KEYDIST_SEND_ID_ADDR  0x02
#define BRSES_KEYDIST_GEN_CSRK1     0x03
#define BRSES_KEYDIST_GEN_CSRK2     0x04
#define BRSES_KEYDIST_WAIT_INIT     0x05

/* End of BrSecKeyDistState */

/*---------------------------------------------------------------------------
 * SmpKeyOperation type
 *
 *     Describes the type of key generation operation that is being performed.
 *     These values map directly to the associated LESMP_EVENT_xxx values.
 */
typedef U8  SmpKeyOperation;

#define SMP_KEYOP_NONE              0x00
#define SMP_KEYOP_PAIRING_RANDOM    0x13    /* Pairing Random Number (128-bit) */
#define SMP_KEYOP_EDIV_RANDOM       0x14    /* Random Number (64-bit) */
#define SMP_KEYOP_LTK               0x16    /* Long-Term Key (LTK) */
#define SMP_KEYOP_CSRK              0x17    /* Connection Signature Resolving Key (CSRK) */
#define SMP_KEYOP_IRK               0x18    /* Identity Resolving Key (IRK) */
#define SMP_KEYOP_VERIFY_CONFIRM    0x19    /* Verify the received Confirm value */
#define SMP_KEYOP_PASSKEY           0x20    /* Passkey (48-bit or 6-digit) */

/* End of SmpKeyOperation */

/*---------------------------------------------------------------------------
 * LeSmpOpCode type
 *
 *     Security manager opcodes for LE.
 */
typedef U8 LeSmpOpCode;

#define SMP_NO_OPER                 0x00
#define SMP_PAIRING_REQUEST         0x01
#define SMP_PAIRING_RESPONSE        0x02
#define SMP_PAIRING_CONFIRM         0x03
#define SMP_PAIRING_RANDOM          0x04
#define SMP_PAIRING_FAILED          0x05
#define SMP_ENCRYPT_INFO            0x06
#define SMP_MASTER_IDENTIFICATION   0x07
#define SMP_IDENTITY_INFO           0x08
#define SMP_IDENTITY_ADDR_INFO      0x09
#define SMP_SIGNING_INFO            0x0A
#define SMP_SECURITY_REQUEST        0x0B
#define SMP_PUBLIC_KEY_EXCHANGE     0x0C
#define SMP_PAIRING_DHKEY_CHECK     0x0D

/* End of LeSmpOpCode */

/*---------------------------------------------------------------------------
 * LeSmpAuthRequirements type
 *
 *     Describes the authentication requirements, specifically whether
 *     Man in the Middle protection and/or Bonding is required during
 *     pairing. These values are ORed together.
 */
typedef U8 LeSmpAuthRequirements;

#define SMP_AUTH_REQ_NONE           0x00 /* No MITM or Bonding required */
#define SMP_AUTH_REQ_BONDING        0x01 /* Bonding requested */
#define SMP_AUTH_REQ_MITM_REQUIRED  0x04 /* Man in the Middle protection required */
#define SMP_AUTH_REQ_SECURE_CON     0x08 /* Inidicates secure connections is supported */
#define SMP_AUTH_REQ_KEYPRESS       0x10 /* Support of key press for secure connections */
#define SMP_AUTH_REQ_CT2            0x20 /* Support for h7 function */
#define SMP_AUTH_REQ_BITMASK        0x05 /* Bitmask of used authentication flags */

/* End of LeSmpAuthRequirements */

/*---------------------------------------------------------------------------
 * SmpSecurityState type
 *
 *     Describes the current state of the SMP security procedure
 */
typedef U8  SmpSecurityState;

/* Security channel is closed. */
#define SMP_STATE_CLOSED                    0x00

/* Security procedure is not in progress. */
#define SMP_STATE_IDLE                      0x01

/* The master device is on the pending queue waiting to send 
 * a Pairing Request command 
 */
#define SMP_STATE_WAIT_PAIRING_REQ          0x02

/* The slave device is on the pending queue waiting to tell
 * the upper layer about the Pairing Request
 */
#define SMP_STATE_WAIT_PAIRING_RSP          0x03

/* The slave device has requested security by sending the Security
 * Request command.  The master should initiate pairing (SmpPairingRequest)
 * or encrypt (SmpStartEncryption) based on the existence of saved
 * security keys (e.g. LTK).
 */
#define SMP_STATE_REQUEST_PAIRING           0x04

 /* The master device has received a security request from the slave */
#define SMP_STATE_SECURITY_REQ              0x05

/* The slave device has received a Pairing Request command and is waiting
 * for a Pairing Response to be issued.
 */
#define SMP_STATE_PAIRING_REQ               0x06

/* The master device has issued a Pairing Request command and is 
 * waiting for a Pairing Response to be received.
 */
#define SMP_STATE_PAIRING_RSP               0x07

/* The local device is expecting a Pairing Confirm to be received */
#define SMP_STATE_PAIRING_CONFIRM_RCV       0x08

/* The local device is expecting a Pairing Confirm to be sent */
#define SMP_STATE_PAIRING_CONFIRM_SND       0x09

/* The local device is expecting a Pairing Random to be received */
#define SMP_STATE_PAIRING_RANDOM_RCV        0x0A

/* The local device is expecting a Pairing Random to be sent */
#define SMP_STATE_PAIRING_RANDOM_SND        0x0B

/* In the process of generating the Short-Term Key (STK) to encrypt
 * the connection (Phase 2) using the Pairing Confirm and Pairing
 * Random commands
 */
#define SMP_STATE_STK_GENERATION            0x0C

/* Phase 2 has completed and encryption is in place */
#define SMP_STATE_ENCRYPTED                 0x0D

/* In the process of transport specific key distribution (Phase 3) to
 * exchange master/slave device security keys to enable reconnection,
 * data signing, and random address generation.
 */
#define SMP_STATE_KEY_DISTRIBUTION          0x0E

/* Waiting for encryption to complete (Phase 2) and any outstanding
 * transmit packet to be returned before allowing the transport specific
 * key distribution (Phase 3) to proceed.
 */
#define SMP_STATE_KEY_DISTRIBUTION_PENDING  0x0F

/* The local device has a link key from a previous pairing and is encrypting the link */
#define SMP_STATE_ENCRYPTING                0x10

/* The local device is expecting a public key exchange to be sent */
#define SMP_STATE_PUBLIC_KEY_SND            0x11

/* The local device is expecting a public key exchange to be received*/
#define SMP_STATE_PUBLIC_KEY_RCV            0x12

/* The local device is performing Secure Connections Numeric Compare authentication */
#define SMP_STATE_SC_AUTH_NUMERIC           0x13

/* The local device is performing Secure Connections Passkey entry authentication */
#define SMP_STATE_SC_AUTH_PASSKEY           0x14

/* The local device is performing Secure Connections OOB authentication */
#define SMP_STATE_SC_AUTH_OOB               0x15

/* The Local device has received or sent a pairing failed */
#define SMP_STATE_PAIRING_FAILED            0x16

/* The pairing is complete except for reading the remote's Central Addr Resolutionn */
#define SMP_STATE_READ_ADDR_RESOLUTION      0x17

/* End of SmpSecurityState */

/*---------------------------------------------------------------------------
 * SmpRandomGenerationState type
 *
 *     Describes the current state of the Random Value Generation
 */
typedef U8  SmpRandomGenerationState;

#define SMP_RANDOM_STATE_BEGIN          0x00        /* Beginning Random value generation */
#define SMP_RANDOM_STATE_GENERATING     0x01        /* Generating the random data */
#define SMP_RANDOM_STATE_END            0x02        /* Random value generated */
#define SMP_RANDOM_STATE_INVALID        0xff        /* Random state is invalid */

/* End of SmpRandomGenerationState */

/*---------------------------------------------------------------------------
 * SmpScSecurityState type
 *
 *     Describes the current state of the SMP security procedure for
 *     Secure Connections
 */
typedef U8  SmpScSecurityState;

/* Getting the first 8 bytes of a 128-bit random number */
#define SMP_SC_STATE_RANDOM_FIRST         0x00
#define SMP_SC_STATE_RANDOM_SECOND        0x01
#define SMP_SC_STATE_WAIT_CONFIRM         0x02
#define SMP_SC_STATE_SEND_CONFIRM         0x03
#define SMP_SC_STATE_WAIT_NONCE           0x04
#define SMP_SC_STATE_CMP_CONFIRM          0x05
#define SMP_SC_STATE_SHOW_NUM_CMP_VAL     0x06
#define SMP_SC_STATE_WAIT_USER_CONFIRM    0x07
#define SMP_SC_STATE_WAIT_DIFFIE_HELLMAN  0x08
#define SMP_SC_STATE_CALC_DHKEY_CHECK     0x09
#define SMP_SC_WAIT_DHKEY_CHECK           0x0A
#define SMP_SC_STATE_REM_DHKEY_CHECK      0x0B
#define SMP_SC_STATE_RANDPK               0x0D
#define SMP_SC_STATE_WAIT_PASSKEY         0x0E
#define SMP_SC_STATE_WAIT_CONFIRM_DHKEY   0x0F
#define SMP_SC_STATE_WAIT_PASSKEY_CONFIRM 0x10
#define SMP_SC_STATE_SHOW_NUM_CMP_VAL_WT  0x11

/* End of SmpScSecurityState */

/*---------------------------------------------------------------------------
 * LeSmpAddressType type
 *
 *     Describes the device address distributed during the
 *     Transport Specific Key Distribution phase.
 */
typedef U8 LeSmpAddressType;

#define SMP_PUBLIC_ADDRESS          0x00        /* Public device address */
#define SMP_STATIC_ADDRESS          0x01        /* Static random device address */

/* End of LeSmpAddressType */

/*---------------------------------------------------------------------------
 * LeSmpOobData type
 *
 *     Describes whether Out of Band data has been received from a remote
 *     device.
 */
typedef U8 LeSmpOobData;

#define SMP_OOB_DATA_NONE           0x00   /* No Out of Band Data        */
#define SMP_OOB_DATA_USED           0x01   /* Out of Band Data is used   */

/* End of LeSmpOobData */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SmpPairingExchange structure
 *
 *     SMP Pairing Request/Response command structure used during the
 *     Pairing Exchange Process.
 */
typedef struct _SmpPairingExchange {
    BtIoCapabilities            cap;
    LeSmpOobData                oobData;
    LeSmpAuthRequirements       authReq;
    U8                          maxKeySize;
    LeSmpKeyDistributionFormat  initiatorKeyDistrib;
    LeSmpKeyDistributionFormat  responderKeyDistrib;
} SmpPairingExchange;

/*---------------------------------------------------------------------------
 * SmpKeyInformation structure
 *
 *     Structure used to identify 128-bit SMP Security Key values.
 */
typedef struct _SmpKeyInformation {
    U8      value[BT_LE_KEY_LENGTH];      /* 128-bit key value */
} SmpKeyInformation;

/*---------------------------------------------------------------------------
 * SmpMasterIdentification structure
 *
 *     SMP Master Identification command structure.
 */
typedef struct _SmpMasterIdentification {
    U16     ediv;           /* Encrypted Diversifier (EDIV) */
    U8      rand[8];        /* 64-bit Random Number (Rand) */
} SmpMasterIdentification;

/*---------------------------------------------------------------------------
 * SmpIdentityAddress structure
 *
 *     SMP Identity Address command structure.
 */
typedef struct _SmpIdentityAddress {
    LeSmpAddressType    type;   /* Device Address Type */
    BD_ADDR             addr;   /* 48-bit device address (in Little Endian format) */
} SmpIdentityAddress;

/*---------------------------------------------------------------------------
 * SmpP256PublicKey structure
 *
 *     SMP P256 public key command structure.
 */
typedef struct _SmpP256PublicKey {
    U8        *key;        /* Pointer to the P256 pubic key */
} SmpP256PublicKey;

/*---------------------------------------------------------------------------
 * LeSmpOp structure
 *
 *     
 */
typedef struct _LeSmpOp {

    /* LeSmpOpCode */
    U8            opCode;

    /* Operation data */
    union {
        SmpPairingExchange          pairReq;        /* Pairing Request command */
        SmpPairingExchange          pairRsp;        /* Pairing Response command */
        SmpKeyInformation           pairRandom;     /* Pairing Random command */
        SmpKeyInformation           pairConfirm;    /* Pairing Confirm command */
        LeSmpPairingFailureReason   pairFailed;     /* Pairing Failed command */
        SmpKeyInformation           identityInfo;   /* Identity Information command */
        SmpKeyInformation           signingInfo;    /* Signing Information command */
        SmpKeyInformation           encryptInfo;    /* Encryption Information command */
        SmpIdentityAddress          addressInfo;    /* Identity Address Information command */
        SmpMasterIdentification     masterId;       /* Master Identification command */
        LeSmpAuthRequirements       securityReq;    /* Security Request command */
        LeSmpKeyDistributionFormat  distributeKeys; /* Key Distribution request */
        SmpP256PublicKey            publicKeyExch;  /* Public key exchange */
        SmpKeyInformation           pairDHKeyCheck; /* Pairing DHKey Check */
    } op;

    /* === Internal use only === */
    void          *opCtx;
    BtHandlerEx   *handler;

} LeSmpOp;

/*---------------------------------------------------------------------------
 *
 *     Smp Encryption Token is used when calling MELE_Encrypt() internally.
 */
typedef struct _SmpEncryptToken {
    /* Key data being passed to MELE_Encrypt */
    U8                key[BT_LE_KEY_LENGTH];
    /* Plain text data being passed to MELE_Encrypt */
    U8                plainText[16];
} SmpEncryptToken;

/*---------------------------------------------------------------------------
 * LeSmpEvent structure
 *
 *     Provides information to the LE SMP user about an LeSmp callback event.
 */
typedef struct _LeSmpEvent {

    /* LE SMP upper layer event. Always valid. */
    BtEventEx       e;

    /* Error code associated with event. */
    BtErrorCode     errCode;

    /* Remote Device associated with event. */
    BtRemoteDevice *remDev;

    /* Valid for LESMP_EVENT_PAIRING_REQ, LESMP_EVENT_PAIRING_RSP, LESMP_EVENT_PAIRING_CONFIRM,
     * LESMP_EVENT_PAIRING_FAILED, LESMP_EVENT_PAIRING_RANDOM, LESMP_EVENT_SECURITY_REQ,
     * LESMP_EVENT_MASTER_ID, LESMP_EVENT_x_INFO and LESMP_EVENT_PACKET_HANDLED
     * events.
     */
    LeSmpOp         smp;

    /* Valid for LESMP_EVENT_STK_VALUE,  
     * LESMP_EVENT_CONFIRM_VALUE, LESMP_EVENT_RANDOM_VALUE and
     * LESMP_EVENT_RANDOM_ADDR_PART events
     */
    struct {
        U8         *value;      /* Key Value */
        U8          len;        /* Key length (7-16 bytes for STK or LTK; 3 or 6 bytes for Random
                                 * Adddress Part, and 16 bytes for all others)
                                 */
    } stk, ltk, irk, csrk, confirm, random, randomAddrPart;

    /* Valid for LESMP_EVENT_PASSKEY_DISPLAY event */
    U32             passkey;    /* 6-digit randomly generated numeric passkey */

    /* Valid for LESMP_EVENT_PASSKEY_DISPLAY event. TRUE indicates that user
     * confirmation is required and SMP_UserConfirm() must be called. FALSE
     * indicates no user confirmation is required.
     */
    BOOL            requireConfirm;

    /* Valid for LESMP_EVENT_PACKET_HANDLED event. */
    BtPacket       *pkt;

    /* Valid for LESMP_EVENT_LOCAL_OOB_DATA */
    struct {
        U8        *randR;
        U8        *hashC;
    } oob;

    /* Valid for LESMP_CROSS_TRANS_KEY */
    BOOL           bonded;
    BtDevAddr      devAddr;

} LeSmpEvent;

/* -- LE Security Context Structure ------------------------------------------------
 *
 * This structure provides the context and state information for the LE Security
 * channel.
 */
typedef struct _BtLeSecContext {
    ListEntry    node;          /* For internal use to place context on a list */
    void                       *channel;
    U8                          iClient;
    BtHandlerEx                 secHandler;        /* Callback to handle L2CAP events */
    BOOL                        initiator;         /* Initiator of Security */
    BtPacket                    txPkt;             /* SMP transmit packet */
    BOOL                        txPktInUse;        /* Tracks transmit packet usage */
    BOOL                        waitRsp;           /* Waiting for a response from the remote device */
    SmpSecurityState            state;             /* Current SMP security state information */
    SmpScSecurityState          scState;           /* Current SMP Secure Connections state information */
    LeSmpOp                     op;                /* SMP operation (used by initiator/responder) */
    LeSmpOpCode                 command;           /* SMP command opcode */
    EvmTimer                    commandTimer;      /* SMP command timer */
    BtStatus                    status;            /* SMP command status */
    U8                          negKeySize;        /* Negotiated Encryption Key Size (7-16 bytes) */
    LeSmpAuthRequirements       negAuthReq;        /* Negotiated Auth requirements; */
    LeSmpKeyDistributionFormat  negInitiatorKeys;  /* Negotiated keys distributed by the initiator */
    LeSmpKeyDistributionFormat  negResponderKeys;  /* Negotiated keys distributed by the responder */
    LeSmpKeyDistributionFormat  rcvdKeys;          /* Received security keys distributed by the initiator/responder */
    SmpPairingExchange          rcvdPairInfo;      /* Received pairing information from remote device (pairing req/rsp) */
    MeLeOpHandler               leOpHandler;       /* Encrypt or Random number handler */
    MeLeOpHandler               leDhKeyHandler;    /* Diffie Hellman key generation handler */
    MeLeHandler                 leHandler;         /* Generic LE handler (used specifically for Start Encryption) */
    ListEntry                   smpTokens;         /* List of registered SMP tokens for the remote device */
    SmpEncryptToken             encrypt;           /* Structure used when calling MELE_Encrypt internally */
    LeSmpEvent                  event;             /* SMP event structure */
    BtErrorCode                 errCode;           /* Error code for the event to be indicated */

    /* Intermediate Key generation/Algorithm data */
    union {
        SmpKeyInformation       random;            /* Random Number from controller (64-bit and 128-bit) */
        SmpKeyInformation       encryptedData;     /* Result from controller encryption (128-bit) */
        U32                     passkey;           /* Result after generating random passkey */
    } generated;

    /* 128-bit received pairing confirm value (pending verification) */
    SmpKeyInformation           rcvdConfirmValue;

    /* 128-bit generated random value (LTK, CSRK, IRK, or Pairing Random) */
    SmpKeyInformation           randomValue;

    /* Determines if bonding is being performed */
    BOOL                        bonding;

    /* TRUE means that HCI has be configured to tie packet handled events to
     * HCI NUM complete packets
     */
    BOOL                        hciCompletedRet;

    /* 128-bit Temporary Key (TK) used when SMP_GenerateSTK is called */
    SmpKeyInformation           tempKey;

    /* Used for Secure Connection cryptographic operations to result of CMAC operation */
    U8                          cmacResult[16];

    /* SMP received Pairing Randomv value used for verification and STK generation */
    SmpKeyInformation           rcvdRandomValue;

    /* SMP generated Pairing Random value to be issued later */
    SmpKeyInformation           genRandomValue;

    /* Pairing request in big endian format without the pairing request command */
    U8                          bePairingReq[6];

    /* Pairing response in big endian format without the pairing response command */
    U8                          bePairingRsp[6];

    /* Identity address received from remote device */
    BtDevAddr                   remoteIdentityAddr;

    SmpRandomGenerationState    randomState; 
    SmpKeyOperation             keyOp; 

    U16                         encryptEdiv;
    U8                          encryptRand[8];
    U8                          encryptKey[16];

    /* Set to to TRUE if remote device supports Central Address Resolution, FALSE otherwise */
    BOOL                        supportAddrRes;

    /* If to TRUE it indicates the Just Works event should be generated */
    BOOL                        genJWevent;

} BtLeSecContext;

/* -- BR Security Context Structure ------------------------------------------------
 *
 * This structure provides the context and state information for the BR/EDR Security
 * channel.
 */
typedef struct _BtBrSecContext {
    ListEntry    node;          /* For internal use to place context on a list */
    void                       *channel;
    U8                          iClient;
    BtHandlerEx                 secHandler;        /* Callback to handle L2CAP events */
    BOOL                        initiator;         /* Initiator of Security */
    BtPacket                    txPkt;             /* SMP transmit packet */
    BOOL                        txPktInUse;        /* Tracks transmit packet usage */
    BOOL                        waitRsp;           /* Waiting for a response from the remote device */
    SmpSecurityState            state;             /* Current SMP security state information */
    BrSecKeyDistState           kdState;           /* Current key distribution state */
    LeSmpKeyDistributionFormat  rcvdKeys;          /* Received security keys distributed by the initiator/responder */
    EvmTimer                    commandTimer;      /* SMP command timer */
    MeLeOpHandler               leOpHandler;       /* Encrypt or Random number handler */
    LeSmpOp                     smpOp;             /* SMP operation (used by initiator/responder) */

    U8                          negKeySize;        /* Negotiated Encryption Key Size (7-16 bytes) */
    LeSmpKeyDistributionFormat  negInitiatorKeys;  /* Negotiated keys distributed by the initiator */
    LeSmpKeyDistributionFormat  negResponderKeys;  /* Negotiated keys distributed by the responder */
    LeSmpAuthRequirements       negAuthReq;        /* Negotiated Auth requirements; */

    SmpKeyInformation           remIdentityInfo;
    SmpKeyInformation           remSigningInfo;
    BtDevAddr                   remDevAddr;

   /* Set to to TRUE if remote device supports Central Address Resolution, FALSE otherwise */
    BOOL                        supportAddrRes;

} BtBrSecContext;


#include "sys/melei.h"

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * LESEC_GetContext()
 *
 *     Find the SEC context associated with the given BtRemoteDevice
 *
 * Parameters:
 *     RemDev - Pointer to BtRemoteDevice structure in which SEC context is
 *         is to be associated.
 *
 * Returns:
 *     Pointer to BtLeSecContext or 0 if RemDev is not valid.
 */
BtLeSecContext *LESEC_GetContext(BtRemoteDevice *RemDev);

/*---------------------------------------------------------------------------
 * LEATT_SendPdu()
 *
 *     Send an LE ATT PDU to the remote LE device.
 *
 * Parameters:
 *     Ctx - Pointer to the LeSec context.
 *     SmpOp - Pointer to the LeSmp opcode.
 *
 * Returns:
 *     TRUE or FALSE
 */
BOOL LESEC_SendPdu(BtLeSecContext *Ctx, LeSmpOp *SmpOp);

/*---------------------------------------------------------------------------
 * LESEC_Close()
 *
 *     Close an LE SMP channel connection.
 *
 * Parameters:
 *     ctx - Pointer to BtLeSecContext representing connection to remote
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
BtStatus LESEC_Close(BtLeSecContext *ctx);

/****************************** BRSEC Functions *****************************************/

/*---------------------------------------------------------------------------
 * BRSEC_RegisterHandler()
 *
 *     Register a handler to receive events for the BR/EDR fixed Security channel. 
 *     The callback in the handler will be called with LeSecEventType events
 *     and a pointer LeSecEvent structure.
 *
 *     To deregister the handler call this function with 0 as the handler.
 *
 * Parameters:
 *     handler- Pointer to BtHandlerEx structure with the handler->callback set to a 
 *         callback function. 
 *
 * Returns:
 *     A pointer to the previous registered handler or 0. BT_STATUS_SUCCESS - The operation completed successfully.
 *
 */

BtHandlerEx *BRSEC_RegisterHandler(BtHandlerEx *handler);


/*---------------------------------------------------------------------------
 * BRSEC_PairingRequest()
 *
 *     Initiates the pairing exchange procedure over the BR/EDR Security
 *     channel to the remote device specified by remDev.  
 *
 * Parameters:
 *     remDev- Pointer to the BR/EDR remote device structure. 
 *
 * Returns:
 *     BT_STATUS_PENDING - request is successful and pending a completion
 *         event.
 *
 *     BT_STATUS_FAILED - request failed because link is not valid, the
 *         data is unable to be sent, encryption is not enabled with a P-256
 *         key or the local controller does not support LE Secure connections. 
 *
 *     BT_STATUS_NOT_READY - incorrect internal state to issue this command.
 *
 *     BT_STATUS_IN_PROGRESS - an SMP operation is already in progress.
 *
 */
BtStatus BRSEC_PairingRequest(BrEdrRemoteDevice *remDev);

/* Helper functions */
void BrSecFinishPairingComplete(BtBrSecContext *ctx, BtStatus status);
void BrSecHandlePairingInfo(BtBrSecContext *ctx, BtStatus status);


#endif /* __LE_SEC_I_H */
