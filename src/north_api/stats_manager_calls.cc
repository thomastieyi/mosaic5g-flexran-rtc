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

/*! \file    stats_manager_calls.cc
 *  \brief   NB API for statistics information
 *  \authors Xenofon Foukas, Robert Schmidt
 *  \company Eurecom
 *  \email   x.foukas@sms.ed.ac.uk, robert.schmidt@eurecom.fr
 */

#include <pistache/http.h>
#include <pistache/http_header.h>

#include "stats_manager_calls.h"

void flexran::north_api::stats_manager_calls::register_calls(Pistache::Rest::Description& desc)
{
  /**
   * @api {get} /stats_manager/:type? Get RAN statistics (human-readable)
   * @apiName GetStatsHumanReadable
   * @apiGroup Stats
   * @apiParam {string=enb_config,mac_stats,all} [type=all] The type of
   * statistics to be returned. The following types are allowed:
   * * `enb_config`: static configuration (for eNB, UE, and LC)
   * * `mac_stats`:  statistics about various eNB layers (PDCP, RLC, MAC)
   * * `all`:        both of the above
   *
   * @apiDescription This API gets the RAN config and status for the current
   * TTI for all eNBs connected to this controller. The output is in a
   * human-readable format. For JSON output, see
   * (#Stats:GetStats).
   *
   * @apiVersion v0.1.0
   * @apiPermission None
   * @apiExample Example usage:
   *    curl -X GET http://127.0.0.1:9999/stats_manager/
   *
   * @apiExample Example usage:
   *    curl -X GET http://127.0.0.1:9999/stats_manager/enb_config
   *
   * @apiError BadRequest The given stats type is invalid.
   *
   * @apiErrorExample Error-Response:
   *    HTTP/1.1 400 BadRequest
   *    invalid statistics type
   */
  desc.route(desc.get("/stats_manager/:type?"),
                  "Get human-readable RAN config")
      .bind(&flexran::north_api::stats_manager_calls::obtain_stats, this);

  auto stats = desc.path("/stats");

  /**
   * @api {get} /stats/:type? Get RAN statistics in JSON
   * @apiName GetStats
   * @apiGroup Stats
   * @apiParam {string=enb_config,mac_stats,all} [type=all] The type of
   * statistics to be returned. The following types are allowed:
   * * `enb_config`: static configuration (for eNB, UE, and LC)
   * * `mac_stats`:  statistics about various eNB layers (PDCP, RLC, MAC)
   * * `all`:        both of the above
   *
   * @apiDescription This API gets the RAN config and status for the current
   * TTI for all eNBs connected to this controller. The output is in JSON
   * format. For human-readable output, see (#Stats:GetStatsHumanReadable).
   *
   * @apiVersion v0.1.0
   * @apiPermission None
   * @apiExample Example usage:
   *     curl -X GET http://127.0.0.1:9999/stats/
   * @apiSuccessExample Success-Response for one agent + UE connected, all type:
   *     HTTP/1.1 200 OK
   *     {
   *       "eNB_config": [
   *         {
   *           "agent_id": 0,
   *           "eNBId": 234881024,
   *           "eNB": {
   *             "header": {
   *               "version": 0,
   *               "type": 8,
   *               "xid": 0
   *             },
   *             "eNBId": "234881024",
   *             "cellConfig": [
   *               {
   *                 "phyCellId": 0,
   *                 "cellId": 0,
   *                 "puschHoppingOffset": 0,
   *                 "hoppingMode": 0,
   *                 "nSb": 1,
   *                 "phichResource": 0,
   *                 "phichDuration": 0,
   *                 "initNrPDCCHOFDMSym": 1,
   *                 "siConfig": {
   *                   "sfn": 548,
   *                   "sib1Length": 15,
   *                   "siWindowLength": 5
   *                 },
   *                 "dlBandwidth": 50,
   *                 "ulBandwidth": 50,
   *                 "ulCyclicPrefixLength": 0,
   *                 "dlCyclicPrefixLength": 0,
   *                 "antennaPortsCount": 1,
   *                 "duplexMode": 1,
   *                 "subframeAssignment": 0,
   *                 "specialSubframePatterns": 0,
   *                 "prachConfigIndex": 0,
   *                 "prachFreqOffset": 2,
   *                 "raResponseWindowSize": 7,
   *                 "macContentionResolutionTimer": 5,
   *                 "maxHARQMsg3Tx": 0,
   *                 "n1PUCCHAN": 0,
   *                 "deltaPUCCHShift": 1,
   *                 "nRBCqi": 0,
   *                 "srsSubframeConfig": 0,
   *                 "srsBwConfig": 0,
   *                 "srsMacUpPts": 0,
   *                 "enable64QAM": 0,
   *                 "carrierIndex": 0,
   *                 "dlFreq": 2685,
   *                 "ulFreq": 2565,
   *                 "eutraBand": 7,
   *                 "dlPdschPower": -27,
   *                 "ulPuschPower": -96,
   *                 "sliceConfig": {
   *                   "dl": [
   *                     {
   *                       "id": 0,
   *                       "label": "xMBB",
   *                       "percentage": 100,
   *                       "isolation": false,
   *                       "priority": 10,
   *                       "positionLow": 0,
   *                       "positionHigh": 25,
   *                       "maxmcs": 28,
   *                       "sorting": [
   *                         "CR_ROUND",
   *                         "CR_SRB12",
   *                         "CR_HOL",
   *                         "CR_LC",
   *                         "CR_CQI",
   *                         "CR_LCP"
   *                       ],
   *                       "accounting": "POL_FAIR",
   *                       "schedulerName": "schedule_ue_spec"
   *                     }
   *                   ],
   *                   "ul": [
   *                     {
   *                       "id": 0,
   *                       "label": "xMBB",
   *                       "percentage": 100,
   *                       "isolation": false,
   *                       "priority": 0,
   *                       "firstRb": 0,
   *                       "maxmcs": 20,
   *                       "accounting": "POLU_FAIR",
   *                       "schedulerName": "schedule_ulsch_rnti"
   *                     }
   *                   ],
   *                   "intrasliceShareActive": true,
   *                   "intersliceShareActive": true
   *                 }
   *               }
   *             ]
   *           },
   *           "UE": {
   *             "header": {
   *               "version": 0,
   *               "type": 10,
   *               "xid": 0
   *             },
   *             "ueConfig": [
   *               {
   *                 "rnti": 10337,
   *                 "timeAlignmentTimer": 7,
   *                 "transmissionMode": 0,
   *                 "ueAggregatedMaxBitrateUL": "0",
   *                 "ueAggregatedMaxBitrateDL": "0",
   *                 "capabilities": {
   *                   "halfDuplex": 0,
   *                   "intraSFHopping": 0,
   *                   "type2Sb1": 1,
   *                   "ueCategory": 4,
   *                   "resAllocType1": 1
   *                 },
   *                 "ueTransmissionAntenna": 2,
   *                 "ttiBundling": 0,
   *                 "maxHARQTx": 4,
   *                 "betaOffsetACKIndex": 0,
   *                 "betaOffsetRIIndex": 0,
   *                 "betaOffsetCQIIndex": 8,
   *                 "ackNackSimultaneousTrans": 0,
   *                 "simultaneousAckNackCqi": 0,
   *                 "aperiodicCqiRepMode": 3,
   *                 "ackNackRepetitionFactor": 0,
   *                 "pcellCarrierIndex": 0,
   *                 "imsi": "208940100001131",
   *                 "dlSliceId": 0,
   *                 "ulSliceId": 0
   *               }
   *             ]
   *           },
   *           "LC": {
   *             "header": {
   *               "version": 0,
   *               "type": 12,
   *               "xid": 2
   *             },
   *             "lcUeConfig": [
   *               {
   *                 "rnti": 10337,
   *                 "lcConfig": [
   *                   {
   *                     "lcid": 1,
   *                     "lcg": 0,
   *                     "direction": 2,
   *                     "qosBearerType": 0,
   *                     "qci": 1
   *                   },
   *                   {
   *                     "lcid": 2,
   *                     "lcg": 0,
   *                     "direction": 2,
   *                     "qosBearerType": 0,
   *                     "qci": 1
   *                   },
   *                   {
   *                     "lcid": 3,
   *                     "lcg": 1,
   *                     "direction": 1,
   *                     "qosBearerType": 0,
   *                     "qci": 1
   *                   }
   *                 ]
   *               }
   *             ]
   *           }
   *         }
   *       ],
   *       "mac_stats": [
   *         {
   *           "agent_id": 0,
   *           "eNBId": 234881024,
   *           "ue_mac_stats": [
   *             {
   *               "rnti": 10337,
   *               "mac_stats": {
   *                 "rnti": 10337,
   *                 "bsr": [
   *                   0,
   *                   0,
   *                   0,
   *                   0
   *                 ],
   *                 "phr": 31,
   *                 "rlcReport": [
   *                   {
   *                     "lcId": 1,
   *                     "txQueueSize": 0,
   *                     "txQueueHolDelay": 0,
   *                     "statusPduSize": 0
   *                   },
   *                   {
   *                     "lcId": 2,
   *                     "txQueueSize": 0,
   *                     "txQueueHolDelay": 0,
   *                     "statusPduSize": 0
   *                   },
   *                   {
   *                     "lcId": 3,
   *                     "txQueueSize": 0,
   *                     "txQueueHolDelay": 0,
   *                     "statusPduSize": 0
   *                   }
   *                 ],
   *                 "pendingMacCes": 0,
   *                 "dlCqiReport": {
   *                   "sfnSn": 10224,
   *                   "csiReport": [
   *                     {
   *                       "servCellIndex": 0,
   *                       "ri": 0,
   *                       "type": "FLCSIT_P10",
   *                       "p10csi": {
   *                         "wbCqi": 15
   *                       }
   *                     }
   *                   ]
   *                 },
   *                 "ulCqiReport": {
   *                   "sfnSn": 10224,
   *                   "cqiMeas": [
   *                     {
   *                       "type": "FLUCT_SRS",
   *                       "servCellIndex": 0
   *                     }
   *                   ],
   *                   "pucchDbm": [
   *                     {
   *                       "p0PucchDbm": 0,
   *                       "servCellIndex": 0
   *                     }
   *                   ]
   *                 },
   *                 "rrcMeasurements": {
   *                   "measid": -1,
   *                   "pcellRsrp": -1,
   *                   "pcellRsrq": -1
   *                 },
   *                 "pdcpStats": {
   *                   "pktTx": 102,
   *                   "pktTxBytes": 46622,
   *                   "pktTxSn": 101,
   *                   "pktTxW": 1,
   *                   "pktTxBytesW": 52,
   *                   "pktTxAiat": 16557,
   *                   "pktTxAiatW": 101,
   *                   "pktRx": 89,
   *                   "pktRxBytes": 20260,
   *                   "pktRxSn": 99,
   *                   "pktRxW": 1,
   *                   "pktRxBytesW": 90,
   *                   "pktRxAiat": 16512,
   *                   "pktRxAiatW": 28,
   *                   "pktRxOo": 0,
   *                   "sfn": "16627"
   *                 },
   *                 "macStats": {
   *                   "tbsDl": 57,
   *                   "tbsUl": 63,
   *                   "prbRetxDl": 0,
   *                   "prbRetxUl": 0,
   *                   "prbDl": 3,
   *                   "prbUl": 0,
   *                   "mcs1Dl": 28,
   *                   "mcs2Dl": 10,
   *                   "mcs1Ul": 10,
   *                   "mcs2Ul": 10,
   *                   "totalBytesSdusUl": 37962,
   *                   "totalBytesSdusDl": 47290,
   *                   "totalPrbDl": 658,
   *                   "totalPrbUl": 1435,
   *                   "totalPduDl": 74,
   *                   "totalPduUl": 314,
   *                   "totalTbsDl": 49923,
   *                   "totalTbsUl": 47484,
   *                   "macSdusDl": [
   *                     {
   *                       "sduLength": 56,
   *                       "lcid": 3
   *                     }
   *                   ],
   *                   "harqRound": 8
   *                 }
   *               },
   *               "harq": [
   *                 "ACK",
   *                 "ACK",
   *                 "ACK",
   *                 "ACK",
   *                 "ACK",
   *                 "ACK",
   *                 "ACK",
   *                 "ACK"
   *               ]
   *             }
   *           ]
   *         }
   *       ]
   *     }
   * @apiError BadRequest The given stats type is invalid.
   *
   * @apiErrorExample Error-Response:
   *     HTTP/1.1 400 BadRequest
   *     { "error": "invalid statistics type" }
   */
  stats.route(desc.get("/:type?"),
              "Get JSON RAN config for type")
       .bind(&flexran::north_api::stats_manager_calls::obtain_json_stats, this);

  /**
   * @api {get} /stats/enb/:id/:type? Get RAN statistics in JSON for one eNB
   * @apiName GetStatsEnb
   * @apiGroup Stats
   * @apiParam {Number} id The ID of the desired BS. This can be one of the
   * following: -1 (last added agent), the eNB ID (in hex, preceded by "0x", or
   * decimal) or the internal agent ID which can be obtained through a `stats`
   * call.  Numbers smaller than 1000 are parsed as the agent ID.
   * @apiParam {string=enb_config,mac_stats,all} [type=all] The type of
   * statistics to be returned. The following types are allowed:
   * * `enb_config`: static configuration (for eNB, UE, and LC)
   * * `mac_stats`:  statistics about various eNB layers (PDCP, RLC, MAC)
   * * `all`:        both of the above
   *
   * @apiDescription This API gets the RAN config and status for the current
   * TTI for a given eNB. The output is in JSON format. No human-readable
   * format exists corresponding to this endpoint.
   *
   * @apiVersion v0.1.0
   * @apiPermission None
   * @apiExample Example usage:
   *    curl -X GET http://127.0.0.1:9999/stats/enb/-1/
   * @apiExample Example usage:
   *    curl -X GET http://127.0.0.1:9999/stats/enb/234881037/mac_stats
   * @apiExample Example usage:
   *    curl -X GET http://127.0.0.1:9999/stats/enb/0xe000000/enb_config
   *
   * @apiError BadRequest The given statistics type or the eNB ID is invalid.
   * @apiErrorExample Error-Response:
   *     HTTP/1.1 400 BadRequest
   *     { "error": "invalid statistics type" }
   *
   * @apiErrorExample Error-Response:
   *     HTTP/1.1 400 BadRequest
   *     { "error": "invalid ID" }
   */
  stats.route(desc.get("/enb/:id/:type?"),
              "Get JSON RAN config for BS and type")
       .bind(&flexran::north_api::stats_manager_calls::obtain_json_stats_enb, this);

  /**
   * @api {get} /stats/ue/:id/:type? Get UE statistics in JSON
   * @apiName GetStatsUE
   * @apiGroup Stats
   * @apiParam {Number} id The ID of the UE in the form of either an RNTI or
   * the IMSI. Everything shorter than 6 digits will be treated as the RNTI,
   * the rest as the IMSI.
   * @apiParam {string=enb_config,mac_stats,all} [type=all] The type of
   * statistics to be returned. The following types are allowed:
   * * `enb_config`: static configuration (for eNB, UE, and LC)
   * * `mac_stats`:  statistics about various eNB layers (PDCP, RLC, MAC)
   * * `all`:        both of the above
   *
   * @apiDescription This API gets the UE statistics (`mac_stats`) for one UE
   * registered at any eNB managed by the controller. No
   * human-readable format exists corresponding to this endpoint.
   *
   * @apiVersion v0.1.0
   * @apiPermission None
   * @apiExample Example usage:
   *     curl -X GET http://127.0.0.1:9999/stats/ue/208940100001131
   *
   * @apiError BadRequest The given eNB ID is invalid.
   * @apiErrorExample Error-Response:
   *     HTTP/1.1 400 BadRequest
   *     { "error": "invalid ID" }
   */
  stats.route(desc.get("/ue/:id_ue"),
              "Gets UE statistics for a given UE")
       .bind(&flexran::north_api::stats_manager_calls::obtain_json_stats_ue, this);

  /**
   * @api {get} /stats/enb/:id_enb/ue/:id_ue Get UE statistics in JSON, delimited to a given eNB
   * @apiName GetStatsUELimited
   * @apiGroup Stats
   * @apiParam {Number} id_enb The ID of the desired BS. This can be one of the
   * following: -1 (last added agent), the eNB ID (in hex, preceded by "0x", or
   * decimal) or the internal agent ID which can be obtained through a `stats`
   * call.  Numbers smaller than 1000 are parsed as the agent ID.
   * @apiParam {number} id_ue The ID of the UE in the form of either an RNTI or
   * the IMSI. Everything shorter than 6 digits will be treated as the RNTI,
   * the rest as the IMSI.
   *
   * @apiDescription This API gets the UE statistics ("mac_stats") for one UE.
   * The search is restrained to a given eNB registered at the controller. No
   * human-readable format exists corresponding to this endpoint.
   *
   * @apiVersion v0.1.0
   * @apiPermission None
   * @apiExample Example usage:
   *     curl -X GET http://127.0.0.1:9999/stats/enb/234881037/ue/208940100001131
   *
   * @apiError BadRequest The given eNB ID or UE ID is invalid.
   * @apiErrorExample Error-Response:
   *     HTTP/1.1 400 BadRequest
   *     { "error": "invalid ID (eNB and/or UE)" }
   */
  stats.route(desc.get("/enb/:id_enb/ue/:id_ue"),
              "Get UE statistics for a UE on a BS")
       .bind(&flexran::north_api::stats_manager_calls::obtain_json_stats_ue, this);

  /**
   * @api {get} /stats/conf/enb/:id? Get statistics configuration
   * @apiName GetStatsConf
   * @apiGroup Stats
   * @apiParam (URL parameter) {Number} [id=-1] The ID of the desired BS. This
   * can be one of the following: -1 (last added agent), the eNB ID (in hex,
   * preceded by "0x", or decimal) or the internal agent ID which can be
   * obtained through a `stats` call.  Numbers smaller than 1000 are parsed as
   * the agent ID.
   *
   * @apiDescription This returns the active statistics configuration for a
   * given BS. The configuration governs which statistics are sent how often
   * from the BS to the controller. For a description of the returned
   * parameters, check the <a href="#api-Stats-SetStatsConf">SetStatsConf</a>
   * API endpoint.
   *
   * @apiVersion v0.1.0
   * @apiPermission None
   * @apiExample Example usage:
   *     curl -XGET http://127.0.0.1:9999/stats/conf/enb/
   * @apiSuccessExample Success-Response:
   * {
   *   "reqs": [
   *     {
   *       "reportFrequency": "FLSRF_PERIODICAL",
   *       "sf": 1000,
   *       "cellReports": [
   *         "FLCST_NOISE_INTERFERENCE"
   *       ],
   *       "ueReports": [
   *         "FLUST_PHR",
   *         "FLUST_DL_CQI",
   *         "FLUST_BSR",
   *         "FLUST_RLC_BS",
   *         "FLUST_MAC_CE_BS",
   *         "FLUST_UL_CQI",
   *         "FLUST_RRC_MEASUREMENTS",
   *         "FLUST_PDCP_STATS",
   *         "FLUST_MAC_STATS",
   *         "FLUST_GTP_STATS"
   *       ]
   *     }
   *   ]
   * }
   *
   * @apiError BadRequest The given eNB ID is invalid.
   * @apiErrorExample Error-Response:
   *     HTTP/1.1 400 BadRequest
   *     { "error": "unknown BS" }
   */
  stats.route(desc.get("/conf/enb/:id?/"),
              "Get the statistics configuration")
       .bind(&flexran::north_api::stats_manager_calls::get_stats_req, this);

  /**
   * @api {post} /stats/conf/enb/:id? Set statistics configuration
   * @apiName SetStatsConf
   * @apiGroup Stats
   * @apiParam (URL parameter) {Number} [id=-1] The ID of the desired BS. This
   * can be one of the following: -1 (last added agent), the eNB ID (in hex,
   * preceded by "0x", or decimal) or the internal agent ID which can be
   * obtained through a `stats` call.  Numbers smaller than 1000 are parsed as
   * the agent ID.
   * @apiParam (JSON parameter) {Object[]} reqs An array of individual
   * configurations.
   * @apiParam (JSON parameter) {String="FLSRF_PERIODICAL"} reqs[reportFrequency] Report
   * frequency type, only periodical reports are implemented at the moment.
   * @apiParam (JSON parameter) {Number{1-}} reqs[sf] The periodicity in subframes
   * (i.e., milliseconds).
   * @apiParam (JSON parameter) {String[]="FLCST_NOISE_INTERFERENCE"} reqs[cellReports] The
   * cell-level statistics. Implemented: noise/interference measurements.
   * @apiParam (JSON parameter) {String[]="FLUST_PHR","FLUST_DL_CQI","FLUST_UL_CQI",
   * "FLUST_BSR","FLUST_RLC_BS","FLUST_MAC_CE_BS","FLUST_RRC_MEASUREMENTS",
   * "FLUST_PDCP_STATS","FLUST_MAC_STATS","FLUST_GTP_STATS"}  reqs[ueReports] The
   * UE-level (per-UE) reports. Implemented: power headroom, DL/UL CQI, buffer
   * status report, RLC report, pending MAC Control Elements, RRC measurements,
   * PDCP statistics (e.g., packets bytes), MAC statistics (e.g., used PRBs),
   * GTP statistics (used tunnel identifiers).
   *
   * @apiDescription This sets a new statistics configuration for a
   * given BS. The configuration governs which statistics are sent how often
   * from the BS to the controller. The controller will delete all current
   * statistics configurations present in the BS and configure the given ones.
   * To modify an existing configuration, use the <a
   * href="#api-Stats-GetStatsConf">GetStatsConf</a> endpoint:
   * `curl localhost:9999/stats/conf/enb | jq . > stats.json`.
   *
   * @apiVersion v0.1.0
   * @apiPermission None
   * @apiExample Example usage:
   *     curl -XPOST http://127.0.0.1:9999/stats/conf/enb/ --data-binary @stats.json
   *
   * @apiError BadRequest The given eNB ID is invalid.
   * @apiErrorExample Error-Response:
   *     HTTP/1.1 400 BadRequest
   *     { "error": "unknown BS" }
   */
  stats.route(desc.post("/conf/enb/:id?/"),
              "Set the statistics configuration")
       .bind(&flexran::north_api::stats_manager_calls::set_stats_req, this);
}

