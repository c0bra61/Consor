#include "ScrollContainer.hpp"
#include "../Util/Debug.hpp"

using namespace Consor;
using namespace std;

ScrollContainer::ScrollContainer(Control& Client, const Size& Size, bool ShrinkToContents) :
	_Client(Client),
	_HScrollbar(),
	_VScrollbar(),
	_ShrinkToContents(ShrinkToContents),
	_MaxSize(Size)
{
	ForceResize(Size);
}

Size ScrollContainer::GetSize()
{
	
	Size clientsize = _Client.GetSize();
	
	// Do we need to resize ourselves?
	if(_ShrinkToContents && clientsize != _Size)
	{
		Size optimal = clientsize;
		
		if(optimal.Width > _MaxSize.Width)
			optimal.Width = _MaxSize.Width;
		if(optimal.Height > _MaxSize.Height)
			optimal.Height = _MaxSize.Height;
		
		if(optimal != _Size)
		{		
			Util::Log("resizing ScrollContainer to %", optimal);
			this->ForceResize(optimal);
		}
	}
	
	Size ret = _Size;

	if(_Size.Width < 0)
		ret.Width = clientsize.Width + _VScrollbar.GetSize().Width;
	if(_Size.Height < 0)
		ret.Height = clientsize.Height + _HScrollbar.GetSize().Height;

	return ret;
}

void ScrollContainer::OnResize(const Size& size)
{
	Size hsize, vsize;
	if(_Size.Width > 0)
		hsize = Size(size.Width - 1, 1); // The -1 because VScrollbar has priority
	else // set its size to nothing
		hsize = Size(0, 0);
	
	if(_Size.Height > 0)
		vsize = Size(1, size.Height - hsize.Height);
	else
		vsize = Size(0, 0);

	_HScrollbar.ForceResize(hsize);
	_VScrollbar.ForceResize(vsize);
}

void ScrollContainer::ForceResize(const Size& size)
{
	_Size = size;
	OnResize(this->GetSize());
}

void ScrollContainer::Draw(Consor::Console::IConsoleRenderer& Renderer, bool HasFocus, const Consor::ISkin& Skin)
{
	Size selfsize = GetSize();
	Size childsize = _Client.GetSize();

	Vector offset;

	Size vscrollsize = _VScrollbar.GetSize();
	Size hscrollsize = _HScrollbar.GetSize();
	Size clientsize = _Client.GetSize();

	if(_Size.Width > 0 && childsize.Width > _Size.Width) // if the width isn't automatic
	{
		offset.X = (int)-(_HScrollbar.GetPercent() * (childsize.Width - (selfsize.Width - vscrollsize.Width)));

		double incremant = 1.0 / (clientsize.Width - selfsize.Width);
		_HScrollbar.SetChangeSize(incremant);
		_HScrollbar.SetScrollRegionSize(clientsize.Width);

		Renderer.PushRenderBounds(Vector(0, selfsize.Height - hscrollsize.Height), hscrollsize);
			_HScrollbar.Draw(Renderer, HasFocus, Skin);
		Renderer.PopRenderBounds();
	}
	else hscrollsize.Height = 0;

	if(_Size.Height > 0 && childsize.Height > _Size.Height) // if the hieht isn't automatic
	{
		offset.Y = (int)-(_VScrollbar.GetPercent() * (childsize.Height - (selfsize.Height - hscrollsize.Height)));

		double incremant = 1.0 / (clientsize.Height - selfsize.Height);
		_VScrollbar.SetChangeSize(incremant);
		_VScrollbar.SetScrollRegionSize(clientsize.Height);

		Renderer.PushRenderBounds(Vector(selfsize.Width - vscrollsize.Width, 0), vscrollsize);
			_VScrollbar.Draw(Renderer, HasFocus, Skin);
		Renderer.PopRenderBounds();
	}
	else vscrollsize.Width = 0;

	Renderer.PushRenderBounds(offset, selfsize - Size(vscrollsize.Width, hscrollsize.Height));
		_Client.Draw(Renderer, HasFocus && _Client.CanFocus(), Skin);
	Renderer.PopRenderBounds();
}

bool ScrollContainer::ScrollDown(size_t count)
{
	Size clientsize = _Client.GetSize();
	Size selfsize = this->GetSize();

	if(clientsize.Height < selfsize.Height)
		return false;

	double perc = _VScrollbar.GetPercent();
	double incremant = 1.0 / (clientsize.Height - selfsize.Height);

	perc += incremant * (double)count;

	if(perc > 1.0)
		perc = 1.0;

	if(perc == _VScrollbar.GetPercent()) // allow shallower scrollcontainers to also scroll
		return false;

	_VScrollbar.SetPercent(perc);
	return true;
}

bool ScrollContainer::ScrollUp(size_t count)
{
	Size clientsize = _Client.GetSize();
	Size selfsize = this->GetSize();

	if(clientsize.Height < selfsize.Height)
		return false;

	double perc = _VScrollbar.GetPercent();
	double incremant = 1.0 / (clientsize.Height - selfsize.Height);

	perc -= incremant * (double)count;

	if(perc < 0)
		perc = 0;

	if(perc == _VScrollbar.GetPercent()) // allow shallower scrollcontainers to also scroll
		return false;

	_VScrollbar.SetPercent(perc);
	return true;
}

bool ScrollContainer::ScrollLeft(size_t count)
{
	Size clientsize = _Client.GetSize();
	Size selfsize = this->GetSize();

	if(clientsize.Width < selfsize.Width)
		return false;

	double perc = _HScrollbar.GetPercent();
	double incremant = 1.0 / (clientsize.Width - selfsize.Width);

	perc -= incremant * (double)count;

	if(perc < 0)
		perc = 0;

	if(perc == _HScrollbar.GetPercent()) // allow shallower scrollcontainers to also scroll
		return false;

	_HScrollbar.SetPercent(perc);
	return true;
}

bool ScrollContainer::ScrollRight(size_t count)
{
	Size clientsize = _Client.GetSize();
	Size selfsize = this->GetSize();

	if(clientsize.Width < selfsize.Width)
		return false;
	double perc = _HScrollbar.GetPercent();
	double incremant = 1.0 / (clientsize.Width - selfsize.Width);

	perc += incremant * (double)count;

	if(perc > 1.0)
		perc = 1.0;

	if(perc == _HScrollbar.GetPercent()) // allow shallower scrollcontainers to also scroll
		return false;

	_HScrollbar.SetPercent(perc);
	return true;
}

bool ScrollContainer::HandleInput(Input::Key Key, Input::IInputSystem& System)
{
	if(System.ControlDown())
	{
		bool handeled = false;

		switch(Key)
		{
		case Input::Key::Left:
			handeled = ScrollLeft();
			break;
		case Input::Key::Right:
			handeled = ScrollRight();
			break;
		case Input::Key::Up:
			handeled = ScrollUp();
			break;
		case Input::Key::Down:
			handeled = ScrollDown();
			break;
		default:
			handeled = false;
		}

		if(handeled)
			return true;
	}

	if(_Client.HandleInput(Key, System))
		return true;

	if(_Size.Height > 0)
	{
		const double context = 1.0 - 0.1; //10% context

		if(Key == Input::Key::PageDown || Key == Input::Key::Numpad3)
			return ScrollDown((double)(_Size.Height * context));
		else if(Key == Input::Key::PageUp || Key == Input::Key::Numpad9)
			return ScrollUp((double)(_Size.Height * context));
	}

	return false;
}

bool ScrollContainer::CanFocus()
{
	return true;
}
