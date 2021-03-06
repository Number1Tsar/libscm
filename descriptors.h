/*
 * Copyright (c) 2010, the Short-term Memory Project Authors.
 * All rights reserved. Please see the AUTHORS file for details.
 * Use of this source code is governed by a BSD license that
 * can be found in the LICENSE file.
 */

#ifndef _DESCRIPTORS_H_
#define	_DESCRIPTORS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "debug.h"
#include "arch.h"
#include "meter.h"
#include "finalizer.h"
#include "object.h"
#include "libscm.h"

#ifndef DESCRIPTORS_PER_PAGE
#define DESCRIPTORS_PER_PAGE \
    ((SCM_DESCRIPTOR_PAGE_SIZE - 2 * sizeof(void*))/sizeof(void*))
#endif

/*
 * A chunk of contiguous memory that holds descriptors with the same
 * expiration date.
 */
typedef struct descriptor_page descriptor_page_t;

struct descriptor_page {
    descriptor_page_t *next;
    unsigned long number_of_descriptors;
    object_header_t* descriptors[DESCRIPTORS_PER_PAGE];
};

/* 
 * singly-linked list of descriptor pages
 */
typedef struct descriptor_page_list descriptor_page_list_t;

struct descriptor_page_list {
    descriptor_page_t* first;
    descriptor_page_t* last;
};

/*
 * singly-linked list of expired descriptor pages
 */
typedef struct expired_descriptor_page_list expired_descriptor_page_list_t;

struct expired_descriptor_page_list {
    descriptor_page_t* first;
    descriptor_page_t* last;
    unsigned long collected;
};

/*
 * Statically allocate memory for the locally clocked descriptor buffers.
 * Size of the locally clocked buffer is SCM_MAX_EXPIRATION_EXTENSION + 1
 * because of the additional slot for:
 * 1. slot for the current time
 *
 * Statically allocate memory for the globally clocked descriptor buffers.
 * Size of the globally clocked buffer is SCM_MAX_EXPIRATION_EXTENSION + 2
 * because of the additional slots for:
 *  1. slot for the current time
 *  2. adding descriptors at current + increment + 1
 *
 * Note: both buffers allocate SCM_MAX_EXPIRATION_EXTENSION + 2 slots for
 * page_lists but the locally clocked buffer uses only
 * SCM_MAX_EXPIRATION_EXTENSION + 1 slots
 */
typedef struct descriptor_buffer descriptor_buffer_t;

struct descriptor_buffer {
    descriptor_page_list_t not_expired[SCM_MAX_EXPIRATION_EXTENSION + 2];

    // The field not_expired_length may have the following values:
    //		0 : indicates that the descriptor buffer is unused
    //		SCM_MAX_EXPIRATION_EXTENSION + 1 : indicates that the descriptor
    //				buffer is used with a thread-local clock, and
    //		SCM_MAX_EXPIRATION_EXTENSION + 2 : indicates that the descriptor
    //				buffer is used with the global clock.
    unsigned int not_expired_length;

    // current_index is an index to the descriptor_page_list in
    // not_expired that will expire after the next tick.
    unsigned int current_index;

    // status: age != descriptor_root->current_time => zombie,
    // Initially, all descriptor buffers but the first one are zombies
    // (because register thread increments descriptor_root->current_time)
    unsigned int age;
};

// The max. amount of memory that fits into a region page
#define SCM_REGION_PAGE_PAYLOAD_SIZE \
    (SCM_REGION_PAGE_SIZE - sizeof(region_page_t*))

/**
 * region_page contains a pointer to the next region_page,
 * and a chunk of memory for allocating memory objects.
 * region_page is allocated page-aligned.
 */
typedef struct region_page region_page_t;

struct region_page {
    region_page_t* nextPage;
    
    char memory[SCM_REGION_PAGE_PAYLOAD_SIZE];
};

