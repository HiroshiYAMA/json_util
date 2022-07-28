# json_util

JSON ファイルと DAT(CBOR) ファイルとの相互変換をするツールです。

# biuld

## CMake
```bash
mkdir build
cd build
cmake ..
make install
```

# usage

## JSON -> DAT(CBOR)
```bash
json_util hoge.json

# generate hoge.dat in same directory as hoge.json.
```

## DAT(CBOR) -> JSON
```bash
json_util hoge.dat

# generate hoge.json in same directory as hoge.dat.
```
