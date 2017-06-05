#include <cassert>
#include <cstring>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#include "st.hpp"

const vector< string > test_data {
    "A",
    "AA",
    "AAA",
    "AAAAAAA",
    "AAAAAAAAAAAA",
    "ABCABC",
    "ABCABCABCABC",
    "ABCABCABCABCABCABCABCABC",
    "ABCATBC",
    "ABCABCABTCABC",
    "ABCABCABCABCABCABTCABCABC",
    "ABCABDABXYABCABDABXZABCABDABXWSABCABDABXYABCABD"
    "ABXZABCABDABXWTABCABDABXYABCABDABXZABCABDABXWU"
};


#if 0

#include <benchmark/benchmark.h>

static string
make_label (const string& s) {
    stringstream ss;

    if (s.size () > 6) {
        ss << s.substr (0, 6) << "...";
        ss << "(" << s.size () << ")";
    }
    else
        ss << s;

    return ss.str ();
}

static void
BM_tree_construction (benchmark::State& state) {
    const string s = test_data [state.range (0)] + "$";

    state.SetLabel (make_label (s));

    while (state.KeepRunning ())
        benchmark::DoNotOptimize (make_suffix_tree< > (s));
}

BENCHMARK (BM_tree_construction)->DenseRange (0, test_data.size () - 1);

BENCHMARK_MAIN ();

#else

struct dot_graph_t {
    template< typename T, typename U >
    explicit dot_graph_t (const ukkonen::suffix_tree_t< T, U >& t)
        : value_ (make_dot (t))
        { }

    const string& value () const {
        return value_;
    }

private:
    template< typename T, typename U >
    static string
    make_dot (const ukkonen::suffix_tree_t< T, U >& t) {
        stringstream ss;

        ss << "#+BEGIN_SRC dot :file t.png :cmdline -Kdot -Tpng\n";
        ss << "digraph g {\n";

        auto first = t.nodes.size (), last = first;

        for (const auto& e : t.edges) {
            size_t s, s_;
            int k, p;

            tie (s, k, p, s_) = e;

            if (0 == s)
                continue;

            ss << "    " << s << " -> " << (0 == s_ ? last++ : s_) << " [label=\""
               << t.text.substr (k, p - k + 1)
               << "\"];\n";
        }

        for (; first != last; ++first)
            ss << "    " << first << " [shape=point];\n";

        ss << "}\n";
        ss << "#+END_SRC\n\n";

        return ss.str ();
    }

private:
    string value_;
};

int main (int, char** argv) {
    const auto t = ukkonen::make_suffix_tree< > (argv [1]);
    cout << dot_graph_t (t).value () << endl;
    return 0;
}

#endif
