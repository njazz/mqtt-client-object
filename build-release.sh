# macOS script to make a release zip files

# 0.0.2

./scripts/build-osx.sh
./scripts/build-win-mingw32.sh

cd bin
mkdir v0.0.2
cd v0.0.2
mkdir mqtt_client-max-osx-v0.0.2
mkdir mqtt_client-pd-osx-v0.0.2
mkdir mqtt_client-max-win-v0.0.2
mkdir mqtt_client-pd-win-v0.0.2

cd ..

cp max/mqtt_client-help.maxpat v0.0.2/mqtt_client-max-osx-v0.0.2/
cp max/mqtt_client-help.maxpat v0.0.2/mqtt_client-max-win-v0.0.2/

cp pd/mqtt_client-help.pd v0.0.2/mqtt_client-pd-osx-v0.0.2/
cp pd/mqtt_client-help.pd v0.0.2/mqtt_client-pd-win-v0.0.2/

cp -r max/mqtt_client.mxo v0.0.2/mqtt_client-max-osx-v0.0.2/
cp max/mqtt_client.mxe v0.0.2/mqtt_client-max-win-v0.0.2/

cp pd/mqtt_client.pd_darwin v0.0.2/mqtt_client-pd-osx-v0.0.2/
cp pd/mqtt_client.dll v0.0.2/mqtt_client-pd-win-v0.0.2/

cd v0.0.2
zip -m -r mqtt_client-max-osx-v0.0.2.zip mqtt_client-max-osx-v0.0.2
zip -m -r mqtt_client-pd-osx-v0.0.2.zip mqtt_client-pd-osx-v0.0.2
zip -m -r mqtt_client-max-win-v0.0.2.zip mqtt_client-max-win-v0.0.2
zip -m -r mqtt_client-pd-win-v0.0.2.zip mqtt_client-pd-win-v0.0.2

