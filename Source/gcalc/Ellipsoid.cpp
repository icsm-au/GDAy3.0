#include "StdAfx.h"
#include "GCalc.h"
#include "Ellipsoid.h"
#include "math.h"

#ifndef PI
#define PI 3.141592653589793
#endif

double Rad180Mod(double dValue)
{
	if (dValue < -PI)
		return dValue + (PI + PI);
	else if (dValue > PI)
		return dValue - (PI + PI);
	return dValue;
}
double radians(const double& degrees) { return (degrees * PI / 180.0); }
double degrees(const double& radians) { return (Rad180Mod(radians) * 180.0 / PI); }
	

CEllipsoid::CEllipsoid(void)
	: m_strEllipsoidName("GRS80")
	, m_dSemiMajor(GDA20a)
	, m_dInverseFlattening(GDA20Inv_f)
{
	UpdateParams();
}
	

CEllipsoid::~CEllipsoid(void)
{

}
	

CEllipsoid::CEllipsoid(const CEllipsoid& e)
{
	m_strEllipsoidName = e.m_strEllipsoidName;
	m_dSemiMajor = e.m_dSemiMajor;
	m_dSemiMinor = e.m_dSemiMinor;
	m_dInverseFlattening = e.m_dInverseFlattening;
	m_dEccen1 = e.m_dEccen1;
	m_dEccen2 = e.m_dEccen2;
}
	

CEllipsoid::CEllipsoid(double dSemiMajor, double dInverseFlattening)
{
	UpdateParams(dSemiMajor, dInverseFlattening);
}
	

CEllipsoid& CEllipsoid::operator= (const CEllipsoid& rhs)
{
	// check for assignment to self!
	if (this == &rhs)
		return *this;

	m_strEllipsoidName = rhs.m_strEllipsoidName;
	m_dSemiMajor = rhs.m_dSemiMajor;
	m_dSemiMinor = rhs.m_dSemiMinor;
	m_dInverseFlattening = rhs.m_dInverseFlattening;
	m_dEccen1 = rhs.m_dEccen1;
	m_dEccen2 = rhs.m_dEccen2;

	return *this;
}
	

bool CEllipsoid::operator== (const CEllipsoid& rhs) const
{
	return (
		m_strEllipsoidName == rhs.m_strEllipsoidName &&
		m_dSemiMajor == rhs.m_dSemiMajor &&
		m_dSemiMinor == rhs.m_dSemiMinor &&
		m_dInverseFlattening == rhs.m_dInverseFlattening &&
		m_dEccen1 == rhs.m_dEccen1 &&
		m_dEccen2 == rhs.m_dEccen2
		);
}
	

void CEllipsoid::UpdateParams(const double& dSemiMajor, const double& dInverseFlattening)
{
	m_dSemiMajor = dSemiMajor;
	m_dInverseFlattening = dInverseFlattening;

	UpdateParams();
}

void CEllipsoid::UpdateParams()
{
	m_dSemiMinor = m_dSemiMajor * (1.0 - (1.0 / m_dInverseFlattening));
	double dAsqd = m_dSemiMajor * m_dSemiMajor;
	double dBsqd = m_dSemiMinor * m_dSemiMinor;
	m_dEccen1 = sqrt((dAsqd - dBsqd) / dAsqd);	// first ecc
	m_dEccen2 = sqrt((dAsqd - dBsqd) / dBsqd);	// second ecc
}
	

double CEllipsoid::PrimeVert(const double& dLatitude) const
{
	double dDel = sqrt(1.0 - m_dEccen1*m_dEccen1*(sin(dLatitude)*sin(dLatitude)));
	return (m_dSemiMajor / dDel);
}
	

double CEllipsoid::PrimeMerid(const double& dLatitude) const
{
	double dDel = sqrt(1.0 - m_dEccen1 * m_dEccen1 * (sin(dLatitude) * sin(dLatitude)));
	return (m_dSemiMajor * ( (1.0 - m_dEccen1 * m_dEccen1) / (dDel * dDel * dDel)) );
}
	
