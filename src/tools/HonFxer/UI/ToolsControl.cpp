/**********************************************************************
This source file is a part of Demi3D
   __  ___  __  __  __
  |  \|_ |\/||   _)|  \ 
  |__/|__|  ||  __)|__/ 

Copyright (c) 2013-2014 Demi team
https://github.com/wangyanxing/Demi3D

Released under the MIT License
https://github.com/wangyanxing/Demi3D/blob/master/License.txt
***********************************************************************/

#include "FxerPch.h"
#include "ToolsControl.h"
#include "PropertiesControl.h"

namespace tools
{
	ToolsControl::ToolsControl(MyGUI::Widget* _parent) :
		wraps::BaseLayout("FxToolsControl.layout", _parent)
	{
        assignBase(mPropertiesCtrl, "PropertiesControl");
	}

	ToolsControl::~ToolsControl()
	{
	}

} // namespace tools
