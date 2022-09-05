/****************************************************************************
 *
 * File:
 *     smpi.h (1d81bd0) 2017-05-16 16:48:57 +0000 Erik Berrio
 *     $Product: OpenSynergy Low Energy SDK v2.x $
 *     Commit: 1d81bd0b5088077c8cc7fc01dc69d3e485acac24
 *
 * Description:
 *     This file contains internal definitions for the Bluetooth
 *     LE Security Manager Protocol.
 *
 * Created:
 *     December 23, 2010
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

#ifndef __SMPI_H
#define __SMPI_H

#include "eventmgr.h"
#include "mele.h"
#include "l2cap_a.h"
#include "le_sec_i.h"

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

/*-------------------------------------------------------------------------
 * SMP_MAX_MTU_LE constant
 *
 *     Indicates the MTU that SMP will use over the LE link.  This value is
 *     dictated by the SMP specification.
 */
#define SMP_MAX_MTU_LE      23

/*---------------------------------------------------------------------------
 * SmpSecurityKey type
 *
 *     Describes the available SMP security keys that can be distributed.
 *
 *     Note: LTK, IRK, and CSRK values map directly to the
 *     LeSmpKeyDistributionFormat values.
 */
typedef U8  SmpSecurityKey;

#define SMP_KEY_LTK                 0x01      /* Long-Term Key (LTK) */
#define SMP_KEY_IRK                 0x02      /* Identity Resolving Key (IRK) */
#define SMP_KEY_EDIV_RAND           0x03      /* EDIV and Rand Key information describing LTK */
#define SMP_KEY_CSRK                0x04      /* Connection Signature Resolving Key (CSRK) */
#define SMP_KEY_ADDRESS             0x05      /* Address information describing IRK */

/* End of SmpSecurityKey */

/*---------------------------------------------------------------------------
 * SmpConfirmGenerationState type
 *
 *     Describes the current state of the Confirmation Value Generation
 */
typedef U8  SmpConfirmGenerationState;

#define SMP_CONFIRM_STATE_BEGIN         0x00        /* Beginning Confirm value generation */
#define SMP_CONFIRM_STATE_ENCRYPTING    0x01        /* Encrypted the data */
#define SMP_CONFIRM_STATE_END           0x02        /* Confirm value generated */

/* End of SmpConfirmGenerationState */



/*---------------------------------------------------------------------------
 * SmpScDHKeyState type
 *
 *     Describes the current state of the DHKey Generation for Secure Connections
 */
typedef U8 SmpScDHKeyState;

#define SMPSC_DHKEY_IDLE     0x00
#define SMPSC_DHKEY_PENDING  0x01
#define SMPSC_DHKEY_COMPLETE 0x02
#define SMPSC_DHKEY_NEED_NEW 0x03

/* End of SmpScDHKeyState */

/*---------------------------------------------------------------------------
 * LeSecurityStatus type
 *
 *     The LE Security Status identifies whether the stored security
 *     information for a particular device is sufficient.  If not, this
 *     status will describe what is insufficient.
 */
typedef U8 LeSecurityStatus;

#define LSS_OK                      0     /* Security is good enough, nothing needed */
#define LSS_OK_ENCRYPT              1     /* Security is good enough, but encryption is needed */
#define LSS_OK_ENCRYPTED            2     /* Security is good enough, but encryption refresh is needed */
#define LSS_OK_ENCRYPTING           3     /* Security is good enough, but encryption is needed and is already in progress */
#define LSS_OK_NO_SECURITY          4     /* Security is good enough, since none is required */
#define LSS_INVALID_KEY_STRENGTH    5     /* Key strength is insufficient */
#define LSS_MISSING_LTK             6     /* Device Record is missing the LTK */
#define LSS_INVALID_LTK             7     /* LTK value is improperly formed */
#define LSS_MISSING_CSRK            8     /* Device Record is missing the CSRK */

/* End of LeSecurityStatus */

/*---------------------------------------------------------------------------
* LeRepAttemptFlags type
*
*     The LE Repeated Attempt Flags are used as part of the repeated attempts
*     procedure used to prevent an intruder from repeating the pairing procedure
*     with a large number of different keys. The values are ORed together.
*/
typedef U8 LeRepAttemptFlags;

