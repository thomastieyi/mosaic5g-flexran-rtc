#include "rt_controller_common.h"
#include "app_firas.h"
#include "enb_rib_info.h"
#include "flexran_log.h"

#include <chrono>
#include <string>
#include <thread>
#include <curl/curl.h>
#include <regex>
#include <iostream>


flexran::app::management::app_firas::app_firas(const flexran::rib::Rib& rib,
    const flexran::core::requests_manager& rm, flexran::event::subscription& sub)
  : component(rib, rm, sub), 
    active_since_(std::chrono::system_clock::now()),
    sent_packets_(0)
{
      app_firas_ep_.push_back("localhost:8080"),
      event_sub_.subscribe_task_tick(
      boost::bind(&flexran::app::management::app_firas::tick, this, _1), 1000);
      curl_multi_ = curl_multi_init();
}


flexran::app::management::app_firas::~app_firas()

{
 
 curl_multi_cleanup(curl_multi_);
  
}


void flexran::app::management::app_firas::trigger_send()
{
  /* place a new transfer handle in curl's transfer queue */
  for (const std::string& addr : app_firas_ep_) {
    
    CURL *temp = curl_create_transfer("localhost:8080/list");
    curl_multi_add_handle(curl_multi_, temp);

    
  }
  /* actual transfer happens in process_curl() */
 
}

CURL *flexran::app::management::app_firas::curl_create_transfer(const std::string& addr)
{
   CURL *curl1; 
   curl1 = curl_easy_init();
    if (!curl1) {
    std::cerr << "curl_easy_init() failed\n";
    return 0;
  }
  //Request options
  curl_easy_setopt(curl1, CURLOPT_URL, addr.c_str());
  curl_easy_setopt(curl1, CURLOPT_VERBOSE, 1L);
 
  return curl1;
}
void flexran::app::management::app_firas::curl_release_handles()
{
  /* check finished transfers and remove/free the handles */
  CURLMsg *m;
  int n;
  do {
   m = curl_multi_info_read(curl_multi_, &n);
   if (m && m->msg == CURLMSG_DONE) {
     CURL *e = m->easy_handle;
     long code = 0;
     curl_easy_getinfo(e, CURLINFO_RESPONSE_CODE, &code);
     if (code == 200) /* if ok */
       sent_packets_ += 1;
     curl_multi_remove_handle(curl_multi_, e);
     curl_easy_cleanup(e);
   }
  } while (m);
}
void flexran::app::management::app_firas::process_curl(uint64_t tick)
{
  _unused(tick);

  int n;
  /* from documentation for curl_multi_perform(): "This function does not
   * require that there actually is any data available for reading or that data
   * can be written, it can be called just in case." */
  CURLMcode mc = curl_multi_perform(curl_multi_, &n);
  if (mc != CURLM_OK) {
    LOG4CXX_ERROR(flog::app, "CURL encountered a problem (" << mc << "), disabling logging");
   
  }

 curl_release_handles();
}


void flexran::app::management::app_firas::tick(uint64_t ms)
{
  _unused(ms);
  LOG4CXX_INFO(flog::app, "Handshaking" );
  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
  trigger_send();
  process_curl(ms);	
}





