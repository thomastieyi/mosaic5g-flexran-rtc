

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
	bool add_endpoint(const std::string& ep);
	int get_sent_packets() const { return sent_packets_; }
        void tick(uint64_t ms);
	CURLM* curl_multi_;
	CURL* curl_create_transfer(const std::string& addr);
	//void curl_release_handles();
	void trigger_send();
 	void process_curl(uint64_t tick);
	void trigger_request(const std::string& id);


     private:
        std::vector<std::string> app_firas_ep_;
	 std::chrono::system_clock::time_point active_since_;
	 int sent_packets_;
	
	
	bs2::connection tick_curl_;

      
      };
    }
  }
}

#endif /* APP_FIRAS_H_ */
