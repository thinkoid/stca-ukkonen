// -*- mode: c++ -*-

//
// E. Ukkonen: Constructing suffix trees on-line in linear time.
// Proc. Information Processing 92, Vol. 1, IFIP Transactions A-12,
// 484-492, Elsevier 1992.
//

#ifndef UKKONEN_SUFFIX_TREE_HPP
#define UKKONEN_SUFFIX_TREE_HPP

#include <iostream>
#include <vector>
#include <tuple>

namespace ukkonen {

template< typename CharT, typename TraitsT = char_traits< CharT > >
struct suffix_tree_t {
    using traits_type = TraitsT;

    using char_type = typename traits_type::char_type;
    using  int_type = typename traits_type::int_type;
    using  pos_type = typename traits_type::pos_type;

    using size_type = size_t;

    using string_type = basic_string< char_type, traits_type >;
    string_type text;

    //
    // 0: link, 1: transition set index
    //
    using node_type = tuple< size_type, size_type >;
    vector< node_type > nodes;

    //
    // 0: transition character, 1: edge index
    //
    using transition_type = tuple< int_type, size_type >;
    vector< vector< transition_type > > transitions;

    //
    // 0: s, 1: k, 2: p, 3: s'
    //
    using edge_type = tuple< size_t, int_type, int_type, size_type >;
    vector< edge_type > edges;

    static constexpr size_type aux = 0, root = 1;
};

template< typename T, typename U >
/* static */ constexpr typename suffix_tree_t< T, U >::size_type
suffix_tree_t< T, U >::aux;

template< typename T, typename U >
/* static */ constexpr typename suffix_tree_t< T, U >::size_type
suffix_tree_t< T, U >::root;

#include "st.cc"

template< typename T = char, typename U = char_traits< T > >
suffix_tree_t< T, U >
make_suffix_tree (const typename suffix_tree_t< T, U >::string_type& text) {
    using tree_type = suffix_tree_t< T, U >;
    using size_type = typename tree_type::size_type;

    tree_type t {
        text + T ('~'),
        { { tree_type::root, 1 }, { tree_type::aux, 0 } },
        { { }, { { -1, 0 } } },
        { { tree_type::aux, 0, 0, tree_type::root } }
    };

    size_type s = tree_type::root;

    for (int k = 0, i = 0; i < int (t.text.size ()); ++i) {
        tie (s, k) = detail::update   (t, s, { k, i });
        tie (s, k) = detail::canonize (t, s, { k, i });
    }

    return t;
}

} // namespace ukkonen

#endif // UKKONEN_SUFFIX_TREE_HPP
