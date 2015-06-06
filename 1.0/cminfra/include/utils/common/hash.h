/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
*/

/*
 *
 * File name: hash.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: 
 * 
 */

#ifndef __CM_HASH_H__
#define __CM_HASH_H__

/*
 * The hash table.
 */

#define MAX_HASH_LEN 8

//typedef struct UCMHashEntry_s struct cm_hash_entry;
struct cm_hash_entry
{
  struct cm_hash_entry *next;
  char *name;
  char *name2;
  char *name3;
  void *payload;
  int32_t valid;
};

/*
 * The entire hash table
 */
struct cm_hash_table
{
  struct cm_hash_entry *table;
  int32_t size;
  int32_t nb_elems;
};

#define ATTRIBUTE_UNUSED

/*
 * function types:
 */
/**
 * cm_hash_deal_locator:
 * @pay_load_p:  the pData in the hash
 * @name_p:  the name_p associated
 *
 * Callback to free pData from a hash.
 */
typedef void (*cm_hash_deal_locator) (void * pay_load_p, char * name_p);
/**
 * cm_hash_copier:
 * @pay_load_p:  the pData in the hash
 * @name_p:  the name_p associated
 *
 * Callback to copy pData from a hash.
 *
 * Returns a copy of the pData or NULL in case of error.
 */
typedef void *(*cm_hash_copier) (void * pay_load_p, char * name_p);
/**
 * cm_hash_scanner:
 * @pay_load_p:  the pData in the hash
 * @pData:  extra scannner pData
 * @name_p:  the name_p associated
 *
 * Callback when scanning pData in a hash with the simple scanner.
 */
typedef void (*cm_hash_scanner) (void * pay_load_p, void * pData,
                                  char * name_p);
/**
 * cm_hash_scanner_full:
 * @pay_load_p:  the pData in the hash
 * @pData:  extra scannner pData
 * @name_p:  the name_p associated
 * @name_p2:  the second name_p associated
 * @name_p3:  the third name_p associated
 *
 * Callback when scanning pData in a hash with the full scanner.
 */
typedef void (*cm_hash_scanner_full) (void * pay_load_p, void * pData,
                                      const char * name_p,
                                      const char * name_p2,
                                      const char * name_p3);

/*
 * Constructor and destructor.
 */
struct cm_hash_table *cm_hash_create (int32_t size);
void cm_hash_free (struct cm_hash_table * table, cm_hash_deal_locator f);

int32_t str_q_equal (const char * pref, const char * name_p,
                   const char * str);

int32_t cm_str_equal (const char * str1, const char * str2);

/*
 * Add a new entry to the hash table.
 */
int32_t
cm_hash_add_entry (struct cm_hash_table * table, const char * name_p,
                 void * user_data_p);

int32_t
cm_hash_update_entry (struct cm_hash_table * table,
                    const char * name_p, void * user_data_p,
                    cm_hash_deal_locator f);

int32_t
cm_hash_add_entry1 (struct cm_hash_table * table, const char * name_p,
                  const char * name_p1, void * user_data_p);

int32_t
cm_hash_add_entry2 (struct cm_hash_table * table,
                  const char * name_p, const char * name_p2,
                  void * user_data_p);
int32_t cm_hash_update_entry2 (struct cm_hash_table * table, const char * name_p,
                             const char * name_p2, void * user_data_p,
                             cm_hash_deal_locator f);
int32_t cm_hash_add_entry3 (struct cm_hash_table * table, const char * name_p,
                          const char * name_p2, const char * name_p3,
                          void * user_data_p);
int32_t cm_hash_update_entry3 (struct cm_hash_table * table, const char * name_p,
                             const char * name_p2, const char * name_p3,
                             void * user_data_p, cm_hash_deal_locator f);

/*
 * Remove an entry from the hash table.
 */
int32_t
cm_hash_remove_entry (struct cm_hash_table * table, const char * name_p,
                    cm_hash_deal_locator f);
int32_t
cm_hash_remove_entry2 (struct cm_hash_table * table, const char * name_p,
                     const char * name_p2, cm_hash_deal_locator f);
int32_t
cm_hash_remove_entry3 (struct cm_hash_table * table, const char * name_p,
                     const char * name_p2, const char * name_p3,
                     cm_hash_deal_locator f);

/*
 * Retrieve the user_data_p.
 */
void *cm_hash_lookup (struct cm_hash_table * table, const char * name_p);
void *cm_hash_lookup2 (struct cm_hash_table * table,
                        const char * name_p, const char * name_p2);
void *cm_hash_lookup3 (struct cm_hash_table * table,
                        const char * name_p,
                        const char * name_p2, const char * name_p3);
void *cm_hash_q_lookup (struct cm_hash_table * table,
                        const char * name_p, const char * prefix);
void *cm_hash_q_lookup2 (struct cm_hash_table * table,
                         const char * name_p,
                         const char * prefix,
                         const char * name_p2, const char * prefix2);
void *cm_hash_q_lookup3 (struct cm_hash_table * table,
                         const char * name_p,
                         const char * prefix,
                         const char * name_p2,
                         const char * prefix2,
                         const char * name_p3, const char * prefix3);

/*
 * Helpers.
 */
struct cm_hash_table *cm_hash_copy (struct cm_hash_table * table, cm_hash_copier f);
int32_t cm_hash_size (struct cm_hash_table * table);
void cm_hash_scan (struct cm_hash_table * table, cm_hash_scanner f, void * pData);
void
cm_hash_scan3 (struct cm_hash_table * table,
              const char * name_p,
              const char * name_p2,
              const char * name_p3, cm_hash_scanner f, void * pData);
void cm_hash_scan_full (struct cm_hash_table * table, cm_hash_scanner_full f,
                        void * pData);
void cm_hash_scan_full3 (struct cm_hash_table * table, const char * name_p,
                         const char * name_p2, const char * name_p3,
                         cm_hash_scanner_full f, void * pData);

#endif
