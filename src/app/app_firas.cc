#include "rt_controller_common.h"
#include "app_firas.h"
#include "enb_rib_info.h"
#include "flexran_log.h"

flexran::app::management::app_firas::app_firas(const flexran::rib::Rib& rib,
    const flexran::core::requests_manager& rm, flexran::event::subscription& sub)
  : component(rib, rm, sub)
{
  event_sub_.subscribe_task_tick(
      boost::bind(&flexran::app::management::rib_management::tick, this, _1), 1000);
}

void flexran::app::management::app_firas::tick(uint64_t ms)
{
  _unused(ms);
  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
  for (uint64_t bs_id: rib_.get_available_base_stations()) {
	LOG4CXX_INFO(flog::app, "Firas " );
    std::chrono::duration<float> inactive = now - rib_.get_bs(bs_id)->last_active();
    /* inactive for longer than 1.5s */
    if (inactive.count() >= 1.5) {
      LOG4CXX_WARN(flog::app, "ne marche pas" << inactive.count() << "s");
    } else {
      if (inactive_bs_.find(bs_id) != inactive_bs_.end()) {
        inactive_bs_.erase(bs_id);
        LOG4CXX_INFO(flog::app, "c bon " );
      }
    }
  }
}

