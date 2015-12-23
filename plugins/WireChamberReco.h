#ifndef __WIRE_CHMABER_RECO__
#define __WIRE_CHMABER_RECO__

#include "PluginBase.h"
#include "interface/PositionTree.h"

using namespace std;

class WireChamberReco: public PluginBase
{
public:
    //---ctors---
    WireChamberReco() {};

    //---dtor---
    ~WireChamberReco() {};

    //---utils---
    bool Begin(CfgManager& opts, uint64* index);
    bool ProcessEvent(const H4Tree& event, map<string, PluginBase*>& plugins, CfgManager& opts);
    
private:
    PositionTree     wireTree_;
    int              chXl_;
    int              chXr_;
    int              chYu_;
    int              chYd_;

    //---datamember for registration
    PLUGIN_REGISTER(WireChamberReco)
};

DEFINE_PLUGIN(WireChamberReco);


#endif
