#pragma once

#include <boost/config.hpp>
#define _UNDER_WINDOWS BOOST_WINDOWS

#if _UNDER_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#endif

#include <cstdlib>
#include <memory>

#include <QtCore/QtCore>
#include <QtGui/QtGui>
