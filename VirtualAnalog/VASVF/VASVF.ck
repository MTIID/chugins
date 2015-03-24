class VASVF extends Chugen {
   
    float cutoff;
    second/samp => float fs;
    1.0/fs => float T;

    float warpedCutoff;
    float g;
    float R;
    25.0 => float Q;

    float a0, a1, a2;
    float s1, s2;
    float p;
    float hp, bp, lp, bs;

    fun void filt(float c) {
        c => cutoff;
        (2.0/T) * Math.tan((2.0 * pi * c * T)/2.0) => warpedCutoff;

        (warpedCutoff * T)/2.0 => g;

        1.0/(2.0 * Q) => R;
        1.0/(1.0 + 2.0 * R * g + g * g) => a0;
        g => a1;
        g => a2;
        2 * R + g => p;
    }

    fun float tick (float in) {
        a0 * (in - p * s1 - s2) => hp;
        a1 * hp + s1 => bp;
        a1 * bp + s2 => lp;

        in - 2.0 * R * bp => bs;

        a1 * hp + bp => s1;
        a2 * bp + lp => s2;
        
        return hp;
        // code here;
    }
}

Noise nois => VASVF va => dac;


while (true) {
    va.filt(800.0 + Math.sin(now/(second/32.0)) * 400.0);
    1::ms => now;
}

1::week => now;
