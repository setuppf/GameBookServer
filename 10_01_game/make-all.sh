args=$1

buildType="Debug"
clean=false

if [ ${args}v = "release"v ];then
    buildType="Release"
elif [ ${args}v = "clean"v ];then
    clean=true
fi


build(){
    for i in `ls -d */`;do 
        cd $i
        if ${clean};then            
            make clean 
            rm -f CMakeCache.txt
            rm -rf CMakeFiles
            rm -rf Debug
            rm -f cmake_install.cmake
            rm -f Makefile
        else
            cmake3 -DCMAKE_BUILD_TYPE=${buildType} ./ 
            make -j4
        fi
        cd ..
    done
}

# cd src/libs/
# build

cd src/apps/
build

cd ../tools/
build
