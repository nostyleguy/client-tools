//======================================================================
//
// ShipComponentStyleManager.cpp
// copyright (c) 2004 Sony Online Entertainment
//
//======================================================================

#include "sharedGame/FirstSharedGame.h"
#include "sharedGame/ShipComponentStyleManager.h"

#include "sharedDebug/InstallTimer.h"
#include "sharedFoundation/TemporaryCrcString.h"
#include "sharedGame/ShipComponentDescriptor.h"
#include "sharedGame/ShipComponentType.h"
#include "sharedUtility/DataTable.h"
#include "sharedUtility/DataTableManager.h"
#include "sharedFoundation/ExitChain.h"
#include "sharedLog/Log.h"
#include <map>
#include <sstream>

//======================================================================

namespace ShipComponentStyleManagerNamespace
{
    enum DatatableColumns
    {
	DC_componentName,
	DC_engine,
	DC_booster,
	DC_weapon0,
	DC_weapon1,
	DC_weapon2,
	DC_modification
    };

    class Data
    {
    public:

	int engine;
	int booster;
	int weapon0;
	int weapon1;
	int weapon2;
	int modification;
	    
	Data() : engine(1),
		 booster(1),
		 weapon0(1),
		 weapon1(1),
		 weapon2(1),
		 modification(1) {
	}
    };

    typedef std::map<uint32, Data> CrcIntMap;
    CrcIntMap s_dataMap;
    
    bool s_installed = false;
}

static int getStyleNumberFromStyleString( const std::string & style_str){
	if (style_str == "")
	{
		return 1;
	}
	std::size_t it = style_str.find("_s");
	if (it != std::string::npos)
	{
		// Convert the next 2 characters after "_s", which are always a 2-digit integer string ("01") into an integer

	    /* Doens't compile on client with stlport, but works on the server. 
		//std::istringstream s(style_str.substr(it + 2, 2));
		//int x = 0;
		//s >> x;
		//return x;
		*/
		return atoi(style_str.substr(it + 2, 2).c_str());
	}
	return 1;
}

using namespace ShipComponentStyleManagerNamespace;

//----------------------------------------------------------------------

void ShipComponentStyleManager::install()
{
	InstallTimer const installTimer("ShipComponentStyleManager::install");

	DEBUG_FATAL(s_installed, ("ShipComponentStyleManager already installed"));
	s_installed = true;

	// Need to use some ship for default styles for all components. Most vanilla JTL ships
	// use the same style for a given component. Just go with player_xwing since we just
	// need to pick one. 
	std::string const & filename = "datatables/space/ship_chassis_player_xwing.iff";
	
	DataTable * const dt = DataTableManager::getTable(filename, true);

	if (dt == nullptr)
	{
		WARNING(true, ("ShipComponentStyleManager no such datatable [%s]", filename.c_str()));
		return;
	}
	
	const int numRows = dt->getNumRows();
	
	for (int row = 0; row < numRows; ++row)
	{
	    std::string const & componentName = dt->getStringValue(static_cast<int>(DC_componentName), row);
	    ShipComponentDescriptor const * const shipComponentDescriptor = 
		ShipComponentDescriptor::findShipComponentDescriptorByName(TemporaryCrcString(componentName.c_str(), true));
		
	    if (shipComponentDescriptor == nullptr)
	    {
		    WARNING(true, ("getComponentType datatable specified invalid component [%s]", componentName.c_str()));
		    continue;
	    }
		
	    if (shipComponentDescriptor->getComponentType() != ShipComponentType::SCT_weapon)
	    {
		WARNING(true, ("ShipComponentStyleManager specified non-weapon component [%s]", componentName.c_str()));
		continue;
	    }
		
	    Data data;
	    data.engine = getStyleNumberFromStyleString(dt->getStringValue(static_cast<int>(DC_engine), row));
	    data.booster = getStyleNumberFromStyleString(dt->getStringValue(static_cast<int>(DC_booster), row));
	    data.weapon0 = getStyleNumberFromStyleString(dt->getStringValue(static_cast<int>(DC_weapon0), row));
	    data.weapon1 = getStyleNumberFromStyleString(dt->getStringValue(static_cast<int>(DC_weapon1), row));
	    data.weapon2 = getStyleNumberFromStyleString(dt->getStringValue(static_cast<int>(DC_weapon2), row));
	    data.modification = getStyleNumberFromStyleString(dt->getStringValue(static_cast<int>(DC_modification), row));


	    IGNORE_RETURN(s_dataMap.insert(std::make_pair(shipComponentDescriptor->getCrc(), data)));
	}

	ExitChain::add(ShipComponentStyleManager::remove, "ShipComponentStyleManager::remove");
}

//----------------------------------------------------------------------

void ShipComponentStyleManager::remove()
{
	s_installed = false;
	s_dataMap.clear ();
}

//----------------------------------------------------------------------

int ShipComponentStyleManager::getDefaultStyleForComponent(uint32 componentCrc)
{
    ShipComponentDescriptor const * const descriptor = 
	ShipComponentDescriptor::findShipComponentDescriptorByCrc(componentCrc);

    if (descriptor == nullptr)
    {
		WARNING(true, ("ShipComponentStyleManager::getDefaultStyleForComponent specified invalid component [%i]", componentCrc));
		return -1;
    }
    
    CrcIntMap::const_iterator const it = s_dataMap.find(componentCrc);
    if (it != s_dataMap.end())
    {
		switch (descriptor->getComponentType())
		{
		case ShipComponentType::SCT_engine:
			return it->second.engine;
		case ShipComponentType::SCT_booster:
			return it->second.booster;
		case ShipComponentType::SCT_weapon:
			return it->second.weapon0;
		default:
			WARNING(true, ("ShipComponentStyleManager::getDefaultStyleForComponent asked for style of non-styled component [%i]", componentCrc));
			return 1;
		}
	}

    // No datatable entry for this component template means we have no idea
    // what the style should be. Hopefully 99 raises some eyebrows. 
    return 99; 
}

