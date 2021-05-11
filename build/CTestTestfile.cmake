# CMake generated Testfile for 
# Source directory: /home/wolf/Desktop/Time/KwinCorners
# Build directory: /home/wolf/Desktop/Time/KwinCorners/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(appstreamtest "/usr/bin/cmake" "-DAPPSTREAMCLI=/usr/bin/appstreamcli" "-DINSTALL_FILES=/home/wolf/Desktop/Time/KwinCorners/build/install_manifest.txt" "-P" "/usr/share/ECM/kde-modules/appstreamtest.cmake")
set_tests_properties(appstreamtest PROPERTIES  _BACKTRACE_TRIPLES "/usr/share/ECM/kde-modules/KDECMakeSettings.cmake;163;add_test;/usr/share/ECM/kde-modules/KDECMakeSettings.cmake;181;appstreamtest;/usr/share/ECM/kde-modules/KDECMakeSettings.cmake;0;;/home/wolf/Desktop/Time/KwinCorners/CMakeLists.txt;29;include;/home/wolf/Desktop/Time/KwinCorners/CMakeLists.txt;0;")