void flexran::north_api::stats_manager_calls::obtain_stats(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
  const std::string type = request.hasParam(":type") ?
      request.param(":type").as<std::string>() : REQ_TYPE::ALL_STATS;

  std::string resp;
  if (type == REQ_TYPE::ALL_STATS) {
    resp = stats_app->all_stats_to_string();
  } else if (type == REQ_TYPE::ENB_CONFIG) {
    resp = stats_app->all_enb_configs_to_string();
  } else if (type == REQ_TYPE::MAC_STATS) {
    resp = stats_app->all_mac_configs_to_string();
  } else {
    response.send(Pistache::Http::Code::Bad_Request, "invalid statistics type\n");
    return;
  }
  response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");
  response.send(Pistache::Http::Code::Ok, resp);
}

void flexran::north_api::stats_manager_calls::obtain_json_stats(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
  const std::string type = request.hasParam(":type") ?
      request.param(":type").as<std::string>() : REQ_TYPE::ALL_STATS;
  std::string resp;
  if (type == REQ_TYPE::ALL_STATS) {
    resp = stats_app->all_stats_to_json_string();
  } else if (type == REQ_TYPE::ENB_CONFIG) {
    resp = stats_app->all_enb_configs_to_json_string();
  } else if (type == REQ_TYPE::MAC_STATS) {
    resp = stats_app->all_mac_configs_to_json_string();
  } else {
    response.send(Pistache::Http::Code::Bad_Request,
        "{ \"error\": \"invalid statistics type\"}", MIME(Application, Json));
    return;
  }
  response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");
  response.send(Pistache::Http::Code::Ok, resp, MIME(Application, Json));
}

