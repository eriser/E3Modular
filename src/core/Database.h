
#pragma once

#include <set>
#include "JuceHeader.h"


namespace e3 {

    class Database 
	{
    public:
		static Database& getInstance();

        void build( bool force = false );

		XmlElement* getXml()	{ return databaseXml_; }


		struct Filter
		{
			String name;
			bool selected = false;

			bool operator==( const Filter& other ) const { return other.name == name; }
			bool operator<( const Filter& other ) const  { return name < other.name; }
		};

		struct Instrument 
		{
			int id;
			String name;
			File file;
			bool selected = false;

			bool operator==( const Instrument& other ) const { return other.id == id; }
			bool operator<( const Instrument& other ) const  { return id < other.id;	}
		};

		struct Preset 
		{
			int presetId, instrumentId;
			String name;
			Filter category1, category2, category3;

			bool operator==( const Preset& other ) const { 
				return other.presetId == presetId && other.instrumentId == instrumentId; 
			}
			bool operator<( const Preset& other ) const  { 
				if( other.instrumentId != instrumentId ) return instrumentId < other.instrumentId;
				return presetId < other.presetId;
			}
		};

		typedef std::set<Instrument> InstrumentSet;
		typedef std::set<Preset> PresetSet;
		typedef std::set<Filter> FilterSet;

		const InstrumentSet& getInstruments() const  { return instruments_; }
		const PresetSet& getPresets() const			 { return presets_; }
		const FilterSet& getCategory1() const	     { return category1_; }
		const FilterSet& getCategory2() const	     { return category2_; }
		const FilterSet& getCategory3() const	     { return category3_; }

	
	protected:
		XmlElement* load( File& file );

        static File createDefaultFilename();
        static File getDatabaseFilename();
		static void scanInstrument( XmlElement* root, const File& file, int count );

		void scanInstrument( const File& file, int count );


		InstrumentSet instruments_;
		PresetSet presets_;
		FilterSet category1_, category2_, category3_;

		ScopedPointer<XmlElement> databaseXml_ = nullptr;
    };
}  // namespace e3
