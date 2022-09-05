#ifndef QUEUEWRITER_H
#define QUEUEWRITER_H

#include <fstream>

#include "ipc_signal.h"

namespace ecu {
namespace lapi {
namespace ipc {

template<std::size_t N = 8, class Signal = signal_t_dyn<N>>
class QueuePortWriter
{
public:
   QueuePortWriter();
   virtual ~QueuePortWriter();

   bool open(const std::string& tx_device);

   bool transmit_data(const IpcSignal& can_signal);
protected:

private:
   std::string m_queue_tx_dev;
   int m_queue_tx_fd;
};

using QueueWriter=QueuePortWriter<>;


}}} // namespace

#endif /* QUEUEWRITER_H */
