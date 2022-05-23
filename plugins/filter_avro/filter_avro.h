/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Fluent Bit
 *  ==========
 *  Copyright (C) 2015-2022 The Fluent Bit Authors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef FLB_FILTER_AVRO_H
#define FLB_FILTER_AVRO_H

#include "avro/value.h"
#include <fluent-bit/flb_info.h>
#include <fluent-bit/flb_filter.h>
#include <fluent-bit/flb_sds.h>
#include <fluent-bit/flb_csv.h>
#include <avro.h>

struct filter_avro;

struct filter_avro_metadata {
    /* from env */
    flb_sds_t wd_platform;
    flb_sds_t wd_env_physical;
    flb_sds_t wd_dc_physical;
    flb_sds_t wd_env_logical;
    /* from schema */
    flb_sds_t wd_service;
    flb_sds_t wd_owner;
    flb_sds_t wd_datatype;
    flb_sds_t wd_objectname;
    flb_sds_t wd_solas;
    flb_sds_t wd_service_instance;
    flb_sds_t swh_server;
};

struct filter_avro_file_state {
    struct flb_csv_state state;
    char *filename;
    flb_sds_t row_buffer;
    avro_schema_t aschema;
    avro_value_iface_t *aclass;
    avro_value_t record;
    size_t record_field_index;
    flb_sds_t avro_schema_json;
    struct filter_avro *ctx;
    struct mk_list _head;
    struct filter_avro_metadata metadata;
};

struct filter_avro {
    struct mk_list states;
    bool convert_to_avro;
    struct flb_filter_instance *ins;
    avro_writer_t awriter;
    char *avro_write_buffer;
    size_t avro_write_buffer_size;
    avro_schema_t logev_schema;
    avro_schema_t meta_schema;
    avro_value_iface_t *logev_class;
    avro_value_iface_t *meta_class;
    avro_value_t meta_value;
    flb_sds_t packbuf;
    size_t payloads_total_size;
    char hostname[256];
    int max_records_per_request;
};

#endif
