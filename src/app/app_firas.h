

#ifndef APP_FIRAS_H_
#define APP_FIRAS_H_

#include "component.h"
#include "rib_common.h"


#include <curl/curl.h>
#include <vector>
#include <chrono>

namespace flexran {

  namespace app {

    namespace management {

      class app_firas : public component {

      public:

        app_firas(const rib::Rib& rib, const core::requests_manager& rm,
            event::subscription& sub);
	~app_firas();
        void tick(uint64_t ms);
	CURLM* curl_easy_;
	CURL* curl_create_transfer(const std::string& addr/*,const std::string& data*/);
	//void trigger_send(const std::string &s);



     private:
        std::vector<std::string> elastic_search_ep_;


      
      };
    }
  }
}

#endif /* APP_FIRAS_H_ */
