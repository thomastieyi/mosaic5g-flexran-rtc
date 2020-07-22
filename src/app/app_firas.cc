#include "rt_controller_common.h"
#include "app_firas.h"
#include "enb_rib_info.h"
#include "flexran_log.h"

flexran::app::management::app_firas::app_firas(const flexran::rib::Rib& rib,
    const flexran::core::requests_manager& rm, flexran::event::subscription& sub)
  : component(rib, rm, sub)
{
  event_sub_.subscribe_task_tick(
      boost::bind(&flexran::app::management::app_firas::tick, this, _1), 1000);
}

void flexran::app::management::app_firas::tick(uint64_t ms)
{
  _unused(ms);
  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
  LOG4CXX_INFO(flog::app, "Handshaking" );	
 
}

