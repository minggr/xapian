/* quartz_table.h: A table in a quartz database
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

#ifndef OM_HGUARD_QUARTZ_TABLE_H
#define OM_HGUARD_QUARTZ_TABLE_H

#include "config.h"
#include "quartz_types.h"
#include "quartz_table_entries.h"
#include <string>
#include <map>

class QuartzDiskTable;

/** A cursor pointing to a position in a quartz table, for reading several
 *  entries in order, or finding approximate matches.
 */
class QuartzCursor {
    private:
	/// Copying not allowed
	QuartzCursor(const QuartzCursor &);

	/// Assignment not allowed
	void operator=(const QuartzCursor &);

    public:
	/// Initialise the cursor
	QuartzCursor() : is_positioned(false) {}

	/// Destroy the cursor
	~QuartzCursor() {}

	/** Current tag.
	 *  FIXME: this is just a dummy implementation - replace with Martins
	 *  Btree manager.
	 */
	QuartzDbTag tag;

	/** Whether the cursor is positioned at a valid entry.
	 */
	bool is_positioned;
};


/** Base class for quartz tables.
 *
 *  This specifies the interface that is shared between objects which
 *  represent actual tables on disk, and objects which represent a modified
 *  table, yet to be written to disk.
 */
class QuartzTable {
    private:
	/// Copying not allowed
	QuartzTable(const QuartzTable &);

	/// Assignment not allowed
	void operator=(const QuartzTable &);
    public:
	/** Create new table.
	 */
	QuartzTable() {}

	/** Close the table.
	 */
	virtual ~QuartzTable() {}

	/** Return a count of the number of entries in the table.
	 *
	 *  @return The number of entries in the table.
	 */
	virtual quartz_tablesize_t get_entry_count() const = 0;

	/** Read an entry from the table.
	 *
	 *  If the key is found in the table, the tag will be filled with
	 *  the data associated with the key.
	 *
	 *  If the key is not found, the key will be set to the value of
	 *  the key preceding that asked for, and the tag will be filled
	 *  with the data associated with that key.
	 *
	 *  If there is no key preceding that asked for, the key and tag
	 *  will be set to a null value.  Note that, if you are testing
	 *  for this condition, you should test whether the key has a null
	 *  value, since null tag values are allowed to be stored in
	 *  tables.
	 *
	 *  @param key    The key to look for in the table.
	 *  @param tag    A tag object to fill with the value found.
	 *  @param cursor A cursor, which will be positioned to point to
	 *                the item found.
	 *
	 *  @return true if the exact key was found in the table, false
	 *          otherwise.
	 */
	virtual bool get_nearest_entry(QuartzDbKey &key,
				       QuartzDbTag &tag,
				       QuartzCursor &cursor) const = 0;

	/** Read the next entry from the table.
	 *
	 *  This moves the cursor forward one position, and then
	 *  reads an entry from the position pointed to by the cursor.
	 *  If there is no such entry, the cursor becomes invalid, (and
	 *  the method returns false).
	 *
	 *  @param key    Will be filled with the key of the item found.
	 *  @param tag    Will be filled with the tag of the item found.
	 *  @param cursor The cursor pointing to the entry.
	 *
	 *  @return true if an entry was read, false otherwise.
	 */
	virtual bool get_next_entry(QuartzDbKey &key,
				    QuartzDbTag &tag,
				    QuartzCursor &cursor) const = 0;

	/** Read an entry from the table, if and only if it is exactly that
	 *  being asked for.
	 *
	 *  If the key is found in the table, the tag will be filled with
	 *  the data associated with the key.  If the key is not found,
	 *  the tag will be unmodified.
	 *
	 *  @param key  The key to look for in the table
	 *  @param tag  A tag object to fill with the value if found.
	 *
	 *  @return true if key is found in table,
	 *          false if key is not found in table.
	 */
	virtual bool get_exact_entry(const QuartzDbKey &key,
				     QuartzDbTag & tag) const = 0;
};

/** Class managing a table in a Quartz database.
 *
 *  A table is a store holding a set of key/tag pairs.  See the
 *  documentation for QuartzDbKey and QuartzDbTag for details of what
 *  comprises a valid key or tag.
 */
class QuartzDiskTable : public QuartzTable {
    private:
	/// Copying not allowed
	QuartzDiskTable(const QuartzDiskTable &);

	/// Assignment not allowed
	void operator=(const QuartzDiskTable &);

	/** Store the data in memory for now.  FIXME: this is only to assist
	 *  the early development.
	 */
	std::map<QuartzDbKey, QuartzDbTag> data;

	/** The revision number in the first temporary file.
	 */
	quartz_revision_number_t revision1;

	/** The revision number in the second temporary file.
	 */
	quartz_revision_number_t revision2;

	/** The path at which the table is stored.
	 */
	std::string path;
	
	/** Whether the table is readonly.
	 */
	bool readonly;

	/** The current revision number.
	 */
	quartz_revision_number_t revision;

    public:
	/** Create a new table.  This does not open the table - the open()
	 *  method must be called before use is made of the table.
	 *
	 *  @param path_          - Path at which the table is stored.
	 *  @param readonly_      - whether to open the table for read only
	 *                          access.
	 *  @param blocksize_     - Size of blocks to use.  This parameter is
	 *                          only used when creating the table.
	 */
	QuartzDiskTable(std::string path_,
			bool readonly_,
			unsigned int blocksize_);

	/** Close the table.
	 */
	~QuartzDiskTable();

	/** Open the table at the specified revision.
	 *
	 *  @param revision_      - revision number to open.
	 *
	 *  @return true if table is successfully opened at desired revision,
	 *          false if table cannot be opened at desired revision.
	 *
	 *  @exception OmDatabaseCorruptError will be thrown if the table is
	 *             in a corrupt state.
	 */
	bool open(quartz_revision_number_t revision_);

