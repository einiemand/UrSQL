build_for_compiler () {
  if [ $# -ne 2 ]
  then
    echo "The number of arguments passed to $FUNCNAME is not 2 (C compiler name and C++ compiler name)"
    echo "The arguments were $@"
    echo "exiting..."
    exit 1
  fi
  
  local proj_dir="$(dirname $(readlink -f $0))"
  local build_dir="$proj_dir/build"
  local bin_dir="$proj_dir/bin"

  if [ -d $build_dir ]
  then
    echo "Directory $build_dir exists, removing now"
    rm -r $build_dir
  fi
  if [ -d $bin_dir ]
  then
    echo "Directory $bin_dir exists, removing now"
    rm -r $bin_dir
  fi

  echo "Creating $build_dir directory"
  mkdir $build_dir || { echo "Unable to create $build_dir, aborting..."; exit 1; }
  cd $build_dir
  cmake -DCMAKE_C_COMPILER=$1 -DCMAKE_CXX_COMPILER=$2 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$bin_dir ..
  make -j8
  cd ..
}

if [ -x $(which clang) -a -x $(which clang++) ]
then
  build_for_compiler clang clang++
elif [-x $(which gcc) -a -x $(which g++)]
then
  build_for_compiler gcc g++
else
  { echo "Unable to build: gcc and clang not detected"; exit 1; }
fi
