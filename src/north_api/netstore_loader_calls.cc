/*
 * Copyright 2016-2020 FlexRAN Authors, Eurecom and The University of Edinburgh
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

/*! \file    netstore_loader_calls.cc
 *  \brief   NB Test API for NetStore trigger
 *  \authors Robert Schmidt
 *  \company Eurecom
 *  \email   robert.schmidt@eurecom.fr
 */

#include <pistache/http.h>

#include "rt_controller_common.h"
#include "flexran_log.h"
#include "netstore_loader_calls.h"

void flexran::north_api::netstore_loader_calls::register_calls(Pistache::Rest::Description& desc)
{
  auto netstore = desc.path("/netstore");
  netstore.route(desc.get("/app/:id"), "Trigger app download")
      .bind(&flexran::north_api::netstore_loader_calls::app_dl_trigger, this);
}

void flexran::north_api::netstore_loader_calls::app_dl_trigger(
    const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response)
{
  const std::string id = request.param(":id").as<std::string>();
  netstore->trigger_request(id);
  response.send(Pistache::Http::Code::Ok, "requested '" + id + "'\n");
}
