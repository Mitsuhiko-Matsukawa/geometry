// Boost.Geometry

// Copyright (c) 2015-2016 Oracle and/or its affiliates.

// Contributed and/or modified by Vissarion Fysikopoulos, on behalf of Oracle

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_FORMULAS_GEOGRAPHIC_AREA_HPP
#define BOOST_GEOMETRY_FORMULAS_GEOGRAPHIC_AREA_HPP

#include <boost/geometry/formulas/andoyer_inverse.hpp>
#include <boost/geometry/formulas/thomas_inverse.hpp>
#include <boost/geometry/formulas/vincenty_inverse.hpp>

namespace boost { namespace geometry { namespace formula
{

/*!
\brief Formulas for computing spherical and ellipsoidal polygon area.
 The current class computes the area of the trapezoid defined by a segment
 the two meridians passing by the endpoints and the equator.
\author See
- Charles F.F Karney, Algorithms for geodesics, 2011
https://arxiv.org/pdf/1109.4448.pdf
*/

template <
    typename CT
>
class geographic_area
{

public:

    template <
                typename PointOfSegment,
                bool LongSegment = false
             >
    static inline CT spherical_excess(PointOfSegment const& p1,
                                      PointOfSegment const& p2)
    {
        //Spherical excess computation

        CT excess;

        if(LongSegment)
        {
            //TODO: TO BE IMPLEMENTED
            excess = 0;

        } else { // Trapezoidal formula
            CT tan_lat1 =
                    tan(geometry::get_as_radian<1>(p1) / 2.0);
            CT tan_lat2 =
                    tan(geometry::get_as_radian<1>(p2) / 2.0);

            excess = 2.0
                    * atan(((tan_lat1 + tan_lat2) / (1 + tan_lat1 * tan_lat2))
                           * tan((geometry::get_as_radian<0>(p2)
                                - geometry::get_as_radian<0>(p1)) / 2));
        }

        return excess;
    }

