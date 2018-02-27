#include <fstream>
#include <iostream>
#include "recorder.h"
#include "flexran_log.h"

void usage(char *pr_name)
{
  std::cerr << "parse-bd: parse binary dumps from the FlexRAN RTController recorder app.\n";
  std::cerr << "usage: " << pr_name << " <input> <output> [<type>]\n";
  std::cerr << "  <input>  is a binary file generated by the recorder app\n";
  std::cerr << "  <output> is a JSON output file\n";
  std::cerr << "  <type>   is an optional format of: all, enb, stats\n";
}

int main(int argc, char *argv[])
{
  std::string path = "";
  if(const char* env_p = std::getenv("FLEXRAN_RTC_HOME")) path = env_p;
  else path = "../";
  flexran_log::PropertyConfigurator::configure(path + "/log_config/basic_log");

  if (argc < 2 || argc > 4) {
    usage(argv[0]);
    return 1;
  }

  std::string input = argv[1];
  std::shared_ptr<std::vector<std::map<int, flexran::app::log::agent_dump>>> v;
  v = flexran::app::log::recorder::read_binary(input);
  LOG4CXX_INFO(flog::app, "read " << v.size() << " data sets");

  std::string file{argv[2]};
  flexran::app::log::job_info info{0, 0, file, flexran::app::log::job_type::all};
  if (argc == 4) {
    std::string format{argv[3]};
    if (format == "all")        info.type = flexran::app::log::job_type::all;
    else if (format == "enb")   info.type = flexran::app::log::job_type::enb;
    else if (format == "stats") info.type = flexran::app::log::job_type::enb;
    else {
      LOG4CXX_ERROR(flog::core, "parse-bd: illegal format " << format
          << ", using all");
    }
  }

  flexran::app::log::recorder::write_json(info, v);

  return 0;
}