/*
***************************************************************************************
***************************************************************************************
***
***     File: cproxy.h
***
***     Project: SRE Linux API
***
***     Copyright (C) Stoneridge Electronics AB, 1990 - $Date:: 2014#$
***
***     This file is a part of the Stoneridge Common Software Linux API
***     and may only be used in accordance with the terms and conditions stipulated
***     in the agreement contract under which it has been supplied.
***
***************************************************************************************
**************************************************************************************/
/** \file

   \ingroup    SRE Linux API

   \Subject    C Proxy to the shared linux api.

   \Reference

   \Abstract



   \Version Id:


   "$Id: cproxy.h 1236 2014-03-17 13:51:35Z STONERIDGE\PSM $"

\{
***************************************************************************************
*/
#ifndef CPROXY_H
#define CPROXY_H

#ifdef __cplusplus
extern "C" {
#endif

/*
**=====================================================================================
** Imported definitions
**=====================================================================================
*/


/*
**=====================================================================================
** Forward declarations, opaque pointer implementation
**=====================================================================================
*/
struct CProxy;
typedef struct CProxy* CProxyHndl;
typedef struct CTlv* CTlvHndl;

typedef void (*cproxy_notifyFnPtr)(void);

CProxyHndl cproxy_createHandle();
void cproxy_deleteHandle(CProxyHndl* proxy_hndl_ptr);


int cproxy_init(CProxyHndl proxy_hndl, const char* host_ptr, const char* port_ptr, cproxy_notifyFnPtr notify_fn);

int cproxy_sendTlv(CProxyHndl proxy_hndl, CTlvHndl tlv);

CTlvHndl cproxy_getNextTlv(CProxyHndl proxyHndl);

#ifdef __cplusplus
}
#endif

#endif //CPROXY_H
/*************************************************************************************/
/** \}
*********************************** End of file **************************************/
