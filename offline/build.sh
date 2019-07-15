echo "building bisheng..."
##cd ../bisheng; make clean && make; cd ../offline
cd ../bisheng; sh make.sh offline clean && sh make.sh offline; cd ../offline
echo "building extractor..."
cd ./extractor; make clean && make; cd ..
echo "building multioutput jar file..."
cd ./utils/multioutput; sh run.sh; cd ../..
echo "pwd:"`pwd`
echo "copy files to appropriate places..."
cp ./extractor/extractor ./bin
cp ./utils/multioutput/FeaMultiout.jar ./jar
