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

/*! \file    rib.cc
 *  \brief   Ran Information Base: the controller's view on all agents
 *  \authors Xenofon Foukas, Navid Nikaein, Robert Schmidt
 *  \company Eurecom
 *  \email   x.foukas@sms.ed.ac.uk, navid.nikaein@eurecom.fr,
 *           robert.schmidt@eurecom.fr
 */

#include "rib.h"
#include <algorithm>

void flexran::rib::Rib::add_pending_agent(int agent_id) {
  pending_agents_.insert(agent_id);
}

void flexran::rib::Rib::remove_pending_agent(int agent_id) {
  pending_agents_.erase(agent_id);
}

bool flexran::rib::Rib::agent_is_pending(int agent_id) {
  auto search = pending_agents_.find(agent_id);
  return search != pending_agents_.end();
}

void flexran::rib::Rib::new_eNB_config_entry(int agent_id) {
  eNB_configs_.insert(std::pair<int,
		      std::shared_ptr<enb_rib_info>>(agent_id,
						     std::shared_ptr<enb_rib_info>(new enb_rib_info(agent_id))));
}

bool flexran::rib::Rib::has_eNB_config_entry(int agent_id) const
{
  auto it = eNB_configs_.find(agent_id);
  return it != eNB_configs_.end();
}

void flexran::rib::Rib::remove_eNB_config_entry(int agent_id) {
  auto it = eNB_configs_.find(agent_id);

  if (it == eNB_configs_.end()) {
    return;
  } else {
    eNB_configs_.erase(it);
  }
}

void flexran::rib::Rib::eNB_config_update(int agent_id,
					  const protocol::flex_enb_config_reply& enb_config_update) {
  auto it = eNB_configs_.find(agent_id);

  if (it == eNB_configs_.end()) {
    return;
  } else {
    it->second->update_eNB_config(enb_config_update);
  }
}

void flexran::rib::Rib::ue_config_update(int agent_id,
					 const protocol::flex_ue_config_reply& ue_config_update) {
  auto it = eNB_configs_.find(agent_id);

  if (it == eNB_configs_.end()) {
    return;
  } else {
    it->second->update_UE_config(ue_config_update);
  }
}

void flexran::rib::Rib::ue_config_update(int agent_id,
					 const protocol::flex_ue_state_change& ue_state_change) {
  auto it = eNB_configs_.find(agent_id);

  if (it == eNB_configs_.end()) {
    return;
  } else {
    it->second->update_UE_config(ue_state_change);
  }
}

void flexran::rib::Rib::lc_config_update(int agent_id,
					 const protocol::flex_lc_config_reply& lc_config_update) {
  auto it = eNB_configs_.find(agent_id);

  if (it == eNB_configs_.end()) {
    return;
  } else {
    it->second->update_LC_config(lc_config_update);
  }
}

void flexran::rib::Rib::update_liveness(int agent_id) {
  auto it = eNB_configs_.find(agent_id);

  if (it == eNB_configs_.end()) {
    return;
  } else {
    it->second->update_liveness();
  }
}

void flexran::rib::Rib::set_subframe_updates(int agent_id,
			       const protocol::flex_sf_trigger& sf_trigger_msg) {
  auto it = eNB_configs_.find(agent_id);

  if (it == eNB_configs_.end()) {
    return;
  } else {
    it->second->update_subframe(sf_trigger_msg);
  }
}

void flexran::rib::Rib::mac_stats_update(int agent_id,
		      const protocol::flex_stats_reply& mac_stats_update) {
  auto it = eNB_configs_.find(agent_id);

  if (it == eNB_configs_.end()) {
    return;
  } else {
    it->second->update_mac_stats(mac_stats_update);
  }
}

std::set<int> flexran::rib::Rib::get_available_agents() const {
  std::set<int> agents;
  for (auto it : eNB_configs_) {
    agents.insert(it.first);
  }
  return agents;
}

std::shared_ptr<flexran::rib::enb_rib_info> flexran::rib::Rib::get_agent(int agent_id) const {
  auto it = eNB_configs_.find(agent_id);
  if (it != eNB_configs_.end()) {
    return it->second;
  }
  return std::shared_ptr<enb_rib_info>(nullptr);
}

void flexran::rib::Rib::dump_mac_stats() const {
  for (auto enb_config : eNB_configs_) {
    enb_config.second->dump_mac_stats();
  }
}

std::string flexran::rib::Rib::dump_all_mac_stats_to_string() const {

  std::string str;
  
  for (auto enb_config : eNB_configs_) {
    str += enb_config.second->dump_mac_stats_to_string();
    str += "\n";
  }

  return str;
}

std::string flexran::rib::Rib::dump_all_mac_stats_to_json_string() const
{
  std::vector<std::string> mac_stats;
  mac_stats.reserve(eNB_configs_.size());
  std::transform(eNB_configs_.begin(), eNB_configs_.end(), std::back_inserter(mac_stats),
      [] (const std::pair<int, std::shared_ptr<enb_rib_info>>& enb_config)
      { return enb_config.second->dump_mac_stats_to_json_string(); }
  );

  return format_mac_stats_to_json(mac_stats);
}

