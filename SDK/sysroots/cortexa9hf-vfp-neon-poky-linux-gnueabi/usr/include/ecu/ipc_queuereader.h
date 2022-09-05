#ifndef QUEUEREADER_H
#define QUEUEREADER_H

#include <fstream>

#include "ipc_signal.h"
#include "ipc_datasource.h"


namespace ecu {
namespace lapi {
namespace ipc {

template<std::size_t N = 8, class Signal = signal_t_dyn<N>>
class QueuePortReader :
   public IDataSource<IpcSignal>
{
public:
   typedef IDataConsumer_ptr<IpcSignal> Consumer;

   QueuePortReader(Consumer consumer);
   QueuePortReader(Consumer consumer, unsigned frame_start_id);
   virtual ~QueuePortReader();

   bool open(const std::string& rx_device);

   virtual void set_consumer(Consumer consumer) override
   {
      m_consumer = std::move(consumer);
   }

   virtual void receive_data() override;

   inline void set_frame_start_id(unsigned frame_start_id)
   { m_frame_start_id = frame_start_id; }

   void stop() { m_stop_reading=true; }

protected:

private:
   Consumer m_consumer;
   unsigned m_frame_start_id;
   int m_queue_rx_fd;
   bool m_stop_reading;

};

using QueueReader = QueuePortReader<>;


}}} // namespace

#endif /* QUEUEREADER_H */
