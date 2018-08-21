
#include "test/tour/tsp_two_level_tree.hpp"

#include "tour/tsp_tour.hpp"
#include "tour/tsp_tour_reverse.hpp"
#include "tour/tsp_vector_tour.hpp"
#include "tour/tsp_two_level_tree_tour.hpp"


TEST(TwoLevelTreeTour, Constructor) {
    for (auto i = 1; i < 10; ++i) {
        std::vector<Index> vs(10);
        iota(vs.begin(), vs.end(), 0);

        TwoLevelTreeTour tour(10);
        Check(tour, vs);
    }
}

TEST(TwoLevelTreeTour, Reverse_Small) {
    {
        TwoLevelTreeTour tour(2);
        tour.Reverse(0, 1);
        Check(tour, {1, 0});
        tour.Reverse(0, 1);
        Check(tour, {0, 1});
        tour.Reverse(0, 0);
        Check(tour, {0, 1});
    }
    {
        TwoLevelTreeTour tour(4);
        tour.Reverse(0, 3);
        Check(tour, {3, 2, 1, 0});
        tour.Reverse(0, 2);
        Check(tour, {3, 0, 1, 2});
        tour.Reverse(1, 3);
        Check(tour, {1, 0, 3, 2});
        tour.Reverse(0, 3);
        Check(tour, {1, 3, 0, 2});
        tour.Reverse(0, 2);
        Check(tour, {1, 3, 2, 0});
        tour.Reverse(0, 2);
        Check(tour, {1, 3, 0, 2});
    }
}

TEST(TwoLevelTreeTour, Reverse_Big) {
    for (auto n = 1; n <= 16; ++n) {
        std::vector<Index> vs(n);
        iota(vs.begin(), vs.end(), 0);

        TwoLevelTreeTour tour(n);
        for (auto i = 0; i < n; ++i) {
            for (auto j = i; j < n; ++j) {
                // cities at positions
                auto a = vs[i];
                auto b = vs[j];

                reverse(vs.begin()+i, vs.begin()+j+1);
                tour.Reverse(a, b);

                Check(tour, vs);
            }
        }
    }
}