#ifndef CUTEHMI_LIBBASE_INCLUDE_BASE_INTERNAL_PLUGINNODEDATA_HPP
#define CUTEHMI_LIBBASE_INCLUDE_BASE_INTERNAL_PLUGINNODEDATA_HPP

#include "common.hpp"
#include "../IPlugin.hpp"
#include "../Plugin.hpp"
#include "../DataBlock.hpp"

namespace cutehmi {
namespace base {
namespace internal {

class CUTEHMI_BASE_API PluginNodeData:
	public DataBlock
{
	public:
		PluginNodeData(Plugin * plugin, const QString & minVersion);

		Plugin * plugin() const;

		QString minVersion() const;

		IPlugin * pluginInstance() const;

	private:
		struct Members
		{
			Plugin * plugin;
			QString minVersion;
		};

		utils::MPtr<Members> m;
};

}
}
}

#endif

//(c)MP: Copyright © 2017, Michal Policht. All rights reserved.
//(c)MP: This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