	/** Open the latest revision of the table.
	 *
	 *  @exception OmDatabaseCorruptError will be thrown if the table is
	 *             in a corrupt state.
	 */
	void open();

	/** Get the revision number at which this table
	 *  is currently open.
	 *
	 *  It is possible that there are other, more recent or older
	 *  revisions available.
	 *
	 *  @return the current revision number.
	 */
	quartz_revision_number_t get_open_revision_number() const;

	/** Get the latest revision number stored in this table.
	 *
	 *  It is possible that there are other, older, revisions of this
	 *  table available, and indeed that the revision currently open
	 *  is one of these older revisions.
	 */
	quartz_revision_number_t get_latest_revision_number() const;

	/** Modify the entries in the table.
	 *
	 *  Each key / tag pair is added to the table.
	 *
	 *  If the key already exists in the table, the existing tag
	 *  is replaced by the supplied one.
	 *
	 *  If an entry is specified with a null pointer for the tag, then
	 *  the entry will be removed from the table, if it exists.  If
	 *  it does not exist, no action will be taken.
	 *
	 *  If an error occurs during the operation, this will be signalled
	 *  by a return value of false.  The table will be left in an
	 *  unmodified state.
	 *
	 *  @param entries       The key / tag pairs to store in the table.
	 *  @param new_revision  The new revision number to store.  This must
	 *          be greater than the latest revision number (see
	 *          get_latest_revision_number()), or undefined behaviour will
	 *          result.
	 *
	 *  @return true if the operation completed successfully, false
	 *          otherwise.
	 */
	bool set_entries(std::map<QuartzDbKey, QuartzDbTag *> & entries,
			 quartz_revision_number_t new_revision);

	/** Virtual methods of QuartzTable.
	 */
	//@{
	quartz_tablesize_t get_entry_count() const;
	bool get_nearest_entry(QuartzDbKey &key,
			       QuartzDbTag &tag,
			       QuartzCursor &cursor) const;
	bool get_next_entry(QuartzDbKey &key,
			    QuartzDbTag &tag,
			    QuartzCursor &cursor) const;
	bool get_exact_entry(const QuartzDbKey &key, QuartzDbTag & tag) const;
	//@}
};

/** A buffered table in a Quartz database.
 *
 *  This class provides buffered access to a quartz database.  This allows
 *  a set of modifications to be built up in memory, and flushed to disk
 *  only when it grows sufficiently large.
 */
class QuartzBufferedTable : public QuartzTable {
    private:
	/// Copying not allowed
	QuartzBufferedTable(const QuartzBufferedTable &);

	/// Assignment not allowed
	void operator=(const QuartzBufferedTable &);

	/// The underlying table
	QuartzDiskTable * disktable;

	/** Entries which have been changed.
	 */
	QuartzTableEntries changed_entries;

	/** Number of entries in the table, including the changed entries.
	 */
	quartz_tablesize_t entry_count;
    public:
	/** Create a new table.  This does not open the table - the open()
	 *  method must be called before use is made of the table.
	 *
	 */
	QuartzBufferedTable(QuartzDiskTable * disktable_);

	/** Close the table.
	 */
	~QuartzBufferedTable();

	/** Apply any outstanding changes.
	 *
	 *  If an error occurs during the operation, this will be signalled
	 *  by a return value of false.  The table on disk will be left in an
	 *  unmodified state, and the changes made to it will be lost.
	 *
	 *  @param new_revision  The new revision number to store the
	 *                       modifications under.
	 *
	 *  @return true if the operation completed successfully, false
	 *          otherwise.
	 */
	bool apply(quartz_revision_number_t new_revision);

	/** Cancel any outstanding changes.
	 *
	 *  This causes any modifications held in memory to be forgotten.
	 */
	void cancel();

	/** Determine whether the object contains modifications.
	 *
	 *  @return true if the changed_entries object contains
	 *          modifications to the database, false if no changes have
	 *          been made.
	 */
	bool is_modified();

	/** Get a pointer to the tag for a given key.
	 *
	 *  If the tag is not present in the database, or is currently
	 *  marked for deletion, this will return a null pointer.
	 *
	 *  The pointer is owned by the QuartzBufferedTable (actually, by
	 *  its changed_entries object) - it may be modified, but must not
	 *  be deleted.
	 */
	QuartzDbTag * get_tag(const QuartzDbKey &key);

	/** Get a pointer to the tag for a given key, creating a new tag if
	 *  not present.
	 *
	 *  This will never return a null pointer.
	 *
	 *  The pointer is owned by the QuartzBufferedTable (actually, by
	 *  its changed_entries object) - it may be modified, but must not
	 *  be deleted.
	 */
	QuartzDbTag * get_or_make_tag(const QuartzDbKey &key);

	/** Remove the tag for a given key.
	 *
	 *  This removes the tag for a given key.  If the tag doesn't exist,
	 *  no action is taken.
	 */
	void delete_tag(const QuartzDbKey &key);

	/** Virtual methods of QuartzTable.
	 */
	//@{
	quartz_tablesize_t get_entry_count() const;
	bool get_nearest_entry(QuartzDbKey &key,
			       QuartzDbTag &tag,
			       QuartzCursor &cursor) const;
	bool get_next_entry(QuartzDbKey &key,
			    QuartzDbTag &tag,
			    QuartzCursor &cursor) const;
	bool get_exact_entry(const QuartzDbKey &key, QuartzDbTag & tag) const;
	//@}
};

#endif /* OM_HGUARD_QUARTZ_TABLE_H */
