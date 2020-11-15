build_for_compiler () {
  local dir_name="build_$1"
  if [ -d $dir_name ]
  then
    echo "Directory $dir_name exists, removing now"
    rm -r $dir_name
  fi

  echo "Creating $dir_name directory"
  mkdir $dir_name
  cd $dir_name || { echo "Unable to create $dir_name folder, aborting..."; exit 1; }
  cmake -DCMAKE_C_COMPILER=$1 -DCMAKE_CXX_COMPILER=$2 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$(pwd)/../bin_$1 ..
  make -j8
  cd ..
}

if [ -x $(which g++) ]
then
build_for_compiler gcc g++
fi

if [ -x $(which clang++) ]
then
build_for_compiler clang clang++
fi
