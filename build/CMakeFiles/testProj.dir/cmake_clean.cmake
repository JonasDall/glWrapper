file(REMOVE_RECURSE
  "&{CMAKE_BINARY_DIR}/test/testProj.exe"
  "&{CMAKE_BINARY_DIR}/test/testProj.exe.manifest"
  "&{CMAKE_BINARY_DIR}/test/testProj.pdb"
  "CMakeFiles/testProj.dir/test/main.cpp.obj"
  "CMakeFiles/testProj.dir/test/main.cpp.obj.d"
  "libtestProj.dll.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/testProj.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
