=======
Consor
======

A console GUI Library, released under the GPLv3 licence.  (c) Copyright 2013 Kate Adams.

The examples below may assume the namespaces `std` and `Consor` are being used.

### Building

Execute `GenerateProjects.sh` or `GenerateProjects.bat` depending upon your operating system.

Building:

On Linux:

```
cd ./Projects
make
```
On Windows, open the solution up located in `./Projects/`

The output goes to `./Binaries/` for the binaries, and `./Include/Consor/` for the headers.  To use the libary, you need to place the headers and binaries in the appropriate places (e.g. `/usr/include/Consor/` maps to `./Include/Consor/`, and `/usr/lib/libConsor.so` maps to `./Binaries/libConsor.so` on Linux).

### Example Usage

#### Positioning and Sizes

Unlike conventional UI libraries, Consor follows a more 'automated' way of positioning controls.
Controls (those that derive from `Consor::Control`) have no position information relating to that control; everything is positioned using containers.  Containers well define how and/or where a control will be drawn, and although it may not determine the controls size, it can influence it.  For example, a label's size can't be smaller than the message it holds, however, if you `ForceResize(const Size&)` the control, the label will attempt to obey the width, but not the height.  EG:

```c++
Label label;
label.SetText("Hello, world!  How are you on this fabulous day?");

cout << "label.GetSize() = " << label.GetSize() << "\n";
```

The result of the operation above should yield the output:

```
label.GetSize() = 48, 1
```

Now, let's say that the label's width should not exceed a width of 30 characters, we can `ForceResize(const Size&)` the label, and it will wrap its text at the most appropriate places:

```c++
Label label;
label.SetText("Hello, world!  How are you on this fabulous day?");
label.ForceResize(Size(30, 1));

cout << "label.GetSize() = " << label.GetSize() << "\n";
``` 

The program above should yield the output:

```
label.GetSize() = 28, 3
```

As we can see, the control's size is now acceptable, but the control is not the desired size (`30, 1`);  this is because a control does not need to obey your force resize instructions, and it is up to you to verify that the control is of a suitable size.

Another, important rule you need to know, is that a control may not render outside of it's designated area.  The drawing of elements is strictly positioned relativly, and any attempt to draw outside of these bounds will be clipped and nullified.  The controls position will be decided over a cascade of containers (and possibly other controls), of which will arive for you to draw at 0, 0 with the width and height of your control.
For example, a `ScrollContainer` whose client is a `FlowContainer` will result in your controls position being none-static, but you controll SHOULD never see this level of abstraction.

Please note, an excepion will NOT be thrown upon rendering outside of the render bounds; this is intended behavour, primarily for use with the `ScrollContainer`.
If you absoloutly must, and your control does not influence other controls (such as a tool tip), you may manually adjust the render bounds stack, but this is not advised and strongly discouraged as compatability may break, or it could interfere with other controls.

##### Positioning

While it may be hard to wrap your head around this concept at first, this tree should give you an idea of how it works:

```
WindowContainer ("Flow Test")
	FlowContainer (Vertical, spacing 1)
		Label ("Some message can go...")
		HScrollBar
		AlignContainer (Horizontal, center)
			FlowContainer (Horizontal, spacing 1)
				Button ("OK")
				Button ("Cancel")
				
# Button actually consists of:
Button
	AlignContainer (Vertical, center)
		AlignContainer (Horizontal, center)
			Label (button_text)
```	
We can see this layed out as an image here:

![alt text](http://screenshot.xiatek.org/Kobra/1368283806.png "Layout")

And what it results in:

![alt text](http://screenshot.xiatek.org/Kobra/1368281849.png "Layout")


#### Input System

The input system provides the standard input Consor will need, and must implement these pure methods:

```c++
using namespace Consor::Input;

bool KeyWaiting(); // is a key waiting on the input buffer?
Key GetKeyPress(); // get the next key press (repeats if held)
bool ControlDown(); // is control currently pressed?
bool ShiftDown(); // is shift currently pressed?
```

Upon input, an event will be generated at the highest level.  The event will be passed (recursively) to all clients (down `Control::HandleInput(Key, IInputSystem&)`); if all clients do not handle the input, then the control checks if itself should handle the input.
A control may steal the input before passing to their clients, such as scrolling a `ScrollContainer` when control is pressed, rather than focus being shifted.

An example of how this would work with a `TextBox`, and a `ScrollContainer` is shown below:

```
Structure of top:
top
	ScrollContainer
		FlowContainer
			TextBox
			Button

Key = 'a'
top
	-> ScrollContainer
		-> FlowContainer (Currently focused: TextBox)
			-> TextBox
			<INPUT HANDLED - Key typed>

Key = Key::Down
top
	-> ScrollContainer
		-> FlowContainer (Currently focused: TextBox)
			-> TextBox
			<UNHANDLED>
		<-
		<INPUT HANDLED - Focus changed to Button>

Key = Key::Enter
top
	-> ScrollContainer
		-> FlowContainer (Currently focused: Button)
			-> Button
			<INPUT HANDLED - Button clicked>

Key = Key::Down
Ctrl = true
top
	-> ScrollContainer
		<INPUT HANDLED (IF COULD SCROLL) - Scrolled down>

Key = Key::Escape
top
	-> ScrollContainer
		-> FlowContainer (Currently focused: Button)
			-> Button
			<UNHANDLED>
		<-
		<UNHANDLED>
	<-
	<UNHANDLED - Exhausted>
```

#### The Window System

The current Window System that is inbuilt into Consor positions any control into a desirable, or requested position.

A position requested of -1, -1 should be centered, and those in the range of 0 to 1 (0 to 1 not implimented yet) should be positioned said percentage along the screen.  So `(-1.0, 2.0/3.0)` will be positioned center X, and 2/3rds down the screen.

The idea of `WindowSystem` is to provide a simple and generic way to create a window for the user (for Prompts, etc...), you may expand or replace `Consor::WindowSystem` with your own.  
The easiest way to achive this, would be to create a blank control of the same size of the console (`IConsoleRenderer::GetSize()`) and use it as your base window system.  This keeps compatibility with prompts (`MessageBox`, `ChoiceList`, and `InputBox`) and any other applications that may use the same instance of `WindowSystem`, while allowing you to build upon your own canvas.

The window system also impliments support for hot keys via the use of `WindowSystem::RegisterHotKey(Control* pControl, Input::Key Key, bool Shift, bool Ctrl, std::function<void()> Callback)`.  Upon the key combo being pressed, `Callback` will be invoked; if `WindowSystem::UnregisterWindow(Control* pControl)`'s pControl matches an existing hot key, then the hot key will be removed.
