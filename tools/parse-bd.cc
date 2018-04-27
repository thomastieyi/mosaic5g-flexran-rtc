/*
 * Copyright 2016-2018 FlexRAN Authors, Eurecom and The University of Edinburgh
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * For more information about Mosaic5G:  contact@mosaic-5g.io
 */

/*! \file    parse-bd.cc
 *  \brief   utility that can convert binary recorder format to JSON
 *  \authors Robert Schmidt
 *  \company Eurecom
 *  \email   robert.schmidt@eurecom.fr
 */

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
  std::vector<std::map<int, flexran::app::log::agent_dump>> v;
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
