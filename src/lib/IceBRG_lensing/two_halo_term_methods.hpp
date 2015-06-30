/*
 * two_halo_term_methods.hpp
 *
 *  Created on: 17 Apr 2015
 *      Author: brg
 */

#ifndef BRG_LENSING_TWO_HALO_TERM_METHODS_HPP_
#define BRG_LENSING_TWO_HALO_TERM_METHODS_HPP_

#define DECLARE_LENSING_EXTENSION_STATIC_VARS()		\
	static IceBRG::interpolator _rho_tilde_cache_;

#define DEFINE_LENSING_EXTENSION_STATIC_VARS(class_name) \
	flt_type IceBRG::class_name::_rho_tilde_cache_;

DECLARE_LENSING_EXTENSION_STATIC_VARS()

void _cache_rho_tilde() const
{
	// TODO: Fill out
	return;
}

void _ensure_rho_tilde_cached() const
{
	if(SPCP(name)->_rho_tilde_cache_.size()==0) _cache_rho_tilde();
}

flt_type _y(const flt_type & k) const
{
	return 0;
	//return SPCP(name)->rho_tilde()/SPCP(name)->Mvir();
}

flt_type _two_halo_power_spectrum(const flt_type & k) const
{
	return SPCP(name)->_linear_power_spectrum(k) * square(_two_halo_ps_integral(k));
}

flt_type _two_halo_ps_integral(const flt_type & k) const
{
	return 0;
	//return 2 * SPCP(name)->rho_mean()*integrate( nu*nu_f_nu(nu) * lambda_nu(nu,k) * b(nu) dln nu);
}

inline flt_type _sigma(flt_type m)
{
	return 0;
	//return std::sqrt( 4*M_PI * integrate( SPCP(name)->_linear_power_spectrum(k) * _W(r,k) * square(k) dk ) );
}

inline flt_type _nu_of_m(flt_type m)
{
	return 0;
	//return delta_c / sigma(std::move(m));
}

flt_type rho_tilde(const flt_type & k) const
{
	_ensure_rho_tilde_cached();

	return SPCP(name)->_rho_tilde_cache_(k);
}


#endif /* BRG_LENSING_TWO_HALO_TERM_METHODS_HPP_ */
