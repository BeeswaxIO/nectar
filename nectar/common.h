#pragma once

// Beeswax nectar common; your one-stop shop for universal utilities.
//
// This is the short list of headers that ought to be available in any file
// without additional steps to add it.
#include "collections.h"
#include "cpp20.h"
#include "cstring_view.h"
#include "scoper.h"

// To inject this namespace under your top-level namespace, place
// the following line within your scope:
//     namespace nectar = beeswax::nectar;
//
// Also recommended:
//     using nectar::operator"" _sz;
