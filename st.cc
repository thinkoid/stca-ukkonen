// -*- mode: c++ -*-

#include <limits>
#include <tuple>

#include "st.hpp"

namespace detail {

template< typename T, typename U >
inline const typename suffix_tree_t< T, U >::node_type&
node_at (const suffix_tree_t< T, U >& t, size_t s) {
    assert (s < t.nodes.size ());
    return t.nodes [s];
}

template< typename T, typename U >
inline typename suffix_tree_t< T, U >::node_type&
node_at (suffix_tree_t< T, U >& t, size_t s) {
    assert (s < t.nodes.size ());
    return t.nodes [s];
}

template< typename Iterator >
inline bool
has_transition (Iterator first, Iterator last, int c) {
    auto iter = find_if (first, last, [&](const auto& arg) {
            const auto x = get<0> (arg);
            return x == c || x < 0;
        });

    return iter != last;
}

template< typename T, typename U >
inline bool
has_transition (const suffix_tree_t< T, U >& t, size_t s, int c) {
    using tree_type = suffix_tree_t< T, U >;
    using size_type = typename tree_type::size_type;

    bool b = tree_type::aux == s;

    if (!b) {
        size_type ignore, i;
        tie (ignore, i) = t.nodes [s];

        if (i && i < t.transitions.size ()) {
            const auto& ts = t.transitions [i];
            b = has_transition (ts.begin (), ts.end (), c);
        }
    }

    return b;
}

template< typename T, typename U >
inline size_t&
link (suffix_tree_t< T, U >& t, size_t s) {
    assert (s < t.nodes.size ());
    return get<0> (t.nodes [s]);
}

template< typename T, typename U >
inline const size_t&
link (const suffix_tree_t< T, U >& t, size_t s) {
    assert (s < t.nodes.size ());
    return get<0> (t.nodes [s]);
}

template< typename T, typename U >
inline size_t
make_transitions (suffix_tree_t< T, U >& t, size_t s) {
    const auto n = t.transitions.size ();
    t.transitions.emplace_back ();

    auto& node = t.nodes [s];
    get<1> (node) = n;

    return n;
}

template< typename T, typename U >
tuple< size_t, int, int, size_t >
g_ (const suffix_tree_t< T, U >& t, size_t s, int c) {
    using tree_type = suffix_tree_t< T, U >;

    using  int_type = typename tree_type::int_type;
    using size_type = typename tree_type::size_type;

    size_type s_ = tree_type::root;
    int_type k = 0, p = 0;

    if (tree_type::aux != s) {
        //
        // Only investigate transitions that do not originate in the auxilliary
        // state:
        //
        size_type ignore, i;
        tie (ignore, i) = t.nodes [s];

        assert (i && i < t.transitions.size ());
        const auto& ts = t.transitions [i];

        const auto iter = find_if (
            ts.begin (), ts.end (), [&](const auto& arg) {
                const auto x = get<0> (arg);
                return c == x;
            });

        assert (iter != ts.end ());

        const auto e = get<1> (*iter);
        assert (e < t.edges.size ());

        tie (s, k, p, s_) = t.edges [e];
    }

    return tie (s, k, p, s_);
}

//
// Find and return the transition details if it exists, possibly modifying it
// with the incoming arguments (as in adjusting an edge, etc.); create a new one
// if not:
//
template< typename T, typename U >
typename suffix_tree_t< T, U >::size_type&
g_ (suffix_tree_t< T, U >& t, size_t s, tuple< int, int > ref) {
    using tree_type = suffix_tree_t< T, U >;

    using size_type = typename tree_type::size_type;
    using  int_type = typename tree_type::int_type;

    using edge_type = typename tree_type::edge_type;

    int_type k, p;
    tie (k, p) = ref;

    size_type ignore, i, *s_ = 0;
    tie (ignore, i) = t.nodes [s];

    if (0 == i) {
        //
        // Node has no transitions:
        //
        i = make_transitions (t, s);
    }

    auto& ts = t.transitions [i];

    auto iter = find_if (
        ts.begin (), ts.end (), [&](const auto& arg) {
            const auto x = get<0> (arg);
            return t.text [k] == x;
        });

    if (iter == ts.end ()) {
        //
        // Insert a new transition/edge if not already present:
        //
        const auto e = t.edges.size ();

        t.edges.emplace_back (edge_type { s, k, p, { } });
        ts.emplace_back (t.text [k], e);

        s_ = &get<3> (t.edges.back ());
    }
    else {
        //
        // Modify the existing transition/edge with incoming arguments:
        //
        auto& e = t.edges [get<1> (*iter)];

        get<1> (e) = k;
        get<2> (e) = p;

        s_ = &get<3> (e);
    }

    return *s_;
}

////////////////////////////////////////////////////////////////////////

//
// canonize, test_and_split, and update implementations are lifted verbatim (as
// much as permitted by the conversion from pseudocode) from the Ukkonen paper:
//

template< typename T, typename U >
tuple< size_t, int >
canonize (const suffix_tree_t< T, U >& t, size_t s, tuple< int, int > ref) {
    using tree_type = suffix_tree_t< T, U >;

    using size_type = typename tree_type::size_type;
    using  int_type = typename tree_type::int_type;

    size_type s_, ignore;
    int_type k, p, k_, p_;

    tie (k, p) = ref;
    assert (k >= 0);

    if (k <= p) {
        const auto tk = t.text [k];

        tie (ignore, k_, p_, s_) = g_ (t, s, tk);
        assert (k_ >= 0);

        while (p_ - k_ <= p - k) {
            k = k + p_ - k_ + 1;
            s = s_;

            if (k <= p) {
                const auto tk = t.text [k];

                tie (ignore, k_, p_, s_) = g_ (t, s, tk);
                assert (k_ >= 0);
            }
        }
    }

    return { s, k };
}

template< typename T, typename U >
tuple< size_t, bool >
test_and_split (
    suffix_tree_t< T, U >& t, size_t s, tuple< int, int > ref, int c) {
    using tree_type = suffix_tree_t< T, U >;

    using size_type = typename tree_type::size_type;
    using  int_type = typename tree_type::int_type;

    using node_type = typename tree_type::node_type;

    size_type s_, ignore;
    int_type k, p, k_, p_;

    tie (k, p) = ref;
    assert (k >= 0);

    if (k <= p) {
        const auto tk = t.text [k];

        //
        // Find tk-transition g'(s,(k',p'))=s':
        //
        tie (ignore, k_, p_, s_) = g_ (t, s, tk);
        assert (k_ >= 0);

        if (c == t.text [k_ + p - k + 1])
            return { s, true };
        else {
            //
            // Create new state, r:
            //
            const auto r = t.nodes.size ();
            t.nodes.emplace_back (node_type { });

            //
            // Modify existing transition, g'(s,(k',k'+p-k))=r, and create a new
            // transition, g'(r,(k'+p-k+1,p'))=s':
            //
            g_ (t, s, { k_, k_ + p - k }) = r;
            g_ (t, r, { k_ + p - k + 1, p_ }) = s_;

            return { r, false };
        }
    }
    else {
        return { s, has_transition (t, s, c) };
    }
}

template< typename T, typename U >
tuple< size_t, int >
update (suffix_tree_t< T, U >& t, size_t s, tuple< int, int > ref) {
    using tree_type = suffix_tree_t< T, U >;

    using size_type = typename tree_type::size_type;
    using  int_type = typename tree_type::int_type;

    int_type k, i;

    tie (k, i) = ref;
    assert (k >= 0);

    const auto ti = t.text [i];

    bool b;
    size_type oldr = tree_type::root, r = 0;

    tie (r, b) = test_and_split (t, s, { k, i - 1 }, ti);

    while (!b) {
        //
        // Create new transition g'(r,(i,∞))=r'
        //
        g_ (t, r, { i, (numeric_limits< int_type >::max) () });

        if (oldr != tree_type::root)
            link (t, oldr) = r;

        oldr = r;

        tie (s, k) = canonize (t, link (t, s), { k, i - 1 });
        tie (r, b) = test_and_split (t, s, { k, i - 1 }, t.text [i]);
    }

    if (oldr != tree_type::root)
        link (t, oldr) = s;

    return { s, k };
}

}
