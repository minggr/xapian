/* branchpostlist.h: virtual base class for branched types of postlist
 *
 * Copyright 1999,2000,2001 BrightStation PLC
 * Copyright 2002 Ananova Ltd
 * Copyright 2003,2004,2007 Olly Betts
 * Copyright 2007 Lemur Consulting Ltd
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#ifndef OM_HGUARD_BRANCHPOSTLIST_H
#define OM_HGUARD_BRANCHPOSTLIST_H

#include "multimatch.h"
#include "postlist.h"

/** Base class for postlists which are generated by merging two
 *  sub-postlists.
 *
 *  These postlists form a tree which is used to perform a sum over all the
 *  terms in the query for each document, in order to calculate the score
 *  for that document.
 */
class BranchPostList : public PostList {
    private:
	// Prevent copying
	BranchPostList(const BranchPostList &);
	BranchPostList & operator=(const BranchPostList &);

    protected:
	/// Left sub-postlist
	PostList *l;

	/// Right sub-postlist
	PostList *r;

	/** The object which is using this postlist to perform
	 *  a match.  This object needs to be notified when the
	 *  tree changes such that the maximum weights need to be
	 *  recalculated.
	 */
	MultiMatch *matcher;

	/** Utility method, to call recalc_maxweight() and do the pruning
	 *  if a next() or skip_to() returns non-NULL result.
	 */
	void handle_prune(PostList *&kid, PostList *ret) {
	    if (ret) {
		delete kid;
		kid = ret;

		// now tell matcher that maximum weights need recalculation.
		matcher->recalc_maxweight();
	    }
	}

    public:
	BranchPostList(PostList *l_, PostList *r_, MultiMatch *matcher_)
		: l(l_), r(r_), matcher(matcher_) {}

	virtual ~BranchPostList();

	/** get_wdf() for branch postlists returns the sum of the wdfs of the
	 *  sub postlists.  The wdf isn't really meaningful in many situations,
	 *  but if the lists are being combined as a synonym we want the sum of
	 *  the wdfs, so we do that in general.
	 */
	virtual Xapian::termcount get_wdf() const;
};

// Helper functions - call next/skip_to on a postlist and handle any
// resulting prune
// 
// Returns true iff a prune was handled, so the caller can recalculate
// weights etc if necessary
inline bool
next_handling_prune(PostList * & pl, Xapian::weight w_min,
		    MultiMatch *matcher)
{
    PostList *p = pl->next(w_min);
    if (!p) return false;
    delete pl;
    pl = p;
    // now tell matcher that maximum weights need recalculation.
    if (matcher) matcher->recalc_maxweight();
    return true;
}

inline bool
skip_to_handling_prune(PostList * & pl, Xapian::docid did, Xapian::weight w_min,
		       MultiMatch *matcher)
{
    PostList *p = pl->skip_to(did, w_min);
    if (!p) return false;
    delete pl;
    pl = p;
    // now tell matcher that maximum weights need recalculation.
    if (matcher) matcher->recalc_maxweight();
    return true;
}

#endif /* OM_HGUARD_BRANCHPOSTLIST_H */
