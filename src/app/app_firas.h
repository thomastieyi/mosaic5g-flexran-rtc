

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
	
        //void tick(uint64_t ms);
	CURLM* curl_multi_;
	CURL* curl_create_transfer(const std::string& addr);
        
	void trigger_send(const std::string& addr);
 	void process_retrieve(uint64_t tick,const std::string& id);
	void trigger_request(const std::string& id);
	void process_list(uint64_t tick,const std::string& id);
	void push_code(uint64_t bs_id, std::string object_name, const char* data, size_t len);
	
	
     private:
        std::vector<std::string> app_firas_ep_;
	std::chrono::system_clock::time_point active_since_;
	 
	
	
	bs2::connection tick_list_;
	bs2::connection tick_retrieve_;

        bool check_list(const std::string& id);
      
      };
    }
  }
}

#endif /* APP_FIRAS_H_ */
