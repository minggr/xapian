/* quartz_types.h: Types used by quartz backend
 *
 * ----START-LICENCE----
 * Copyright 1999,2000 BrightStation PLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 * -----END-LICENCE-----
 */

#ifndef OM_HGUARD_QUARTZ_TYPES_H
#define OM_HGUARD_QUARTZ_TYPES_H

#include "config.h"

typedef unsigned int quartz_blocksize_t;

/** A type used to store a revision number for a table.
 *
 *  A table's revision number increases monotonically, incrementing by
 *  one each time a modification is applied to the table.
 *
 *  FIXME: ensure that this is of a suitable minimum size - 32 bits is
 *  satisfactory in most cases, but in extreme cases could cause the revision
 *  number to roll over after a few years.  It would be better to use 64 bits
 *  (and / or to ensure that rolling over causes no problem).
 */
typedef unsigned long int quartz_revision_number_t;

/** A type used to store the number of entries in a table.
 *
 *  Again, this must be of suitable minimum size.
 */
typedef unsigned long int quartz_tablesize_t;

/** An integer type for storing the length of a document - ie, the sum of the
 *  wdfs of the terms in the document.
 */
typedef unsigned long int quartz_doclen_t;

/** An integer type which can store the sum of the lengths of the documents
 *  in the database.
 *
 *  FIXME - change this to a double?
 */
typedef unsigned long long int quartz_totlen_t;

/** The maximum length that a key is allowed to have in a B-tree table.
 */
#define MAX_KEY_LENGTH 252

/** The default block size to use in a B-tree table.
 *  If this is changed, be sure to update the API documentation
 *  correspondingly.
 */
#define QUARTZ_BTREE_DEF_BLOCK_SIZE 8192

#endif /* OM_HGUARD_QUARTZ_TYPES_H */
