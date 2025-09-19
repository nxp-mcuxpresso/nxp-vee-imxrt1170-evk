/*
 * Java
 *
 * Copyright 2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
package ej.microai.util;

public class TestUtilities {

    /**
     * Compares 2 float with a tolerance.
     *
     * @param referenceFloat the reference float.
     * @param checkFloat the float to check.
     * @param tolerance the acceptable tolerance.
     *
     * @return true if the difference between the two components is smaller or equal to the tolerance.
     */
    public static boolean compareFloat(float referenceFloat, float checkFloat, float tolerance) {
        float diff = Math.abs(referenceFloat - checkFloat);
        return diff <= tolerance;
    }
}