bool flexran::rib::Rib::dump_mac_stats_by_agent_id_to_json_string(int agent_id, std::string& out) const
{
  auto it = eNB_configs_.find(agent_id);
  if (it == eNB_configs_.end()) return false;

  out = format_mac_stats_to_json(std::vector<std::string>{it->second->dump_mac_stats_to_json_string()});
  return true;
}

std::string flexran::rib::Rib::format_mac_stats_to_json(
    const std::vector<std::string>& mac_stats_json)
{
  std::string str;
  str += "\"mac_stats\":[";
  for (auto it = mac_stats_json.begin(); it != mac_stats_json.end(); it++) {
    if (it != mac_stats_json.begin()) str += ",";
    str += "{";
    str += *it;
    str += "}";
  }
  str += "]";
  return str;
}

void flexran::rib::Rib::dump_enb_configurations() const {
  for (auto eNB_config : eNB_configs_) {
    eNB_config.second->dump_configs();
  }
}

std::string flexran::rib::Rib::dump_all_enb_configurations_to_string() const {

  std::string str;

  for (auto eNB_config : eNB_configs_) {
    str += eNB_config.second->dump_configs_to_string();
    str += "\n";
  }

  return str;
}

std::string flexran::rib::Rib::dump_all_enb_configurations_to_json_string() const
{
  std::vector<std::string> enb_configurations;
  enb_configurations.reserve(eNB_configs_.size());
  std::transform(eNB_configs_.begin(), eNB_configs_.end(), std::back_inserter(enb_configurations),
      [] (const std::pair<int, std::shared_ptr<enb_rib_info>>& enb_config)
      { return enb_config.second->dump_configs_to_json_string(); }
  );

  return format_enb_configurations_to_json(enb_configurations);
}

bool flexran::rib::Rib::dump_enb_configurations_by_agent_id_to_json_string(int agent_id, std::string& out) const
{
  auto it = eNB_configs_.find(agent_id);
  if (it == eNB_configs_.end()) return false;

  out = format_enb_configurations_to_json(std::vector<std::string>{it->second->dump_configs_to_json_string()});
  return true;
}

std::string flexran::rib::Rib::format_enb_configurations_to_json(
    const std::vector<std::string>& enb_configurations_json)
{
  std::string str;
  str += "\"eNB_config\":[";
  for (auto it = enb_configurations_json.begin(); it != enb_configurations_json.end(); it++) {
    if (it != enb_configurations_json.begin()) str += ",";
    str += "{";
    str += *it;
    str += "}";
  }
  str += "]";
  return str;
}

bool flexran::rib::Rib::dump_ue_by_rnti_by_agent_id_to_json_string(rnti_t rnti, std::string& out, int agent_id) const
{
  auto it = eNB_configs_.find(agent_id);
  if (it == eNB_configs_.end()) return false;
  return it->second->dump_ue_spec_stats_by_rnti_to_json_string(rnti, out);
}

int flexran::rib::Rib::get_agent_id(uint64_t enb_id) const
{
  auto it = find_agent(enb_id);
  if (it == eNB_configs_.end()) return -1;
  return it->first;
}

int flexran::rib::Rib::parse_enb_agent_id(const std::string& enb_agent_id_s) const
{
  if (enb_agent_id_s == "-1") {
    return eNB_configs_.empty() ? -1 : std::prev(eNB_configs_.end())->first;
  }
  uint64_t enb_id;
  if (enb_agent_id_s.length() >= AGENT_ID_LENGTH_LIMIT && enb_agent_id_s.substr(0, 2) == "0x") {
    /* it is in long form and hex -> it must be an eNodeB ID */
    try {
      enb_id = std::stoll(enb_agent_id_s, 0, 16);
    } catch (std::invalid_argument e) {
      return -1;
    }
    return get_agent_id(enb_id);
  }

  try {
    enb_id = std::stoll(enb_agent_id_s);
  } catch (std::invalid_argument e) {
    return -1;
  }
  if (enb_agent_id_s.length() >= AGENT_ID_LENGTH_LIMIT)
    return get_agent_id(enb_id);
  /* it is short -> it is sure it fits in an int */
  if (!get_agent(enb_id)) return -1;
  return enb_id;
}

std::map<int, std::shared_ptr<flexran::rib::enb_rib_info>>::const_iterator
flexran::rib::Rib::find_agent(uint64_t enb_id) const
{
  return std::find_if(eNB_configs_.begin(), eNB_configs_.end(),
      [enb_id] (const std::pair<int, std::shared_ptr<enb_rib_info>>& enb_config)
      { return enb_id == enb_config.second->get_enb_config().enb_id(); }
  );
}