void flexran::north_api::stats_manager_calls::obtain_json_stats_enb(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
  uint64_t bs_id = stats_app->parse_bs_agent_id(request.param(":id").as<std::string>());
  if (bs_id == 0) {
    response.send(Pistache::Http::Code::Bad_Request,
        "{ \"error\": \"can not find agent\" }", MIME(Application, Json));
    return;
  }

  const std::string type = request.hasParam(":type") ?
      request.param(":type").as<std::string>() : REQ_TYPE::ALL_STATS;

  std::string resp;
  if (type == REQ_TYPE::ALL_STATS) {
    stats_app->stats_by_bs_id_to_json_string(bs_id, resp);
  } else if (type == REQ_TYPE::ENB_CONFIG) {
    stats_app->enb_configs_by_bs_id_to_json_string(bs_id, resp);
  } else if (type == REQ_TYPE::MAC_STATS) {
    stats_app->mac_configs_by_bs_id_to_json_string(bs_id, resp);
  } else {
    response.send(Pistache::Http::Code::Bad_Request,
        "{ \"error\": \"invalid statistics type\" }", MIME(Application, Json));
    return;
  }

  response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");
  response.send(Pistache::Http::Code::Ok, resp, MIME(Application, Json));
}

void flexran::north_api::stats_manager_calls::obtain_json_stats_ue(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
  uint64_t bs_id = 0;
  const bool check_enb = request.hasParam(":id_enb");
  if (check_enb) {
    bs_id = stats_app->parse_bs_agent_id(request.param(":id_enb").as<std::string>());
    if (bs_id == 0) {
      response.send(Pistache::Http::Code::Bad_Request,
          "{ \"error\": \"can not find BS\" }", MIME(Application, Json));
      return;
    }
  }

  const std::string ue_id_s = request.param(":id_ue").as<std::string>();
  flexran::rib::rnti_t rnti;
  bool found = false;
  found = check_enb ? stats_app->parse_rnti_imsi(bs_id, ue_id_s, rnti) :
                      stats_app->parse_rnti_imsi_find_bs(ue_id_s, rnti, bs_id);
  if (!found) {
    response.send(Pistache::Http::Code::Bad_Request,
        "{ \"error\": \"invalid UE ID\" }", MIME(Application, Json));
    return;
  }

  /* at this point, both the correct bs_id and RNTI will be known */
  std::string resp;
  stats_app->ue_stats_by_rnti_by_bs_id_to_json_string(rnti, resp, bs_id);
  response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");
  response.send(Pistache::Http::Code::Ok, resp, MIME(Application, Json));
}

