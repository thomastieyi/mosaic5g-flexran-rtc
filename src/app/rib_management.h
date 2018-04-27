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

/*! \file    rib_management.h
 *  \brief   app polling agents for connection management purposes
 *  \authors Robert Schmidt
 *  \company Eurecom
 *  \email   robert.schmidt@eurecom.fr
 */

#ifndef RIB_MANAGEMENT_H_
#define RIB_MANAGEMENT_H_

#include <chrono>

#include "periodic_component.h"
#include "rib_common.h"

namespace flexran {

  namespace app {

    namespace management {

      class rib_management : public periodic_component {

      public:

        rib_management(rib::Rib& rib, const core::requests_manager& rm)
	  : periodic_component(rib, rm),
            ms_counter(1),
            last_now(std::chrono::steady_clock::now()) {}

        void periodic_task();

      private:
        int ms_counter;
        std::chrono::steady_clock::time_point last_now;
        std::set<int> inactive_agents;

        void send_enb_config_request(int agent_id);
        void send_ue_config_request(int agent_id);
      };
    }
  }
}

#endif /* RIB_MANAGEMENT_H_ */
