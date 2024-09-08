#pragma once

/**
 * Pre-compiled headers for JUCE plugins
 */

// C++/STL headers here...

// JUCE modules
#include <chowdsp_gui/chowdsp_gui.h>
#include <chowdsp_plugin_base/chowdsp_plugin_base.h>
#include <chowdsp_dsp_utils/chowdsp_dsp_utils.h>
#include <chowdsp_sources/chowdsp_sources.h>

// Any other widely used headers that don't change...
#include <chowdsp_wdf/chowdsp_wdf.h>
namespace wdft = chowdsp::wdft;

#include <cmrc/cmrc.hpp>
CMRC_DECLARE (gui);
