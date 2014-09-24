// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2014, Oracle and/or its affiliates.

// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle

// Licensed under the Boost Software License version 1.0.
// http://www.boost.org/users/license.html

#ifndef BOOST_GEOMETRY_ITERATORS_SEGMENT_ITERATOR_TYPE_HPP
#define BOOST_GEOMETRY_ITERATORS_SEGMENT_ITERATOR_TYPE_HPP

#include <boost/range.hpp>

#include <boost/geometry/core/interior_type.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/core/ring_type.hpp>
#include <boost/geometry/core/tags.hpp>

#include <boost/geometry/iterators/concatenate_iterator.hpp>
#include <boost/geometry/iterators/flatten_iterator.hpp>
#include <boost/geometry/iterators/point_iterator_type.hpp>
#include <boost/geometry/iterators/range_segment_iterator.hpp>

#include <boost/geometry/geometries/segment.hpp>

#include <boost/geometry/iterators/dispatch/segment_iterator_type.hpp>
#include <boost/geometry/iterators/dispatch/segment_iterator.hpp>


namespace boost { namespace geometry
{


template <typename Geometry>
struct segment_iterator_value_type
{
    typedef typename detail_dispatch::point_iterator_value_type
        <
            Geometry
        >::type point_iterator_value_type;

    typedef geometry::model::referring_segment
        <
            point_iterator_value_type
        > type;
};



#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch
{


template <typename Linestring>
struct segment_iterator_type<Linestring, linestring_tag>
{
    typedef range_segment_iterator
        <
            Linestring,
            typename segment_iterator_value_type<Linestring>::type
        > type;
};


template <typename Ring>
struct segment_iterator_type<Ring, ring_tag>
{
    typedef range_segment_iterator
        <
            Ring,
            typename segment_iterator_value_type<Ring>::type
        > type;
};


template <typename Polygon>
class segment_iterator_type<Polygon, polygon_tag>
{
private:
    typedef typename detail_dispatch::point_iterator_inner_range_type
        <
            Polygon
        >::type inner_range;

public:
    typedef concatenate_iterator
        <
            range_segment_iterator
                <
                    inner_range,
                    typename segment_iterator_value_type<Polygon>::type
                >,
            flatten_iterator
                <
                    typename boost::range_iterator
                        <
                            typename geometry::interior_type<Polygon>::type
                        >::type,
                    typename dispatch::segment_iterator_type<inner_range>::type,
                    typename segment_iterator_value_type<Polygon>::type,
                    dispatch::segments_begin<inner_range>,
                    dispatch::segments_end<inner_range>,
                    typename segment_iterator_value_type<Polygon>::type
                >,
            typename segment_iterator_value_type<Polygon>::type,
            typename segment_iterator_value_type<Polygon>::type
        > type;
};


template <typename MultiLinestring>
class segment_iterator_type<MultiLinestring, multi_linestring_tag>
{
private:
    typedef typename detail_dispatch::point_iterator_inner_range_type
        <
            MultiLinestring
        >::type inner_range;

public:
    typedef flatten_iterator
        <
            typename boost::range_iterator<MultiLinestring>::type,
            typename dispatch::segment_iterator_type<inner_range>::type,
            typename segment_iterator_value_type<MultiLinestring>::type,
            dispatch::segments_begin<inner_range>,
            dispatch::segments_end<inner_range>,
            typename segment_iterator_value_type<MultiLinestring>::type
        > type;
};


template <typename MultiPolygon>
class segment_iterator_type<MultiPolygon, multi_polygon_tag>
{
private:
    typedef typename detail_dispatch::point_iterator_inner_range_type
        <
            MultiPolygon
        >::type inner_range;

public:
    typedef flatten_iterator
        <
            typename boost::range_iterator<MultiPolygon>::type,
            typename dispatch::segment_iterator_type<inner_range>::type,
            typename segment_iterator_value_type<MultiPolygon>::type,
            dispatch::segments_begin<inner_range>,
            dispatch::segments_end<inner_range>,
            typename segment_iterator_value_type<MultiPolygon>::type
        > type;
};



} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH


}} // namespace boost::geometry


#endif // BOOST_GEOMETRY_ITERATORS_SEGMENT_ITERATOR_TYPE_HPP
