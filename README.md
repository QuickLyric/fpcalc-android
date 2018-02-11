# fpcalc-android
Use chromaprint library easily on Android with fpcalc-android

Simple Android wrapper for fpcalc utilizing Chromaprint (https://github.com/acoustid/chromaprint)

Usage:

```
dependencies {
  implementation 'com.github.QuickLyric:fpcalc-android:1.0.0'
}
```

Pass ```String[]``` to ```FpCalc.fpCalc``` in command line format, ig:

```
String[] args = {"-length", "16", pathToFile};
String result = FpCalc.fpCalc(args);
```

Currently using Chromaprint v1.4.3 and ffmpeg 3.2.10.
