
#pragma once

#include "JuceHeader.h"
#include "core/Database.h"


namespace e3 {


	class DatabaseBrowser : public Component
	{
		class ModelBase : public ListBoxModel
		{
		public:
			void setOwner( DatabaseBrowser* owner );
			virtual void update() = 0;

			void paintListBoxItem( int row, Graphics& g, int width, int height, bool rowIsSelected ) override;
			void listBoxItemClicked( int row, const MouseEvent & ) override;

		protected:
			virtual String getText( int row ) = 0;
			int getIntAttribute( int row, const String& attributeName );

			bool isRowSelected( int row );

			DatabaseBrowser* owner_ = nullptr;
		};


		struct InstrumentModel : public ModelBase
		{
			int getNumRows() override;
			void update() override;
			String getText( int row ) override;

			typedef std::vector<Database::Instrument const*> List;
			List list_;
		};

		
		struct PresetModel : public ModelBase
		{
			int getNumRows() override;
			void update() override;
			String getText( int row ) override;

			typedef std::vector<Database::Preset const*> List;
			List list_;
		};



	public:
		DatabaseBrowser();

		void paint( Graphics& g ) override;
		void resized() override;
		void update();

	protected:
		ListBox instrumentBox_;
		ListBox filter1Box_;
		ListBox filter2Box_;
		ListBox filter3Box_;
		ListBox presetBox_;

		InstrumentModel instrumentModel_;
		PresetModel presetModel_;
	};

} // namespace e3