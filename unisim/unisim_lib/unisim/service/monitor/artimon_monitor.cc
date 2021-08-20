#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_ARTIMON

#include <unisim/service/monitor/artimon_monitor.hh>
#include <unisim/service/monitor/artimon_monitor.tcc>

namespace unisim {
namespace service {
namespace monitor {

template
class ArtimonMonitor<uint32_t>;
template
class ArtimonMonitor<uint64_t>;

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

#endif // HAVE_ARTIMON

