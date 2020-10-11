if [ -d "build" ]
then
  echo "Folder build exists, removing now"
  rm -r build
fi

echo "Creating build folder"
mkdir build
cd build || { echo "Unable to create build folder, aborting..."; exit 1; }
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j8