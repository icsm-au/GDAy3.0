#pragma once

class CEllipsoid
{
public:
	CEllipsoid(void);
	CEllipsoid(const CEllipsoid&);
	virtual ~CEllipsoid(void);

	CEllipsoid(double dSemiMajor, double dInverseFlattening);

	CEllipsoid& operator= (const CEllipsoid& rhs);
	bool operator== (const CEllipsoid& rhs) const;
	inline CEllipsoid& operator[](int iIndex) { return this[iIndex];};	

	inline const CString& GetName() const { return m_strEllipsoidName;}
	inline const double& GetE1() const { return m_dEccen1;}
	inline const double& GetE2() const { return m_dEccen2;}
	inline double GetE1sqd() const { return m_dEccen1*m_dEccen1;}
	inline double GetE2sqd() const { return m_dEccen2*m_dEccen2;}
	inline const double& GetSMajor() const { return m_dSemiMajor;}
	inline const double& GetSMinor() const { return m_dSemiMinor;}
	inline const double& GetInverseFlattening() const { return m_dInverseFlattening;}

	inline void SetEllipsoidString(const CString& str) { m_strEllipsoidName = str; };
	void	UpdateParams(const double&, const double&);
	void	UpdateParams();
	double	PrimeVert(const double&) const;
	double	PrimeMerid(const double&) const;

// member variables
private:
	CString	m_strEllipsoidName;
	double	m_dSemiMajor;
	double	m_dSemiMinor;
	double	m_dInverseFlattening;	// 1 on F
	double	m_dEccen1;		// first eccentricity (not sqd)
	double	m_dEccen2;		// second eccentricity (not sqd)
};
