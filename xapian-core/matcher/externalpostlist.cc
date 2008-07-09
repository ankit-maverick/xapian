/** @file externalpostlist.cc
 * @brief Return document ids from an external source.
 */
/* Copyright 2008 Olly Betts
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <config.h>

#include "externalpostlist.h"

#include <xapian/postingsource.h>

#include "omassert.h"
#include "omdebug.h"

using namespace std;

ExternalPostList::ExternalPostList(Xapian::PostingSource *source_,
				   double factor_)
    : source(source_), current(0), factor(factor_)
{
    Assert(source);
    source->reset();
}

Xapian::doccount
ExternalPostList::get_termfreq_min() const
{
    Assert(source);
    return source->get_termfreq_min();
}

Xapian::doccount
ExternalPostList::get_termfreq_est() const
{
    Assert(source);
    return source->get_termfreq_est();
}

Xapian::doccount
ExternalPostList::get_termfreq_max() const
{
    Assert(source);
    return source->get_termfreq_max();
}

Xapian::weight
ExternalPostList::get_maxweight() const
{
    DEBUGCALL(MATCH, Xapian::weight, "ExternalPostList::get_maxweight", "");
    Assert(source);
    if (factor == 0.0) RETURN(factor);
    RETURN(factor * source->get_maxweight());
}

Xapian::docid
ExternalPostList::get_docid() const
{
    DEBUGCALL(MATCH, Xapian::docid, "ExternalPostList::get_docid", "");
    Assert(current);
    RETURN(current);
}

Xapian::weight
ExternalPostList::get_weight() const
{
    DEBUGCALL(MATCH, Xapian::weight, "ExternalPostList::get_weight", "");
    Assert(source);
    if (factor == 0.0) RETURN(factor);
    RETURN(factor * source->get_weight());
}

Xapian::doclength
ExternalPostList::get_doclength() const
{
    // FIXME
    return 0;
}

Xapian::weight
ExternalPostList::recalc_maxweight()
{
    return ExternalPostList::get_maxweight();
}

PositionList *
ExternalPostList::read_position_list()
{
    return NULL;
}

PositionList *
ExternalPostList::open_position_list() const
{
    return NULL;
}

PostList *
ExternalPostList::update_after_advance() {
    DEBUGCALL(MATCH, PostList *, "ExternalPostList::update_after_advance", "");
    Assert(source);
    if (source->at_end()) {
	DEBUGLINE(MATCH, "ExternalPostList now at end");
	source = NULL;
    } else {
	current = source->get_docid();
    }
    RETURN(NULL);
}

PostList *
ExternalPostList::next(Xapian::weight w_min)
{
    DEBUGCALL(MATCH, PostList *, "ExternalPostList::next", w_min);
    Assert(source);
    source->next(w_min);
    RETURN(update_after_advance());
}

PostList *
ExternalPostList::skip_to(Xapian::docid did, Xapian::weight w_min)
{
    DEBUGCALL(MATCH, PostList *, "ExternalPostList::skip_to",
	      did << ", " << w_min);
    Assert(source);
    if (did <= current) RETURN(NULL);
    source->skip_to(did, w_min);
    RETURN(update_after_advance());
}

PostList *
ExternalPostList::check(Xapian::docid did, Xapian::weight w_min, bool &valid)
{
    DEBUGCALL(MATCH, PostList *, "ExternalPostList::check",
	      did << ", " << w_min << ", <valid>");
    Assert(source);
    if (did <= current) {
	valid = true;
	RETURN(NULL);
    }
    valid = source->check(did, w_min);
    if (source->at_end()) {
	DEBUGLINE(MATCH, "ExternalPostList now at end");
	source = NULL;
    } else {
	current = valid ? source->get_docid() : current;
    }
    RETURN(NULL);
}

bool
ExternalPostList::at_end() const
{
    DEBUGCALL(MATCH, bool, "ExternalPostList::at_end", "");
    RETURN(source == NULL);
}

string
ExternalPostList::get_description() const
{
    string desc = "ExternalPostList(";
    if (source) desc += source->get_description();
    desc += ")";
    return desc;
}
