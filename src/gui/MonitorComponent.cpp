
#include <sstream>
#include <iomanip>

#include <e3_Trace.h>
#include <e3_CommonMacros.h>
#include "core/GlobalHeader.h"
#include "core/Processor.h"
#include "gui/Style.h"
#include "gui/MonitorComponent.h"


namespace e3 {

    MonitorComponent::MonitorComponent() : Component()
    {
        labels_.insert(std::make_pair(MonitorType, new Label()));
        labels_.insert(std::make_pair(MonitorValue1, new Label()));
        labels_.insert(std::make_pair(MonitorValue2, new Label()));
        labels_.insert(std::make_pair(MonitorVoices, new Label()));
        labels_.insert(std::make_pair(MonitorCpuMeter, new Label()));

		Style& style = Style::getInstance();
		Colour bkgndCol = style.findColour( Style::MonitorBackground );
		Colour textCol = style.findColour( Style::MonitorText );

        for (LabelMap::iterator it = labels_.begin(); it != labels_.end(); it++)
        { 
            Label* label = it->second;
            label->setFont(Font("Lucida Console", 10, Font::plain));
            label->setJustificationType(Justification::centred);
            label->setMinimumHorizontalScale(0.5f);
            label->setColour(Label::backgroundColourId, bkgndCol);
			label->setColour( Label::textColourId, textCol );

            addAndMakeVisible(*label);
        }
        // TODO: set background color via Component::properties
    }


    void MonitorComponent::resized()
    {
        labels_[MonitorType]->setBounds(Rectangle<int>(0, 0, 45, 25));
        labels_[MonitorValue1]->setBounds(Rectangle<int>(48, 0, 27, 25));
        labels_[MonitorValue2]->setBounds(Rectangle<int>(78, 0, 27, 25));
        labels_[MonitorVoices]->setBounds(Rectangle<int>(118, 0, 27, 25));
        labels_[MonitorCpuMeter]->setBounds(Rectangle<int>(148, 0, 45, 25));
    }


    void MonitorComponent::monitor(MonitorEvent e)
    {
        std::ostringstream stream;

        switch (e.type)
        {
        case MonitorVoices:
        {
            stream << e.numVoices;
            labels_[MonitorVoices]->setText(stream.str(), dontSendNotification);
            break;
        }
        case MonitorCpuMeter:
        {
            stream << std::setprecision(2) << std::fixed << e.value1 << "%";
            labels_[MonitorCpuMeter]->setText(stream.str(), dontSendNotification);
            break;
        }
		case MonitorProcessorState:
		{
			labels_[MonitorCpuMeter]->setColour( Label::backgroundColourId, getProcessorStateColour( e.value1 ) );
			break;
		}
        case MonitorNote:
        {
            labels_[MonitorType]->setText("Note", dontSendNotification);

            double intpart;
            if (modf(e.value1, &intpart) == 0)
                stream << (int)e.value1;
            else
                stream << std::setprecision(2) << std::fixed << e.value1;
            labels_[MonitorValue1]->setText(stream.str(), dontSendNotification);

            stream.str("");
            //stream << std::setprecision(2) << std::fixed << evt.value2;
            stream << (int)(e.value2 * 128);
            labels_[MonitorValue2]->setText(stream.str(), dontSendNotification);
            break;
        }
        case MonitorController:
        {
            labels_[MonitorType]->setText("CC", dontSendNotification);

            stream << (int)e.value1;
            labels_[MonitorValue1]->setText(stream.str(), dontSendNotification);

            stream.str("");
            stream << e.value2;
            labels_[MonitorValue2]->setText(stream.str(), dontSendNotification);
            break;
        }
        case MonitorPitchbend:
        {
            labels_[MonitorType]->setText("PB", dontSendNotification);

            stream << e.value1 - 8192;
            labels_[MonitorValue1]->setText(stream.str(), dontSendNotification);
            labels_[MonitorValue2]->setText("", dontSendNotification);
            break;
        }
        }
    }


	Colour MonitorComponent::getProcessorStateColour( double state ) const
	{
		Style& style = Style::getInstance();

		switch( (int)state )
		{
		case ProcessorNotInitialized: return style.findColour( Style::MonitorNotInitialized);
		case ProcessorReady:          return style.findColour( Style::MonitorReady );
		case ProcessorSuspended:      return style.findColour( Style::MonitorSuspended );
		case ProcessorCrashed:        return style.findColour( Style::MonitorCrashed );
		}
		return Colours::transparentBlack;
	}


} // namespace e3