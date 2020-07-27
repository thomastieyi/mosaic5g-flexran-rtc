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




#define BUFSIZE 65536*2
int cb_call = 0;
char buf[BUFSIZE];
size_t bufpos = 0;

size_t callback(char *p, size_t , size_t nmemb, void *v) {
  if (nmemb + bufpos + 1 > BUFSIZE) {
	
    nmemb = BUFSIZE - bufpos - 1;
    std::cerr << "buffer size exceeded, limiting output to " << nmemb << "\n";
  }

  std::memcpy(&buf[bufpos], p, nmemb);
  bufpos += nmemb;
  buf[bufpos] = 0;

  cb_call++;
  std::cout << __func__ << ":" << cb_call << ": writing " << nmemb
            << "B, total " << bufpos << "B\n";
  	
  return nmemb; // if buffer exceeded, reduced nmemb will trigger error in libcurl
}

void number_output() {
  std::vector<std::string> f;
  std::string s{buf};
  std::string delimiter = "\n";

  size_t pos = 0;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    f.push_back(s.substr(0, pos));
    s.erase(0, pos + delimiter.length());
  }

  int i = 0;
  for (const std::string& si : f) {
    std::cout << i << ": " << si << "\n";
    i++;
  }
bufpos=0;
}
 




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

 

CURL *flexran::app::management::app_firas::curl_create_transfer(const std::string& addr)
{
CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  if (!curl) {
    std::cerr << "curl_easy_init() failed\n";
    return 0;
  }
  curl_easy_setopt(curl, CURLOPT_URL, addr.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
  /* Perform the request, res will get the return code */
  res = curl_easy_perform(curl);
  	
  /* Check for errors */	
  if (res != CURLE_OK)
    std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
  else
    std::cout << "curl_easy_perform() finished\n";
  number_output();
  /* always cleanup */
  curl_easy_cleanup(curl);
  return 0;

}
