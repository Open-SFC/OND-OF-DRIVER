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
 * File name: hash.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/23/2013
 * Description: 
 * 
 */

#ifdef CM_SUPPORT
#ifdef OF_XML_SUPPORT

#include "cmincld.h"
#include <hash.h>

/*
 * cm_hash_compute_key:
 * Calculate the hash key
 */
static unsigned long
cm_hash_compute_key (struct cm_hash_table * table, const char * name,
                   const char * name2, const char * name3)
{
  unsigned long value = 0L;
  char ch;

  if (name != NULL)
  {
    value += 30 * (*name);
    while ((ch = *name++) != 0)
    {
      value = value ^ ((value << 5) + (value >> 3) + (unsigned long) ch);
    }
  }
  if (name2 != NULL)
  {
    while ((ch = *name2++) != 0)
    {
      value = value ^ ((value << 5) + (value >> 3) + (unsigned long) ch);
    }
  }
  if (name3 != NULL)
  {
    while ((ch = *name3++) != 0)
    {
      value = value ^ ((value << 5) + (value >> 3) + (unsigned long) ch);
    }
  }
  return (value % table->size);
}

static unsigned long
cm_hash_compute_qkey (struct cm_hash_table * table,
                    const char * prefix, const char * name,
                    const char * prefix2, const char * name2,
                    const char * prefix3, const char * name3)
{
  unsigned long value = 0L;
  char ch;

  if (prefix != NULL)
    value += 30 * (*prefix);
  else
    value += 30 * (*name);

  if (prefix != NULL)
  {
    while ((ch = *prefix++) != 0)
    {
      value = value ^ ((value << 5) + (value >> 3) + (unsigned long) ch);
    }
    value = value ^ ((value << 5) + (value >> 3) + (unsigned long) ':');
  }
  if (name != NULL)
  {
    while ((ch = *name++) != 0)
    {
      value = value ^ ((value << 5) + (value >> 3) + (unsigned long) ch);
    }
  }
  if (prefix2 != NULL)
  {
    while ((ch = *prefix2++) != 0)
    {
      value = value ^ ((value << 5) + (value >> 3) + (unsigned long) ch);
    }
    value = value ^ ((value << 5) + (value >> 3) + (unsigned long) ':');
  }
  if (name2 != NULL)
  {
    while ((ch = *name2++) != 0)
    {
      value = value ^ ((value << 5) + (value >> 3) + (unsigned long) ch);
    }
  }
  if (prefix3 != NULL)
  {
    while ((ch = *prefix3++) != 0)
    {
      value = value ^ ((value << 5) + (value >> 3) + (unsigned long) ch);
    }
    value = value ^ ((value << 5) + (value >> 3) + (unsigned long) ':');
  }
  if (name3 != NULL)
  {
    while ((ch = *name3++) != 0)
    {
      value = value ^ ((value << 5) + (value >> 3) + (unsigned long) ch);
    }
  }
  return (value % table->size);
}

/**
 * cm_hash_create:
 * @size: the size of the hash table
 *
 * Create a new struct cm_hash_table *.
 *
 * Returns the newly created object, or NULL if an error occured.
 */
struct cm_hash_table *cm_hash_create (int32_t size)
{
  struct cm_hash_table *table;

  if (size <= 0)
    size = 256;

  table = of_malloc (sizeof (struct cm_hash_table));
  if (table)
  {
    table->size = size;
    table->nb_elems = 0;
    table->table = of_malloc (size * sizeof (struct cm_hash_entry));
    if (table->table)
    {
      memset (table->table, 0, size * sizeof (struct cm_hash_entry));
      return (table);
    }
    of_free (table);
  }
  return (NULL);
}

/**
 * cm_hash_grow:
 * @table: the hash table
 * @size: the new size of the hash table
 *
 * resize the hash table
 *
 * Returns 0 in case of success, -1 in case of failure
 */
