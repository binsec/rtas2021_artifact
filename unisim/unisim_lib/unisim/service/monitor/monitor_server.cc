
#include <unisim/service/monitor/monitor_server.hh>
#include <unisim/service/monitor/monitor_server.tcc>

namespace unisim {
namespace service {
namespace monitor {

template
class MonitorServer<uint32_t>;
template
class MonitorServer<uint64_t>;

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

