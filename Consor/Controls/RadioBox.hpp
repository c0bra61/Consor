#ifndef CONTROLS_RADIOBOX_H
#define CONTROLS_RADIOBOX_H

#include "Control.hpp"
#include "CheckBox.hpp"
#include "Containers/FlowContainer.hpp"

#include <string>
#include <tuple>

namespace Consor
{
	class CRadioBox : public CControl
	{
	protected:
		std::list<std::tuple<CCheckBox*, std::string>> m_Checkboxes;
		CFlowContainer m_FlowContainer;
	public:
		CRadioBox();
		~CRadioBox();
		
		// Will select
		void AddChoice(const std::string& Choice);
		std::string GetChoice();

		virtual void Draw(Consor::Console::IConsoleRenderer& Renderer, bool HasFocus, const Consor::ISkin& Skin);
		virtual CSize Size();
		virtual void ForceResize(const CSize& Size);
		virtual void OnResize(const CSize& Size);
		virtual bool HandleInput(Input::Key Key, Input::IInputSystem& System);
		virtual bool CanFocus();
	};
}

#endif