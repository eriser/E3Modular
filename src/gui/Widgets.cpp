
#include <e3_Exception.h>
#include <e3_CommonMacros.h>

#include "gui/Widgets.h"


namespace e3 {

    //-------------------------------------------------------------
    // class CustomCombox
    //-------------------------------------------------------------

    CustomComboBox::CustomComboBox()
    {
        setInterceptsMouseClicks( true, false );

        addAndMakeVisible( comboBox_ );
        comboBox_.addListener( this );
        comboBox_.setWantsKeyboardFocus( false );
        comboBox_.setEditableText( false );
        comboBox_.setInterceptsMouseClicks( false, false );
        comboBox_.setBroughtToFrontOnMouseClick( false );
    }


    CustomComboBox::~CustomComboBox()
    {
        comboBox_.removeListener( this );
    }


    void CustomComboBox::comboBoxChanged( ComboBox* comboBox )
    {
        ASSERT( comboBox == &comboBox_ );
        UNUSED( comboBox );
        
        int selectedId = comboBox_.getSelectedId();
        Component::BailOutChecker checker( this );

        if (selectedId == 0) // nasty way to recognize that text has changed
        {
            String text = comboBox_.getText();
            comboBox_.setEditableText( false );
            comboBox_.changeItemText( selectedId_ + 1, text );
            listeners_.callChecked( checker, &Listener::comboBoxTextChanged, this );
        }
        else {              // another item was selected
            selectedId_ = selectedId - 1;
            listeners_.callChecked( checker, &Listener::comboBoxChanged, this ); 
        }
    }


    void CustomComboBox::resized()
    {
        comboBox_.setBounds( getLocalBounds() );
    }


    void CustomComboBox::mouseDown( const MouseEvent& e )
    {
        comboBox_.setEditableText( false );
        e.mods.isCtrlDown() ? showEditor() : comboBox_.mouseDown( e );
    }


    void CustomComboBox::clear()
    {
        comboBox_.clear( dontSendNotification );
    }

    
    void CustomComboBox::addItem( const std::string& text, int itemId )
    {
        comboBox_.addItem( text, itemId + 1 );
    }


    const std::string CustomComboBox::getText() const
    {
        return comboBox_.getText().toStdString();
    }


    void CustomComboBox::setSelectedId( int itemId )
    {
        selectedId_ = itemId;
        comboBox_.setSelectedId( itemId + 1, dontSendNotification );
    }


    int CustomComboBox::getSelectedId() const
    {
        return selectedId_;
    }


    void CustomComboBox::showEditor()
    {
        comboBox_.setEditableText( true );
        comboBox_.showEditor();
    }

} // namespace e3