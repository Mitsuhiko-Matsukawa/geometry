// Boost.Geometry (aka GGL, Generic Geometry Library)
// Unit Test

// Copyright (c) 2014, Oracle and/or its affiliates.

// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle

// Licensed under the Boost Software License version 1.0.
// http://www.boost.org/users/license.html

#include <iostream>

#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE test_distance_spherical_equatorial_pl_pl
#endif

#include <boost/range.hpp>

#include <boost/test/included/unit_test.hpp>
#include <boost/geometry/strategies/strategies.hpp>

#include "test_distance_se_common.hpp"


typedef bg::cs::spherical_equatorial<bg::degree> cs_type;
typedef bg::model::point<double, 2, cs_type> point_type;
typedef bg::model::multi_point<point_type> multi_point_type;

namespace services = bg::strategy::distance::services;
typedef bg::default_distance_result<point_type>::type return_type;

typedef bg::strategy::distance::haversine<double> point_point_strategy;

//===========================================================================

inline bg::default_distance_result<point_type>::type
distance_from_wkt(std::string const& wkt1, std::string const& wkt2)
{
    point_type p1, p2;
    bg::read_wkt(wkt1, p1);
    bg::read_wkt(wkt2, p2);
    return bg::distance(p1, p2);
}

inline bg::default_comparable_distance_result<point_type>::type
comparable_distance_from_wkt(std::string const& wkt1, std::string const& wkt2)
{
    point_type p1, p2;
    bg::read_wkt(wkt1, p1);
    bg::read_wkt(wkt2, p2);
    return bg::comparable_distance(p1, p2);
}

//===========================================================================

template <typename Strategy>
void test_distance_point_point(Strategy const& strategy)
{
#ifdef BOOST_GEOMETRY_TEST_DEBUG
    std::cout << std::endl;
    std::cout << "point/point distance tests" << std::endl;
#endif
    typedef test_distance_of_geometries<point_type, point_type> tester;

    tester::apply("POINT(10 10)",
                  "POINT(0 0)",
                  0.24619691677893202,
                  0.0150768448035229,
                  strategy);
    tester::apply("POINT(10 10)",
                  "POINT(10 10)",
                  0, 0, strategy);

    // antipodal points
    tester::apply("POINT(0 10)",
                  "POINT(180 -10)",
                  180.0 * bg::math::d2r, 1.0, strategy);
}

//===========================================================================

template <typename Strategy>
void test_distance_point_multipoint(Strategy const& strategy)
{
#ifdef BOOST_GEOMETRY_TEST_DEBUG
    std::cout << std::endl;
    std::cout << "point/multipoint distance tests" << std::endl;
#endif
    typedef test_distance_of_geometries<point_type, multi_point_type> tester;

    tester::apply("POINT(10 10)",
                  "MULTIPOINT(10 10,20 10,20 20,10 20)",
                  0, 0, strategy);
    tester::apply("POINT(10 10)",
                  "MULTIPOINT(20 20,20 30,30 20,30 30)",
                  distance_from_wkt("POINT(10 10)", "POINT(20 20)"),
                  comparable_distance_from_wkt("POINT(10 10)", "POINT(20 20)"),
                  strategy);
    tester::apply("POINT(3 0)",
                  "MULTIPOINT(20 20,20 40,40 20,40 40)",
                  distance_from_wkt("POINT(3 0)", "POINT(20 20)"),
                  comparable_distance_from_wkt("POINT(3 0)", "POINT(20 20)"),
                  strategy);

    // almost antipodal points
    tester::apply("POINT(179 2)",
                  "MULTIPOINT(3 3,4 3,4 4,3 4)",
                  distance_from_wkt("POINT(179 2)", "POINT(4 4)"),
                  comparable_distance_from_wkt("POINT(179 2)", "POINT(4 4)"),
                  strategy);

    // minimum distance across the dateline
    tester::apply("POINT(355 5)",
                  "MULTIPOINT(10 10,20 10,20 20,10 20)",
                  distance_from_wkt("POINT(355 5)", "POINT(10 10)"),
                  comparable_distance_from_wkt("POINT(355 5)", "POINT(10 10)"),
                  strategy);
}

//===========================================================================

