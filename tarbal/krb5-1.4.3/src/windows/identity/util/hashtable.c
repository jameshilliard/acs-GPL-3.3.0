/*
 * Copyright (c) 2004 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* $Id$ */

#include<hashtable.h>
#include<stdlib.h>

KHMEXP hashtable * KHMAPI hash_new_hashtable(khm_int32 n, 
                               hash_function_t hash, 
                               comp_function_t comp,
                               add_ref_function_t addr,
                               del_ref_function_t delr) 
{
    hashtable * h;

    h = malloc(sizeof(hashtable));

    h->n = n;
    h->addr = addr;
    h->comp = comp;
    h->delr = delr;
    h->hash = hash;

    h->bins = calloc(sizeof(hash_bin *), n);

    return h;
}

KHMEXP void KHMAPI hash_del_hashtable(hashtable * h) {
    hash_bin * b;
    int i;

    for(i=0;i<h->n;i++) {
        LPOP(&h->bins[i], &b);
        while(b) {
            if(h->delr)
                h->delr(b->key, b->data);
            free(b);
            LPOP(&h->bins[i], &b);
        }
    }

    free(h);
}

KHMEXP void KHMAPI hash_add(hashtable * h, void * key, void * data) {
    int hv;
    hash_bin * b;

    hv = h->hash(key) % h->n;
    b = h->bins[hv];
    while(b) {
        if(!h->comp(b->key, key)) {
            /* found an existing value */
            if(h->delr)
                h->delr(b->key, b->data);
            b->key = key;
            b->data = data;
            if(h->addr)
                h->addr(b->key, b->data);
            break;
        }
        b = LNEXT(b);
    }

    if(!b) {
        b = malloc(sizeof(hash_bin));
        b->data = data;
        b->key = key;
        LINIT(b);
        LPUSH(&h->bins[hv], b);
        if(h->addr)
            h->addr(b->key, b->data);
    }
}

KHMEXP void KHMAPI hash_del(hashtable * h, void * key) {
    hash_bin * b;
    int hv;

    hv = h->hash(key) % h->n;

    b = h->bins[hv];
    while(b) {
        if(!h->comp(b->key, key)) {
            /* found it */
            LDELETE(&h->bins[hv], b);
            if(h->delr)
                h->delr(b->key, b->data);
            free(b);
            break;
        }
        b = LNEXT(b);
    }
}

KHMEXP void * KHMAPI hash_lookup(hashtable * h, void * key) {
    hash_bin * b;
    int hv;

    hv = h->hash(key) % h->n;

    b = h->bins[hv];

    while(b) {
        if(!h->comp(b->key, key)) {
            return b->data;
        }
        b = LNEXT(b);
    }

    return NULL;
}

KHMEXP khm_boolean KHMAPI hash_exist(hashtable * h, void * key) {
    hash_bin * b;
    int hv;

    hv = h->hash(key) % h->n;
    b = h->bins[hv];
    while(b) {
        if(!h->comp(b->key, key))
            return 1;
        b = LNEXT(b);
    }

    return 0;
}

KHMEXP khm_int32 hash_string(const void *vs) {
    /* DJB algorithm */

    khm_int32 hv = 13331;
    wchar_t * c;
    
    for(c = (wchar_t *) vs; *c; c++) {
        hv = ((hv<<5) + hv) + (khm_int32) *c;
    }

    return (hv & KHM_INT32_MAX);
}

KHMEXP khm_int32 hash_string_comp(const void *vs1, const void *vs2) {
    return wcscmp((const wchar_t *) vs1, (const wchar_t *) vs2);
}
