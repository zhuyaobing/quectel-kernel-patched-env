#ifndef SIGNALADAPTER_INTERNAL_H
#define SIGNALADAPTER_INTERNAL_H

#ifndef DOXYGEN_IGNORE
#include <map>
#include <vector>

#define LAPI_RT_SUPPORT_LIB "/etc/alex/libcslclapi_rtsupport.so"

namespace ecu {
namespace lapi {
namespace rt {
namespace _internal {

enum can_bus{
   can_bus_1,
   can_bus_2,
   can_bus_no_can,
};

enum can_variant{
   can_variant_ican,
   can_variant_fms,
   can_variant_nhmr,
   can_variant_man,
   can_variant_j1939,
   can_variant_vw,
   can_variant_no_can,
};

enum status_eval {
   status_eval_j1939,
   status_eval_datatype_max,
   status_eval_enum,
   status_eval_with_status,
   status_eval_none,
};

struct signal_def {
   const char* name;
   int data_type_bits;
   int raw_size_bits;
   double factor;
   unsigned int min;
   unsigned int max;
   int offset;
   const char* unit;
   status_eval status_evaluation;
};

struct signalgroup_def {
   unsigned int idx;
   const char* name;
   can_bus bus;
   can_variant variant;
   bool retain;
};

class SignalGroupsDefinition
{
public:
   using SignalGroupDef          = std::pair<signalgroup_def, std::vector<signal_def>>;
   using TopicSignalGroupDefPair = std::pair<std::string, SignalGroupDef>;
   using TopicSignalGroupDefMap  = std::map<TopicSignalGroupDefPair::first_type, TopicSignalGroupDefPair::second_type>;

   virtual ~SignalGroupsDefinition() { }
   virtual TopicSignalGroupDefMap get_topics_group_mapping() const = 0;
};

}}}} // end namespace

#endif /* ifndef DOXYGEN_IGNORE */


#endif /* SIGNALADAPTER_INTERNAL_H */