#define LEREPATT_FAIL_TIMER         0x01
#define LEREPATT_SUCCESS_TIMER      0x02

/* End of LeRepAttemptFlags */

/* Forward reference of SmpCmacToken. */
typedef struct _SmpCmacToken SmpCmacToken;

/*---------------------------------------------------------------------------
 * Callback used to notify a SMP layer that CMAC function is complete.
 */
typedef void (*SmpCmacComplete)(SmpCmacToken*);


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

 /*---------------------------------------------------------------------------
 *
 *     Smp Repeated Attempts Record.
 */
typedef struct _SmpRepAttRecord {
    BtDevAddr         devAddr;
    EvmTimer          timer;
} SmpRepAttRecord;


/*---------------------------------------------------------------------------
 *
 *     Smp CMAC Token is used when calling .
 */
struct _SmpCmacToken {
    /* Must be first item. Used internally by GATT */
    MeLeOpHandler     opHandler;

    /* Context area for use by caller */
    void             *context;

    /* Function to call when CMAC is complete */
    SmpCmacComplete   func;

    /**************** Input fields ************************************/
    /* Total amount of data pointed to by "data" */
    U16               dataLen;

    /* Points to the data in big endian format */
    U8               *data;

    /* Holds the key. Must be in big endian format */
    U8                key[16];

    /********************** Output Fields *****************************/
    /* Status of the operation */
    BtStatus          status;

    /* The result of the CMAC function */
    U8                mac[16];

    /* === Internal use only === */

    /* Last block of data. */
    U8                lastDataBlock[16];

     /* Tmp and scratch are used to hold temporary values in the calculation. */
    U8                scratch[16];

    /* n1 holds the amount of data in firstDataBlock (not including the counter) which
     * is used in the last bock sent to CMAC. 16 - n1 is the amount of data left which
     * must be combined with octets in data to form a complete block (in the case where
     * the dataLen+4 is greater than 16).
     */
    U8                n1;

    /* The number of rounds to perform in the data */
    U8                nData;

    /* The number of rounds to perform from the tmp buffer */
    U8                nTmp;

    /* Block of data being passed to MELE_Encrypt */
    U8                block[16];

    /* Current round */
    U8                i;

    /* Holds the state */
    U8                state;
};

/*---------------------------------------------------------------------------
 *
 * SmpContext contains all the data used internally by the SMP Entity.
 */