static int32_t cm_hash_grow (struct cm_hash_table * table, int32_t size)
{
  unsigned long key;
  int32_t oldsize, i;
  struct cm_hash_entry *iter, *next;
  struct cm_hash_entry *oldtable;
#ifdef DEBUG_GROW
  unsigned long nbElem = 0;
#endif

  if (table == NULL)
    return (-1);
  if (size < 8)
    return (-1);
  if (size > 8 * 2048)
    return (-1);

  oldsize = table->size;
  oldtable = table->table;
  if (oldtable == NULL)
    return (-1);

  table->table = of_malloc (size * sizeof (struct cm_hash_entry));
  if (table->table == NULL)
  {
    table->table = oldtable;
    return (-1);
  }
  memset (table->table, 0, size * sizeof (struct cm_hash_entry));
  table->size = size;

  /*  If the two loops are merged, there would be situations where
     a new entry needs to allocated and pData copied int32_to it from 
     the main table. So instead, we run through the array twice, first
     copying all the elements in the main array (where we can't get
     conflicts) and then the rest, so we only free (and don't allocate)
   */
  for (i = 0; i < oldsize; i++)
  {
    if (oldtable[i].valid == 0)
      continue;
    key = cm_hash_compute_key (table, oldtable[i].name, oldtable[i].name2,
                             oldtable[i].name3);
    memcpy (&(table->table[key]), &(oldtable[i]), sizeof (struct cm_hash_entry));
    table->table[key].next = NULL;
  }

  for (i = 0; i < oldsize; i++)
  {
    iter = oldtable[i].next;
    while (iter)
    {
      next = iter->next;

      /*
       * put back the entry in the new table
       */

      key = cm_hash_compute_key (table, iter->name, iter->name2, iter->name3);
      if (table->table[key].valid == 0)
      {
        memcpy (&(table->table[key]), iter, sizeof (struct cm_hash_entry));
        table->table[key].next = NULL;
        of_free (iter);
      }
      else
      {
        iter->next = table->table[key].next;
        table->table[key].next = iter;
      }

#ifdef DEBUG_GROW
      nbElem++;
#endif

      iter = next;
    }
  }

  of_free (oldtable);

#ifdef DEBUG_GROW
  ucmGenericError (ucmGenericErrorContext,
                   "cm_hash_grow : from %d to %d, %d elems\n", oldsize, size,
                   nbElem);
#endif

  return (0);
}

/**
 * cm_hash_free:
 * @table: the hash table
 * @f:  the deallocator function for items in the hash
 *
 * Free the hash @table and its contents. The user_data_p is
 * deallocated with @f if provided.
 */
void cm_hash_free (struct cm_hash_table * table, cm_hash_deal_locator f)
{
  int32_t i;
  struct cm_hash_entry *iter;
  struct cm_hash_entry *next;
  int32_t inside_table = 0;
  int32_t nb_elems;

  if (table == NULL)
    return;
  if (table->table)
  {
    nb_elems = table->nb_elems;
    for (i = 0; (i < table->size) && (nb_elems > 0); i++)
    {
      iter = &(table->table[i]);
      if (iter->valid == 0)
        continue;
      inside_table = 1;
      while (iter)
      {
        next = iter->next;
        if ((f != NULL) && (iter->payload != NULL))
          f (iter->payload, iter->name);
        if (iter->name)
          of_free (iter->name);
        if (iter->name2)
          of_free (iter->name2);
        if (iter->name3)
          of_free (iter->name3);
        iter->payload = NULL;
        if (!inside_table)
          of_free (iter);
        nb_elems--;
        inside_table = 0;
        iter = next;
      }
      inside_table = 0;
    }
    of_free (table->table);
  }
  of_free (table);
}

/**
 * cm_hash_add_entry:
 * @table: the hash table
 * @name: the name of the user_data_p
 * @user_data_p: a point32_ter to the user_data_p
 *
 * Add the @user_data_p to the hash @table. This can later be retrieved
 * by using the @name. Duplicate names generate errors.
 *
 * Returns 0 the addition succeeded and -1 in case of error.
 */
int32_t
cm_hash_add_entry (struct cm_hash_table * table, const char * name,
                 void * user_data_p)
{
  return (cm_hash_add_entry3 (table, name, NULL, NULL, user_data_p));
}

int32_t
cm_hash_add_entry1 (struct cm_hash_table * table, const char * name,
                  const char * name1, void * user_data_p)
{
  return (cm_hash_add_entry3 (table, name, name1, NULL, user_data_p));
}

/**
 * cm_hash_add_entry2:
 * @table: the hash table
 * @name: the name of the user_data_p
 * @name2: a second name of the user_data_p
 * @user_data_p: a point32_ter to the user_data_p
 *
 * Add the @user_data_p to the hash @table. This can later be retrieved
 * by using the (@name, @name2) tuple. Duplicate tuples generate errors.
 *
 * Returns 0 the addition succeeded and -1 in case of error.
 */
