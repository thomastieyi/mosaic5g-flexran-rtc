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
#include <streambuf>

#define BUFSIZE 65536*2
int cb_call = 0;
char buf[BUFSIZE];
size_t bufpos = 0;
/*size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}*/
size_t callback(char *p, size_t , size_t nmemb, void *v) {
 if (nmemb + bufpos + 1 > BUFSIZE) {
   nmemb = BUFSIZE - bufpos - 1;
   std::cerr << "buffer size exceeded, limiting output to " << nmemb << "\n";
 }
 std::memcpy(&buf[bufpos], p, nmemb);
 bufpos += nmemb;
 buf[bufpos] = 0;
 cb_call++;
 //std::cout<<buf;
 //std::cout << cb_call << ": writing " << nmemb
 //  << "B, total " << bufpos << "B\n";
 return nmemb; // if buffer exceeded, reduced nmemb will trigger error in libcurl
}

bool flexran::app::management::app_firas::check_list(const std::string& id) {
  std::vector<std::string> f;
  std::string s{buf};
  std::string delimiter = "\n";
  size_t pos = 0;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    f.push_back(s.substr(0, pos));
    s.erase(0, pos + delimiter.length());
  }
  bufpos = 0;
  for (const std::string& si : f) {
    if (id == si) {
      LOG4CXX_INFO(flog::app, id << " exists");
      return true;
    }
  }
  LOG4CXX_INFO(flog::app, id << " does not exist");
  return false;
}

flexran::app::management::app_firas::app_firas(const flexran::rib::Rib& rib,
   const flexran::core::requests_manager& rm, flexran::event::subscription& sub)
   : component(rib, rm, sub), 
   active_since_(std::chrono::system_clock::now()) 
{
 curl_global_init(CURL_GLOBAL_DEFAULT);
 curl_multi_ = curl_multi_init();
}

flexran::app::management::app_firas::~app_firas() {
 if (tick_list_.connected())
   tick_list_.disconnect();
 if (tick_retrieve_.connected())
   tick_retrieve_.disconnect();	
 curl_multi_cleanup(curl_multi_);
 curl_global_cleanup();  
}

void flexran::app::management::app_firas::trigger_send(const std::string& addr) {
 /* place a new transfer handle in curl's transfer queue */
 CURL *temp = curl_create_transfer(addr);
 curl_multi_add_handle(curl_multi_, temp);
 
}




CURL *flexran::app::management::app_firas::curl_create_transfer(const std::string& addr) {
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

void flexran::app::management::app_firas::process_retrieve(uint64_t tick,const std::string& id)
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
   tick_retrieve_.disconnect();
   return;
  }	
 do {
  /*Ask the curl_multi_ which is a multi handle if there are a message from the individual transfers, 
   *and if this message is CURLMSG_DONE, we remove the easy_handle from the multi_handle */	
   m = curl_multi_info_read(curl_multi_, &k); 
   if (m && m->msg == CURLMSG_DONE) {
     CURL *e = m->easy_handle;
     long code = 0;
     curl_easy_getinfo(e, CURLINFO_RESPONSE_CODE, &code);
     if (code != 200){ /* if it is not ok */ 
       bufpos = 0;
       curl_multi_remove_handle(curl_multi_, e);
       return;
     }  
     for (uint64_t bs_id : rib_.get_available_base_stations()) { 	
	push_code(bs_id, id, buf,bufpos); 
	   
	}   
     curl_multi_remove_handle(curl_multi_, &e);
     curl_easy_cleanup(e);
     tick_retrieve_.disconnect();	
   }
 } 	
 while (m);
}

void flexran::app::management::app_firas::process_list(uint64_t tick,const std::string& id) { 
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
   tick_list_.disconnect();
   return;
 }
 do {
   /*Ask the curl_multi_ which is a multi handle if there are a message from the individual transfers, 
    *and if this message is CURLMSG_DONE, we remove the easy_handle from the multi_handle */	
   m = curl_multi_info_read(curl_multi_, &k); 
   if (m && m->msg == CURLMSG_DONE) {
     CURL *e = m->easy_handle;
     long code = 0;
     curl_easy_getinfo(e, CURLINFO_RESPONSE_CODE, &code);
     if (code != 200){ /* if it is not ok */ 
       bufpos = 0;
       curl_multi_remove_handle(curl_multi_, e);  
       return;
     }    
     curl_multi_remove_handle(curl_multi_, &e);
     curl_easy_cleanup(e);
     bool b = check_list(id);
     
      tick_list_.disconnect();
     if (b==true){
       
       trigger_send("localhost:8080/retrieve/"+id);
       tick_retrieve_ = event_sub_.subscribe_task_tick(
       boost::bind(&flexran::app::management::app_firas::process_retrieve, this, _1,id),
       1, 0); 
       
       
	 		 
     }
   }
 }
 while (m);
}

void flexran::app::management::app_firas::trigger_request(const std::string& id) { 
  LOG4CXX_INFO(flog::app, __func__ << "(): trigger for ID '" << id << "'");
  const std::string& _ID=id;
  trigger_send("localhost:8080/list");
  tick_list_ = event_sub_.subscribe_task_tick(
  boost::bind(&flexran::app::management::app_firas::process_list, this, _1,_ID),
  10, 0);
  	
}
void flexran::app::management::app_firas::push_code(uint64_t bs_id, std::string object_name, const char* data, size_t len )
{   
      
    protocol::flexran_message d_message;
    // Create control delegation message header
    protocol::flex_header *delegation_header(new protocol::flex_header);
    delegation_header->set_type(protocol::FLPT_DELEGATE_CONTROL);
    delegation_header->set_version(0);
    delegation_header->set_xid(0);
    protocol::flex_control_delegation *control_delegation_msg(new protocol::flex_control_delegation);
    control_delegation_msg->set_allocated_header(delegation_header);
    control_delegation_msg->set_delegation_type(protocol::FLCDT_MAC_DL_UE_SCHEDULER);
    control_delegation_msg->set_payload(data,len);
    control_delegation_msg->set_name(object_name);	
    // Create and send the flexran message
    d_message.set_msg_dir(protocol::INITIATING_MESSAGE);
    LOG4CXX_INFO(flog::app, object_name<<" sent to the agent\n");	
    d_message.set_allocated_control_delegation_msg(control_delegation_msg);
    req_manager_.send_message(bs_id, d_message); 	
    	
}