typedef struct _SmpContext {
    /* Current Encryption Diversifier (EDIV) that is an index for the Long-Term Key (LTK) */
    U16                         curEdivIndex;

    /* Used to indicate the security events asynchronously */
    EvmTask                     securityEventTask;

    /* I/O Capabilities for this device */
    BtIoCapabilities            smpIoCap;
    /* Authentication Requirements for this device */
    LeSmpAuthRequirements       smpAuthReq;

    /* Maximum key size supported on this device (7-16 bytes) */
    U8                          smpMaxKeySize;
    /* Keys to distribute during pairing */
    LeSmpKeyDistributionFormat  smpLocalKeyDistrib;
    LeSmpKeyDistributionFormat  smpRemoteKeyDistrib;

    /* Handler for the LE pairing operation */
    BtHandlerEx                *smpSecPairingHandler;
    /* Handler for the LE authorzation operation */
    BtHandlerEx                *leAuthorizeHandler;

    /* List of BtLeSecContext structures waiting to perform pairing */
    ListEntry                   pendingList;
    void                       *curCtx;

    /* Holds the generated or typed passky used by LE secure Connections  */
    U32                         scPasskey;

    /* Holds the passkey repetition count */
    U8                          scCount;

    /* Holds the current rai/rbi value */
    U8                          ri;

    /* DHKeyState indicates the state of key generation. */
    SmpScDHKeyState             DHKeyState;

    /* Scratch area in which to put values used for Secure Connection
     * cryptographic operations
     */
    U8                          scScratch[80];

    /* Remote device's public key */
    U8                          remotePublicKey[64];

    /* LE secure connections macKey */
    U8                          mackey[16];

    /* ra and rb used in LE secure Connections in little endian format */
    U8                          ra[16];
    U8                          rb[16];

    /* Holds LE Secure Connection Out of Band (OOB) data received from a remote device. */
    LeSecScOobData              remoteScOobData;

    /* Holds LE legacy Out of Band (OOB) data received from a remote device. */
    LeSecLegOobData             remoteLegOobData;

    /* TRUE indicates that the OOB mechanism is secure from eavesdropping
     * so the resulting key will be Authenticated. FALSE indicates that the
     * OOB mechanism is not secure from eavedropping or the level of 
     * eavesdropping protection is unknown so the resulting key will be
     * Unauthenticated.
     *
     * Note that when OOB data is present the MITM field in a pairing request 
     * and pairing response are set according to this flag, overriding the value 
     * set via SEC_SetAuthRequirements().  
     */
    BOOL                        oobSupportsMitm;


    /* Indicates whether or not LE Secure Connection OOB data has been received 
     * from a remote device.  
     */
    BOOL                        oobScRemoteDataRcv;

    /* Indicates whether or not LE Legacy Connection OOB data has been received
    * from a remote device.
    */
    BOOL                        oobLegRemoteDataRcv;

    /* Used for generating local data for LE Secure Connections OOB */
    struct {
        /* Handler used to generate a random number used for LE Secure
         * connections local OOB data 
         */
        MeLeOpHandler               randHandler;

        /* Holds the Secure Connections OOB generation state */
        U8                          state;

        /* Holds the random number generated for LE Legacy or LE Secure Connections
         * OOB data
         */
        U8                          rand[16];

        /* Holds the hashC value generated for LE Secure Connections OOB data */
        U8                          hashC[16];

        /* Scratch area in which to put values used for Secure Connection
         * cryptographic operations
         */
        U8                          scratch[65];
    } oob;

    /* LE secure Connections local IOCap in Big Endian Format used in the f6
     * cryptographic function
     */
    U8                          ioCapBE[3];

    /* Handler used to report results for the BR/EDR security channel */
    BtHandlerEx                *brHandler;

    /* The following are used for handling repeated attempts */
    MeLeOpHandler               readLocalP256Handler;
#if NUM_LE_DEVICES > 0
    SmpRepAttRecord             repAttRecords[LE_REPEATED_ATTEMPTS_NUM_RECORDS];
#endif
    EvmTimer                    repAttTimer;
    TimeT                       repAttTime;

    U8                          repAttErrCount;
    U8                          repAttSuccessCount;

    /* Used to set cause the local identity address to be sent as 0. 
     * Mainly used for testing
     */
    BOOL                        useZeroIdentityAddr;

    /* Used to control if the LESECE_CONFIRM_JW_REQ will be generated during
     * pairing */
    BOOL                        secGenJwEvent;

#if JETTEST == XA_ENABLED
    /* Override AuthReq in Pairing request and pairing response if non-0 */
    U8                          jtAuthReq;

    /* Cause Secure Connection Pairing to Fail based on the fail type */
    U8                          jtFailType;
#endif /* JETTEST == XA_ENABLED */
} SmpContext;




/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

BtStatus SMP_Init(void);
void     SMP_Deinit(void);
void SmpConvertToBigEndian(U8 *dest, const U8 *source, U16 length);
void SmpConvertToLittleEndian(U8 *dest, const U8 *source, U16 length);
void SmpReportEncryptionResult(BtRemoteDevice *remDev, BtErrorCode errCode);
BtStatus SmpGenRandomAddrHash(const U8* key, const U8* prand, U8 *out);
BtStatus SmpGenPrand(U8 *out);
BtStatus SmpStartOperation(void);
MeLeKeyType SmpGenerateKeyTypeSC(BOOL initiator, BtIoCapabilities initiatorCap,
                                 BtIoCapabilities responderCap, BOOL mitm,
                                 BOOL *generateKey, BOOL *passKey);
