/* -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include <stdio.h>
#include <libmemcached/memcached.h>
#include "memcached.h"
#include "cproxy.h"

int cproxy_init(const char *cfg) {
    memcached_st *mst;

    mst = memcached_create(NULL);
    if (!mst) {
        fprintf(stderr, "failed memcached_create.\n");
        exit(EXIT_FAILURE);
    }

    if (settings.verbose > 1)
        fprintf(stderr, "cproxy initted\n");

    return 0;
}
