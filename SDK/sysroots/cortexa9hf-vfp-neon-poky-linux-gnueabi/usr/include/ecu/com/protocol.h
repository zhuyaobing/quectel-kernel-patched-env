#ifndef PROTOCOL_H
#define PROTOCOL_H

#define SRE_HEADER_LENGTH 18

namespace ecu {
namespace lapi {
namespace com {

/**
 *******************************************************************************
 *
 * @brief The different protocol types availble within the alexandra platfrom
 *
 *******************************************************************************
 */
enum ProtocolType {
   PROTOCOL_TYPE_NOT_USED        = 0,  /**< Not used */
   PROTOCOL_TYPE_SRE_PROTOBUF    = 1,  /**< default, sre internal profobuf */
   PROTOCOL_TYPE_VCP_PROTOBUF    = 2   /**< man, Vehicle Communication Protocol profobuf */
};

}}} // end namespace
#endif /* PROTOCOL_H */
