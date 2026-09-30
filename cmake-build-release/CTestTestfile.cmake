# CMake generated Testfile for 
# Source directory: /mnt/d/Work/SLSCM-LaB/contest/src2
# Build directory: /mnt/d/Work/SLSCM-LaB/contest/src2/cmake-build-release
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[soict_core_test]=] "/mnt/d/Work/SLSCM-LaB/contest/src2/cmake-build-release/soict_core_test")
set_tests_properties([=[soict_core_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/mnt/d/Work/SLSCM-LaB/contest/src2/CMakeLists.txt;105;add_test;/mnt/d/Work/SLSCM-LaB/contest/src2/CMakeLists.txt;0;")
add_test([=[soict_evaluator_test]=] "/usr/bin/python3.10" "/mnt/d/Work/SLSCM-LaB/contest/src2/tests/EvaluatorTest.py" "/mnt/d/Work/SLSCM-LaB/contest/src2/cmake-build-release/SoICT2026")
set_tests_properties([=[soict_evaluator_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/mnt/d/Work/SLSCM-LaB/contest/src2/CMakeLists.txt;107;add_test;/mnt/d/Work/SLSCM-LaB/contest/src2/CMakeLists.txt;0;")