    template <
                typename AzimuthStrategy,
                size_t SeriesOrder = 2,
                typename PointOfSegment,
                typename SpheroidConst
             >
    static inline CT ellipsoidal_correction(PointOfSegment const& p1,
                                            PointOfSegment const& p2,
                                            SpheroidConst spheroid_const)
    {

        //Azimuth Approximation

        CT alp1, alp2;


        alp1 = AzimuthStrategy::apply(get_as_radian<0>(p1),
                                      get_as_radian<1>(p1),
                                      get_as_radian<0>(p2),
                                      get_as_radian<1>(p2),
                                      spheroid_const.spheroid).azimuth;

        alp2 = AzimuthStrategy::apply(get_as_radian<0>(p1),
                                      get_as_radian<1>(p1),
                                      get_as_radian<0>(p2),
                                      get_as_radian<1>(p2),
                                      spheroid_const.spheroid).reverse_azimuth;
        //Integral approximation

        CT result;


        // Epsoidal constants
        CT const a2 = spheroid_const.a2;
        CT const e2 = spheroid_const.e2;
        CT const ep2 = spheroid_const.ep2;
        CT const ep = spheroid_const.ep;
        CT const c2 = spheroid_const.c2;  // authalic radius


        //CT cos_a1 = geometry::math:cos(a1);
        //CT sin_a1 = geometry::math:sin(a1);
        //CT sin_ph1 = geometry::math:sin(get_as_radian<1>(p1));

        //CT cos_a0 = std::hypot(cos_a1, sin_a1 * sin_ph1);
        //CT sin_a0 = math::sqrt(1 - cos_a0 * cos_a0);

        CT cos_bet1 = cos(get_as_radian<1>(p1));
        CT cos_bet2 = cos(get_as_radian<1>(p2));

        CT sin_alp1 = sin(alp1);
        CT cos_alp1 = cos(alp1);
        //CT sin_alp2 = sin(alp2);
        CT cos_alp2 = cos(alp2);

        CT sin_alp0 = sin_alp1 * cos_bet1;
        //TODO: do it more accurate!
        CT cos_alp0 = math::sqrt(1 - math::sqr(sin_alp0));

        //CT sin_sig1 = sin_bet1;
        CT cos_sig1 = cos_alp1 * cos_bet1;
        //CT sin_sig2 = sin_bet2;
        CT cos_sig2 = cos_alp2 * cos_bet2;

        CT cos3_sig1 = 4.0 * std::pow(cos_sig1,3) - 3.0 * cos_sig1;
        CT cos3_sig2 = 4.0 * std::pow(cos_sig2,3) - 3.0 * cos_sig2;

        CT k2 = math::sqr(ep * cos_alp0);

        CT I12;

        if(SeriesOrder == 1)
        {
            CT c_40 = ((2.0 / 3.0) - (1.0 / 15.0) * ep2)
                    - ((1.0 / 20.0) - (1.0 / 35.0) * ep2) * k2;
            CT c_41 = ((1.0 / 180.0) - (1.0 / 315.0) * ep2) * k2;

            I12 = c_40 * (cos_sig2 - cos_sig1)
                    + c_41 * (cos3_sig2 - cos3_sig1);
        }
        if(SeriesOrder == 2)
        {
            CT ep4 = math::sqr(ep2);
            CT k4 = math::sqr(k2);

            CT c_40 = ((2.0 / 3.0) - (1.0 / 15.0) * ep2
                       + (4.0 / 105.0) * ep4)
                    - ((1.0 / 20.0) - (1.0 / 35.0) * ep2
                       + (2.0 /105.0) * ep4) * k2
                    + ((1.0 / 42.0) - (1.0 / 63.0) * ep2
                       + (8.0 / 693.0) * ep4) * k4;

            CT c_41 = ((1.0 / 180.0) - (1.0 / 315.0) * ep2
                       + (2.0 / 945.0) * ep4) * k2
                    - ((1.0 / 252.0) - (1.0 / 378.0) * ep2
                       + (4.0 / 2079.0) * ep4) * k4;

            CT c_42 = ((1.0 / 2100.0) - (1.0 / 3150.0) * ep2
                       + (4.0 / 17325.0) * ep4) * k4;

            CT cos5_sig1 = 16.0 * std::pow(cos_sig1,5)
                    - 20.0 * std::pow(cos_sig1,3)
                    + 5.0 * cos_sig1;
            CT cos5_sig2 = 16.0 * std::pow(cos_sig2,5)
                    - 20.0 * std::pow(cos_sig2,3)
                    + 5.0 * cos_sig2;

            I12 = c_40 * (cos_sig2 - cos_sig1)
                    + c_41 * (cos3_sig2 - cos3_sig1)
                    + c_42 * (cos5_sig2 - cos5_sig1);
        }

        return cos_alp0 * sin_alp0 * I12;

    }

    // Keep track whenever a segment crosses the prime meridian
    // First normalize to [0,360)
    template <typename PointOfSegment, typename StateType>
    static inline int crosses_prime_meridian(PointOfSegment const& p1,
                                             PointOfSegment const& p2,
                                             StateType& state)
    {
        CT const pi
            = geometry::math::pi<CT>();
        CT const two_pi
            = geometry::math::two_pi<CT>();

        CT p1_lon = get_as_radian<0>(p1)
                                - ( floor( get_as_radian<0>(p1) / two_pi )
                                  * two_pi );
        CT p2_lon = get_as_radian<0>(p2)
                                - ( floor( get_as_radian<0>(p2) / two_pi )
                                  * two_pi );

        CT max_lon = std::max(p1_lon, p2_lon);
        CT min_lon = std::min(p1_lon, p2_lon);

        if(max_lon > pi && min_lon < pi && max_lon - min_lon > pi)
        {
            state.crosses_prime_meridian++;
        }

        return state.crosses_prime_meridian;
    }

};

}}} // namespace boost::geometry::formula


#endif // BOOST_GEOMETRY_FORMULAS_GEOGRAPHIC_AREA_HPP