int32_t
cm_hash_add_entry2 (struct cm_hash_table * table, const char * name,
                  const char * name2, void * user_data_p)
{
  return (cm_hash_add_entry3 (table, name, name2, NULL, user_data_p));
}

/**
 * cm_hash_update_entry:
 * @table: the hash table
 * @name: the name of the user_data_p
 * @user_data_p: a point32_ter to the user_data_p
 * @f: the deallocator function for replaced item (if any)
 *
 * Add the @user_data_p to the hash @table. This can later be retrieved
 * by using the @name. Existing entry for this @name will be removed
 * and freed with @f if found.
 *
 * Returns 0 the addition succeeded and -1 in case of error.
 */
int32_t
cm_hash_update_entry (struct cm_hash_table * table, const char * name,
                    void * user_data_p, cm_hash_deal_locator f)
{
  return (cm_hash_update_entry3 (table, name, NULL, NULL, user_data_p, f));
}

/**
 * cm_hash_update_entry2:
 * @table: the hash table
 * @name: the name of the user_data_p
 * @name2: a second name of the user_data_p
 * @user_data_p: a point32_ter to the user_data_p
 * @f: the deallocator function for replaced item (if any)
 *
 * Add the @user_data_p to the hash @table. This can later be retrieved
 * by using the (@name, @name2) tuple. Existing entry for this tuple will
 * be removed and freed with @f if found.
 *
 * Returns 0 the addition succeeded and -1 in case of error.
 */
int32_t
cm_hash_update_entry2 (struct cm_hash_table * table, const char * name,
                     const char * name2, void * user_data_p,
                     cm_hash_deal_locator f)
{
  return (cm_hash_update_entry3 (table, name, name2, NULL, user_data_p, f));
}

/**
 * cm_hash_lookup:
 * @table: the hash table
 * @name: the name of the user_data_p
 *
 * Find the user_data_p specified by the @name.
 *
 * Returns the point32_ter to the user_data_p
 */
void *cm_hash_lookup (struct cm_hash_table * table, const char * name)
{
  return (cm_hash_lookup3 (table, name, NULL, NULL));
}

/**
 * cm_hash_lookup2:
 * @table: the hash table
 * @name: the name of the user_data_p
 * @name2: a second name of the user_data_p
 *
 * Find the user_data_p specified by the (@name, @name2) tuple.
 *
 * Returns the point32_ter to the user_data_p
 */
void *cm_hash_lookup2 (struct cm_hash_table * table, const char * name,
                        const char * name2)
{
  return (cm_hash_lookup3 (table, name, name2, NULL));
}

/**
 * cm_hash_q_lookup:
 * @table: the hash table
 * @prefix: the prefix of the user_data_p
 * @name: the name of the user_data_p
 *
 * Find the user_data_p specified by the QName @prefix:@name/@name.
 *
 * Returns the point32_ter to the user_data_p
 */
void *cm_hash_q_lookup (struct cm_hash_table * table, const char * prefix,
                        const char * name)
{
  return (cm_hash_q_lookup3 (table, prefix, name, NULL, NULL, NULL, NULL));
}

/**
 * cm_hash_q_lookup2:
 * @table: the hash table
 * @prefix: the prefix of the user_data_p
 * @name: the name of the user_data_p
 * @prefix2: the second prefix of the user_data_p
 * @name2: a second name of the user_data_p
 *
 * Find the user_data_p specified by the QNames tuple
 *
 * Returns the point32_ter to the user_data_p
 */
void *cm_hash_q_lookup2 (struct cm_hash_table * table, const char * prefix,
                         const char * name, const char * prefix2,
                         const char * name2)
{
  return (cm_hash_q_lookup3 (table, prefix, name, prefix2, name2, NULL, NULL));
}

/**
 * cm_hash_add_entry3:
 * @table: the hash table
 * @name: the name of the user_data_p
 * @name2: a second name of the user_data_p
 * @name3: a third name of the user_data_p
 * @user_data_p: a point32_ter to the user_data_p
 *
 * Add the @user_data_p to the hash @table. This can later be retrieved
 * by using the tuple (@name, @name2, @name3). Duplicate entries generate
 * errors.
 *
 * Returns 0 the addition succeeded and -1 in case of error.
 */