template <typename Strategy>
void test_distance_multipoint_multipoint(Strategy const& strategy)
{
#ifdef BOOST_GEOMETRY_TEST_DEBUG
    std::cout << std::endl;
    std::cout << "multipoint/multipoint distance tests" << std::endl;
#endif
    typedef test_distance_of_geometries
        <
            multi_point_type, multi_point_type
        > tester;

    tester::apply("MULTIPOINT(10 10,11 10,10 11,11 11)",
                  "MULTIPOINT(11 11,12 11,12 12,11 12)",
                  0, 0, strategy);
    tester::apply("MULTIPOINT(10 10,11 10,10 11,11 11)",
                  "MULTIPOINT(12 12,12 13,13 12,13 13)",
                  distance_from_wkt("POINT(11 11)", "POINT(12 12)"),
                  comparable_distance_from_wkt("POINT(11 11)", "POINT(12 12)"),
                  strategy);

    // example with many points in each multi-point so that the r-tree
    // does some splitting.

    tester::apply("MULTIPOINT(1 1,1 2,1 3,1 4,1 5,1 6,1 7,1 8,1 9,1 10,\
                  2 1,2 2,2 3,2 4,2 5,2 6,2 7,2 8,2 9,2 10,\
                  3 1,3 2,3 3,3 4,3 5,3 6,3 7,3 8,3 9,3 10,\
                  10 1,10 10)",
                  "MULTIPOINT(11 11,11 12,11 13,11 14,11 15,\
                  11 16,11 17,11 18,11 19,11 20,\
                  12 11,12 12,12 13,12 24,12 15,\
                  12 16,12 17,12 18,12 29,12 20,\
                  13 11,13 12,13 13,13 24,13 15,\
                  13 16,13 17,13 18,13 29,13 20,\
                  20 11,20 20)",
                  distance_from_wkt("POINT(10 10)", "POINT(11 11)"),
                  comparable_distance_from_wkt("POINT(10 10)", "POINT(11 11)"),
                  strategy);

}

//===========================================================================

template <typename Point, typename Strategy>
void test_more_empty_input_pointlike_pointlike(Strategy const& strategy)
{
#ifdef BOOST_GEOMETRY_TEST_DEBUG
    std::cout << std::endl;
    std::cout << "testing on empty inputs... " << std::flush;
#endif
    bg::model::multi_point<Point> multipoint_empty;

    Point point = from_wkt<Point>("POINT(0 0)");

    // 1st geometry is empty
    test_empty_input(multipoint_empty, point, strategy);

    // 2nd geometry is empty
    test_empty_input(point, multipoint_empty, strategy);

    // both geometries are empty
    test_empty_input(multipoint_empty, multipoint_empty, strategy);

#ifdef BOOST_GEOMETRY_TEST_DEBUG
    std::cout << "done!" << std::endl;
#endif
}

//===========================================================================

BOOST_AUTO_TEST_CASE( test_all_point_point )
{
    test_distance_point_point(point_point_strategy());
    test_distance_point_point(point_point_strategy(earth_radius_km));
    test_distance_point_point(point_point_strategy(earth_radius_miles));
}

BOOST_AUTO_TEST_CASE( test_all_point_multipoint )
{
    test_distance_point_multipoint(point_point_strategy());
    test_distance_point_multipoint(point_point_strategy(earth_radius_km));
    test_distance_point_multipoint(point_point_strategy(earth_radius_miles));
}

BOOST_AUTO_TEST_CASE( test_all_multipoint_multipoint )
{
    test_distance_multipoint_multipoint(point_point_strategy());
    test_distance_multipoint_multipoint(point_point_strategy(earth_radius_km));
    test_distance_multipoint_multipoint(point_point_strategy(earth_radius_miles));
}

BOOST_AUTO_TEST_CASE( test_all_empty_input_pointlike_pointlike )
{
    test_more_empty_input_pointlike_pointlike
        <
            point_type
        >(point_point_strategy());

    test_more_empty_input_pointlike_pointlike
        <
            point_type
        >(point_point_strategy(earth_radius_km));

    test_more_empty_input_pointlike_pointlike
        <
            point_type
        >(point_point_strategy(earth_radius_miles));
}
