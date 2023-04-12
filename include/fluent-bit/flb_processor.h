/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Fluent Bit
 *  ==========
 *  Copyright (C) 2015-2023 The Fluent Bit Authors
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

#ifndef FLB_PROCESSOR_H
#define FLB_PROCESSOR_H

#include <fluent-bit/flb_info.h>
#include <fluent-bit/flb_sds.h>
#include <fluent-bit/flb_config.h>
#include <fluent-bit/flb_config_map.h>
#include <fluent-bit/flb_config_format.h>

#define FLB_PROCESSOR_LOGS      1
#define FLB_PROCESSOR_METRICS   2
#define FLB_PROCESSOR_TRACES    4

/* Type of processor unit: 'pipeline filter' or 'native unit' */
#define FLB_PROCESSOR_UNIT_NATIVE    0
#define FLB_PROCESSOR_UNIT_FILTER    1

struct flb_processor_unit {
    int event_type;
    int unit_type;
    flb_sds_t name;

    /*
     * Opaque data type for custom reference (for pipeline filters this
     * contains the filter instance context.
     */
    void *ctx;

    /*
     * pipeline filters needs to be linked somewhere since the destroy
     * function will do the mk_list_del(). To avoid corruptions we link
     * normal filters here, this list is never iterated or used besides
     * for this purpose.
     */
    struct mk_list unused_list;

    /* link to struct flb_processor->(logs, metrics, traces) list */
    struct mk_list _head;

    /* link to parent processor */
    void *parent;
};

struct flb_processor {
    int is_active;

    /* user-defined processor name */
    flb_sds_t name;

    /* lists for different types */
    struct mk_list logs;
    struct mk_list metrics;
    struct mk_list traces;

    /*
     * opaque data type to reference anything specific from the caller, for input
     * plugins this will contain the input instance context.
     */
    void *data;

    /* Fluent Bit context */
    struct flb_config *config;
};


struct flb_processor *flb_processor_create(struct flb_config *config, char *name, void *data);

int flb_processor_is_active(struct flb_processor *proc);

int flb_processor_init(struct flb_processor *proc);
void flb_processor_destroy(struct flb_processor *proc);

int flb_processor_run(struct flb_processor *proc,
                      int type,
                      const char *tag, size_t tag_len,
                      void *data, size_t data_size,
                      void **out_buf, size_t *out_size);


struct flb_processor_unit *flb_processor_unit_create(struct flb_processor *proc,
                                                     int event_type,
                                                     char *unit_name);
void flb_processor_unit_destroy(struct flb_processor_unit *pu);
int flb_processor_unit_set_property(struct flb_processor_unit *pu, const char *k, const char *v);

int flb_processors_load_from_config_format_group(struct flb_processor *proc, struct flb_cf_group *g);




#include <ctraces/ctraces.h>
#include <cmetrics/cmetrics.h>
#include <cmetrics/cmt_counter.h>

struct flb_input_instance;
struct flb_native_processor_instance;

struct flb_native_processor_plugin {
    int event_type;
    int flags;             /* Flags (not available at the moment */
    char *name;            /* Processor short name               */
    char *description;     /* Description                        */

    /* Config map */
    struct flb_config_map *config_map;

    /* Callbacks */
    int (*cb_init) (struct flb_native_processor_instance *,
                    struct flb_config *,
                    void *);

    int (*cb_process_logs) (const void *, size_t,
                            const char *, int,
                            void **, size_t *,
                            struct flb_native_processor_instance *,
                            struct flb_input_instance *,
                            void *, struct flb_config *);
    int (*cb_process_metrics) (struct cmt *,
                               const char *, int,
                               void **, size_t *,
                               struct flb_native_processor_instance *,
                               struct flb_input_instance *,
                               void *, struct flb_config *);

    int (*cb_process_traces) (struct ctrace *,
                              const char *, int,
                              void **, size_t *,
                              struct flb_native_processor_instance *,
                              struct flb_input_instance *,
                              void *, struct flb_config *);

    int (*cb_exit) (void *, struct flb_config *);

    struct mk_list _head;  /* Link to parent list (config->filters) */
};

struct flb_native_processor_instance {
    int event_type;
    int id;                                /* instance id              */
    int log_level;                         /* instance log level       */
    char name[32];                         /* numbered name            */
    char *alias;                           /* alias name               */
    void *context;                         /* Instance local context   */
    void *data;
    struct flb_native_processor_plugin *p; /* original plugin          */
    struct mk_list properties;             /* config properties        */
    struct mk_list *config_map;            /* configuration map        */

    struct mk_list _head;                  /* link to config->filters  */

    /*
     * CMetrics
     * --------
     */
    struct cmt *cmt;                      /* parent context               */

    /* Keep a reference to the original context this instance belongs to */
    struct flb_config *config;
};

static inline int flb_native_processor_config_map_set(
                    struct flb_native_processor_instance *ins,
                    void *context)
{
    return flb_config_map_set(&ins->properties, ins->config_map, context);
}

int flb_native_processor_set_property(struct flb_native_processor_instance *ins,
                                      const char *k, const char *v);

const char *flb_native_processor_get_property(
                const char *key,
                struct flb_native_processor_instance *ins);

struct flb_native_processor_instance *flb_native_processor_new(
                                        struct flb_config *config,
                                        const char *name, void *data);

void flb_native_processor_instance_exit(
        struct flb_native_processor_instance *ins,
        struct flb_config *config);

// void flb_native_processor_exit(struct flb_config *config);

const char *flb_native_processor_name(struct flb_native_processor_instance *ins);

int flb_native_processor_plugin_property_check(
        struct flb_native_processor_instance *ins,
        struct flb_config *config);

int flb_native_processor_init(
        struct flb_config *config,
        struct flb_native_processor_instance *ins);

// int flb_native_processor_init_all(struct flb_config *config);
void flb_native_processor_set_context(
        struct flb_native_processor_instance *ins,
        void *context);

void flb_native_processor_instance_destroy(
        struct flb_native_processor_instance *ins);

#endif
