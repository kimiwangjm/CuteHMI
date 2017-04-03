#ifndef CUTEHMI_LIBBASE_INCLUDE_BASE_ERROREXCEPTION_HPP
#define CUTEHMI_LIBBASE_INCLUDE_BASE_ERROREXCEPTION_HPP

#include "Exception.hpp"
#include "ErrorInfo.hpp"

namespace cutehmi {
namespace base {

class CUTEHMI_BASE_API ErrorException:
	public Exception
{
	public:
		template <class ERROR>
		explicit ErrorException(ERROR error);

		const ErrorInfo & info() const;

	private:
		ErrorInfo m_errorInfo;
};

template <class ERROR>
ErrorException::ErrorException(ERROR error):
	Exception(error.str()),
	m_errorInfo(::cutehmi::base::errorInfo<ERROR>(error))
{
}

}
}

#endif

//(c)MP: Copyright © 2017, Michal Policht. All rights reserved.
//(c)MP: This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
