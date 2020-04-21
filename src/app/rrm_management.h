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

/*! \file    rrm_management.h
 *  \brief   app is RRM calls helper (slice configuration, restart)
 *  \authors Robert Schmidt
 *  \company Eurecom
 *  \email   robert.schmidt@eurecom.fr
 */

#ifndef RRM_MANAGEMENT_H_
#define RRM_MANAGEMENT_H_

#include "component.h"
#include "rib_common.h"

#include <atomic>

namespace flexran {

  namespace app {

    namespace management {

      class rrm_management : public component {

      public:

        rrm_management(rib::Rib& rib, const core::requests_manager& rm,
            event::subscription& sub);

        void apply_slice_config_policy(const std::string& bs, const std::string& policy);
        void remove_slice(const std::string& bs, const std::string& policy);
        void change_ue_slice_association(const std::string& bs, const std::string& policy);
        bool apply_cell_config_policy(uint64_t bs_id, const std::string& policy,
            std::string& error_reason);

        uint64_t parse_enb_agent_id(const std::string& enb_agent_id_s) const;
        uint64_t get_last_bs() const;
        bool parse_rnti_imsi(uint64_t bs_id, const std::string& rnti_imsi_s,
            flexran::rib::rnti_t& rnti) const;

        static bool parse_imsi_list(const std::string& list,
            std::vector<uint64_t>& imsis, std::string& error_reason);

        int instantiate_vnetwork(uint64_t bps, std::string& error_reason);
        bool remove_vnetwork(uint32_t slice_id, std::string& error_reason);
        bool associate_ue_vnetwork(uint32_t slice_id, const std::string& policy,
            std::string& error_reason);
        int remove_ue_vnetwork(const std::string& policy, std::string& error_reason);
        int remove_ue_vnetwork(uint32_t slice_id);
        void ue_add_update(uint64_t bs_id, flexran::rib::rnti_t rnti);

        void reconfigure_agent_string(uint64_t bs_id, std::string policy);

      private:

        void push_cell_config_reconfiguration(uint64_t bs_id,
            const protocol::flex_cell_config& cell_config);
        void push_ue_config_reconfiguration(uint64_t bs_id,
            const protocol::flex_ue_config_reply& ue_config);
        static bool verify_ue_slice_assoc_msg(const protocol::flex_ue_config& c,
            std::string& error_message);
        bool verify_rnti_imsi(uint64_t bs_id, protocol::flex_ue_config *c,
            std::string& error_message);

        void verify_static_slice_configuration(const protocol::flex_slice_config& c);
        protocol::flex_slice_config transform_to_static_slice_configuration(
            const std::shared_ptr<flexran::rib::enb_rib_info> bs,
            const protocol::flex_slice_config& c);

        void verify_nvs_slice_configuration(const protocol::flex_slice_config& c);
        protocol::flex_slice_config transform_to_nvs_slice_configuration(
            const std::shared_ptr<flexran::rib::enb_rib_info> bs,
            const protocol::flex_slice_config& c);

        void verify_scn19_slice_configuration(const protocol::flex_slice_config& c);
        protocol::flex_slice_config transform_to_scn19_slice_configuration(
            const std::shared_ptr<flexran::rib::enb_rib_info> bs,
            const protocol::flex_slice_config& c);

        static bool verify_cell_config_for_restart(const protocol::flex_cell_config& c,
            std::string& error_message);
	
        static int calculate_rbs_percentage(int bw, uint64_t bps);
        bool is_free_common_slice_id(int slice_id) const;

        /// association IMSI -> slice_id
        std::map<uint64_t, uint32_t> ue_slice_;
      };
    }
  }
}

#endif /* RRM_MANAGEMENT_H_ */
