
#include "app/AppWindow.h"


namespace e3 {

    AppWindow::AppWindow(const String& title) : DocumentWindow(
        title, 
        Colours::black,
        DocumentWindow::minimiseButton | DocumentWindow::closeButton,
        true)
    {
        removeColour(ResizableWindow::backgroundColourId);
        //setTitleBarButtonsRequired(DocumentWindow::minimiseButton | DocumentWindow::closeButton, false);
        setTitleBarTextCentred(false);

        Rectangle<int> r = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
        setResizeLimits(640, 480, r.getWidth(), r.getHeight());
    }


    void AppWindow::closeButtonPressed()
    {
        JUCEApplicationBase::quit();
    }

} // namespace e3