int32_t
cm_hash_add_entry3 (struct cm_hash_table * table, const char * name,
                  const char * name2, const char * name3,
                  void * user_data_p)
{
  unsigned long key, len = 0;
  struct cm_hash_entry *entry;
  struct cm_hash_entry *insert;

  if ((table == NULL) || name == NULL)
    return (-1);

  /*
   * Check for duplicate and insertion location.
   */
  key = cm_hash_compute_key (table, name, name2, name3);
  if (table->table[key].valid == 0)
  {
    insert = NULL;
  }
  else
  {
    for (insert = &(table->table[key]); insert->next != NULL;
         insert = insert->next)
    {
      if ((cm_str_equal (insert->name, name)) &&
          (cm_str_equal (insert->name2, name2)) &&
          (cm_str_equal (insert->name3, name3)))
        return (-1);
      len++;
    }
    if ((cm_str_equal (insert->name, name)) &&
        (cm_str_equal (insert->name2, name2)) &&
        (cm_str_equal (insert->name3, name3)))
      return (-1);
  }

  if (insert == NULL)
  {
    entry = &(table->table[key]);
  }
  else
  {
    entry = of_malloc (sizeof (struct cm_hash_entry));
    if (entry == NULL)
      return (-1);
  }

  if (name)
    entry->name = of_strdup ((char *) name);
  /*else
     entry->name = NULL; *//*DEADCODE */

  if (name2)
    entry->name2 = of_strdup ((char *) name2);
  else
    entry->name2 = NULL;

  if (name3)
    entry->name3 = of_strdup ((char *) name3);
  else
    entry->name3 = NULL;

  entry->payload = user_data_p;
  entry->next = NULL;
  entry->valid = 1;

  if (insert != NULL)
    insert->next = entry;

  table->nb_elems++;

  if (len > MAX_HASH_LEN)
    cm_hash_grow (table, MAX_HASH_LEN * table->size);

  return (0);
}

/**
 * cm_hash_update_entry3:
 * @table: the hash table
 * @name: the name of the user_data_p
 * @name2: a second name of the user_data_p
 * @name3: a third name of the user_data_p
 * @user_data_p: a point32_ter to the user_data_p
 * @f: the deallocator function for replaced item (if any)
 *
 * Add the @user_data_p to the hash @table. This can later be retrieved
 * by using the tuple (@name, @name2, @name3). Existing entry for this tuple
 * will be removed and freed with @f if found.
 *
 * Returns 0 the addition succeeded and -1 in case of error.
 */
int32_t
cm_hash_update_entry3 (struct cm_hash_table * table, const char * name,
                     const char * name2, const char * name3,
                     void * user_data_p, cm_hash_deal_locator f)
{
  unsigned long key;
  struct cm_hash_entry *entry;
  struct cm_hash_entry *insert;

  if ((table == NULL) || name == NULL)
    return (-1);

  /*
   * Check for duplicate and insertion location.
   */
  key = cm_hash_compute_key (table, name, name2, name3);
  if (table->table[key].valid == 0)
  {
    insert = NULL;
  }
  else
  {
    for (insert = &(table->table[key]); insert->next != NULL;
         insert = insert->next)
    {
      if ((cm_str_equal (insert->name, name)) &&
          (cm_str_equal (insert->name2, name2)) &&
          (cm_str_equal (insert->name3, name3)))
      {
        if (f)
          f (insert->payload, insert->name);
        insert->payload = user_data_p;
        return (0);
      }
    }
    if ((cm_str_equal (insert->name, name)) &&
        (cm_str_equal (insert->name2, name2)) &&
        (cm_str_equal (insert->name3, name3)))
    {
      if (f)
        f (insert->payload, insert->name);
      insert->payload = user_data_p;
      return (0);
    }
  }

  if (insert == NULL)
  {
    entry = &(table->table[key]);
  }
  else
  {
    entry = of_malloc (sizeof (struct cm_hash_entry));
    if (entry == NULL)
      return (-1);
  }

  /*if(name)
     entry->name = of_strdup((char*)name);
     if(name2)
     entry->name2 = of_strdup((char*)name2);
     if(name3)
     entry->name3 = of_strdup((char*)name3);
   */

  if (name)
    entry->name = of_strdup ((char *) name);
/*  else
                              entry->name = NULL;*//* DEAD CODE */

  if (name2)
    entry->name2 = of_strdup ((char *) name2);
  else
    entry->name2 = NULL;

  if (name3)
    entry->name3 = of_strdup ((char *) name3);
  else
    entry->name3 = NULL;

  entry->payload = user_data_p;
  entry->next = NULL;
  entry->valid = 1;
  table->nb_elems++;

  if (insert != NULL)
  {
    insert->next = entry;
  }
  return (0);
}

