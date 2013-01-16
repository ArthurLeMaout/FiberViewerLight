#
# Test executable
#


include( CTest )


add_test(NAME FiberViewerLightTest COMMAND $<TARGET_FILE:FiberViewerLight> --help )

