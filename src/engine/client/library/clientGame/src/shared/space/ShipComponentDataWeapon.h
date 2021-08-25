//======================================================================
//
// ShipComponentDataWeapon.h
// copyright (c) 2004 Sony Online Entertainment
//
//======================================================================

#ifndef INCLUDED_ShipComponentDataWeapon_H
#define INCLUDED_ShipComponentDataWeapon_H

//======================================================================

#include "clientGame/ClientShipComponentData.h"

//----------------------------------------------------------------------

class ShipComponentDataWeapon : public ClientShipComponentData
{
public:

	explicit ShipComponentDataWeapon (ShipComponentDescriptor const & shipComponentDescriptor);
	virtual ~ShipComponentDataWeapon ();

	virtual bool readDataFromShip      (int chassisSlot, ShipObject const & ship);

	virtual void printDebugString      (Unicode::String & result, Unicode::String const & padding) const;

	void getAttributes(stdvector<std::pair<std::string, Unicode::String> >::fwd & data) const;

private:
	ShipComponentDataWeapon (const ShipComponentDataWeapon & rhs);
	ShipComponentDataWeapon & operator= (const ShipComponentDataWeapon & rhs);

public:

	//-- these members are public because this instances of this class are short-lived temporary objects,
	//-- to simplify the conversion from the ShipObject & shipt representation of components.

	float     m_weaponDamageMaximum;
	float     m_weaponDamageMinimum;
	float     m_weaponEffectivenessShields;
	float     m_weaponEffectivenessArmor;
	float     m_weaponEnergyPerShot;
	float     m_weaponRefireRate;
	int       m_weaponAmmoCurrent;
	int       m_weaponAmmoMaximum;
	uint32    m_weaponAmmoType;
	int		  m_weaponProjectileIndex;
	int       m_style;
};

//======================================================================

#endif
