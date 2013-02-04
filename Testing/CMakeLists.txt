#
# Test executable
#


add_test(NAME FiberViewerLightTest COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:FiberViewerLight> --help )

