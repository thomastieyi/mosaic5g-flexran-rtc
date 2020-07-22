

#ifndef APP_FIRAS_H_
#define APP_FIRAS_H_

#include "component.h"
#include "rib_common.h"

namespace flexran {

  namespace app {

    namespace management {

      class app_firas : public component {

      public:

        app_firas(const rib::Rib& rib, const core::requests_manager& rm,
            event::subscription& sub);
        void tick(uint64_t ms);

    

      
      };
    }
  }
}

#endif /* APP_FIRAS_H_ */
