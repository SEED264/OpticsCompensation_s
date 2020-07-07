# OpticsCompensation_s
[![Packagist](https://img.shields.io/packagist/l/doctrine/orm.svg)](LICENSE)

### ※ このエフェクトはOpenCLを使用しているため、対応しているデバイスをお持ちでない場合は動作しません。

## ビルド

cmake_batch.shの`cmake_install_path`にDLLとスクリプトをインストールしたいディレクトリ、`lua_include_dir`と`lua_library_dir`にそれぞれLuaのヘッダとライブラリのディレクトリをそれぞれ指定し、
```bash
$ mkdir build
$ cd build
$ ../cmake_batch.sh msvc
```
でビルド用のプロジェクトを生成できます。  
MSBuild.exeがあるディレクトリをPATHに追加済みの場合は、Visual Studioを起動しなくても、
```bash
$ ../msvc_build.sh install
```
でビルドとインストールができます。

## スクリプト内での呼び出し
このDLLの関数は、事前に`obj.putpixeldata()`の呼び出し等の前準備を必要としません。画像の取得などの下準備から処理後のデータの仕上げまですべてDLL内で完結しています。  
実際の呼び出しは、
```lua
require("OpticsCompensation_s")
OpticsCompensation_s.OpticsCompensation(amount, anti_aliasing, offset_x, offset_y)
```
のようになります。らくちん。


## 関数
このDLLモジュールに含まれている関数です。

```lua
OpticsCompensation(amount, anti_aliasing, offset_x, offset_y)
```
OpticsCompensationのメインの関数です。これを呼び出すとレンズ補正のエフェクトがかかった状態になります
#### 引数
* `amount : float`  
    レンズ補正の変化量
    +の時は樽型  
    -の時は糸巻き型
* `anti_aliasing : bool`  
    trueにすると樽型時にアンチエイリアスが有効になる
* `offset_x : float`  
    中心点のX方向のオフセット
* `offset_y : float`  
    中心点のY方向のオフセット