/**
 * region contains the descriptor counter for the SCM implementation,
 * a field to count the amount of region pages and pointers to the
 * first and last region page.
 * To distinguish unused regions from used regions, the age parameter 
 * is checked against the current_time field of the descriptor_root.
 * Unused regions can be registered with scm_register_region().
 *
 * Fast allocation is achieved by keeping track of the next_free_address
 * and the last_address_in_last_page pointers. The next_free_address
 * pointer points to available free space in the last region page.
 * The last_address_in_last_page pointer points to the last address in the
 * last region page. The next_free_address pointer can never point to an 
 * address behind the last_address_in_last_page pointer.
 */
typedef struct region region_t;

struct region {
    unsigned int dc;

    unsigned int number_of_region_pages;

    region_page_t* firstPage;
    region_page_t* lastPage;

    unsigned int age;

    void* next_free_address;
    void* last_address_in_last_page;
};

/**
 * Descriptor root holds thread-local data for descriptor
 * and region management.
 */
typedef struct descriptor_root descriptor_root_t;

struct descriptor_root {
    // global_phase indicates if the thread has already ticked in the current 
    // global phase. A global phase is the interval between two increments of
    // the global clock (global_time).
    // 
    // global_phase == global_time => thread has not ticked yet 
    // global_phase == global_time+1 => thread has already ticked at least once
    unsigned long global_phase;

    expired_descriptor_page_list_t list_of_expired_obj_descriptors;
    expired_descriptor_page_list_t list_of_expired_reg_descriptors;

    descriptor_buffer_t globally_clocked_obj_buffer;
    descriptor_buffer_t globally_clocked_reg_buffer;

    descriptor_buffer_t locally_clocked_obj_buffer[SCM_MAX_CLOCKS];
    descriptor_buffer_t locally_clocked_reg_buffer[SCM_MAX_CLOCKS];

    unsigned int next_clock_index;

    // The following field indicates the time when the thread was created.
    // The field is necessary to distinguish zombie descriptor buffers
    // from currently used descriptor buffers.
    // Initially, all descriptor buffers but the first one are zombies
    // (because register thread increments the current_time)
    unsigned int current_time;

    // The round_robin field is an index of the locally_clocked buffers
    // which constantly increases modulo SCM_MAX_CLOCKS - 1.
    // round_robin is never set to 0 because the first locally_clocked
    // buffer is the base clock of the thread and can never be a
    // zombie buffer.
    // round_robin enables constant-time cleaning of zombie buffers.
    unsigned int round_robin;

    // thread participates in global time protocol if flag is false
    bool blocked;

    // A pool of descriptor pages for re-use.
    descriptor_page_t* descriptor_page_pool[SCM_DESCRIPTOR_PAGE_FREELIST_SIZE];
    unsigned long number_of_pooled_descriptor_pages;

    region_t regions[SCM_MAX_REGIONS];
    unsigned int next_reg_index;

    region_page_t* region_page_pool;
    unsigned long number_of_pooled_region_pages;

    // Singly-linked list of terminated descriptor_roots.
    // This is only used after the thread terminated.
    descriptor_root_t *next;
};

extern __thread descriptor_root_t* descriptor_root;

inline void increment_current_index(descriptor_buffer_t *buffer)
    __attribute__((visibility("hidden")));

/* Takes an object or a region as parameter ptr */
void insert_descriptor(void* ptr,
                       descriptor_buffer_t *buffer, unsigned int expiration)
    __attribute__((visibility("hidden")));

/* Expires the descriptor buffer by appending
 * the just-expired descriptors to the
 * list_of_expired_[obj|reg]_descriptors. */
void expire_buffer(descriptor_buffer_t *buffer,
                   expired_descriptor_page_list_t *exp_list)
    __attribute__((visibility("hidden")));

/* expire_object_descriptor_if_exists()
 * expires object descriptors */
int expire_object_descriptor_if_exists(expired_descriptor_page_list_t *list)
    __attribute__((visibility("hidden")));

/* expire_region_descriptor_if_exists()
 * expires region descriptors */
int expire_region_descriptor_if_exists(expired_descriptor_page_list_t *list)
    __attribute__((visibility("hidden")));

#endif	/* _DESCRIPTORS_H_ */