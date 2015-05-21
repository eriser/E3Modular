
#include <string>
#include <limits>
#include "JuceHeader.h"


namespace e3 {

    class NumericLabel : public Label
    {
    public:
        void editorShown( TextEditor* editor ) override
        {
            editor->setInputRestrictions( 10, "+-0123456789" );
        }

        void textWasChanged() override
        {
            var value = getText();
            value = std::min<int>( maxValue_, value );
            value = std::max<int>( minValue_, value );
            setText( value, dontSendNotification );
        }

        void setRange( int minValue, int maxValue )                
        { 
            minValue_ = minValue;
            maxValue_ = maxValue;
        }

    protected:
        int minValue_ = std::numeric_limits<int>::min();
        int maxValue_ = std::numeric_limits<int>::max();
    };


    class CustomComboBox : public Component, public ComboBox::Listener
    {
    public:

        CustomComboBox();
        ~CustomComboBox();

        void comboBoxChanged( ComboBox* comboBox ) override;
        void resized() override;

        void clear();
        void addItem( const std::string& text, int itemId );
        const std::string getText() const;
        void showEditor();

        void setSelectedId( int itemId );
        int getSelectedId() const;

        void mouseDown( const MouseEvent& event ) override;

        class Listener
        {
        public:
            virtual ~Listener() {}
            virtual void comboBoxChanged( CustomComboBox* comboBox ) = 0;
            virtual void comboBoxTextChanged( CustomComboBox* comboBox ) = 0;
        };

        void addListener( Listener* listener )       { listeners_.add( listener ); }
        void removeListener( Listener* listener )    { listeners_.remove( listener ); }

    protected:
        ComboBox comboBox_;
        ListenerList <Listener> listeners_;
        int selectedId_ = -1;
    };

} // namespace e3