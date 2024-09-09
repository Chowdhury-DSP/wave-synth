include(CMakeRC)
cmrc_add_resource_library(gui_resources
    NAMESPACE gui
    synth.png
    phaser.png
)
# Need to build BinaryData with -fPIC flag on Linux
set_target_properties(gui_resources PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
