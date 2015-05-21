
#include <e3_CommonMacros.h>
#include <e3_Exception.h>
#include "gui/Style.h"
#include "gui/DatabaseBrowser.h"



namespace e3 {

	//----------------------------------------------------------
	// class InstrumentModel
	//----------------------------------------------------------

	void DatabaseBrowser::ModelBase::setOwner( DatabaseBrowser* owner ) 
	{
		owner_ = owner;
	}
		

	void DatabaseBrowser::ModelBase::paintListBoxItem( int row, Graphics& g, int width, int height, bool rowIsSelected )
	{
		Style& style  = Style::getInstance();
		rowIsSelected = isRowSelected( row );

		Colour bkgndCol = style.findColour( rowIsSelected ? 
			Style::HighlightColourId : 
			Style::ListBackgroundColourId );

		Colour textCol = style.findColour( rowIsSelected ?
			Style::LightTextColourId :
			Style::DarkTextColourId );

		g.fillAll( bkgndCol );

		g.setColour( textCol );
		g.setFont( height * 0.7f );

		// draw text
		String text = getText( row );
		g.drawText( text, 5, 0, width - 10, height, Justification::centredLeft, true );
	}


	void DatabaseBrowser::ModelBase::listBoxItemClicked( int row, const MouseEvent & )
	{
		UNUSED( row );
		//XmlElement* xml = data_->getChildElement( row );
		//if( xml != nullptr )
		//{
		//	int state = !xml->getIntAttribute( "sel" );
		//	xml->setAttribute( "sel", state );

		//	owner_->update();
		//}
	}


	int DatabaseBrowser::ModelBase::getIntAttribute( int row, const String& attributeName )
	{
		UNUSED( row );
		UNUSED( attributeName );
		//const XmlElement* xml = data_->getChildElement( row );
		//if( xml != nullptr ) {
		//	return xml->getIntAttribute( StringRef( attributeName ) );
		//}
		return 0;
	}


	bool DatabaseBrowser::ModelBase::isRowSelected( int row )
	{
		int state = getIntAttribute( row, "sel" );
		return state == 1;
	}



	//--------------------------------------------------------------------
	// class DatabaseBrowser::InstrumentModel
	//--------------------------------------------------------------------

	void DatabaseBrowser::InstrumentModel::update()
	{
		const Database::InstrumentSet& set = Database::getInstance().getInstruments();
		for( Database::InstrumentSet::const_iterator it = set.begin(); it != set.end(); ++it )
		{
			list_.push_back( &*it );
		}
	}

	int DatabaseBrowser::InstrumentModel::getNumRows()
	{
		return list_.size();
	}


	String DatabaseBrowser::InstrumentModel::getText( int row )
	{
		Database::Instrument const* i = list_.at( row );
		return i->name;
	}



	//--------------------------------------------------------------------
	// class DatabaseBrowser::PresetModel
	//--------------------------------------------------------------------

	void DatabaseBrowser::PresetModel::update()
	{
		const Database::PresetSet& set = Database::getInstance().getPresets();
		for( Database::PresetSet::const_iterator it = set.begin(); it != set.end(); ++it )
		{
			list_.push_back( &*it );
		}
	}

	int DatabaseBrowser::PresetModel::getNumRows()
	{
		return list_.size();
	}


	String DatabaseBrowser::PresetModel::getText( int row )
	{
		Database::Preset const* i = list_.at( row );
		return i->name;
	}



	//----------------------------------------------------------
	// class DatabaseBrowser
	//----------------------------------------------------------

	DatabaseBrowser::DatabaseBrowser()
	{
		instrumentModel_.setOwner( this );
		instrumentBox_.setModel( &instrumentModel_ );

		presetModel_.setOwner( this );
		presetBox_.setModel( &presetModel_ );

		addAndMakeVisible( instrumentBox_ );
		addAndMakeVisible( filter1Box_ );
		addAndMakeVisible( filter2Box_ );
		addAndMakeVisible( filter3Box_ );
		addAndMakeVisible( presetBox_ );
	}


	void DatabaseBrowser::resized()
	{
		Rectangle<int> bounds = getLocalBounds();
		int w                 = bounds.getWidth() / 5;
		Rectangle<int> b      = bounds.withWidth( w );

		instrumentBox_.setBounds( b.withWidth( w - 2) );
		filter1Box_.setBounds( b.withX( w * 1 ).withWidth( w - 2 ) );
		filter2Box_.setBounds( b.withX( w * 2 ).withWidth( w - 2 ) );
		filter3Box_.setBounds( b.withX( w * 3 ).withWidth( w - 2 ) );
		presetBox_.setBounds(  b.withX( w * 4 ).withWidth( w - 2 ) );
	}


	void DatabaseBrowser::paint( Graphics& g )
	{
		Colour c = findColour( Style::ContentBackground2ColourId );
		g.fillAll( c );
	}


	void DatabaseBrowser::update()
	{
		instrumentModel_.update();
		instrumentBox_.updateContent();

		presetModel_.update();
		presetBox_.updateContent();
	}

} // namespace e3