# Galacean Effects Native
Galacean Effects Native is an animation rendering engine targeting mobile platform(Android & iOS), which shares the same [Galacean Effects format](https://github.com/galacean/effects-specification) and [Galacean Effects Studio](https://galacean.antgroup.com/effects/) workflow.


## Features
* Support for multiple animation types, such as layer, particle, and 3D.
* Effortlessly melds with UI components, crafting a more vibrant and enriched visual experience.
* A vector-based solution with [high performance](resources/benchmark.md). The core engine is crafted in C++ and conforms to the industry-standard OpenGL graphics layer for optimal fidelity.
* Widely adopted by Alipay, China's leading mobile payment platform.
* Managed by the dedicated team, the project is advancing along a well-defined and stable roadmap, ensuring consistent enhancement.
* For more, visit: [What is Galacean Effects](https://galacean.antgroup.com/effects/#/user/wrgzr8).

## API Documentation
GEPlayer is the animation player provided by Galacean Effects Native. For documentation on it, visit: [GEPlayer Documentation](https://galacean.antgroup.com/effects/#/user/ox4pb0gu4zuol6st).

## Usage
- See [Galacean Effects Native Examples](https://github.com/galacean/effects-native-examples).

## Build Guides
### Android
``` bash
# macOS
cd path/to/project/android
./gradlew :library:assembleRelease
```
If all goes well, the AAR artifact will be located in library/build/outputs/aar.
### iOS
``` bash
# macOS
cd path/to/project/ios/GalaceanEffects
pod install
open GalaceanEffects.xcworkspace
# Then press Cmd+B to compile the framework
```
 If all goes well, the build product will be located in the Products directory of the Xcode Project Navigator.

## Directory Layout
Galacean Effects Native includes an adapter for separating the Alipay environment, which provides basic capabilities such as file downloading, file decompression, thread scheduling, logging, dynamic configuration, and degradation. It has two implementations: Adapter_Alipay and Adapter_OSS. The former is located within the Alipay internal environment, while the latter is included in this repository. All code outside the adapter is maintained consistently between Alipay and this repository.
```
GalaceanEffectsNative
├── android # Android Studio project
│   ├── library
│   │   ├── adapter_oss # Adapter_OSS
│   │   ├── src # common part, shared by Alipay and OSS
│   │   └── ...
│   └── ...
├── ios/GalaceanEffects # XCode project
│   ├── Sources
│   │   ├── AdapterOSS # Adapter_OSS
│   │   └── ... # common part, shared by Alipay and OSS
│   └── ...
├── core # C++ code, shared by iOS and Android.
│   ├── CMakeLists.txt # for NDK
│   └── ... 
└── ...
```

## Contribution
Contributions are welcome! If you have any suggestions or improvements, please feel free to open an issue or submit a pull request.

## License
This project is licensed under the [MIT License](LICENSE).