BtStatus SmpGenerateDHKey(void *secCtx); 
BOOL SmpStartAuthenticationSC(void *secCtx);
BOOL SmpScL2capHandler(void *secCtx, L2capEvent *Event);
BtStatus SmpScStartRand(void *secCtx);
void SmpGenCmacAes(U8 *key, U8 *msg, U16 msgLen, U8 *out);
void LeSecHandleAuthorizeRequest(BtRemoteDevice *remDev);
BtStatus GattSecurityAuthorize(BtRemoteDevice *remDev);
LeSecurityStatus LeSecIsSecurityRequired(BtRemoteDevice *remDev,
                                         LeSmpAuthRequirements authReq);
BOOL SmpScIsBrEdrCollision(BtLeSecContext *ctx, LeSmpAuthRequirements authReq);
void LESEC_Create(BtLeSecContext *Ctx);
void LESEC_InitFixedChannelClient(void *Client);
void LeSecClearStateInfo(BtLeSecContext *ctx);
void LeSecPairingComplete(BtLeSecContext *ctx, BtStatus status);
void LeSecFinishPairingComplete(BtLeSecContext *ctx, BtStatus status);
void SmpNotifyHandler(BtLeSecContext *ctx, LeSecEvent *leSecEvent, BtStatus status);
void LeSecStartCommandTimer(BtLeSecContext *ctx, LeSmpOpCode opCode);
void LeSecStopCommandTimer(BtLeSecContext *ctx);
void SmpScGenBrEdrKey(BtLeSecContext *ctx);
void SmpSetupRepeatedAttempts(MeLeRemoteDevice *leRemDev, BtStatus status);
BOOL SmpIsRepeatedAttemptsActive(MeLeRemoteDevice *leRemDev);
void SmpClearRepeatedAttempts(void);

void BRSEC_InitFixedChannelClient(void *c);
BtBrSecContext *BRSEC_GetContext(BrEdrRemoteDevice *RemDev);
BOOL BRSEC_IsLeCollision(BrEdrRemoteDevice *remDev);
void BRSEC_HandleL2capInfoComplete(BrEdrRemoteDevice *remDev, BOOL linkClose);
void SmpGenerateConfirmValue(BtLeSecContext *ctx, U8 *key, U8 *random);
BOOL SmpPairingConfirmed(BtLeSecContext *ctx);
BtStatus SmpGenerateRandomValue(BtLeSecContext  *ctx);
BtStatus SmpGenPairingConfirm(BtLeSecContext *ctx);
BOOL SmpPublicKeyExchange(BtLeSecContext *ctx);
BtStatus SmpDistributeKey(BtLeSecContext *ctx, SmpSecurityKey key);
BtStatus SmpDistributeKeys(BtLeSecContext *ctx);
BtStatus SmpStartEncryptionOp(BtLeSecContext *ctx);
BOOL SmpHandlePairingRequest(BtLeSecContext *ctx);
BOOL SmpHandleSecurityRequest(BtLeSecContext *ctx, LeSmpAuthRequirements authReq);
BtStatus SmpPairingRequest(BtRemoteDevice *remDev);
BtStatus SmpPairingResponse(BtRemoteDevice *remDev);
BtStatus SmpSecurityRequest(BtRemoteDevice *remDev);
BtStatus SmpPairingFailed(BtLeSecContext *ctx, LeSmpPairingFailureReason reason);
BOOL SmpGenLtkMacKey(BtLeSecContext *ctx);
BtStatus SmpStartEncryption(BtRemoteDevice *remDev, U16 ediv, U8 *rand, U8* key, U8 keySize);
void SmpScOobCallback(void *ctx, MeLeEvent *event);
void SmpCheckIdentityAddress(BtDevAddr *curAddr, BtDevAddr *devAddr);
void SmpGenIdAddr(BtDevAddr *devAddr);

#if LEL2CAP_TOT_NUM_DYNAMIC_CHNLS > 0
void LeSecProcessSecurityTokens(MeLeRemoteDevice *remDev);
void LeSecProcessFixHandlers(MeLeRemoteDevice *remDev, LeSecEvent *leSecEvent);
#endif

#if LL_PRIVACY == XA_ENABLED
void SmpAddToResolvingList(void *dc, MeLeRemoteDevice *leRemDev);
void SmpGenUniqueUndefAddr(BtDevAddr *devAddr);
#endif


#endif /* __SMPI_H */
