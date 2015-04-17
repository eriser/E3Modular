
#include <sstream>
#include <iomanip>

#include <e3_Trace.h>
#include <e3_CommonMacros.h>
#include "core/GlobalHeader.h"
#include "gui/Monitor.h"

namespace e3 {

    Monitor::Monitor() : Component()
    {
        labels_.insert(std::make_pair(kType, new Label()));
        labels_.insert(std::make_pair(kValue1, new Label()));
        labels_.insert(std::make_pair(kValue2, new Label()));
        labels_.insert(std::make_pair(kVoices, new Label()));
        labels_.insert(std::make_pair(kCpuMeter, new Label()));

        for (LabelMap::iterator it = labels_.begin(); it != labels_.end(); it++)
        { 
            Label* label = it->second;
            label->setFont(Font("Lucida Console", 10, Font::plain));
            label->setJustificationType(Justification::centred);
            label->setMinimumHorizontalScale(0.5f);
            label->setColour(Label::backgroundColourId, Colour(0xff3e4042));
            label->setColour(Label::textColourId, Colour(0xffbdbcba));

            addAndMakeVisible(*label);
        }
        // TODO: set background color via Component::properties
    }


    void Monitor::resized()
    {
        labels_[kType]->setBounds(Rectangle<int>(0, 0, 45, 25));
        labels_[kValue1]->setBounds(Rectangle<int>(48, 0, 27, 25));
        labels_[kValue2]->setBounds(Rectangle<int>(78, 0, 27, 25));
        labels_[kVoices]->setBounds(Rectangle<int>(118, 0, 27, 25));
        labels_[kCpuMeter]->setBounds(Rectangle<int>(148, 0, 45, 25));
    }


    void Monitor::monitor(MonitorEvent e)
    {
        std::ostringstream stream;

        switch (e.type)
        {
        case kMonitorVoices:
        {
            stream << e.numVoices;
            labels_[kVoices]->setText(stream.str(), dontSendNotification);
            break;
        }
        case kMonitorCpuMeter:
        {
            stream << std::setprecision(2) << std::fixed << e.value1 << "%";
            labels_[kCpuMeter]->setText(stream.str(), dontSendNotification);
            break;
        }
        case kMonitorNote:
        {
            labels_[kType]->setText("Note", dontSendNotification);

            double intpart;
            if (modf(e.value1, &intpart) == 0)
                stream << (int)e.value1;
            else
                stream << std::setprecision(2) << std::fixed << e.value1;
            labels_[kValue1]->setText(stream.str(), dontSendNotification);

            stream.str("");
            //stream << std::setprecision(2) << std::fixed << evt.value2;
            stream << (int)(e.value2 * 128);
            labels_[kValue2]->setText(stream.str(), dontSendNotification);
            break;
        }
        case kMonitorController:
        {
            labels_[kType]->setText("CC", dontSendNotification);

            stream << (int)e.value1;
            labels_[kValue1]->setText(stream.str(), dontSendNotification);

            stream.str("");
            stream << e.value2;
            labels_[kValue2]->setText(stream.str(), dontSendNotification);
            break;
        }
        case kMonitorPitchbend:
        {
            labels_[kType]->setText("PB", dontSendNotification);

            stream << e.value1 - 8192;
            labels_[kValue1]->setText(stream.str(), dontSendNotification);
            labels_[kValue2]->setText("", dontSendNotification);
            break;
        }
        }
    }


} // namespace e3