void flexran::north_api::stats_manager_calls::get_stats_req(
    const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
  std::string bs = "-1";
  if (request.hasParam(":id")) bs = request.param(":id").as<std::string>();

  std::string resp;
  if (!stats_app->get_stats_requests(bs, resp)) {
    response.send(Pistache::Http::Code::Bad_Request,
        "{ \"error\": \"" + resp +"\" }", MIME(Application, Json));
    return;
  }

  response.headers().add<Pistache::Http::Header::AccessControlAllowOrigin>("*");
  response.send(Pistache::Http::Code::Ok, resp, MIME(Application, Json));
}

void flexran::north_api::stats_manager_calls::set_stats_req(
    const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
  std::string bs = "-1";
  if (request.hasParam(":id")) bs = request.param(":id").as<std::string>();

  std::string policy = request.body();
  if (policy.length() == 0) {
    response.send(Pistache::Http::Code::Bad_Request,
        "{ \"error\": \"empty request body\" }\n", MIME(Application, Json));
    return;
  }

  std::string error_reason;
  if (!stats_app->set_stats_requests(bs, policy, error_reason)) {
    response.send(Pistache::Http::Code::Bad_Request,
        "{ \"error\": \"" + error_reason + "\" }\n", MIME(Application, Json));
    return;
  }
  response.send(Pistache::Http::Code::Ok, "");
}