/**
 * cm_hash_lookup3:
 * @table: the hash table
 * @name: the name of the user_data_p
 * @name2: a second name of the user_data_p
 * @name3: a third name of the user_data_p
 *
 * Find the user_data_p specified by the (@name, @name2, @name3) tuple.
 *
 * Returns the a point32_ter to the user_data_p
 */
void *cm_hash_lookup3 (struct cm_hash_table * table, const char * name,
                        const char * name2, const char * name3)
{
  unsigned long key;
  struct cm_hash_entry *entry;

  if (table == NULL)
    return (NULL);
  if (name == NULL)
    return (NULL);
  key = cm_hash_compute_key (table, name, name2, name3);
  if (table->table[key].valid == 0)
    return (NULL);
  for (entry = &(table->table[key]); entry != NULL; entry = entry->next)
  {
    if ((cm_str_equal (entry->name, name)) &&
        (cm_str_equal (entry->name2, name2)) &&
        (cm_str_equal (entry->name3, name3)))
      return (entry->payload);
  }
  return (NULL);
}

/**
 * cm_hash_q_lookup3:
 * @table: the hash table
 * @prefix: the prefix of the user_data_p
 * @name: the name of the user_data_p
 * @prefix2: the second prefix of the user_data_p
 * @name2: a second name of the user_data_p
 * @prefix3: the third prefix of the user_data_p
 * @name3: a third name of the user_data_p
 *
 * Find the user_data_p specified by the (@name, @name2, @name3) tuple.
 *
 * Returns the a point32_ter to the user_data_p
 */
void *cm_hash_q_lookup3 (struct cm_hash_table * table,
                         const char * prefix, const char * name,
                         const char * prefix2, const char * name2,
                         const char * prefix3, const char * name3)
{
  unsigned long key;
  struct cm_hash_entry *entry;

  if (table == NULL)
    return (NULL);
  if (name == NULL)
    return (NULL);
  key = cm_hash_compute_qkey (table, prefix, name, prefix2,
                            name2, prefix3, name3);
  if (table->table[key].valid == 0)
    return (NULL);
  for (entry = &(table->table[key]); entry != NULL; entry = entry->next)
  {
    if ((str_q_equal (prefix, name, entry->name)) &&
        (str_q_equal (prefix2, name2, entry->name2)) &&
        (str_q_equal (prefix3, name3, entry->name3)))
      return (entry->payload);
  }
  return (NULL);
}

typedef struct
{
  cm_hash_scanner hashscanner;
  void *pData;
} stubData;

static void
stub_hash_scanner_full (void * payload, void * pData, const char * name,
                     const char * name2 ATTRIBUTE_UNUSED,
                     const char * name3 ATTRIBUTE_UNUSED)
{
  stubData *stubpData = (stubData *) pData;
  stubpData->hashscanner (payload, stubpData->pData, (char *) name);
}

/**
 * cm_hash_scan:
 * @table: the hash table
 * @f:  the scanner function for items in the hash
 * @pData:  extra pData passed to f
 *
 * Scan the hash @table and applied @f to each value.
 */
void cm_hash_scan (struct cm_hash_table * table, cm_hash_scanner f, void * pData)
{
  stubData stubpData;
  stubpData.pData = pData;
  stubpData.hashscanner = f;
  cm_hash_scan_full (table, stub_hash_scanner_full, &stubpData);
}

/**
 * cm_hash_scan_full:
 * @table: the hash table
 * @f:  the scanner function for items in the hash
 * @pData:  extra pData passed to f
 *
 * Scan the hash @table and applied @f to each value.
 */
