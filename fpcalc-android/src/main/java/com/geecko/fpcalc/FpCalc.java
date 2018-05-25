package com.geecko.fpcalc;

/**
 * Created by steve on 2/11/18.
 */

public class FpCalc {
    static {
        try {
            System.loadLibrary("fpcalc");
        } catch (UnsatisfiedLinkError | SecurityException e) {
            e.printStackTrace();
        }
    }
    public static native String fpCalc(String[] args);
}
