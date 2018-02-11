package com.geecko.fpcalc;

/**
 * Created by steve on 2/11/18.
 */

public class FpCalc {
    static {
        System.loadLibrary("fpcalc");
    }
    public static native String fpCalc(String[] args);
}
