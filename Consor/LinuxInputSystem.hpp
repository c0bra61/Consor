#ifndef CONSOR_LINUXINPUTSYSTEM_H
#define CONSOR_LINUXINPUTSYSTEM_H

#include "InputSystem.hpp"

namespace Consor
{
	namespace Input
	{
		class LinuxInputSystem : public IInputSystem
		{
		public:
			bool KeyWaiting();
			Key GetKeyPress();
			bool ControlDown();
			bool ShiftDown();
		};
	};
}
#endif
