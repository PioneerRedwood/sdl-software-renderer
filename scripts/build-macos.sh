rm -rf ./build

# cmake -G Xcode -B ./build . --debug-output # Print debug output
cmake -G Xcode -B ./build .

open build/sdl-shmup.xcodeproj/project.xcworkspace