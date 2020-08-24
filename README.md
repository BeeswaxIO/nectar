# nectar
The raw material for your sweet, sweet honey.

## To add to your Bazel project

### In your WORKSPACE

    # nectar
    http_archive(
        name = "com_beeswax_nectar",
        strip_prefix = "nectar-master",
        url = "https://github.com/BeeswaxIO/nectar/archive/master.zip",
    )

    native.bind(
        name = "nectar-lib",
        actual = "@com_beeswax_nectar//:nectar-lib",
    )

### In your `cc_library` or `cc_bin` rule in BUILD

    deps = [
        "//external:nectar-lib",
    ],

### In your code

    #include "nectar/common.h"
    
    namespace MyNamespace {
        namespace nectar = beeswax::nectar;
    }