void
cm_hash_scan_full (struct cm_hash_table * table, cm_hash_scanner_full f, void * pData)
{
  int32_t i;
  struct cm_hash_entry *iter;
  struct cm_hash_entry *next;

  if (table == NULL)
    return;
  if (f == NULL)
    return;

  if (table->table)
  {
    for (i = 0; i < table->size; i++)
    {
      if (table->table[i].valid == 0)
        continue;
      iter = &(table->table[i]);
      while (iter)
      {
        next = iter->next;
        if ((f != NULL) && (iter->payload != NULL))
          f (iter->payload, pData, iter->name, iter->name2, iter->name3);
        iter = next;
      }
    }
  }
}

/**
 * cm_hash_scan3:
 * @table: the hash table
 * @name: the name of the user_data_p or NULL
 * @name2: a second name of the user_data_p or NULL
 * @name3: a third name of the user_data_p or NULL
 * @f:  the scanner function for items in the hash
 * @pData:  extra pData passed to f
 *
 * Scan the hash @table and applied @f to each value matching
 * (@name, @name2, @name3) tuple. If one of the names is null,
 * the comparison is considered to match.
 */
void
cm_hash_scan3 (struct cm_hash_table * table, const char * name,
              const char * name2, const char * name3,
              cm_hash_scanner f, void * pData)
{
  cm_hash_scan_full3 (table, name, name2, name3, (cm_hash_scanner_full) f, pData);
}

/**
 * cm_hash_scan_full3:
 * @table: the hash table
 * @name: the name of the user_data_p or NULL
 * @name2: a second name of the user_data_p or NULL
 * @name3: a third name of the user_data_p or NULL
 * @f:  the scanner function for items in the hash
 * @pData:  extra pData passed to f
 *
 * Scan the hash @table and applied @f to each value matching
 * (@name, @name2, @name3) tuple. If one of the names is null,
 * the comparison is considered to match.
 */
void
cm_hash_scan_full3 (struct cm_hash_table * table, const char * name,
                  const char * name2, const char * name3,
                  cm_hash_scanner_full f, void * pData)
{
  int32_t i;
  struct cm_hash_entry *iter;
  struct cm_hash_entry *next;

  if (table == NULL)
    return;
  if (f == NULL)
    return;

  if (table->table)
  {
    for (i = 0; i < table->size; i++)
    {
      if (table->table[i].valid == 0)
        continue;
      iter = &(table->table[i]);
      while (iter)
      {
        next = iter->next;
        if (((name == NULL) || (cm_str_equal (name, iter->name))) &&
            ((name2 == NULL) || (cm_str_equal (name2, iter->name2))) &&
            ((name3 == NULL) || (cm_str_equal (name3, iter->name3))) &&
            (iter->payload != NULL))
        {
          f (iter->payload, pData, iter->name, iter->name2, iter->name3);
        }
        iter = next;
      }
    }
  }
}

/**
 * cm_hash_copy:
 * @table: the hash table
 * @f:  the copier function for items in the hash
 *
 * Scan the hash @table and applied @f to each value.
 *
 * Returns the new table or NULL in case of error.
 */
struct cm_hash_table *cm_hash_copy (struct cm_hash_table * table, cm_hash_copier f)
{
  int32_t i;
  struct cm_hash_entry *iter;
  struct cm_hash_entry *next;
  struct cm_hash_table *ret;

  if (table == NULL)
    return (NULL);
  if (f == NULL)
    return (NULL);

  ret = cm_hash_create (table->size);
  if (table->table)
  {
    for (i = 0; i < table->size; i++)
    {
      if (table->table[i].valid == 0)
        continue;
      iter = &(table->table[i]);
      while (iter)
      {
        next = iter->next;
        cm_hash_add_entry3 (ret, iter->name, iter->name2,
                          iter->name3, f (iter->payload, iter->name));
        iter = next;
      }
    }
  }
  ret->nb_elems = table->nb_elems;
  return (ret);
}

/**
 * cm_hash_size:
 * @table: the hash table
 *
 * Query the number of elements installed in the hash @table.
 *
 * Returns the number of elements in the hash table or
 * -1 in case of error
 */
int32_t cm_hash_size (struct cm_hash_table * table)
{
  if (table == NULL)
    return (-1);
  return (table->nb_elems);
}

/**
 * cm_hash_remove_entry:
 * @table: the hash table
 * @name: the name of the user_data_p
 * @f: the deallocator function for removed item (if any)
 *
 * Find the user_data_p specified by the @name and remove
 * it from the hash @table. Existing user_data_p for this tuple will be removed
 * and freed with @f.
 *
 * Returns 0 if the removal succeeded and -1 in case of error or not found.
 */
