#pragma once

#if 0 // Глаза мозолит суак
#include <boost/config/helper_macros.hpp>

#define _TODO_FIXME_ETC_HEADER  __FILE__ "(" BOOST_STRINGIZE(__LINE__) "): "
#define _TODO(Message)  _TODO_FIXME_ETC_HEADER "TODO >>> " Message
#define _FIXME(Message) _TODO_FIXME_ETC_HEADER "FIXME >>> " Message
#define _HACK(Message)  _TODO_FIXME_ETC_HEADER "HACK >>> " Message
#define _WARN(Message)  _TODO_FIXME_ETC_HEADER "WARN >>> " Message

// Using example: #pragma message(_WARN("You are using TODO assistant"))
#endif
