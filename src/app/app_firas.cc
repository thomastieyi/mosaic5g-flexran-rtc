#include "rt_controller_common.h"
#include "app_firas.h"
#include "enb_rib_info.h"
#include "flexran_log.h"

#include <chrono>
#include <string>
#include <thread>
#include <curl/curl.h>
#include <regex>

/*flexran::app::log::app_firas::app_firas(const rib::Rib& rib,
    const core::requests_manager& rm, event::subscription& sub)
  : component(rib, rm, sub),
    active_since_(std::chrono::system_clock::now()),
    sent_packets_(0),
    freq_stats_ (100),
    freq_config_(5000),
    batch_stats_max_no_(100),
    batch_config_max_no_(5)
{
  app_firas_ep_.push_back("localhost:9200"),
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl_multi_ = curl_multi_init();
}
flexran::app::log::app_firas::~app_firas()
{
  disable_logging();
  curl_multi_cleanup(curl_multi_);
  curl_global_cleanup();
}
*/


flexran::app::management::app_firas::app_firas(const flexran::rib::Rib& rib,
    const flexran::core::requests_manager& rm, flexran::event::subscription& sub)
  : component(rib, rm, sub)
{
      elastic_search_ep_.push_back("localhost:8080"),
  event_sub_.subscribe_task_tick(
      boost::bind(&flexran::app::management::app_firas::tick, this, _1), 1000);
      curl_easy_ = curl_easy_init();
}


flexran::app::management::app_firas::~app_firas()

{
  curl_easy_cleanup(curl_easy_);
  
}







void flexran::app::management::app_firas::tick(uint64_t ms)
{
  _unused(ms);
  LOG4CXX_INFO(flog::app, "Handshaking" );
  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
  //trigger_send("/push/a");
   //trigger_send("/list");
	
  //curl_create_transfer("localhost:8080/push/a");
  curl_create_transfer("localhost:8080/list");	
  	
 

}

 


CURL *flexran::app::management::app_firas::curl_create_transfer(const std::string& addr/*,const std::string& data*/)
{

  CURL *curl;
  CURLcode res;
 
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, addr.c_str());
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
    curl_easy_setopt(curl, CURLOPT_POST, 0L);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
   /* provide lambda that swallows all output. operator+ converts to function
    * pointer which is necessary since we call a C library */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
      +[](char *, size_t, size_t nmemb, void *) { return nmemb; });
	



    /* example.com is redirected, so we tell libcurl to follow redirection */ 
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
  }
   return curl;

}
