
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
    // class ParameterModel
    // Stores the data of a parameter
    //--------------------------------------------------------------------

    class ParameterModel
    {
    public:
        ParameterModel() {}

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
    // class ParameterModelMap
    //-------------------------------------------

    class ParameterModelMap : public std::map< uint16_t, ParameterModel >
    {
    public:
        void add(const ParameterModel& model);

        // Returns true, if a ParameterModel with the given id exists in the map.
        bool has(const uint16_t id) const;

        // Updates the ParameterModel in the map with the data of the given ParameterModel. 
        // ParameterModel is identified by model.id_;
        // If the id does not exist in the map, a std::out_of_range is thrown.
        void update(const ParameterModel& model);
        void setDefaultValues();
    };


    //-------------------------------------------
    // class LinkModel
    // Stores the data of a link. A LinkModel describes a connection between two 
    // module ports. 
    //-------------------------------------------

    class LinkModel : public ParameterModel
    {
    public:
        bool operator==(const LinkModel& other) const;
        bool operator==(const LinkModel* other) const   { return operator==(*other); }
        bool operator!=(const LinkModel& other) const   { return !(*this == other); }
        bool operator!=(const LinkModel* other) const   { return !(this == other); }
        bool operator<(const LinkModel& other) const;
        bool operator>(const LinkModel& other) const    { return !(*this == other && *this < other); }

        uint16_t leftModule_  = 0;  // module that sends data into the link
        uint16_t rightModule_ = 0;  // module to where the link sends the data. This module is the owner of the link.
        uint16_t leftPort_    = 0;  // port within the left module
        uint16_t rightPort_   = 0;  // port within the right module
    };

    //----------------------------------------------------------------
    // class LinkModelList
    // A container for LinkModels
    //----------------------------------------------------------------

    class LinkModelList : public std::vector< LinkModel >
    {
    public:
        void add(LinkModel& link);
        LinkModel& get(const LinkModel& link);
        void update(const LinkModel& link);
        void remove(const LinkModel& link);
        int16_t getIndex(const LinkModel& link);
        bool contains(const LinkModel& link);
        bool hasLeftModule(uint16_t id);

    private:
        iterator find(const LinkModel& link);
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
        ParameterModelMap parameters_;
        LinkModelList links_;
        LinkModelList removedLinks_;

        void addLink(LinkModel& link);
        void removeLink(const LinkModel& link);
        LinkModel& getLink(uint16_t index);
    };
} // namespace e3