int32_t cm_hash_remove_entry (struct cm_hash_table * table, const char * name,
                            cm_hash_deal_locator f)
{
  return (cm_hash_remove_entry3 (table, name, NULL, NULL, f));
}

/**
 * cm_hash_remove_entry2:
 * @table: the hash table
 * @name: the name of the user_data_p
 * @name2: a second name of the user_data_p
 * @f: the deallocator function for removed item (if any)
 *
 * Find the user_data_p specified by the (@name, @name2) tuple and remove
 * it from the hash @table. Existing user_data_p for this tuple will be removed
 * and freed with @f.
 *
 * Returns 0 if the removal succeeded and -1 in case of error or not found.
 */
int32_t
cm_hash_remove_entry2 (struct cm_hash_table * table, const char * name,
                     const char * name2, cm_hash_deal_locator f)
{
  return (cm_hash_remove_entry3 (table, name, name2, NULL, f));
}

/**
 * cm_hash_remove_entry3:
 * @table: the hash table
 * @name: the name of the user_data_p
 * @name2: a second name of the user_data_p
 * @name3: a third name of the user_data_p
 * @f: the deallocator function for removed item (if any)
 *
 * Find the user_data_p specified by the (@name, @name2, @name3) tuple and remove
 * it from the hash @table. Existing user_data_p for this tuple will be removed
 * and freed with @f.
 *
 * Returns 0 if the removal succeeded and -1 in case of error or not found.
 */
int32_t
cm_hash_remove_entry3 (struct cm_hash_table * table, const char * name,
                     const char * name2, const char * name3,
                     cm_hash_deal_locator f)
{
  unsigned long key;
  struct cm_hash_entry *entry;
  struct cm_hash_entry *prev = NULL;

  if (table == NULL || name == NULL)
    return (-1);

  key = cm_hash_compute_key (table, name, name2, name3);
  if (table->table[key].valid == 0)
  {
    return (-1);
  }
  else
  {
    for (entry = &(table->table[key]); entry != NULL; entry = entry->next)
    {
      if (cm_str_equal (entry->name, name) &&
          cm_str_equal (entry->name2, name2) &&
          cm_str_equal (entry->name3, name3))
      {
        if ((f != NULL) && (entry->payload != NULL))
          f (entry->payload, entry->name);
        entry->payload = NULL;
        if (entry->name)
          of_free (entry->name);
        if (entry->name2)
          of_free (entry->name2);
        if (entry->name3)
          of_free (entry->name3);
        if (prev)
        {
          prev->next = entry->next;
          of_free (entry);
        }
        else
        {
          if (entry->next == NULL)
          {
            entry->valid = 0;
          }
          else
          {
            entry = entry->next;
            memcpy (&(table->table[key]), entry, sizeof (struct cm_hash_entry));
            of_free (entry);
          }
        }
        table->nb_elems--;
        return (0);
      }
      prev = entry;
    }
    return (-1);
  }
}

int32_t
str_q_equal (const char * pref, const char * name, const char * str)
{
  if (pref == NULL)
    return (cm_str_equal (name, str));
  if (name == NULL)
    return (0);
  if (str == NULL)
    return (0);

  do
  {
    if (*pref++ != *str)
      return (0);
  }
  while ((*str++) && (*pref));
  if (*str++ != ':')
    return (0);
  do
  {
    if (*name++ != *str)
      return (0);
  }
  while (*str++);
  return (1);
}

/**
 * cm_str_equal:
 * @str1:  the first char *
 * @str2:  the second char *
 *
 * Check if both string are equal of have same content
 * Should be a bit more readable and faster than cm_str_equal()
 *
 * Returns 1 if they are equal, 0 if they are different
 */

int32_t cm_str_equal (const char * str1, const char * str2)
{
  if (str1 == str2)
    return (1);
  if (str1 == NULL)
    return (0);
  if (str2 == NULL)
    return (0);
  do
  {
    if (*str1++ != *str2)
      return (0);
  }
  while (*str2++);
  return (1);
}

                                                                                                                                   /*#endif *//* OF_XML_SCHEMA_VAL_SUPPORT */
#endif /* OF_XML_SUPPORT */
#endif /* CM_SUPPORT */
