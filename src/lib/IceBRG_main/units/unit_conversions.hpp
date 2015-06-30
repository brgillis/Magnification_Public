/**********************************************************************\
  @file unit_conversions.hpp

 **********************************************************************

 Copyright (C) 2014  Bryan R. Gillis

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

\**********************************************************************/

#ifndef _BRG_UNIT_CONVERSIONS_HPP_INCLUDED_
#define _BRG_UNIT_CONVERSIONS_HPP_INCLUDED_

#include "IceBRG_main/common.h"

namespace IceBRG{ namespace unitconv {
// All unit conversions are exact (to the limit of flt_type variables) unless noted

// Distance
// Default unit: meter (m)
constexpr flt_type mtom = 1;
constexpr flt_type mtomm = 1e3;
constexpr flt_type mmtom = 1 / mtomm;
constexpr flt_type mtocm = 1e2;
constexpr flt_type cmtom = 1 / mtocm;
constexpr flt_type mtoum = 1e6;
constexpr flt_type umtom = 1 / mtoum;
constexpr flt_type mtonm = 1e9;
constexpr flt_type nmtom = 1 / mtonm;
constexpr flt_type mtoangstrom = 1e10;
constexpr flt_type angstromtom = 1 / mtoangstrom;
constexpr flt_type mtokm = 1e-3;
constexpr flt_type kmtom = 1 / mtokm;
constexpr flt_type ltyrtom = 9460730472580800;
constexpr flt_type mtoltyr = 1 / ltyrtom;
constexpr flt_type AUtom = 149597870700;
constexpr flt_type mtoAU = 1 / AUtom;
constexpr flt_type pctom = AUtom * 648000 / pi;
constexpr flt_type mtopc = 1 / pctom;
constexpr flt_type kpctom = 1000 * pctom;
constexpr flt_type mtokpc = 1 / kpctom;
constexpr flt_type Mpctom = 1000000 * pctom;
constexpr flt_type mtoMpc = 1 / Mpctom;
constexpr flt_type mitom = 1609.344;
constexpr flt_type mtomi = 1 / mitom;
constexpr flt_type Mmitom = 1e6 * mitom;
constexpr flt_type mtoMmi = 1 / Mmitom;
constexpr flt_type fttom = 0.3048;
constexpr flt_type mtoft = 1 / fttom;
constexpr flt_type intom = .0254;
constexpr flt_type mtoin = 1 / intom;
constexpr flt_type ydtom = 0.9144;
constexpr flt_type mtoyd = 1 / ydtom;

// Time
// Default unit: second (s)
constexpr flt_type stos = 1;
constexpr flt_type stocs = 1e2;
constexpr flt_type cstos = 1 / stocs;
constexpr flt_type stoms = 1e3;
constexpr flt_type mstos = 1 / stoms;
constexpr flt_type stous = 1e6;
constexpr flt_type ustos = 1 / stous;
constexpr flt_type stons = 1e9;
constexpr flt_type nstos = 1 / stons;
constexpr flt_type mintos = 60;
constexpr flt_type stomin = 1 / mintos;
constexpr flt_type hrtos = mintos * 60;
constexpr flt_type stohr = 1 / hrtos;
constexpr flt_type daytos = hrtos * 24;
constexpr flt_type stoday = 1 / daytos;
constexpr flt_type weektos = daytos * 7;
constexpr flt_type stoweek = 1 / weektos;
constexpr flt_type yrtos = daytos * 365.25; // Julian year - Astronomic definition (exact)
constexpr flt_type stoyr = 1 / yrtos;
constexpr flt_type monthtos = yrtos / 12; // Mean month length for Julian year
constexpr flt_type stomonth = 1 / monthtos;
constexpr flt_type kyrtos = yrtos * 1e3;
constexpr flt_type stokyr = 1 / kyrtos;
constexpr flt_type Myrtos = yrtos * 1e6;
constexpr flt_type stoMyr = 1 / Myrtos;
constexpr flt_type Gyrtos = yrtos * 1e9;
constexpr flt_type stoGyr = 1 / Gyrtos;

// Velocity
// Default units: meters per second (mps)
constexpr flt_type mpstomps = 1;
constexpr flt_type mpstokmps = 1e-3;
constexpr flt_type kmpstomps = 1 / mpstokmps;
constexpr flt_type ctomps = 299792458;
constexpr flt_type mpstoc = 1 / ctomps;
constexpr flt_type mpstomiphr = mtomi / stohr;
constexpr flt_type miphr = 1 / mpstomiphr;

// Mass
// Default unit: kilogram (kg)
constexpr flt_type kgtokg = 1;
constexpr flt_type kgtogm = 1e3;
constexpr flt_type gmtokg = 1 / kgtogm;
constexpr flt_type Mearthtokg = 5.9736e24; // Approximate
constexpr flt_type kgtoMearth = 1 / Mearthtokg; // Approximate
constexpr flt_type Msuntokg = 1.9891e30; // Approximate
constexpr flt_type kgtoMsun = 1 / Msuntokg; // Approximate
constexpr flt_type kgtottMsun = kgtoMsun * 1e-10; // Approximate
constexpr flt_type ttMsuntokg = 1 / kgtottMsun; // Approximate

// Temperature
// Default unit: Kelvin (K)
constexpr flt_type KtoK = 1;
constexpr flt_type KtodegF = 1.8;
constexpr flt_type degCtoK = KtoK;
constexpr flt_type KtodegC = 1 / degCtoK;
constexpr flt_type degFtoK = 1 / KtodegF;
constexpr flt_type degCtodegF = KtodegF;
constexpr flt_type degFtodegC = degFtoK;
constexpr flt_type KtodegR = KtodegF;
constexpr flt_type degRtoK = degFtoK;
constexpr flt_type degCtodegR = KtodegF;
constexpr flt_type degRtodegC = degFtoK;

// Angle
// Default unit: radian (rad)
constexpr flt_type radtorad = 1;
constexpr flt_type degtorad = pi / 180;
constexpr flt_type radtodeg = 1 / degtorad;
constexpr flt_type degtoamin = 60;
constexpr flt_type amintodeg = 1 / degtoamin;
constexpr flt_type amintoasec = 60;
constexpr flt_type asectoamin = 1 / amintoasec;
constexpr flt_type asectodeg = asectoamin * amintodeg;
constexpr flt_type degtoasec = 1 / asectodeg;
constexpr flt_type amintorad = amintodeg * degtorad;
constexpr flt_type radtoamin = 1 / amintorad;
constexpr flt_type asectorad = asectodeg * degtorad;
constexpr flt_type radtoasec = 1 / asectorad;

// Charge
// Default unit: Coulomb (C)
constexpr flt_type CtoC = 1;
constexpr flt_type Ctoesu = 6.241509324e18; // Approximate
constexpr flt_type esutoC = 1 / Ctoesu; // Approximate

} } // namespace IceBRG::unitconv



#endif /* _BRG_UNIT_CONVERSIONS_HPP_INCLUDED_ */
