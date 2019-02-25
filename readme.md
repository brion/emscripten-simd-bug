This is extracted from dav1d AV1 decoder's `wedge.c`, where with SIMD-enabled Wasm builds with latest-upstream-3454 I'm seeing errors.

The SIMD version explodes like this:

```
test-simd.js:41 Uncaught RuntimeError: memory access out of bounds
    at fill2d_16x2 (wasm-function[9]:1895)
    at dav1d_init_wedge_masks (wasm-function[8]:971)
    at main (wasm-function[10]:7)
    at Object.Module._main (https://brionv.com/misc/kersplat/test-simd.js:1823:74)
    at Object.callMain (https://brionv.com/misc/kersplat/test-simd.js:1949:30)
    at doRun (https://brionv.com/misc/kersplat/test-simd.js:2006:60)
    at https://brionv.com/misc/kersplat/test-simd.js:2017:7
```

It seems to explode in `fill2d_16x2`'s inline friend `init_chroma`, and many attempts to fiddle in there to see what's going on result in the memory access error going away (though other errors occur elsewhere in the decoder).
