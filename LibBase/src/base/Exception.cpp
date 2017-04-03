#include "../../include/base/Exception.hpp"

namespace cutehmi {
namespace base {

Exception::Exception(const QString & what):
	m_whatArr(what.toLocal8Bit())
{
}

const char * Exception::what() const noexcept
{
	return m_whatArr.constData();
}

}
}

//(c)MP: Copyright © 2017, Michal Policht. All rights reserved.
//(c)MP: This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
