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
            rm CMakeCache.txt
            make clean 
        else
            cmake3 -DCMAKE_BUILD_TYPE=${buildType} ./ 
            make
        fi
        cd ..
    done
}

cd src/apps/
build
