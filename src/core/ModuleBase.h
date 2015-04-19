
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>

#include "e3_Math.h"
#include "core/GlobalHeader.h"
#include "core/ParameterShaper.h"


namespace e3 {

    
    //--------------------------------------------------
    // class PortModel
    // Stores the data of a port
    //--------------------------------------------------

    class PortModel
    {
    public:
        PortModel() {}

        uint16_t    id_ = 0;
        ControlType controlType_ = kControlHidden;
        std::string label_;
    };
    typedef std::vector< PortModel > PortModelList;


    //--------------------------------------------------------------------
    // class Parameter
    // Stores the data of a parameter
    //--------------------------------------------------------------------

    class Parameter
    {
    public:
        Parameter() {}

        uint16_t id_               = 0;
        ControlType controlType_   = kControlHidden;
        NumberFormat numberFormat_ = kNumberFloat;
        double defaultValue_       = 0;
        double value_              = 0;
        double veloSens_           = 0;
        double keyTrack_           = 0;
        double resolution_         = 1;
        std::string label_;
        std::string unit_;

        ParameterShaper valueShaper_;
        MidiParameterShaper midiShaper_;
    };


    //-------------------------------------------
    // class ParameterMap
    //-------------------------------------------

    class ParameterMap : public std::map< uint16_t, Parameter >
    {
    public:
        void add(const Parameter& model);

        // Returns true, if a Parameter with the given id exists in the map.
        bool containsId(const uint16_t id) const;

        // Updates the Parameter in the map with the data of the given Parameter. 
        // Parameter is identified by model.id_;
        // If the id does not exist in the map, a std::out_of_range is thrown.
        void update(const Parameter& model);
        void setDefaultValues();
    };


    //-------------------------------------------
    // class Link
    // Represents a connection between ports.
    //-------------------------------------------

    class Link : public Parameter
    {
    public:
        bool operator==(const Link& other) const;
        bool operator==(const Link* other) const   { return operator==(*other); }
        bool operator!=(const Link& other) const   { return !(*this == other); }
        bool operator!=(const Link* other) const   { return !(this == other); }
        bool operator<(const  Link& other) const;
        bool operator>(const  Link& other) const   { return !(*this == other && *this < other); }

        uint16_t leftModule_  = 0;  // module that sends data into the link
        uint16_t rightModule_ = 0;  // module to where the link is sending. This module is the owner of the link.
        uint16_t leftPort_    = 0;  // port within the left module
        uint16_t rightPort_   = 0;  // port within the right module
    };


    //----------------------------------------------------------------
    // class LinkList
    // A container for Links
    //----------------------------------------------------------------

    class LinkList : public std::vector< Link >
    {
    public:
        void add(Link& link);
        Link& get(const Link& link);
        void update(const Link& link);
        void remove(const Link& link);
        int16_t getIndex(const Link& link);
        bool contains(const Link& link);
        bool hasLeftModule(uint16_t id);

    private:
        iterator find(const Link& link);
    };




    //--------------------------------------------------
    // class ModuleModel
    // Stores the data of a module
    //--------------------------------------------------

    class ModuleModel
    {
    public:

        int16_t id_                    = -1;
        ModuleType moduleType_         = kModuleUndefined;
        ProcessingType processingType_ = kProcessAudio;
        VoicingType voicingType_       = kPolyphonic;
        BoxStyle boxStyle_             = (BoxStyle)(kBoxVisible | kBoxLabel);

        std::string label_;
        PortModelList inPortModels_;
        PortModelList outPortModels_;
        ParameterMap parameters_;
        LinkList links_;
        LinkList removedLinks_;

        void addLink(Link& link);
        void removeLink(const Link& link);
        Link& getLink(uint16_t index);
    };
} // namespace e3



