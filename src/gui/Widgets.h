
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

} // namespace e3