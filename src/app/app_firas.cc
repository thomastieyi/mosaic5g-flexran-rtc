#include "rt_controller_common.h"
#include "app_firas.h"
#include "enb_rib_info.h"
#include "flexran_log.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <cstring>
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
  std::cout << cb_call << ": writing " << nmemb
            << "B, total " << bufpos << "B\n";
  return nmemb; // if buffer exceeded, reduced nmemb will trigger error in libcurl
}
void number_output() {
  std::vector<std::string> f;
  std::string s{buf};
  std::string delimiter = "\n";
  const char *path="/home/user/file.txt";	
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
  bufpos = 0;
}
int exist(const std::string& id){
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
     if(id ==si) {
	return 1;}	
    i++;
  } 
return 0;
  }




flexran::app::management::app_firas::app_firas(const flexran::rib::Rib& rib,
    const flexran::core::requests_manager& rm, flexran::event::subscription& sub)
  : component(rib, rm, sub), 
    active_since_(std::chrono::system_clock::now())
    
{
      event_sub_.subscribe_task_tick(
      boost::bind(&flexran::app::management::app_firas::tick, this, _1), 1000);

      curl_multi_ = curl_multi_init();

}


flexran::app::management::app_firas::~app_firas()

{
 disable_curl();
 curl_multi_cleanup(curl_multi_);
  
}


void flexran::app::management::app_firas::trigger_send(const std::string& addr)
{
  /* place a new transfer handle in curl's transfer queue */
  
    
    CURL *temp = curl_create_transfer(addr);
    
    curl_multi_add_handle(curl_multi_, temp);

    
  
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
  //curl_easy_setopt(curl1, CURLOPT_VERBOSE, 1L);		
  curl_easy_setopt(curl1, CURLOPT_WRITEFUNCTION, callback);
  
  return curl1;
}

void flexran::app::management::app_firas::process_curl(uint64_t tick)
{ 
  _unused(tick);
  CURLMsg *m;
  int k;
  

  int n;
  /* from documentation for curl_multi_perform(): "This function does not
   * require that there actually is any data available for reading or that data
   * can be written, it can be called just in case." */
  CURLMcode mc = curl_multi_perform(curl_multi_, &n);
  if (mc != CURLM_OK) {
    LOG4CXX_ERROR(flog::app, "erreur");
	disable_curl(); 
  }
 
 
  do {
   /*Ask the curl_multi_ which is a multi handle if there are a message from the individual transfers, 
    *and if this message is CURLMSG_DONE, we remove the easy_handle from the multi_handle */	
   m = curl_multi_info_read(curl_multi_, &k); 
   if (m && m->msg == CURLMSG_DONE) {
     CURL *e = m->easy_handle;
     long code = 0;
     	
     curl_easy_getinfo(e, CURLINFO_RESPONSE_CODE, &code);
    if (code != 200) /* if it is not ok */ 
	{bufpos = 0;
	curl_multi_remove_handle(curl_multi_, e);
 	
     
	return ;
	
 		}    
     curl_multi_remove_handle(curl_multi_, &e);
 	
     curl_easy_cleanup(e);
     
 
    number_output();	
     
   }
  } while (m);

 
}



void flexran::app::management::app_firas::tick(uint64_t ms)
{
  _unused(ms);
  	
 // LOG4CXX_INFO(flog::app, "Handshaking" );
 
  
// trigger_send("localhost:8080/list");
 /*tick_curl_ = event_sub_.subscribe_task_tick(
      boost::bind(&flexran::app::management::app_firas::process_curl, this, _1),
        10, 0);	*/
}
bool flexran::app::management::app_firas::disable_curl()
{
  if (tick_curl_.connected()) tick_curl_.disconnect();
  return true;
}


void flexran::app::management::app_firas::trigger_request(const std::string& id)
{ 
  LOG4CXX_INFO(flog::app, __func__ << "(): trigger for ID '" << id << "'");
  
  trigger_send("localhost:8080/list");
  tick_curl_ = event_sub_.subscribe_task_tick(
      boost::bind(&flexran::app::management::app_firas::process_curl, this, _1),
        10, 0);
	
 LOG4CXX_INFO(flog::app, exist(id) );
 if (exist(id)==1){
	std::cout <<"exist\n";
	trigger_send("localhost:8080/retrieve/"+id);
	tick_retrieve_ = event_sub_.subscribe_task_tick(
        boost::bind(&flexran::app::management::app_firas::process_curl, this, _1),
        10, 0);
  }
  else {
	std::cout <<" does not exist\n";
	std::ofstream MyFile("/home/nymphe/log.txt");
   
  }
}
